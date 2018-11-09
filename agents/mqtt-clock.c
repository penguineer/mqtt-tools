/*
 * Clock Example
 *
 * SPDX-License-Identifier: MIT
 * License-Filename: LICENSES/MIT.txt
 */

#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <time.h>
#include <sys/time.h>

#include <mqtt-tools/mqtta.h>
#include <mqtt-tools/mosqhelper.h>

#define WITH_SYSLOG
#ifdef WITH_SYSLOG
#	include <syslog.h>
#else
#	define syslog fprintf
#	define LOG_INFO stdout
#	define LOG_ERR  stderr
#endif

struct clock_state {
  uint8_t current_second;
  uint8_t current_minute;
};

struct datetime {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

/**
 * Get the seconds since epoch.
 */
time_t current_unixtime() {
  struct timeval tv;
  gettimeofday(&tv, 0);

  return tv.tv_sec;
}


void decode_current_time(struct datetime *dt)
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  dt->year 	= tm.tm_year;
  dt->month	= tm.tm_mon;
  dt->day	= tm.tm_mday;
  dt->hour	= tm.tm_hour;
  dt->minute 	= tm.tm_min;
  dt->second	= tm.tm_sec;
}

struct mqtta_message* create_value_message(const char* topic,
                                           const char* format,
                                           const int val)
{
    char buf[16];

    snprintf(buf, 16, format, val);

    struct mqtta_message *msg;
    msg = mqtta_create_message(topic,
                               buf,     /* payload */
                               2,       /* qos */
                               false    /* retain */
                              );

    return msg;
}

int mqtta_send_message(struct mosqagent* agent,
                       struct mqtta_message *msg)
{
    if (!agent || !msg) {
        errno = EINVAL;
        goto fail;
    }

    int ret;

    ret = mqtt_publish(agent->mosq, NULL,
                       msg->topic,
                       strlen(msg->payload), msg->payload,
                       msg->qos,
                       msg->retain);

    return ret;

fail:
    return -1;
}

void send_value(struct mosqagent* agent,
                const char *topic,
                const char* format,
                int val)
{
    int ret;

    struct mqtta_message *msg;
    msg = create_value_message(topic, format, val);

    ret = mqtta_send_message(agent, msg);

    mqtta_dispose_message(msg);

    if (ret)
        syslog(LOG_ERR, "Error on publish %d", ret);
}

struct mosqagent_result* clock_idle(struct mosqagent *agent)
{
  struct clock_state *state;
  struct datetime current_dt;

  state = (struct clock_state*) mosqagent_get_private_data(agent);

  decode_current_time(&current_dt);


  if (state->current_minute != current_dt.minute) {
    send_value(agent,
		"Netz39/Service/Clock/Wallclock/Year",
		"%02d",
		1900 + current_dt.year);

    send_value(agent,
		"Netz39/Service/Clock/Wallclock/Simple/Month",
		"%02d",
		1 + current_dt.month);

    send_value(agent,
		"Netz39/Service/Clock/Wallclock/Simple/Day",
		"%02d",
		current_dt.day);

    send_value(agent,
		"Netz39/Service/Clock/Wallclock/Simple/Hour",
		"%02d",
		current_dt.hour);

    send_value(agent,
		"Netz39/Service/Clock/Wallclock/Simple/Minute",
		"%02d",
		current_dt.minute);
    state->current_minute = current_dt.minute;
  }

  if (state->current_second != current_dt.second) {
    send_value(agent,
		"Netz39/Service/Clock/Wallclock/Simple/Second",
		"%02d",
		current_dt.second);
    state->current_second = current_dt.second;

    send_value(agent,
		"Netz39/Service/Clock/UnixTimestamp",
		"%d",
		current_unixtime());
  }

  return NULL;
}

bool run = true;

void sig_finish_handler(int signum) {
    printf("Interrupted!\n");

    run = false;
}

int set_signal_handlers() {
    struct sigaction sact;
    sigset_t block_mask;
    int ret = 0;

    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGINT);
    sigaddset(&block_mask, SIGTERM);
    sigaddset(&block_mask, SIGQUIT);

    sact.sa_handler = &sig_finish_handler;
    sact.sa_mask = block_mask;
    sact.sa_flags = 0;

    ret |= sigaction(SIGINT, &sact, NULL);
    ret |= sigaction(SIGTERM, &sact, NULL);
    ret |= sigaction(SIGQUIT, &sact, NULL);

    return ret;
}

int main(int argc, char *argv[]) {
    int ret;

    ret = set_signal_handlers();
    if (ret) {
        printf("Error setting signal handlers!\n");
        return -1;
    }

    // set to non-time values
    struct clock_state state = {
        .current_second = 0xff,
        .current_minute = 0xff,
    };

    struct mosqagent* agent;
    agent = mosqagent_init_agent(&state);
    if (!agent) {
        printf("Could not initialize the agent!\n");
        return -1;
    }

    if (mqtta_load_configuration(agent, "mqtta-config")) {
        printf("Failed to load the configuration!\n");
        return -1;
    }

    printf("Agent configuration:\n"
        "\tName    %s\n"
        "\tBroker  %s:%d\n",
        mqtta_get_configuration(agent)->client_name,
        mqtta_get_configuration(agent)->host,
        mqtta_get_configuration(agent)->port);

  // initialize the system logging
#ifdef WITH_SYSLOG
    openlog(mqtta_get_configuration(agent)->client_name, LOG_CONS | LOG_PID, LOG_USER);
#endif
  syslog(LOG_INFO, "MQTT Clock serivce started.");

    ret = mosqagent_setup_mqtt(agent);
    if (ret) {
      syslog(LOG_ERR, "Mosquitto Agent could not connect: %s",
	    mosqagent_strerror(errno));
    }

    ret = mosqagent_add_idle_call(agent, clock_idle);
    if (ret)
      syslog(LOG_ERR, "Cannot add clock idle call: %s",
	     mosqagent_strerror(ret));

  run &= (agent != NULL);
  while (run) {
    mosqagent_idle(agent);

    usleep(200*1000);
  } // while (run)

  mosqagent_close_agent(agent);

  syslog(LOG_INFO, "MQTT Clock serivce quit.");

#ifdef WITH_SYSLOG
  closelog();
#endif

  return 0;
}

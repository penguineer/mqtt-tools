#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <time.h>

#include "mosqhelper.h"

#define WITH_SYSLOG
#ifdef WITH_SYSLOG
#	include <syslog.h>
#else
#	define syslog fprintf
#	define LOG_INFO stdout
#	define LOG_ERR  stderr
#endif

const char* MQTT_HOST 	= "platon";
const int   MQTT_PORT 	= 1883;

/**
 * Get the milliseconds since epoch.
 */
long current_millis() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  
  return tv.tv_sec*1000L + tv.tv_usec/1000L;
}

struct datetime {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

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

void send_value(struct mosquitto *mosq, const char *topic,
		const char* format, long val)
{
  char buf[16];
  int ret;

  snprintf(buf, 16, format, val);

  ret = mqtt_publish(mosq, NULL,
		     topic,
		     strlen(buf),  	/* payloadlen */
		     buf,		/* payload */
		     2, 		/* qos */
		     false		/* retain */
  );
  if (ret)
    syslog(LOG_ERR, "Error on publish %d", ret);
}

int main(int argc, char *argv[]) {
  const char* mqtt_client_name = "mqtt-clock";


  // initialize the system logging
#ifdef WITH_SYSLOG
  openlog(mqtt_client_name, LOG_CONS | LOG_PID, LOG_USER);
#endif
  syslog(LOG_INFO, "MQTT Clock serivce started.");

  void *mqtt_obj;
  struct mosquitto *mosq;

  if (mqtt_init(mqtt_client_name,
	        &mosq,
	        &mqtt_obj))
    return -1;

  if (mqtt_connect(mosq,
	           MQTT_HOST,
	           MQTT_PORT,
	           0))
    return -1;

  struct datetime old_dt = {
    .year = -1,
    .month = -1,
    .day = -1,
    .hour = -1,
    .minute = -1,
    .second = -1
  };

  int ret;

  bool run = (mosq != NULL);
  while (run) {
    struct datetime current_dt;
    decode_current_time(&current_dt);

    if (old_dt.minute != current_dt.minute) {
      send_value(mosq,
		 "Netz39/Service/Clock/Wallclock/Year",
		 "%02d",
		 1900 + current_dt.year);
      old_dt.year = current_dt.year;

      send_value(mosq,
		 "Netz39/Service/Clock/Wallclock/Simple/Month",
		 "%02d",
		 current_dt.month);
      old_dt.month = current_dt.month;

      send_value(mosq,
		 "Netz39/Service/Clock/Wallclock/Simple/Day",
		 "%02d",
		 current_dt.day);
      old_dt.day = current_dt.day;

      send_value(mosq,
		 "Netz39/Service/Clock/Wallclock/Simple/Hour",
		 "%02d",
		 current_dt.hour);
      old_dt.hour = current_dt.hour;

      send_value(mosq,
		 "Netz39/Service/Clock/Wallclock/Simple/Minute",
		 "%02d",
		 current_dt.minute);
      old_dt.minute = current_dt.minute;
    }

    if (old_dt.second != current_dt.second) {
      send_value(mosq,
		 "Netz39/Service/Clock/Wallclock/Simple/Second",
		 "%02d",
		 current_dt.second);
      old_dt.second = current_dt.second;

      send_value(mosq,
	         "Netz39/Service/Clock/UnixTimestamp",
		 "%ld",
		 current_millis());
    }

    usleep(200*1000);
    mqtt_loop(mosq);
  } // while (run)


  // clean-up MQTT
  mqtt_close(mosq);

  syslog(LOG_INFO, "MQTT Clock serivce quit.");

#ifdef WITH_SYSLOG
  closelog();
#endif
    
  return 0;
}

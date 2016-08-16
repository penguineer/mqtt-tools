#include "mosqagent.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include <mosquitto.h>

struct mosqagent_idle_list {
  struct mosqagent_idle_list *next;
  mosqagent_idle_call idle_call;
};

void mosqagent_clear_idle_list(struct mosqagent *agent);


struct mosqagent* mosqagent_init_agent(void *priv_data)
{
  struct mosqagent *agent;

  agent = malloc(sizeof(*agent));

  if (!agent) {
    errno = ENOMEM;
    return NULL;
  }

  agent->idle = NULL;
  agent->priv_data = priv_data;

  return agent;
}

int mosqagent_setup_mqtt(struct mosqagent *agent,
			 struct mosqagent_config *config)
{
  if (!agent || !config) {
    errno = EINVAL;
    return -1;
  }

  if (mqtt_init(config->client_name,
	        &(agent->mosq),
	        agent)) {
    // errno is already set
    return -1;
  }

  if (mqtt_connect(agent->mosq,
	           config->host,
	           config->port,
	           0)) {
    // errno is already set
    return -1;
  }

  return 0;
}

int mosqagent_close_agent(struct mosqagent *agent)
{
  if (!agent)
    return -EINVAL;

  mosqagent_clear_idle_list(agent);

  // clean-up MQTT
  mqtt_close(agent->mosq);

  free(agent);

  return 0;
}

void* mosqagent_get_private_data(const struct mosqagent *agent)
{
  return agent->priv_data;
}

int mosqagent_add_idle_call(struct mosqagent *agent,
                            mosqagent_idle_call call)
{
  if (!agent || !call) {
    errno = EINVAL;
    goto fail;
  }

  // new entry
  struct mosqagent_idle_list* entry;

  entry = malloc(sizeof(*entry));
  if (!entry) {
    errno = ENOMEM;
    goto fail;
  }

  // initialize the entry
  entry->next = NULL;
  entry->idle_call = call;


  // add to idle list
  if (!agent->idle) {
    // new idle list
    agent->idle = entry;
  } else {
    // find tail
    struct mosqagent_idle_list *e;
    e = agent->idle;
    while (e)
      e = e->next;

    // add new entry
    e->next = entry;
  }

  return 0;

fail:
  return -1;
}

void mosqagent_clear_idle_list(struct mosqagent *agent)
{
  if (!agent)
    return;

  if (!agent->idle)
    return;

  struct mosqagent_idle_list *e;
  e = agent->idle;

  while (e) {
    struct mosqagent_idle_list *f;

    f = e;
    e = e->next;

    free(f);
  }

  agent->idle = NULL;
}

int mosqagent_idle(struct mosqagent *agent)
{
  struct mosqagent_idle_list *e;
  e = agent->idle;


  while (e) {
    e->idle_call(agent);

    e = e->next;
  }
}

const char* mosqagent_strerror(int mosq_errno)
{
  return mosquitto_strerror(mosq_errno);
}
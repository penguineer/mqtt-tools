#include "mosqagent.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <mosquitto.h>

#include "mosqhelper.h"

struct mosqagent_idle_list {
    struct mosqagent_idle_list *next;
    mosqagent_idle_call idle_call;
};

void mosqagent_clear_idle_list(struct mosqagent *agent);

/**
 * Create a message and deep-copy the parameter values.
 */
struct mqtta_message* mqtta_create_message(const char* topic,
                                           const char* payload,
                                           const int qos,
                                           const bool retain)
{
    // Check parameters

    if (!topic || (strlen(topic) == 0)) {
        errno = EINVAL;
        goto fail;
    }

    if (!payload || (strlen(payload) == 0)) {
        errno = EINVAL;
        goto fail;
    }

    if ((qos < 0) || (qos > 2)) {
        errno = EINVAL;
        goto fail;
    }

    // create the struct
    struct mqtta_message *msg;
    msg = malloc(sizeof(*msg));

    if (!msg) {
        errno = ENOMEM;
        goto fail;
    }

    // copy topic and payload
    const int topiclen = strlen(topic) + 1; // plus \0
    msg->topic = malloc(topiclen);
    if (!msg->topic) {
        errno = ENOMEM;
        goto fail_with_msg;
    }
    strncpy(msg->topic, topic, topiclen);

    const int payloadlen = strlen(payload) + 1; // plus \0
    msg->payload = malloc(payloadlen);
    if (!msg->payload) {
        errno = ENOMEM;
        goto fail_with_topic;
    }
    strncpy(msg->payload, payload, payloadlen);

    // set the remaining parameters
    msg->qos = qos;
    msg->retain = retain;

    // done
    return msg;

fail_with_topic:
    if (msg)
        free(msg->topic);

fail_with_msg:
    free(msg);

fail:
    return NULL;
}

void mqtta_dispose_message(struct mqtta_message *msg)
{
    if (!msg)
        return;

    // free memory for all message parts
    if (msg->topic)
        free (msg->topic);

    if (msg->payload)
        free(msg->payload);

    free(msg);
}


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

// TODO ignore the unused variable in GCC until res from idle calls is used
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
int mosqagent_idle(struct mosqagent *agent)
{
    struct mosqagent_idle_list *e;
    e = agent->idle;


    while (e) {
        struct mosqagent_result *res;

        res = e->idle_call(agent);

        //TODO process idle call result, esp. check for errors
        e = e->next;

    }

    int ret;
    // call the mosquitto loop
    // TODO only when idle calls had not critical errors
    ret = mqtt_loop(agent->mosq);

    // TODO return type based on errros from idle calls
    return ret;
}

const char* mosqagent_strerror(int mosq_errno)
{
    return mosquitto_strerror(mosq_errno);
}

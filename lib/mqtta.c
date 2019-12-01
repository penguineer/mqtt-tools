/*
 *
 * SPDX-License-Identifier: MIT
 * License-Filename: LICENSES/MIT.txt
 */

#include "mqtt-tools/mqtta.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <libconfig.h>

#include <mosquitto.h>

#include "mqtt-tools/mosqhelper.h"
#include "mqtta-build.h"


void* mqtta_mo_ptr(const struct mqtta_memory_object *mo)
{
    return mo ? mo->ptr : NULL;
}

struct mqtta_memory_object* mqtta_mo_move(struct mqtta_memory_object *mo,
                                          void *ptr,
                                          mqtta_mo_deallocator *deallocator)
{
    if (mo) {
        mo->ptr = ptr;
        mo->deallocator = deallocator;
    }

    return mo;
}

struct mqtta_memory_object* mqtta_mo_set(struct mqtta_memory_object *mo,
                                         void *ptr)
{
    // same as move without de-allocator
    return mqtta_mo_move(mo, ptr, NULL);
}

void mqtta_mo_free(struct mqtta_memory_object *mo)
{
    if (mo && mo->ptr && mo->deallocator) {
        mo->deallocator(mo->ptr);
        mo->ptr = NULL;
        //mo->deallocator = NULL;  // <-- not necessary
    }
}


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
    free (msg->topic);
    free(msg->payload);

    free(msg);
}

/*
 * Destroy the internal configuration object, if ownership
 * is with the agent.
 */
static void destroy_configuration(struct mosqagent *agent)
{
    if (!agent)
        return;

    // call the de-allocator of the configuration mo
    mqtta_mo_free(&agent->config_mo);
}

int mqtta_load_configuration(struct mosqagent *agent,
                             const char* filepath)
{
    int ret = 0;

    struct mosqagent_config *config;

    config_t configuration;

    const char* client_name;
    const char* broker_host;

    // Init the configuration struct from libconfig
    config_init(&configuration);

    /*
     * Check this part first. Not finding the config file is the most likely
     * failure and we'll fail this before reserving memory.
     */

    if (config_read_file(&configuration, filepath) == CONFIG_FALSE) {
        fprintf(stderr, "Cannot read config file: %s\n", config_error_text(&configuration));
        ret = MQTTA_ERR_CONFIG_READ_FAILED;
        goto cleanup_with_configuration;
    }

    // Now create the memory object
    config = malloc(sizeof(*config));
    if (!config) {
        ret = -ENOMEM;
        goto cleanup_with_configuration;
    }

    // We have to find an agent name!
    if (config_lookup_string(&configuration, "mosqagent.name", &client_name))
    {
        const int slen = strlen(client_name);
        config->client_name = malloc(slen + 1);
        strncpy(config->client_name, client_name, slen+1);
        // ensure string termination
        config->client_name[slen] = '\0';
    } else {
        ret = MQTTA_ERR_CONFIG_NO_CLIENTNAME;
        goto fail_with_config_object;
    }

    // Host is optional
    if (config_lookup_string(&configuration, "mosqagent.broker.host", &broker_host))
    {
        const int slen = strlen(broker_host);
        config->host = malloc(slen + 1);
        strncpy(config->host, broker_host, slen+1);
        // ensure string termination
        config->host[slen] = '\0';
    } else {
        printf("Host not found, using configuration default.");
        // TODO should the warning be communicated and if yes, how?
    }

    // Port is optional
    config_lookup_int(&configuration, "mosqagent.broker.port", &config->port);

    // If we got through to here, store configuration to agent.
    // Destroy old config first.
    destroy_configuration(agent);

    // transfer ownership of the config object to the agent
    mqtta_move_configuration(agent, config);

    // standard cleanup
    goto cleanup_with_configuration;


fail_with_config_object:
    free(config);

cleanup_with_configuration:
    config_destroy(&configuration);

    return ret;
}

void mqtta_set_configuration(struct mosqagent *agent,
                            struct mosqagent_config* config)
{
    if (agent)
        mqtta_mo_set(&agent->config_mo, config);
}

void mqtta_move_configuration(struct mosqagent *agent,
                             struct mosqagent_config* config)
{
    if (agent)
        mqtta_mo_move(&agent->config_mo, config,
                      mqtta_configuration_deallocator);
}

void mqtta_dispose_configuration(struct mosqagent_config *config)
{
    if (!config)
        return;
    free(config->client_name);
    free(config->host);
}

void mqtta_configuration_deallocator(void* config)
{
    if (!config)
        return;

    struct mosqagent_config *c = (struct mosqagent_config*)config;

    mqtta_dispose_configuration(c);
    free(c);
}


struct mosqagent_config* mqtta_get_configuration(const struct mosqagent *agent)
{
    return agent ? agent->config_mo.ptr : NULL;
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
    mqtta_mo_move(&agent->config_mo, NULL, NULL);

    return agent;
}

int mosqagent_setup_mqtt(struct mosqagent *agent)
{
    if (!agent || !mqtta_get_configuration(agent)) {
        errno = EINVAL;
        return -1;
    }

    struct mosqagent_config *config =
        mqtta_get_configuration(agent);


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

    destroy_configuration(agent);

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
        while (e->next)
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

const char* mqtta_version( void )
{
    return MQTTA_VERSION;
}

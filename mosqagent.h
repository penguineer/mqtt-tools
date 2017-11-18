/*
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>


#define MQTTA_ERR_CONFIG_READ_FAILED        1
#define MQTTA_ERR_CONFIG_NO_CLIENTNAME      2

struct mosqagent_idle_list;
struct mosqagent_config;

struct mosqagent {
    struct mosqagent_config *config;

    struct mosqagent_idle_list *idle;

    struct mosquitto *mosq;

    void *priv_data;
};

struct mqtta_message {
    char* topic;
    char* payload;
    int qos;
    bool retain;
};

struct mqtta_message* mqtta_create_message(const char* topic,
                                           const char* payload,
                                           const int qos,
                                           const bool retain);

int mqtta_send_message(struct mosqagent* agent,
                       struct mqtta_message *msg);

void mqtta_dispose_message(struct mqtta_message *msg);


struct mqtta_message_list {
    struct mqtta_message_list *next;
    struct mqtta_message *msg;
};

// TODO
struct mqtta_message_list* mqtta_message_list_append(struct mqtta_message_list *list,
                                                     struct mqtta_message *msg);

//TODO
struct mqtta_message_list* mqtta_message_list_remove(struct mqtta_message_list* list,
                                                     struct mqtta_message* msg);

//TODO
struct mqtta_message_list* mqtta_message_list_dispose(struct mqtta_message_list* list);


struct mosqagent_result {

};

/**
 * \brief The agent's configuration settings
 */
struct mosqagent_config {
    char* client_name;
    char* host;
    int port;
};

/**
 * Load configuration from file into the client. A configuration must be
 * loaded before mqtt can be set up.
 *
 * If *agent already had a configuration loaded, the library will close and
 * free it and load the new configuration instead.
 */
int mqtta_load_configuration(struct mosqagent *agent,
                             const char* filepath);


typedef struct mosqagent_result* (*mosqagent_idle_call)(struct mosqagent*);


struct mosqagent* mosqagent_init_agent(void *priv_data);

int mosqagent_setup_mqtt(struct mosqagent *agent);

int mosqagent_close_agent(struct mosqagent *agent);

void* mosqagent_get_private_data(const struct mosqagent *agent);

int mosqagent_add_idle_call(struct mosqagent *agent,
                            mosqagent_idle_call call);

int mosqagent_idle(struct mosqagent *agent);

const char* mosqagent_strerror(int mosq_errno);

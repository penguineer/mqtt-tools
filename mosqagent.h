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

/**
 * \brief A memory object that stores ownership information.
 *
 * Never manipulate the values directly! Always use mqtta_mo_move and
 * mqtta_mo_set to change pointers. Ownership information is managed
 * automatically by these functions.
 */
struct mqtta_memory_object {
    void *ptr;
    bool ownership;
};

/**
 * \brief Get the pointer from a memory object.
 */
void* mqtta_mo_ptr(struct mqtta_memory_object *mo);

/**
 * Get the ownership information from a memory object.
 */
bool mqtta_mo_ownership(struct mqtta_memory_object *mo);

/**
 * \brief Set the pointer and transfer ownership.
 *
 * Ownership transfer implies that the pointer will be managed be the entity
 * owning the memory object, i.e. the pointer should not be manipulated
 * afterwards, especially free must not be called.
 *
 * Validity of the pointer is tied to the implementation of the functions using
 * the memory object.
 *
 * Never transfer ownership for stack pointers and global variables.
 */
struct mqtta_memory_object* mqtta_mo_move(struct mqtta_memory_object *mo,
                                          void *ptr);

/**
 * \brief Set the pointer, but do not move ownership.
 *
 * The pointer in the memory object is set, but ownership remains with the
 * caller. The pointer will not be manipulated by the entity owning the memory
 * object, especially free will not be called.
 *
 * The caller has to ensure that the pointer is valid as long as the entity
 * owning the memory object will need it.
 *
 * Use this function for stack pointers and global variables.
 */
struct mqtta_memory_object* mqtta_mo_set(struct mqtta_memory_object *mo,
                                         void *ptr);

/**
 * \brief Free the memory object.
 *
 * In case of ownership free will be called, otherwise nothing happens.
 */
void mqtta_mo_free(struct mqtta_memory_object *mo);


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

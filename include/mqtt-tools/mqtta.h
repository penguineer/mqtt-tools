/*******************************************************************//**
 * \file		mqtta.h
 *
 * \brief		Library to simplify MQTT agents
 *
 * \author		Stefan Haun <tux@netz39.de>
 *
 * SPDX-License-Identifier: MIT
 * License-Filename: LICENSES/MIT.txt
 *
 * \copyright	2017 Stefan Haun, Netz39 e.V., and mqtta contributors
 **********************************************************************/

#pragma once

#include <stdbool.h>


#define MQTTA_ERR_CONFIG_READ_FAILED        1
#define MQTTA_ERR_CONFIG_NO_CLIENTNAME      2

struct mosqagent_idle_list;
struct mosqagent_config;


/**
 * \brief De-allocator function pointer
 *
 * The de-allocator is responsible for cleaning up a pointer and possible
 * heap structures in the pointed-to struct. The exact handling is based
 * on the struct, so the user (or the library) has to provide the correct
 * de-allocator for each memory object.
 *
 * Providing `NULL` as de-allocator reference will be interpreted as
 * "no de-allocation", i.e. the object will be left alone. Use this for stack
 * pointers of if clean-up is handled somewhere else.
 *
 * \note The de-allocator is also responsible for cleaning up the pointed-to
 * memory. The pendant for `malloc`-created objects would be a pointer to
 * `free`.
 */
typedef void (mqtta_mo_deallocator)(void*);

/**
 * \brief A memory object that stores ownership information.
 *
 * Never manipulate the values directly! Always use `mqtta_mo_move` and
 * `mqtta_mo_set` to change pointers. Ownership information is managed
 * automatically by these functions.
 */
struct mqtta_memory_object {
    void *ptr;
    mqtta_mo_deallocator *deallocator;
};

/**
 * \brief Get the pointer from a memory object.
 */
void* mqtta_mo_ptr(const struct mqtta_memory_object *mo);

/**
 * \brief Set the pointer and transfer ownership by providing a de-allocator.
 *
 * Ownership transfer implies that the pointer will be managed be the entity
 * owning the memory object, i.e. the pointer should not be manipulated
 * afterwards, especially free must not be called. The new owner will call
 * the de-allocator when the memory object is no longer used.
 *
 * Validity of the pointer is tied to the implementation of the functions using
 * the memory object.
 *
 * \returns the pointer provided for `mo` to enable function chaining.
 *
 * \warning Never transfer ownership for stack pointers and global variables!
 *
 * \note Provding a `NULL` de-allocator is equivalent to not transferring
 *       ownership, i.e. the memory object will not be freed.
 * \note Provide `free` as the standard de-allocator for `malloc`-created
 *       objects.
 */
struct mqtta_memory_object* mqtta_mo_move(struct mqtta_memory_object *mo,
                                          void *ptr,
                                          mqtta_mo_deallocator *deallocator);

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
 *
 * \returns the pointer provided for `mo` to enable function chaining.
 */
struct mqtta_memory_object* mqtta_mo_set(struct mqtta_memory_object *mo,
                                         void *ptr);

/**
 * \brief Free the memory object.
 *
 * The de-allocator will be called if provided and the internal pointer is
 * not `NULL`, otherwise nothing happens.
 *
 * Safe to call multiple times, but not before initialization
 * (might point anywhere).
 */
void mqtta_mo_free(struct mqtta_memory_object *mo);


struct mosqagent {
    struct mqtta_memory_object config_mo;

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
                                           int qos,
                                           bool retain);

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

/**
 * \brief Set a configuration, but keep ownership.
 *
 * Use this if the configuration struct is a stack pointer or global variable.
 */
void mqtta_set_configuration(struct mosqagent *agent,
                            struct mosqagent_config* config);

/**
 * \brief Set a configuration and transfer ownership.
 *
 * Use this if the configuration is a heap object and the agent should
 * destroy the configuration on clean-up.
 */
void mqtta_move_configuration(struct mosqagent *agent,
                             struct mosqagent_config* config);

/**
 * \brief Dispose of a configuration object.
 *
 * Note: This frees the configuration object, not a memory object struc.
 * Ownership is not relevant for this function!
 *
 * \note Call this on stack pointers.
 */
void mqtta_dispose_configuration(struct mosqagent_config *config);

/**
 * \brief (Heap) De-allocator for configuration objects
 *
 * Frees the __char*__ fields and then the struct itself.
 *
 * \warning Only use with heap objects where a call to __free__ is safe!
 */
void mqtta_configuration_deallocator(void *config);

struct mosqagent_config* mqtta_get_configuration(const struct mosqagent *agent);


typedef struct mosqagent_result* (*mosqagent_idle_call)(struct mosqagent*);


struct mosqagent* mosqagent_init_agent(void *priv_data);

int mosqagent_setup_mqtt(struct mosqagent *agent);

int mosqagent_close_agent(struct mosqagent *agent);

void* mosqagent_get_private_data(const struct mosqagent *agent);

int mosqagent_add_idle_call(struct mosqagent *agent,
                            mosqagent_idle_call call);

int mosqagent_idle(struct mosqagent *agent);

const char* mosqagent_strerror(int mosq_errno);

const char* mqtta_version( void );

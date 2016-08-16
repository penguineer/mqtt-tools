#pragma once

struct mosqagent_idle_list;

struct mosqagent {
  struct mosqagent_idle_list *idle;

  struct mosquitto *mosq;

   void *priv_data;
};

struct mosqagent_result {

};

struct mosqagent_config {
  const char* client_name;
  const char* host;
  int port;
};

typedef struct mosqagent_result* (*mosqagent_idle_call)(struct mosqagent*);


struct mosqagent* mosqagent_init_agent(void *priv_data);

int mosqagent_setup_mqtt(struct mosqagent *agent,
			 struct mosqagent_config *config);

int mosqagent_close_agent(struct mosqagent *agent);

void* mosqagent_get_private_data(const struct mosqagent *agent);

int mosqagent_add_idle_call(struct mosqagent *agent,
                            mosqagent_idle_call call);

int mosqagent_idle(struct mosqagent *agent);

const char* mosqagent_strerror(int mosq_errno);

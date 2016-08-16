/*
 * Helper Lib for libmosquitto
 */

#pragma once

#include <mosquitto.h>


int mqtt_init(const char* clientname,
	      struct mosquitto **mosq,
	      void *mqtt_obj);

int mqtt_connect(struct mosquitto *mosq,
                 const char* host,
		 const int port,
		 const int retries);

int mqtt_loop(struct mosquitto *mosq);

int mqtt_close(struct mosquitto *mosq);

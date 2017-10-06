#include "mosqhelper.h"

#include <errno.h>

#include <syslog.h>
#include <unistd.h>

int mqtt_init(const char* clientname,
	      struct mosquitto **mosq,
	      void *mqtt_obj)
{
  // initialize MQTT
  mosquitto_lib_init();

  *mosq = mosquitto_new(clientname,
		       false, 	/* clean session */
		       mqtt_obj);
  if (!(*mosq)) {
    syslog(LOG_ERR, "MQTT error on init: %d (%s)",
		errno,
		mosquitto_strerror(errno));

    return errno;
  }

  return 0;
}

int mqtt_connect(struct mosquitto *mosq,
                 const char* host,
		 const int port,
		 const int retries)
{
  int ret;
  int tries = retries ? retries : -1;
  const int wait = 10;

  // try until we're successful
  while (tries) {
    ret = mosquitto_connect(mosq, host, port, 30);

    if (ret == MOSQ_ERR_SUCCESS) {
      ret = 0; // finished
      break;
    }

      // Check if we need another try
    if (ret == MOSQ_ERR_SUCCESS) {
      return 0;
    }

    if (ret == MOSQ_ERR_ERRNO) {
      ret = errno;

      if (ret == ECONNREFUSED) {
	syslog(LOG_ERR, "MQTT connection refused by %s, trying again in %d seconds.",
	                 host,
			 wait);
      } else {
	syslog(LOG_ERR, "MQTT error on connect, trying again in %d seconds: %d (%s)",
			wait,
			ret,
			mosquitto_strerror(ret));
      }

      sleep(wait);
    } else {
      syslog(LOG_ERR, "MQTT unknown error on connect: %d", ret);
    }

    --tries;
  }

   return ret;
}

int mqtt_loop(struct mosquitto *mosq)
{
  int ret;
  ret = mosquitto_loop(mosq,
		       100, /* timeout */
		       1    /* maxpackets, 1 for future compat */
		      );

  // if failed, try to reconnect
  if (ret)
    mosquitto_reconnect(mosq);

  return ret;
}

int mqtt_close(struct mosquitto *mosq)
{
    int ret;

    ret = mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return ret;
}

int mqtt_publish(struct mosquitto *mosq,
		 int* mid,
		 const char* topic,
		 int payloadlen,
		 const char* payload,
		 int qos,
		 bool retain)
{
  int ret;

  ret = mosquitto_publish(mosq,
			  mid,
			  topic,
			  payloadlen, payload,
			  qos,
			  retain);

  return ret == MOSQ_ERR_SUCCESS ? 0 : ret;
}

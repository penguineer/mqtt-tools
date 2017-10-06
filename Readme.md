# MQTT basic tools for Netz39

This is a library to handle 95% of MQTT use-cases in simple client software and provide an easy way to create a program that just sits around and reacts either on/with external events or MQTT messages

## Goals
This project has two general goals:
* Encapsulate calls to the mosquitto (or any other library) to get a simple, but independent way for MQTT setup/subscribe/send/react. This should take care of 95% of all use cases.
* Provide an easy way to setup a program to run as an agent in the background and react to messages on MQTT topics or create messages based on external triggers processed by the software.

## Status
This is a very basic first go. Some parts of the API are working, but it is clearly visible that work needs to be done; in the API and the implementation. Contributions are welcome.

The mqtt-clock.c shows a simple example of a (not yet daemonizied) agent that provides the current time over different MQTT topics. There is no reaction to incoming messages yet.

## Roadmap
* Add configuration handling. MQTT parameters (and others needed for an agent implementation) should be provided either by the command line arguments or via a configuration file. See CMakeLists.txt for prepared inlcusion of popts and libconfig.
* Add subscription handling and callback methods on incoming MQTT messages.
* Provide a way to return to-be-sent messages from the callback and idle handlers. Sending MQTT messages is blocking and should at some point go to a different thread/process.
* Jim™ uses a second thread or process to handle possibly blocking network i/o. Jim is a smart developer. Be like Jim.
* Documentation …

## Building

Development happens with CMake and GCC.
CLANG might work as well, except for some temporary #pragmas to handle temporary situations otherwise leading to compiler warnings.

### with Debian

```
apt-get install libmosquitto-dev
```

Not yet used:
```
apt-get install libconfig-dev
apt-get install libpthread-stubs0-dev
apt-get install libpopt-dev
```

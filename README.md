# MQTT Tools

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)
[![reuse compliant](https://reuse.software/badge/reuse-compliant.svg)](https://reuse.software/)
[![Build Status](https://travis-ci.org/penguineer/mqtt-tools.svg?branch=master)](https://travis-ci.org/penguineer/mqtt-tools)

> A library to simplify agents that react on/with external events or MQTT messages by handling 95% of the MQTT use-cases.

This project has two general goals:
* Encapsulate calls to the mosquitto (or any other library) to get a simple, but independent way for MQTT setup/subscribe/send/react. This should take care of 95% of all use cases.
* Provide an easy way to setup a program to run as an agent in the background and react to messages on MQTT topics or create messages based on external triggers processed by the software.

## Install
Installation is not yet provided.

Development happens with CMake and GCC.
CLANG might work as well, except for some temporary #pragmas to handle temporary situations otherwise leading to compiler warnings.

### with Debian
```
apt-get install libmosquitto-dev
apt-get install libconfig-dev
```

Not yet used:
```
apt-get install libpthread-stubs0-dev
apt-get install libpopt-dev
```

## Usage
The mqtt-clock.c shows a simple example of a (not yet daemonizied) agent that provides the current time over different MQTT topics. There is no reaction to incoming messages yet.

## Status
This is a very basic first go. Some parts of the API are working, but it is clearly visible that work needs to be done; in the API and the implementation. Contributions are welcome.


## Roadmap
* Add subscription handling and callback methods on incoming MQTT messages.
* Provide a way to return to-be-sent messages from the callback and idle handlers. Sending MQTT messages is blocking and should at some point go to a different thread/process.
* Make this a real library with the clock as a usage example.
* Jim™ uses a second thread or process to handle possibly blocking network i/o. Jim is a smart developer. Be like Jim.
* Implement the daemonizied agent part.
* Improve configuration roadmap: Add a nicer API for configuration handling, keeping in mind the varous sources of configuration instances (file, built-in, some web-service, …).
* Documentation …

## Maintainers

This project is mostly, but not necessary solely, maintained by Stefan Haun (<tux@netz39.de>).

Contributions by:
* Alexander Dahl <alex@netz39.de>, known on GitHub as @LeSpocky
* Stefan Haun <tux@netz39.de>, known on GitHub as @penguineer

## Contribute
Contributions are welcome. Please consider the roadmap for your contributions. The maintainers are happy to answer questions.

To contribute, fork the repository and create a feature branch. Please do not add commits on the master branch, as these will not be accepted. Create a PR to have your contribution reviewed and eventually accepted into the code base. Write access to this repository is only granted to maintainers. Any contribution should come from a fork via pull request.

Bugs or wished may be reported with an issue. You can also open an issue to state the intention of a certain contribution. Please mark this as an extension and comment in the issue that you are going to work on this.


## License

This project is licensed under the MIT License.
Instead of quoting the full license text in each source file, this project uses [SPDX license tags](https://spdx.org/).

The [Mosquitto library](https://mosquitto.org/) is dual licensed under the Eclipse Public License 1.0 and the Eclipse Distribution License 1.0 as described in the epl-v10 and edl-v10 files.

See LICENSE.txt for further reading.

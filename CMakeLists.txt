cmake_minimum_required(VERSION 2.8.9)

project (MQTTAGENT C)

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")


# These libraries are noted here for later use
#find_library(CONFIG_LIBRARY NAMES config)
#find_library(PTHREAD_LIBRARY NAMES pthread)
#find_library(POPT_LIBRARY NAMES popt)
find_library(MOSQUITTO_LIBRARY NAMES mosquitto)

add_executable(mqtt-clock
mqtt-clock.c
mosqagent.c
mosqhelper.c
)

#target_link_libraries(mqtt-clock ${CONFIG_LIBRARY})
#target_link_libraries(mqtt-clock ${POPT_LIBRARY})
#target_link_libraries(mqtt-clock ${PTHREAD_LIBRARY})
target_link_libraries(mqtt-clock ${MOSQUITTO_LIBRARY})

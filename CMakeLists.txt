  cmake_minimum_required(VERSION 3.5)

project(httpexperiments)
include_directories(.)

add_executable(http_client
        http_client.c)

add_executable(listener
        listener.c)

add_executable(server
        server.c)

add_executable(talker
        talker.c)



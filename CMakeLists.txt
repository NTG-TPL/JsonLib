cmake_minimum_required(VERSION 3.24)
project(JsonLib)

set(CMAKE_CXX_STANDARD 17)

set(INCLUDE include/json.h
        include/json_builder.h
        include/test_framework.h
        include/tests.h)

set(SRC src/json.cpp
        src/json_builder.cpp
        src/tests.cpp)

add_executable(JsonLib main.cpp ${INCLUDE} ${SRC})

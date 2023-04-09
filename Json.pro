TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/json.cpp \
        src/json_builder.cpp \
        src/tests.cpp \
        main.cpp

HEADERS += \
    include/json.h \
    include/json_builder.h \
    include/test_framework.h \
    include/tests.h

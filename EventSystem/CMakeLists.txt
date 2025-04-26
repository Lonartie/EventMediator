project(EventSystem)

add_library(EventSystem
        System.h
        System.cpp
        Receiver.h
        Event.h
        Receiver.cpp
        Event.cpp
        Policy.cpp
        Policy.h)

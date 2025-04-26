# Event-Mediator

## Synopsis
Event-Mediator is a simple class to demonstrate the use of the mediator pattern
to decouple event handling in a system. It allows different components to communicate
without needing to know about each other directly. It offers different event types, 
categories, and policies to handle events in a flexible way. Policies can be validated
by the mediator to ensure the receiver is _authorized_ to handle the event.

## Disclaimer
This is just a toy project to demonstrate the use of the mediator pattern.
**It is not intended for production use.** It is not a complete implementation and does not
handle all edge cases. It is just a simple example.

## Compilation
You need CMake and doctest installed (doctest can be installed via vcpkg).

```shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Example
Examples can be found in the `Tests/main.cpp` file.

### Create a receiver
```c++
System::instance()
    .onReceiving()
    .ofGroup(System::Data)
    // .ofCategory(System::Any) <-- optional, Any is default
    .withPolicy(System::Secure)
    .withData("SecurityKey", 1234)
    .withAction([&](std::string message) { // <-- can handle multiple data types
       std::cout << message << std::endl;
    })
    .withAction([&](int number) { // <-- can handle multiple data types
       std::cout << number << std::endl;
    })
    .finalize();
```

### Sending an event
```c++
System::Event()
    // .ofGroup(System::Any) <-- optional, Any is default
    .ofCategory(System::Data)
    .withPolicy(System::Secure)
    .withData(std::string("SUCCESS"))
    //.withData(42) <-- can send multiple data types (even at once)
    .send();
```

### Validating a policy
```c++
System::instance()
    .onForwarding(System::Secure)
    .validate("SecurityKey", [](std::string data) {
       return data == "1234";
    })
    .validate("SecurityKey", [](int data) {
       return data == 1234;
    })
    .orCall([] { throw std::runtime_error("Invalid Key!"); })
    // .orSend(Event()) <-- or trigger another event
    .finalize();
```

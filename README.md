# Welcome to the OpenEDOS repository

The Open Source Event Driven Operating System (OpenEDOS) is a modular and extensible operating system designed for microcontrollers. It provides a framework for building event-driven applications with message-based communication between modules, allowing for scalability and extensibility. The provided structures and functions can be used to implement a multitasking operating system or a single-threaded embedded system with modular components. The library is written in C and aims to offer a flexible and efficient environment for developing embedded systems.

## Update v0.2.1

The versioning has been adjusted. The latest version is changed from v2.0.0 to v0.2.0. (With this update the version will be v0.2.1)
With v0.2.1, the OpenEDOS SystemBuilder is introduced. It is a command line tool that allows you to quickly set up an event-based application.

## Key Concepts
- The system lets you define custom requests and register custom handlers for these requests.
- One kernel handles requests in an infinite loop, going to idle mode when no requests need to be handled. 
- This leads to a completely event-driven system flow.
- The system can be used standalone, with one kernel providing one main context.
- When combined with FreeRTOS, the system supports multiple kernels, each running in its own task.
- Kernels register message handlers to handle custom requests.
- The kernel switch routes messages to the correct kernels, facilitating communication.
- Message queues are utilized for inter-module communication.
- The library supports request limits and message handling based on request IDs.

## Work in progress / Coming soon
- Detailed introduction and examples, including build instructions
- Detailed documentation and design guide
- Complete Unit Test coverage
- features, updates and fixes

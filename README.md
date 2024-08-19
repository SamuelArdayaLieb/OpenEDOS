# Welcome to the OpenEDOS repository

The Open Source Event Driven Operating System (OpenEDOS) is a modular and extensible operating system designed for microcontrollers. It provides a framework for building event-driven applications with message-based communication between modules, allowing for scalability and extensibility. The provided structures and functions can be used to implement a multitasking operating system or a single-threaded embedded system with modular components. The library is written in C and aims to offer a flexible and efficient environment for developing embedded systems.

## The OpenEDOS Systembuilder
The OpenEDOS Systembuilder is a command line interface (CLI) built with python click. It accelerates the development
of firmware projects through automated code generation. 
### Autocompletion
If you want autocompletion for the systembuilder commands to work,
you can copy the script .openedos-complete.bash to your home directory (~/) and add the line  
```
. ~/.openedos-complete.bash
```
to .bashrc.

### Setting it up
Place the the file openedos-2.0.0.tar.gz in a directory. If you want to build the tarball yourself,
clone the repository, step inside and use 
```
python3 -m venv venv
. venv/bin/activate
pip install build
python3 -m build .
```
openedos-2.0.0.tar.gz should appear in dist/. 

Now you have openedos-2.0.0.tar.gz in a directory. When you are in that directory, use:
```
python3 -m venv venv
. venv/bin/activate
pip install openedos-2.0.0.tar.gz
```  

This installs OpenEDOS in the venv you created.
OpenEDOS is, at this point, a Command Line Interface (CLI) which you can use at will.
To see a list of the provided commands just type:
```
openedos
```  

## The OpenEDOS Core

The OpenEDOS Core (OE Core) is a small C library that implements the message-based event handling for a 
microcontroller (uC) firmware. Its only dependencies are <stddef.h>, <stdint.h> and <stdbool.h>. This
means that it runs not only on uCs but on basically any system (e.g. Linux). The OE Core itself should be independent of any application. Application specific configurations however can be made in the OpenEDOS
Config (OE Config). This is also the place for specific uC ports.

## Work in progress / Coming soon
- Detailed introduction and examples, including build instructions
- Detailed documentation and design guide
- Complete Unit Test coverage
- features, updates and fixes

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
## Example 1: Hello World on Linux

### Introduction
In this example a simple Hello World programm running on Linux is shown. OpenEDOS is a system which targets microcontroller platforms. But as it is written in C using just the standard library, it is easily compilable for Linux aswell.

### Prerequisites
Only CMake needs to be available for the given build process.
```
sudo apt install cmake
```
Check if CMake is installed.
```
cmake --version
```

### Requirements for the example software
Any software development process should start with obtaining software requirements.
The requirements for this example project are the following:
- The software shall print 'Hello World' to the terminal periodically
- Printing 'Hello World' shall happen once every two seconds

### Defining software modules
For this example, two modules are defined according to the requirements:
- The Printer: This module is responsible for the IO interaction, i.e. printing 'Hello World' to the terminal.
- The Timer: This module is responsible for providing a correct time basis, i.e. yielding a timer tick every two seconds.

### Defining the module behaviour and API
The behaviour of the modules needs to be described in more detail. In this step, the API of each module is defined.
The API of an OpenEDOS module is stored in a _config.yaml_ and consists of two parts:
1. The requests which are defined by the module
2. The requests which are subscribed and used by the module

In a project, the configs of the modules (Printer and Timer) are placed in their folders after code generation. 
If you place a valid config in the Modules/ folder and run openedos update-project, a subdirectoriy including the source code of the module is created in Modules/. 

### Implementing the modules
The source code of the modules is found in ```Modules/Printer/``` and ```Modules/Timer/```. 

### Build and run
In the project directory, we build the application by running:  

```mkdir build && cd build```  
```cmake ..```  
```make```  
```./Hello_World```  

#### Additional thoughts
One of the key concepts of OpenEDOS (and in software engineering generally) is to split the software into modules and assigning individual jobs to those modules (the principle of single responsibilities). Deriving a suitable modularisation from the requirements is task of the developer, and this can often be done in multiple, equally correct ways. 

Requests are events in an OpenEDOS application. They are transported in request messages. This means that when a module creates a request, it actually sends a message containing the corresponding request ID to the kernel. Modules may subscribe to a request by registering a handler (which is a function pointer). The handler is called whenever the request occures. This core mechanic is what makes OpenEDOS event driven.  

The 'API first' approach is encouraged when using OpenEDOS. The reason for this is that the code generation of modules is based on their API description. The 'API first' approach also leads to a good understanding of all the procedures in the firmware even before the first line of code is written. 

Obviously, the requirements for this example project can be implemented much simpler. This project just showcases the framework. 
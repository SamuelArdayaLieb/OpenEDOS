# This is a basic CMakeLists.txt that may be adjusted as needed.

cmake_minimum_required(VERSION 3.10)

#------------------------------------------ set project name

set(PROJECT_NAME "Example")
project(${PROJECT_NAME} VERSION 1.0)

#------------------------------------------ set absolute paths

set(CORE_PATH "${PROJECT_SOURCE_DIR}/Modules/OE_Core")
set(CONFIG_PATH "${PROJECT_SOURCE_DIR}/OE_Config")
set(MODULE_PATH "${PROJECT_SOURCE_DIR}/Modules")
set(EXEC_FILE "${PROJECT_SOURCE_DIR}/main.c")

#------------------------------------------ add executable

add_executable(${PROJECT_NAME} 
${EXEC_FILE} 
)

#------------------------------------------ add the core library "OpenEDOS" 
#------------------------------------------ and include the config

add_subdirectory(${CORE_PATH} "${PROJECT_SOURCE_DIR}/build/OE_Core")
target_include_directories(OpenEDOS PUBLIC 
"${CONFIG_PATH}"
)

#------------------------------------------ add all modules as library "Modules" 
#------------------------------------------ and include OpenEDOS Core and Config

add_subdirectory(${MODULE_PATH} "${PROJECT_SOURCE_DIR}/build/Modules")
target_include_directories(Modules PUBLIC
"${CORE_PATH}"
"${CONFIG_PATH}"
)

#------------------------------------------ link against the Modules library 

target_link_libraries(${PROJECT_NAME} PRIVATE 
OpenEDOS
Modules
)

#------------------------------------------ Link against the pthread library
#------------------------------------------ (can be used for Linux)

# find_package(Threads REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
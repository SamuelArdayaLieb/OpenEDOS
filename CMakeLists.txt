# This is a basic CMakeLists.txt that may be adjusted as needed.

cmake_minimum_required(VERSION 3.10)

#------------------------------------------ set project name

set(PROJECT_NAME "Example")
project(${PROJECT_NAME} VERSION 1.0)

#------------------------------------------ set absolute paths

set(CONFIG_PATH "${PROJECT_SOURCE_DIR}/OE_Config")
set(MODULE_PATH "${PROJECT_SOURCE_DIR}/Modules")
set(EXEC_FILE "${PROJECT_SOURCE_DIR}/main.c")

#------------------------------------------ add executable

add_executable(${PROJECT_NAME} 
${EXEC_FILE} 
)

#------------------------------------------ add all modules as library "Modules" 
#------------------------------------------ and include OpenEDOS Config

add_subdirectory(${MODULE_PATH} "${PROJECT_SOURCE_DIR}/build/Modules")
target_include_directories(Modules PUBLIC
"${CONFIG_PATH}"
)

#------------------------------------------ link library "Modules" 
#------------------------------------------ with the executable

target_link_libraries(${PROJECT_NAME} PRIVATE 
Modules
)
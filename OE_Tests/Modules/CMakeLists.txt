add_library(Modules)

# include all modules and add sources
FILE(GLOB modules RELATIVE ${MODULE_PATH} ${MODULE_PATH}/*)
set(modules_sources "")  # Helper list to keep track of source files

FOREACH(module ${modules})
    IF(IS_DIRECTORY ${MODULE_PATH}/${module})
        FILE(GLOB source "${MODULE_PATH}/${module}/*.c")
        list(APPEND modules_sources ${source})
        target_sources(Modules PRIVATE ${source})
        target_include_directories(Modules PUBLIC ${MODULE_PATH}/${module})
    ENDIF()
ENDFOREACH()

# Add a dummy source file if no module source files are found
if(NOT modules_sources)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/null.c" "")
    target_sources(Modules PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/null.c")
endif()
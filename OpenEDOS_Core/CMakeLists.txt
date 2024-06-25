add_library(OpenEDOS 
    kernel/request_map.c
    kernel/kernel_switch.c
    kernel/kernel.c
    kernel/message_queue.c
    system_requests/system_requests_intf.c
)

# include the kernel
target_include_directories(OpenEDOS PUBLIC
kernel/include
)
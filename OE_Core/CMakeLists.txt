add_library(OpenEDOS 
    oe_core_intf.c
    oe_core_mod.c
    oe_kernel.c
    oe_message_queue.c
    oe_request_map.c
)

# include the kernel
target_include_directories(OpenEDOS PUBLIC
.
)
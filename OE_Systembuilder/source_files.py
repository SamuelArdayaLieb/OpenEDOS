"""
OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

from typing import List, Dict

from .message_handlers import Request, RequestHandler, ResponseHandler
from .user_code import UserCode
from . import utils


class File:
    def __init__(
        self,
        filename: str,
        author: str,
        version: str,
        copyright_notice: str,
        user_codes: Dict[str, UserCode] = {},
    ) -> None:
        self.filename = filename
        self.author = author
        self.version = version
        try:
            if copyright_notice[-1] != "\n":
                copyright_notice += "\n"
        except:
            copyright_notice = "\n"
        finally:
            self.copyright_notice = copyright_notice

        id = "COPYRIGHT NOTICE"
        if id in user_codes:
            self.intro = user_codes[id]
        else:
            code = f"@version {self.version}\n"
            if self.copyright_notice != "\n":
                code += "\n"
                code += self.copyright_notice

            self.intro = UserCode(identifier=id, code=utils.text_to_comment(code))

        id = "FILE INTRODUCTION"
        self.file_header = (
            user_codes[id] if id in user_codes else UserCode(identifier=id)
        )

        self.includes: List[str] = []
        self.sections: List[str] = []

    def _includes(self) -> str:
        text = ""
        for include in self.includes:
            text += f"#include {include}\n"
        text += "\n"
        return text

    def _introduction(self) -> str:
        text = f"""@note This file was autogenerated with OpenEDOS v{utils.OPENEDOS_VERSION}.
Sections inside USER CODE BEGIN and USER CODE END will be left untouched 
when rerunning the code generation. Happy coding!

@file {self.filename}
@author {self.author}\n"""
        text = utils.text_to_comment(text)
        text += "\n"
        return text

    def get_text(self) -> str:
        text = self._introduction()
        text += self.intro.get_text()
        text += "\n"
        text += self.file_header.get_text()
        text += "\n"
        for section in self.sections:
            text += section
        return text


class InterfaceHeader(File):
    def __init__(
        self,
        name: str,
        author: str,
        version: str,
        copyright_notice: str,
        requests: Dict[str, Request] = {},
        user_codes: Dict[str, UserCode] = {},
    ) -> None:
        filename = utils.name_to_filename(f"{name}_intf.h")
        super().__init__(
            filename=filename,
            author=author,
            version=version,
            copyright_notice=copyright_notice,
            user_codes=user_codes,
        )

        self.name = name
        self.requests = requests

        self.includes = ['"oe_defines.h"']
        id = "INTERFACE HEADER"
        self.user_code = user_codes[id] if id in user_codes else UserCode(identifier=id)

    def _guard_top(self) -> str:
        text = f"#ifndef {self.filename.replace('.', '_').upper()}\n"
        text += f"#define {self.filename.replace('.', '_').upper()}\n\n"
        return text

    def _guard_bot(self) -> str:
        text = f"#endif // {self.filename.replace('.', '_').upper()}"
        return text

    def _requests(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Requests ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//\n\n"
        if len(self.requests) > 0:
            for request in self.requests.values():
                text += request.get_header_text()
        else:
            text += "/* There are no requests associated with this module. */\n\n"
        return text

    def get_text(self) -> str:
        self.sections.append(self._guard_top())
        self.sections.append(self._includes())
        self.sections.append("/* Includes, defines, typedefs, etc. */\n")
        self.sections.append(f"{self.user_code.get_text()}\n")
        self.sections.append(self._requests())
        self.sections.append(self._guard_bot())
        return super().get_text()


class InterfaceSource(File):
    def __init__(
        self,
        name: str,
        author: str,
        version: str,
        copyright_notice: str,
        requests: Dict[str, Request] = {},
        user_codes: Dict[str, UserCode] = {},
    ) -> None:
        filename = utils.name_to_filename(f"{name}_intf.c")
        super().__init__(
            filename=filename,
            author=author,
            version=version,
            copyright_notice=copyright_notice,
            user_codes=user_codes,
        )

        self.name = name
        self.requests = requests

        header_name = utils.name_to_filename(f"{name}_intf.h")
        self.includes = [f'"{header_name}"', '"oe_core_mod.h"']
        id = "INTERFACE GLOBALS"
        self.user_code_globals = (
            user_codes[id] if id in user_codes else UserCode(identifier=id)
        )
        id = "INTERFACE SOURCE"
        self.user_code = user_codes[id] if id in user_codes else UserCode(identifier=id)

    def _user_includes(self) -> str:
        text = "/* Includes, typedefs, globals, etc. */\n"
        text += self.user_code_globals.get_text()
        text += "\n"
        return text

    def _requests(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Requests ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//\n\n"

        if len(self.requests) > 0:
            for request in self.requests.values():
                text += request.get_source_text()
        else:
            text += "/* There are no requests associated with this interface. */\n\n"

        return text

    def get_text(self) -> str:
        self.sections.append(self._includes())
        self.sections.append(self._user_includes())
        self.sections.append(self._requests())
        self.sections.append("\n/* Something else...? */\n")
        self.sections.append(self.user_code.get_text())
        return super().get_text()


class ModuleHeader(File):
    def __init__(
        self,
        name: str,
        author: str,
        version: str,
        copyright_notice: str,
        user_codes: Dict[str, UserCode] = {},
    ) -> None:
        filename = utils.name_to_filename(f"{name}_mod.h")
        super().__init__(
            filename=filename,
            author=author,
            version=version,
            copyright_notice=copyright_notice,
            user_codes=user_codes,
        )

        self.name = name

        id = "MODULE GLOBALS"
        self.user_code_globals = (
            user_codes[id] if id in user_codes else UserCode(identifier=id)
        )
        id = "MODULE DATA"
        self.user_code_module_data = (
            user_codes[id] if id in user_codes else UserCode(identifier=id)
        )
        self.user_code_module_data.indents = 1
        id = "MODULE PROTOTYPES"
        self.user_code_prototypes = (
            user_codes[id] if id in user_codes else UserCode(identifier=id)
        )

        self.includes = ['"oe_defines.h"']

    def _guard_top(self) -> str:
        text = f"#ifndef {self.filename.replace('.', '_').upper()}\n"
        text += f"#define {self.filename.replace('.', '_').upper()}\n\n"
        return text

    def _guard_bot(self) -> str:
        text = f"#endif // {self.filename.replace('.', '_').upper()}"
        return text

    def _user_includes(self) -> str:
        text = "/* Includes, typedefs, globals, etc. */\n"
        text += self.user_code_globals.get_text()
        text += "\n"
        return text

    def _module_definition(self) -> str:
        text = f"""//~~~~~~~~~~~~~~~~~~~~~~~~ Module definition ~~~~~~~~~~~~~~~~~~~~~~~~//

typedef struct module_{self.name}_s {'{'}
    /* The connection to the kernel. */
    OE_Kernel_t *Kernel;

    /* Module data. */
{self.user_code_module_data.get_text()}
{'}'} module_{self.name}_t;\n\n"""
        return text

    def _init_prototype(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~~~~~ Init prototype ~~~~~~~~~~~~~~~~~~~~~~~~~//\n\n"
        comment = f"""@brief Initialize the module {self.name}.

This function registers the request handlers of the module
and connects the module to the kernel. It then calls 
the specific init function of the module.

@param {self.name} A pointer to the module to be initialized.

@param Args A pointer to the init params for the module.

@param Kernel A pointer to the kernel to be connected.

@return OE_Error_t An error is returned if
- initializing the module results in an error.
Otherwise OE_ERROR_NONE is returned.\n"""
        text += utils.text_to_comment(comment)
        text += f"""OE_Error_t initModule_{self.name}(
    module_{self.name}_t *{self.name},
    void *Args,
    OE_Kernel_t *Kernel);\n\n"""
        return text

    def _user_prototypes(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~~~~~ User prototypes ~~~~~~~~~~~~~~~~~~~~~~~~//\n\n"
        text += self.user_code_prototypes.get_text()
        text += "\n"
        return text

    def get_text(self) -> str:
        self.sections.append(self._guard_top())
        self.sections.append(self._includes())
        self.sections.append(self._user_includes())
        self.sections.append(self._module_definition())
        self.sections.append(self._init_prototype())
        self.sections.append(self._user_prototypes())
        self.sections.append(self._guard_bot())
        return super().get_text()


class ModuleSource(File):
    def __init__(
        self,
        name: str,
        author: str,
        version: str,
        copyright_notice: str,
        request_handlers: Dict[str, RequestHandler] = {},
        response_handlers: Dict[str, ResponseHandler] = {},
        user_codes: Dict[str, UserCode] = {},
    ) -> None:
        filename = utils.name_to_filename(f"{name}_mod.c")
        super().__init__(
            filename=filename,
            author=author,
            version=version,
            copyright_notice=copyright_notice,
            user_codes=user_codes,
        )

        self.name = name
        id = "MODULE GLOBALS"
        self.user_code_globals = (
            user_codes[id] if id in user_codes else UserCode(identifier=id)
        )
        id = "MODULE INIT"
        self.user_code_init = (
            user_codes[id] if id in user_codes else UserCode(identifier=id)
        )
        self.user_code_init.indents = 1
        id = "MODULE FUNCTIONS"
        self.user_code_funcs = (
            user_codes[id] if id in user_codes else UserCode(identifier=id)
        )
        self.request_handlers = request_handlers
        self.response_handlers = response_handlers

        headername = utils.name_to_filename(f"{name}_mod.h")
        self.includes = [f'"{headername}"', '"oe_kernel.h"']

    def _user_includes(self) -> str:
        text = "/* Includes, prototypes, globals, etc. */\n"
        text += self.user_code_globals.get_text()
        text += "\n"
        return text

    def _global_pointer(self) -> str:
        text = "/* Global pointer to the module. */\n"
        text += f"static module_{self.name}_t *{self.name};\n\n"
        return text

    def _init_prototype(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~~~ Custom init prototype ~~~~~~~~~~~~~~~~~~~~~~~~//\n\n"
        comment = f"@brief Custom initializer for the module: {self.name}.\n\n"
        if self.user_code_init.description != "\n":
            comment += self.user_code_init.description + "\n"
        comment += "@param Args A pointer to the init params for the module.\n\n"
        comment += "@return OE_Error_t An error is returned if\n"
        comment += "- initializing the module results in an error.\n"
        comment += "Otherwise OE_ERROR_NONE is returned.\n"
        text += utils.text_to_comment(comment)
        text += f"static inline OE_Error_t init_{self.name}(void *Args);\n\n"
        return text

    def _request_handler_prototypes(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~ Request handler prototypes ~~~~~~~~~~~~~~~~~~~~~//\n\n"
        if len(self.request_handlers) == 0:
            text += "/* This module does not implement any request handlers. */\n\n"
            return text
        for handler in self.request_handlers.values():
            text += handler.get_prototype()
        return text

    def _respone_handler_prototypes(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~ Response handler prototypes ~~~~~~~~~~~~~~~~~~~~~//\n\n"
        if len(self.response_handlers) == 0:
            text += "/* This module does not implement any response handlers. */\n\n"
            return text
        for handler in self.response_handlers.values():
            text += handler.get_prototype()
        return text

    def _module_init(self) -> str:
        text = f"""//~~~~~~~~~~~~~~~~~~~~~~~~ Module initialization ~~~~~~~~~~~~~~~~~~~~~~~~//

/* Initialize the module and register handlers. */
OE_Error_t initModule_{self.name}(
    module_{self.name}_t *p{self.name},
    void *Args,
    OE_Kernel_t *Kernel)
{'{'}
    OE_Error_t Error;
    
    /* List the requests this module will handle. */
    OE_RequestID_t RequestIDs[] = {'{'}\n"""
        for handler in self.request_handlers.values():
            text += f"{utils.INDENT*2}{handler.RID},\n"
        text += "\t};"
        text += f"""

    /* List the request handlers accordingly. */
    OE_MessageHandler_t RequestHandlers[] = {'{'}\n"""
        for handler in self.request_handlers.values():
            text += f"{utils.INDENT*2}(OE_MessageHandler_t){handler.func_name},\n"
        text += "\t};"
        text += f"""

    /* Setup the module connections. */
    {self.name} = p{self.name};
    {self.name}->Kernel = Kernel;

    /* Initialize the module. */
    Error = init_{self.name}(Args);

    /* Check for errors. */
    if (Error != OE_ERROR_NONE)
    {'{'}
        {self.name}->Kernel = NULL;
        {self.name} = NULL;
        
        return Error;
    {'}'}

    /* Register the request handlers. */
    Error = OE_Kernel_registerHandlers(
        Kernel,
        RequestIDs,
        RequestHandlers,
        sizeof(RequestIDs)/sizeof(OE_RequestID_t));

    /* Check for errors. */
    if (Error != OE_ERROR_NONE)
    {'{'}
        /* Unregister handlers if an error occured. */
        OE_Kernel_unregisterHandlers(
            Kernel,
            RequestIDs,
            RequestHandlers,
            sizeof(RequestIDs)/sizeof(OE_RequestID_t));
        
        {self.name}->Kernel = NULL;
        {self.name} = NULL;  

        return Error; 
    {'}'}

    /* Nice, we're done here. */
    return OE_ERROR_NONE;
{'}'}\n\n"""
        return text

    def _custom_init(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~~~~ Custom init function ~~~~~~~~~~~~~~~~~~~~~~~~//\n\n"
        text += f"OE_Error_t init_{self.name}(void *Args)\n"
        text += "{\n"
        if self.user_code_init.code == "\n":
            self.user_code_init.code = "\t/* Avoid unused warning. */\n"
            self.user_code_init.code += "\t(void)Args;\n"
            self.user_code_init.code += (
                f"\n\n\t/* Return no error if everything is fine. */\n"
            )
            self.user_code_init.code += "\treturn OE_ERROR_NONE;\n"
        text += self.user_code_init.get_text()
        text += "}\n\n"
        return text

    def _requests(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~~~~~ Request handlers ~~~~~~~~~~~~~~~~~~~~~~~~~~~//\n\n"
        if len(self.request_handlers) > 0:
            for handler in self.request_handlers.values():
                text += f"{handler.get_body()}"
        else:
            text += "/* This module does not implement any request handlers. */\n\n"
        return text

    def _responses(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~~~~~ Response handlers ~~~~~~~~~~~~~~~~~~~~~~~~~~//\n\n"
        if len(self.response_handlers) > 0:
            for handler in self.response_handlers.values():
                text += f"{handler.get_body()}\n"
        else:
            text += "/* This module does not implement any response handlers. */\n\n"
        return text

    def _user_funcs(self) -> str:
        text = "//~~~~~~~~~~~~~~~~~~~~~~~~~~~ User functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~//\n\n"
        text += self.user_code_funcs.get_text()
        text += "\n"
        return text

    def get_text(self) -> str:
        self.sections.append(self._includes())
        self.sections.append(self._user_includes())
        self.sections.append(self._global_pointer())
        self.sections.append(self._init_prototype())
        self.sections.append(self._request_handler_prototypes())
        self.sections.append(self._respone_handler_prototypes())
        self.sections.append(self._module_init())
        self.sections.append(self._custom_init())
        self.sections.append(self._requests())
        self.sections.append(self._responses())
        self.sections.append(self._user_funcs())
        return super().get_text()


class RequestsHeader(File):
    def __init__(
        self,
        author: str,
        version: str,
        copyright_notice: str,
        request_ids: Dict[str, List[str]] = {},
        user_codes: Dict[str, UserCode] = {},
    ) -> None:
        filename = "oe_requests.h"

        super().__init__(
            filename=filename,
            author=author,
            version=version,
            copyright_notice=copyright_notice,
            user_codes=user_codes,
        )

        self.request_ids = request_ids

        self.includes = ['"oe_config.h"']

    def _guard_top(self) -> str:
        text = f"#ifndef {self.filename.replace('.', '_').upper()}\n"
        text += f"#define {self.filename.replace('.', '_').upper()}\n\n"
        return text

    def _file_description(self) -> str:
        return """/**
 * @brief This file is used to create a unique ID for each request that exists in the
 * system. In order to create the ID, the request is listed in the following enum.
 */\n\n"""

    def _request_ids(self) -> str:
        text = f"""/* Define all requests by giving them a request ID. */
typedef enum OE_RequestID_e
{'{'}
#if OE_USE_SYSTEM_REQUESTS
    /* OpenEDOS Core */
    RID_Kernel_Start,
#endif\n\n"""
        for interface_name, ids in self.request_ids.items():
            # Skip system requests. They are added manually.
            if interface_name == "OE_Core":
                continue
            text += f"\t/* {interface_name} */\n"
            for id in ids:
                text += f"\t{id},\n"
            text += "\n"
        text += f"""\t/* The LAST element in this enum MUST be "OE_NUMBER_OF_REQESTS"! */
    OE_NUMBER_OF_REQUESTS
{'}'} OE_RequestID_t;\n\n"""
        return text

    def _guard_bot(self) -> str:
        text = f"#endif // {self.filename.replace('.', '_').upper()}"
        return text

    def get_text(self) -> str:
        self.sections.append(self._guard_top())
        self.sections.append(self._file_description())
        self.sections.append(self._request_ids())
        self.sections.append(self._guard_bot())
        return super().get_text()


class AllModules(File):
    def __init__(
        self,
        author: str,
        version: str,
        copyright_notice: str,
        module_headers: List[ModuleHeader],
        user_codes: Dict[str, UserCode] = {},
    ) -> None:
        filename = "all_modules.h"
        super().__init__(
            filename=filename,
            author=author,
            version=version,
            copyright_notice=copyright_notice,
            user_codes=user_codes,
        )

        self.includes = [f'"{header.filename}"' for header in module_headers]

    def _guard_top(self) -> str:
        text = f"#ifndef {self.filename.replace('.', '_').upper()}\n"
        text += f"#define {self.filename.replace('.', '_').upper()}\n\n"
        return text

    def _guard_bot(self) -> str:
        text = f"#endif // {self.filename.replace('.', '_').upper()}"
        return text

    def get_text(self) -> str:
        self.sections.append(self._guard_top())
        self.sections.append(self._includes())
        self.sections.append(self._guard_bot())
        return super().get_text()


class KernelThread:
    def __init__(
        self,
        kernel_id: int,
        module_names: List[str] = [],
        user_codes: Dict[str, UserCode] = {},
    ) -> None:
        self.kernel_id = kernel_id
        self.module_names = module_names

        id = f"KERNEL {self.kernel_id} INIT"
        if id in user_codes:
            self.user_code_init = user_codes[id]
            self.user_code_init.indents = 1
        elif (id in user_codes and user_codes[id].code == "\n") or id not in user_codes:
            code = "\tOE_Error_t Error;\n"
            code += "\tvoid *ModuleArgs;\n"
            code += f"\tOE_Kernel_t Kernel_{self.kernel_id};\n\n"
            for module_name in self.module_names:
                code += f"\tmodule_{module_name}_t {module_name};\n"
            code += "\n\t/* Avoid unused warning. */\n\t(void)Args;\n\n"
            code += "\t/* Initialize kernel. */\n"
            code += f"\tError = OE_Kernel_staticInit(&Kernel_{self.kernel_id});\n"
            code += """\tif (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return;
    }\n\n"""
            code += "\t/* Initialize all modules. */\n"
            for module_name in self.module_names:
                code += f"""\tModuleArgs = NULL;
    Error = initModule_{module_name}(
        &{module_name},
        ModuleArgs,
        &Kernel_{self.kernel_id});
    if (Error != OE_ERROR_NONE)
    {'{'}
        /* Error handling or debugging... */
        return;
    {'}'}\n\n"""
            self.user_code_init = UserCode(identifier=id, code=code, indents=1)

        id = f"KERNEL {self.kernel_id} RUN"
        if id in user_codes:
            self.user_code_run = user_codes[id]
            self.user_code_run.indents = 1
        elif (id in user_codes and user_codes[id].code == "\n") or id not in user_codes:
            code = "\t/* Enter kernel main routine. */\n"
            code += f"\tOE_Kernel_run(&Kernel_{self.kernel_id});\n"
            self.user_code_run = UserCode(identifier=id, code=code, indents=1)

    def get_prototype(self) -> str:
        return f"void Kernel_{self.kernel_id}_thread(void *Args);\n"

    def get_body(self) -> str:
        text = f"void Kernel_{self.kernel_id}_thread(void *Args)\n"
        text += "{\n"
        text += self.user_code_init.get_text()
        text += self.user_code_run.get_text()
        text += "}\n\n"
        return text


class MainFile(File):
    def __init__(
        self,
        author: str,
        version: str,
        copyright_notice: str,
        user_codes: Dict[str, UserCode] = {},
        modules: Dict[int, List[str]] = {},
    ) -> None:
        filename = "main.c"

        super().__init__(filename, author, version, copyright_notice, user_codes)

        self.includes = ['"oe_core_mod.h"', '"oe_kernel.h"', '"all_modules.h"']

        self.kernel_threads: List[KernelThread] = []
        for kernel_id, module_names in modules.items():
            self.kernel_threads.append(
                KernelThread(
                    kernel_id=kernel_id,
                    module_names=module_names,
                    user_codes=user_codes,
                )
            )

        id = "MAIN GLOBALS"
        self.user_code_globals = user_codes[id] if id in user_codes else UserCode(id)
        id = "MAIN SOURCE"
        self.user_code_source = user_codes[id] if id in user_codes else UserCode(id)

        id = "MAIN FUNCTION"
        if id in user_codes:
            self.user_code_main = user_codes[id]
            self.user_code_main.indents = 1
        elif (id in user_codes and user_codes[id].code == "\n") or id not in user_codes:
            code = "\tinitModule_OE_Core(&OE_Core, NULL, NULL);\n\n"
            if len(self.kernel_threads) == 1:
                code += "\t/* Call the one thread function (no multi threading). */\n"
                code += "\tKernel_0_thread(NULL);\n"
            else:
                code += "\t/* Create threads. */\n\n"
            self.user_code_main = UserCode(identifier=id, code=code, indents=1)

    def _user_includes(self) -> str:
        text = "/* Includes, prototypes, globals, etc. */\n"
        text += self.user_code_globals.get_text()
        text += "\n"
        return text

    def _kernel_threads_prototypes(self) -> str:
        text = "/* Kernel threads. */\n"
        for kernel_thread in self.kernel_threads:
            text += kernel_thread.get_prototype()
        text += "\n"
        return text

    def _oe_core(self) -> str:
        return "static OE_Core_t OE_Core;\n\n"

    def _main_function(self) -> str:
        text = "int main(void)\n{\n"
        text += self.user_code_main.get_text()
        text += "\n\t/* Never reached... */\n"
        text += "\treturn 0;\n}\n\n"
        return text

    def _kernel_threads(self) -> str:
        text = ""
        for kernel_thread in self.kernel_threads:
            text += kernel_thread.get_body()
        return text

    def get_text(self) -> str:
        self.sections.append(self._includes())
        self.sections.append(self._user_includes())
        self.sections.append(self._kernel_threads_prototypes())
        self.sections.append(self._oe_core())
        self.sections.append(self._main_function())
        self.sections.append(self._kernel_threads())
        self.sections.append(self.user_code_source.get_text())
        return super().get_text()

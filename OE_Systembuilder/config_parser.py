"""
OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

import os
import logging
from typing import Dict

from .module import Module
from .interface import Interface
from .user_code import CodeParser
from .message_handlers import Request, Parameter, RequestHandler, ResponseHandler
from . import utils


class ConfigParser:
    def __init__(
        self,
        config: dict,
        project_config: dict,
        path_to_folder: str,
        config_filename: str,
    ) -> None:
        self.config: dict = config
        self.project_config: dict = project_config
        self.path_to_folder: str = path_to_folder
        self.config_filename: str = config_filename
        self.path_to_config: str = os.path.join(path_to_folder, config_filename)

        self.has_module_header: bool = False
        self.has_module_source: bool = False
        self.has_interface_header: bool = False
        self.has_interface_source: bool = False

        self.interface: Interface = None
        self.module: Module = None

        self.requests: Dict[str, Request] = {}
        self.request_handlers: Dict[str, RequestHandler] = {}
        self.response_handlers: Dict[str, ResponseHandler] = {}
        self.subscribed_requests: Dict[str, Request] = {}
        self.used_requests: Dict[str, Request] = {}

    def check_config(self) -> None:
        logging.debug(f"Checking config at {self.path_to_config}...")
        if "name" not in self.config:
            raise Exception(f"Config at {self.path_to_config}:\n'name' not specified!")
        self.name: str = self.config["name"]
        if "author" not in self.config:
            logging.debug(
                f"Config '{self.name}': 'author' not specified. Setting to 'Anonymous'."
            )
            self.config["author"] = "Anonymous"
        if "interface" not in self.config:
            logging.debug(f"Config '{self.name}': 'interface' not specified!")
            self.config["interface"] = {"create": False, "requests": None}
        self.interface_config = self.config["interface"]
        if "create" not in self.interface_config:
            logging.debug(
                f"Config '{self.name}': 'interface': 'create' not specified. Setting to False."
            )
            self.interface_config["create"] = False
        if "requests" not in self.interface_config:
            logging.debug(
                f"Config '{self.name}': 'interface': 'requests' not specified. Setting to None."
            )
            self.interface_config["requests"] = None
        if "module" not in self.config:
            logging.debug(f"Config '{self.name}': 'module' not specified!")
            self.config["module"] = {
                "create": False,
                "kernel": 0,
                "subscribed requests": None,
                "used requests": None,
            }
        self.module_config = self.config["module"]
        if "create" not in self.module_config:
            logging.debug(
                f"Config '{self.name}': 'module': 'create' not specified. Setting to False."
            )
            self.module_config["create"] = False
        if "kernel" not in self.module_config:
            logging.debug(
                f"Config '{self.name}': 'module': 'kernel' not specified. Setting to 0."
            )
            self.module_config["kernel"] = 0
        if "subscribed requests" not in self.module_config:
            logging.debug(
                f"Config '{self.name}': 'module': 'subscribed requests' not specified. Setting to None."
            )
            self.module_config["subscribed requests"] = None
        if "used requests" not in self.module_config:
            logging.debug(
                f"Config '{self.name}': 'module': 'subscribed requests' not specified. Setting to None."
            )
            self.module_config["used requests"] = None

    def parse_config(self) -> int:
        ret = 0
        self.check_config()

        logging.debug(f"Parsing config '{self.name}'...")

        if self.path_to_folder.endswith(
            f"/{self.name}"
        ) or self.path_to_folder.endswith(f"\{self.name}"):
            self.has_folder = True
            logging.debug(f"Config '{self.name}': Using existing module folder.")
        else:
            self.has_folder = False
            self.path_to_folder = os.path.join(self.path_to_folder, self.name)
            logging.debug(f"Config '{self.name}': Creating new module folder...")

        filename = utils.name_to_filename(self.name)
        self.path_to_module_header = os.path.join(
            self.path_to_folder, f"{filename}_mod.h"
        )
        self.path_to_module_source = os.path.join(
            self.path_to_folder, f"{filename}_mod.c"
        )
        self.path_to_interface_header = os.path.join(
            self.path_to_folder, f"{filename}_intf.h"
        )
        self.path_to_interface_source = os.path.join(
            self.path_to_folder, f"{filename}_intf.c"
        )

        self.look_for_files()

        ret += self.create_requests()

        self.create_interface()

        return ret

    def look_for_files(self):
        debug = f"Config '{self.name}': Looking for existing source code:\n"
        debug += "Module header: "
        if os.path.isfile(self.path_to_module_header):
            self.has_module_header = True
            debug += f"Found file {self.path_to_module_header}.\n"
        else:
            debug += "No file found.\n"
        debug += "Module source: "
        if os.path.isfile(self.path_to_module_source):
            self.has_module_source = True
            debug += f"Found file {self.path_to_module_source}.\n"
        else:
            debug += "No file found.\n"
        debug += "Interface header: "
        if os.path.isfile(self.path_to_interface_header):
            self.has_interface_header = True
            debug += f"Found file {self.path_to_interface_header}.\n"
        else:
            debug += "No file found.\n"
        debug += "Interface source: "
        if os.path.isfile(self.path_to_interface_source):
            self.has_interface_source = True
            debug += f"Found file {self.path_to_interface_source}."
        else:
            debug += "No file found."
        logging.debug(debug)

    def create_requests(self) -> int:
        logging.debug(f"Config '{self.name}': Creating requests...")
        ret = 0
        request_configs = self.interface_config["requests"]
        if request_configs is not None:
            for request_config in request_configs:
                name = request_config["name"]
                if name in self.requests:
                    logging.error(
                        f"Config '{self.name}': Multiple definitions of request '{name}'!"
                    )
                    ret += 1
                    continue
                request_parameters: Dict[str, Parameter] = {}
                response_parameters: Dict[str, Parameter] = {}
                logging.debug(f"Config '{self.name}': Creating request '{name}'...")
                request_parameter_configs = request_config["request parameters"]
                if request_parameter_configs is not None:
                    for request_parameter_config in request_parameter_configs:
                        request_parameter_name = request_parameter_config["name"]
                        request_parameter = Parameter(
                            name=request_parameter_name,
                            type=request_parameter_config["type"],
                            description=request_parameter_config["description"],
                        )
                        if request_parameter_name in request_parameters:
                            logging.error(
                                f"Config '{self.name}': Request '{name}':\n"
                                f"Multiple definitions of request parameter '{request_parameter_name}'!"
                            )
                            ret += 1
                            continue
                        debug = f"Config '{self.name}': Request '{name}': Adding request parameter with:\n"
                        debug += f"Parameter name: {request_parameter_name}\n"
                        debug += f"Parameter type: {request_parameter_config['type']}\n"
                        debug += f"Parameter description: {request_parameter_config['description']}"
                        logging.debug(debug)
                        request_parameters[request_parameter_name] = request_parameter

                has_response = request_config["response"]
                response_description = ""
                if has_response:
                    logging.debug(
                        f"Config '{self.name}': Request '{name}': Creating response..."
                    )
                    response_parameter_configs = request_config["response parameters"]
                    if response_parameter_configs is not None:
                        for response_parameter_config in response_parameter_configs:
                            response_parameter_name = response_parameter_config["name"]
                            response_parameter = Parameter(
                                name=response_parameter_name,
                                type=response_parameter_config["type"],
                                description=response_parameter_config["description"],
                            )
                            if response_parameter_name in response_parameters:
                                logging.error(
                                    f"Config '{self.name}': Request '{name}':\n"
                                    f"Multiple definitions of response parameter '{response_parameter_name}'!"
                                )
                                ret += 1
                                continue
                            debug = f"Config '{self.name}': Request '{name}': Adding response parameter with:\n"
                            debug += f"Parameter name: {response_parameter_name}\n"
                            debug += (
                                f"Parameter type: {response_parameter_config['type']}\n"
                            )
                            debug += f"Parameter description: {response_parameter_config['description']}"
                            logging.debug(debug)
                            response_parameters[response_parameter_name] = (
                                response_parameter
                            )
                    response_description = request_config["response description"]

                request = Request(
                    name=name,
                    request_description=request_config["description"],
                    request_args=request_parameters,
                    has_response=has_response,
                    response_description=response_description,
                    response_args=response_parameters,
                )
                self.requests[name] = request
        else:
            logging.debug(f"Config '{self.name}': No requests defined.")
        return ret

    def create_interface(self) -> None:
        if not self.interface_config["create"]:
            logging.info(f"Config '{self.name}': Skip generating interface.")
            return
        logging.debug(f"Config '{self.name}': Creating interface...")
        header_user_codes = {}
        source_user_codes = {}
        if self.has_interface_header:
            logging.debug(
                f"Config '{self.name}': Parsing interface header user codes..."
            )
            parser = CodeParser(self.path_to_interface_header)
            header_user_codes = parser.parse_source_code()
        if self.has_interface_source:
            logging.debug(
                f"Config '{self.name}': Parsing interface source user codes..."
            )
            parser = CodeParser(self.path_to_interface_source)
            source_user_codes = parser.parse_source_code()
        self.interface = Interface(
            name=self.config["name"],
            author=self.config["author"],
            version=self.project_config["version"],
            copyright_notice=self.project_config["copyright notice"],
            requests=self.requests,
            header_user_codes=header_user_codes,
            source_user_codes=source_user_codes,
            path_to_folder=self.path_to_folder,
        )
        for request in self.requests.values():
            request.interface = self.interface

    def create_module(self, all_requests: Dict[str, Request]) -> int:
        ret = 0
        if not self.module_config["create"]:
            logging.info(f"Config '{self.name}': Skip generating module.")
            return ret
        logging.debug(f"Config '{self.name}': Creating module...")
        ret += self.create_request_handlers(all_requests)
        ret += self.create_response_handlers(all_requests)
        header_user_codes = {}
        source_user_codes = {}
        if self.has_module_header:
            logging.debug(f"Config '{self.name}': Parsing module header user codes...")
            parser = CodeParser(self.path_to_module_header)
            header_user_codes = parser.parse_source_code()
        if self.has_module_source:
            logging.debug(f"Config '{self.name}': Parsing module source user codes...")
            parser = CodeParser(self.path_to_module_source)
            source_user_codes = parser.parse_source_code()
        self.module = Module(
            name=self.config["name"],
            author=self.config["author"],
            version=self.project_config["version"],
            copyright_notice=self.project_config["copyright notice"],
            kernel_nr=self.module_config["kernel"],
            request_handlers=self.request_handlers,
            response_handlers=self.response_handlers,
            header_user_codes=header_user_codes,
            source_user_codes=source_user_codes,
            path_to_folder=self.path_to_folder,
        )

        if self.interface_config["create"]:
            self.module.add_include(self.interface.get_include())

        for request in self.subscribed_requests.values():
            if request.interface is not None:
                self.module.add_include(request.interface.get_include())

        for request in self.used_requests.values():
            if request.interface is not None:
                self.module.add_include(request.interface.get_include())
        return ret

    def create_request_handlers(self, all_requests: Dict[str, Request]) -> int:
        ret = 0
        handler_configs = self.module_config["subscribed requests"]
        if handler_configs is not None:
            logging.debug(f"Config '{self.name}': Parsing subscribed requests...")
            user_codes = {}
            if self.has_module_source:
                parser = CodeParser(self.path_to_module_source)
                user_codes = parser.parse_source_code()
            for handler_config in handler_configs:
                name = handler_config["name"]
                logging.debug(
                    f"Config '{self.name}': Creating request handler for subscribed request '{name}'..."
                )
                if name in self.subscribed_requests:
                    logging.error(
                        f"Config '{self.name}': Subscribed request '{name}' is already listed!"
                    )
                    ret += 1
                    continue
                if not (name in all_requests):
                    logging.warning(
                        f"Config '{self.name}': Subscribed request '{name}' is not defined!"
                    )
                    request = Request(name)
                    ret += 1
                else:
                    request = all_requests[name]
                self.subscribed_requests[name] = request
                handler = RequestHandler(
                    request=request,
                    description=handler_config["description"],
                    user_codes=user_codes,
                    module_name=self.name,
                )
                request.request_handlers.append(handler)
                self.request_handlers[name] = handler
        return ret

    def create_response_handlers(self, all_requests: Dict[str, Request]) -> int:
        ret = 0
        handler_configs = self.module_config["used requests"]
        if handler_configs is not None:
            logging.debug(f"Config '{self.name}': Parsing used requests...")
            user_codes = {}
            if self.has_module_source:
                parser = CodeParser(self.path_to_module_source)
                user_codes = parser.parse_source_code()
            for handler_config in handler_configs:
                name = handler_config["name"]
                if name in self.used_requests:
                    logging.error(
                        f"Config '{self.name}': Used request '{name}' is already listed!"
                    )
                    ret += 1
                    continue
                if not (name in all_requests):
                    logging.warning(
                        f"Config '{self.name}': Used request '{name}' is not defined!"
                    )
                    request = Request(name)
                    ret += 1
                else:
                    request = all_requests[name]
                request.used_by.add(self.name)
                self.used_requests[name] = request
                if request.has_response:
                    logging.debug(
                        f"Config '{self.name}': Creating response handler for used request '{name}'..."
                    )
                    handler = ResponseHandler(
                        request=request,
                        description=handler_config["description"],
                        user_codes=user_codes,
                        module_name=self.name,
                    )
                    request.response_handlers.append(handler)
                    self.response_handlers[name] = handler
        return ret

    def generate(self) -> None:
        logging.debug(f"Config '{self.name}': Generating code files...")
        if not self.has_folder:
            os.mkdir(self.path_to_folder)
            os.replace(
                self.path_to_config,
                os.path.join(self.path_to_folder, self.config_filename),
            )
        if self.interface is not None:
            self.interface.generate()
        if self.module is not None:
            self.module.generate()

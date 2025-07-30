"""
OpenEDOS, (c) 2022-2025 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

import os
import yaml
import logging
from typing import Dict, Set, Tuple

from .message_handlers import Request
from .config_parser import ConfigParser
from .kernel import Kernel
from . import utils


class Firmware:
    def __init__(self, path_to_module_folder: str, path_to_project_config: str) -> None:
        self.path_to_module_folder = path_to_module_folder
        self.path_to_project_config = path_to_project_config
        self.requests: Dict[str, Request] = {}
        self.config_parsers: Dict[str, ConfigParser] = {}
        self.kernels: Dict[int, Kernel] = {}
        debug_message = "Set up firmware object with:\n"
        debug_message += f"Path to module folder: {self.path_to_module_folder}"
        if self.path_to_project_config is not None:
            debug_message += f",\nPath to project config: {self.path_to_project_config}"
        logging.debug(debug_message)

    def parse(self) -> int:
        error_count = 0
        if self.path_to_project_config is not None:
            with open(self.path_to_project_config, "r") as config_file:
                logging.debug("Loading project config...")
                self.project_config = yaml.load(config_file, Loader=yaml.FullLoader)
                # check if keys are missing in the given project config
                if "project name" not in self.project_config:
                    error_count += 1
                    logging.error(
                        f"{utils.bcolors.FAIL}{error_count}{utils.bcolors.ENDC}: Missing key in project config: 'project name'!"
                    )
                    self.project_config["project name"] = "default"
                if "version" not in self.project_config:
                    self.project_config["version"] = ""
                if "copyright notice" not in self.project_config:
                    self.project_config["copyright notice"] = ""
                if "generate main" not in self.project_config:
                    self.project_config["generate main"] = False
        else:
            self.project_config = {
                "project name": "",
                "version": "",
                "copyright notice": "",
                "generate main": False,
            }
        self.project_name = self.project_config["project name"]
        for tup in os.walk(self.path_to_module_folder):
            path_to_folder = tup[0]
            error_count = self.parse_interfaces_in_folder(path_to_folder, error_count)
        for tup in os.walk(self.path_to_module_folder):
            path_to_folder = tup[0]
            error_count = self.parse_modules_in_folder(path_to_folder, error_count)
        error_count = self.create_kernels(error_count)
        logging.debug("Checking if each request is used and handled...")
        for request in self.requests.values():
            # We consider 3 cases...
            # Case 1: A request is not used and not handled. This makes the definition
            # of the request obsolete, but is not considered an error.
            if not request.used_by and not request.request_handlers:
                logging.warning(f"Request {request.name} is not used and not handled!")
            # Case 2: A request is handled but not used. This is considered an error
            # because it indicates an incomplete system description.
            elif not request.used_by and request.request_handlers:
                error_count += 1
                logging.warning(
                    f"{utils.bcolors.FAIL}{error_count}{utils.bcolors.ENDC}: Request {request.name} is handled but not used!"
                )
            # Case 3: A request is used but not handled. This is considered an error
            # because it indicates an incomplete system description.
            elif request.used_by and not request.request_handlers:
                error_count += 1
                logging.warning(
                    f"{utils.bcolors.FAIL}{error_count}{utils.bcolors.ENDC}: Request {request.name} is used but not handled!"
                )
        return error_count

    def parse_interfaces_in_folder(
        self, path_to_folder: str, error_count: int = 0
    ) -> int:
        logging.debug(f"Parsing configs in folder:\n{path_to_folder}...")
        for filename in os.listdir(path_to_folder):
            if filename.endswith("_config.yaml"):
                with open(os.path.join(path_to_folder, filename), "r") as config_file:
                    logging.debug(f"Found config file: {filename}. Loading config...")
                    config = yaml.load(config_file, Loader=yaml.FullLoader)

                    name = config["name"]
                    if name in self.config_parsers:
                        error_count += 1
                        logging.error(
                            f"{utils.bcolors.FAIL}{error_count}{utils.bcolors.ENDC}: Config with name '{name}' already exists!"
                        )

                    config_parser = ConfigParser(
                        config=config,
                        project_config=self.project_config,
                        path_to_folder=path_to_folder,
                        config_filename=filename,
                    )

                    error_count = config_parser.parse_config(error_count)

                    for request in config_parser.requests.values():
                        if request.name in self.requests:
                            error_count += 1
                            logging.error(
                                f"{utils.bcolors.FAIL}{error_count}{utils.bcolors.ENDC}: Request with name '{request.name}' already exists!"
                            )
                        self.requests[request.name] = request
                    self.config_parsers[config_parser.name] = config_parser
        return error_count

    def parse_modules_in_folder(self, path_to_folder: str, error_count: int = 0) -> int:
        logging.debug(f"Parsing module configs in folder:\n{path_to_folder}...")
        for filename in os.listdir(path_to_folder):
            if filename.endswith("_config.yaml"):
                with open(os.path.join(path_to_folder, filename), "r") as config_file:
                    logging.debug(f"Found config file: {filename}. Loading config...")
                    config = yaml.load(config_file, Loader=yaml.FullLoader)
                    config_parser = self.config_parsers[config["name"]]
                    error_count = config_parser.create_module(
                        self.requests, error_count
                    )
        return error_count

    def create_kernels(self, error_count: int = 0) -> int:
        kernel_numbers: Set[int] = set()
        for config_parser in self.config_parsers.values():
            if config_parser.module is None:
                continue
            kernel_number = config_parser.module.kernel_nr
            kernel_numbers.add(kernel_number)

        if len(kernel_numbers):
            if (len(kernel_numbers) - 1) != max(kernel_numbers):
                # nok
                error_count += 1
                message = f"{utils.bcolors.FAIL}{error_count}{utils.bcolors.ENDC}: Invalid kernel numbers: {kernel_numbers}!\n"
                message += "Please ensure that all kernels are enumerated consecutively, starting with 0!"
                logging.error(message)
            else:
                # ok
                if len(kernel_numbers) == 1:
                    logging.debug(f"The system contains one kernel.")
                else:
                    logging.debug(f"The system contains {len(kernel_numbers)} kernels.")

        for config_parser in self.config_parsers.values():
            if config_parser.module is None:
                continue
            kernel_number = config_parser.module.kernel_nr
            if kernel_number not in self.kernels:
                self.kernels[kernel_number] = Kernel(kernel_number=kernel_number)
            self.kernels[kernel_number].add_module(config_parser.module)

        return error_count

    def generate(self) -> None:
        for config_parser in self.config_parsers.values():
            config_parser.generate()

    def system_info(self) -> None:
        max_handlers: Dict[str, int] = dict.fromkeys(self.requests.keys(), 0)
        handlers: Dict[str, int] = dict.fromkeys(self.requests.keys(), 0)

        for kernel in self.kernels.values():
            for module in kernel.modules.values():
                for request_handler in module.request_handlers.values():
                    if request_handler.request.name in handlers:
                        handlers[request_handler.request.name] += 1
            for name, n_handlers in handlers.items():
                if n_handlers > max_handlers[name]:
                    max_handlers[name] = n_handlers

        handler_limit = max(max_handlers.values()) if len(max_handlers) > 0 else 0

        logging.info(f"The number of kernels is {len(self.kernels)}.")
        logging.info(f"The request handler limit is {handler_limit}.")

        message = f"Determination of the message data size is not implemented yet.\n"
        message += "Please determine the message data size manually!"
        logging.info(message)


def parse_configs(
    path_to_modules: str, path_to_project_config: str
) -> Tuple[Firmware, int]:
    firmware = Firmware(path_to_modules, path_to_project_config)
    error_count = firmware.parse()

    if error_count == 0:
        logging.info(
            f"{utils.bcolors.OKGREEN}Found 0 configuration errors.{utils.bcolors.ENDC}"
        )
    elif error_count == 1:
        logging.warning(
            f"{utils.bcolors.FAIL}Found 1 configuration error.{utils.bcolors.ENDC}"
        )
    else:
        logging.warning(
            f"{utils.bcolors.FAIL}Found {error_count} configuraion errors.{utils.bcolors.ENDC}"
        )

    return firmware, error_count

"""
OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT license

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
        ret = 0
        if self.path_to_project_config is not None:
            with open(self.path_to_project_config, "r") as config_file:
                logging.debug("Loading project config...")
                self.project_config = yaml.load(config_file, Loader=yaml.FullLoader)
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
            ret += self.parse_interfaces_in_folder(path_to_folder)
        for tup in os.walk(self.path_to_module_folder):
            path_to_folder = tup[0]
            ret += self.parse_modules_in_folder(path_to_folder)
        ret += self.create_kernels()
        for request in self.requests.values():
            if not request.request_handlers:
                logging.warning(f"Request {request.name} has no handlers!")
        return ret

    def parse_interfaces_in_folder(self, path_to_folder: str) -> int:
        ret = 0
        logging.debug(f"Parsing configs in folder:\n{path_to_folder}...")
        for filename in os.listdir(path_to_folder):
            if filename.endswith("_config.yaml"):
                with open(os.path.join(path_to_folder, filename), "r") as config_file:
                    logging.debug(f"Found config file: {filename}. Loading config...")
                    config = yaml.load(config_file, Loader=yaml.FullLoader)

                    name = config["name"]
                    if name in self.config_parsers:
                        logging.error(f"Config with name '{name}' already exists!")
                        ret += 1

                    config_parser = ConfigParser(
                        config=config,
                        project_config=self.project_config,
                        path_to_folder=path_to_folder,
                        config_filename=filename,
                    )

                    ret += config_parser.parse_config()

                    for request in config_parser.requests.values():
                        if request.name in self.requests:
                            logging.error(
                                f"Request with name '{request.name}' already exists!"
                            )
                            ret += 1

                        self.requests[request.name] = request
                    self.config_parsers[config_parser.name] = config_parser
        return ret

    def parse_modules_in_folder(self, path_to_folder: str) -> int:
        ret = 0
        logging.debug(f"Parsing module configs in folder:\n{path_to_folder}...")
        for filename in os.listdir(path_to_folder):
            if filename.endswith("_config.yaml"):
                with open(os.path.join(path_to_folder, filename), "r") as config_file:
                    logging.debug(f"Found config file: {filename}. Loading config...")
                    config = yaml.load(config_file, Loader=yaml.FullLoader)

                    config_parser = self.config_parsers[config["name"]]
                    ret = config_parser.create_module(self.requests)
        return ret

    def create_kernels(self) -> int:
        ret = 0
        kernel_numbers: Set[int] = set()
        for config_parser in self.config_parsers.values():
            if config_parser.module is None:
                continue
            kernel_number = config_parser.module.kernel_nr
            kernel_numbers.add(kernel_number)

        if len(kernel_numbers):
            if (len(kernel_numbers) - 1) != max(kernel_numbers):
                # nok
                message = f"Invalid kernel numbers: {kernel_numbers}!\n"
                message += "Please ensure that all kernels are enumerated consecutively, starting with 0!"
                logging.error(message)
                ret += 1
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

        return ret

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
    ret = firmware.parse()

    if ret == 0:
        logging.info(
            f"{utils.bcolors.OKGREEN}Found 0 configuration errors.{utils.bcolors.ENDC}"
        )
    elif ret == 1:
        logging.warning(f"Found 1 configuration error.")
    else:
        logging.warning(f"Found {ret} configuraion errors.")

    return firmware, ret

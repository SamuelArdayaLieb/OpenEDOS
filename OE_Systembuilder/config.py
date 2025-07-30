"""
OpenEDOS, (c) 2022-2025 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

import os
import glob
import yaml
import logging
from typing import Tuple

from .message_handlers import Request, Parameter


def create_module_config(
    path_to_folder: str,
    file_name: str,
    config_name: str = "",
    author: str = "",
    override: bool = False,
) -> None:
    config = {
        "name": config_name,
        "author": author,
        "interface": {"create": True, "requests": []},
        "module": {
            "create": True,
            "kernel": 0,
            "subscribed requests": [],
            "used requests": [],
        },
    }

    save_config(
        path_to_folder=path_to_folder,
        file_name=file_name,
        config=config,
        override=override,
    )


def save_config(
    path_to_folder: str, file_name: str, config: dict, override: bool = False
) -> None:
    path_to_file = os.path.join(path_to_folder, file_name)
    if os.path.isfile(path_to_file) and not override:
        logging.warning(
            f"Creating module config '{config['name']}': File already exists! @\n{path_to_file}"
        )
        return
    logging.debug(f"Saving module config '{config['name']}' @\n{path_to_file}")
    with open(path_to_file, "w") as outfile:
        yaml.Dumper.ignore_aliases = lambda *args: True
        yaml.dump(config, outfile, default_flow_style=False, sort_keys=False)


def add_request(
    config: dict,
    request_name: str = "",
    has_response: bool = False,
    used: bool = False,
    subscribed: bool = False,
    defined: bool = False,
) -> dict:

    if defined:
        dummy_param = Parameter("", "", "")
        args = {"": dummy_param}
        request = Request(request_name, "", args, has_response, "", args)

        request_dict = {
            "name": request.name,
            "description": request.request_description,
            "request parameters": [],
            "response": request.has_response,
        }

        if request.has_request_args:
            for request_arg in request.request_sender.args.values():
                request_dict["request parameters"].append(
                    {
                        "name": request_arg.name,
                        "type": request_arg.type,
                        "description": request_arg.description,
                    }
                )

        if request.has_response:
            request_dict["response description"] = request.response_description
            if request.has_response_args:
                request_dict["response parameters"] = []
                for response_arg in request.response_sender.args.values():
                    request_dict["response parameters"].append(
                        {
                            "name": response_arg.name,
                            "type": response_arg.type,
                            "description": response_arg.description,
                        }
                    )

        config["interface"]["requests"].append(request_dict)

    request_dict = {"name": request_name, "description": ""}

    if used:
        config["module"]["used requests"].append(request_dict)

    if subscribed:
        config["module"]["subscribed requests"].append(request_dict)

    return config


def read_config(path_to_folder: str) -> Tuple[dict, str]:
    filelist = glob.glob(f"{path_to_folder}/*_config.yaml")
    if len(filelist) == 0:
        logging.error(f"Could not find config file in folder\n{path_to_folder}!")
        return None, None
    if len(filelist) > 1:
        logging.error(f"Found more than one config files in folder\n{path_to_folder}!")
        return None, None
    filename = filelist[0]
    with open(os.path.join(path_to_folder, filename), "r") as config_file:
        logging.debug(f"Found config file: {filename}. Loading config...")
        config = yaml.load(config_file, Loader=yaml.FullLoader)
    if config["interface"]["requests"] is None:
        config["interface"]["requests"] = list()
    if config["module"]["used requests"] is None:
        config["module"]["used requests"] = list()
    if config["module"]["subscribed requests"] is None:
        config["module"]["subscribed requests"] = list()
    return config, filename

"""
This is the OpenEDOS SystemBuilder v0.1.
(c) 2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

import os
import yaml
import logging
from firmware import Firmware
from typing import Tuple

INDENT = "\t"

OPENEDOS_SYSTEMBUILDER_VERSION = "0.1"
OPENEDOS_CORE_VERSION          = "0.2.1"

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def text_to_comment(text:str) -> str:
    comment = "/**\n"
    lines = text.split("\n")[:-1]
    for line in lines:
        comment += f" * {line}\n"
    comment += " */\n"
    return comment
        
def name_to_filename(name:str) -> str:
    name = name.replace(' ', '_')
    if len(name) < 3:
        return name.lower()
    charakter_0 = name[0]
    charakter_1 = name[1]
    filename = charakter_0
    for charakter in name[2:]:
        if charakter_0.islower() and charakter_1.isupper():
            filename += f"_{charakter_1}"
        elif charakter_0.isupper() and charakter_1.isupper() and charakter.islower():
            filename += f"_{charakter_1}"
        else:
            filename += charakter_1

        charakter_0 = charakter_1
        charakter_1 = charakter
    filename += charakter
    return filename.lower()

def set_logging(
    debug:bool
    ) -> None:
    logging_level = logging.INFO
    if debug:
        logging_level = logging.DEBUG

    logging.basicConfig(
        format=f"{bcolors.OKBLUE}OpenEDOS System Builder:{bcolors.ENDC} "\
               f"{bcolors.BOLD}%(levelname)s{bcolors.ENDC}: %(message)s", 
        level=logging_level)

def parse_configs(
    modules:str,
    project:str
    ) -> Tuple[Firmware, int]:
    firmware = Firmware(modules, project)
    ret = firmware.parse()

    if ret == 0:
        logging.info(f"Found 0 config errors after parsing all config files.")
    elif ret == 1:
        logging.warning(f"Found 1 config error after parsing all config files.")
    else:
        logging.warning(f"Found {ret} config errors after parsing all config files.")

    return firmware, ret

def create_module_config(
    path_to_folder:str,
    file_name:str,
    config_name:str="",
    author:str=""
    ) -> None:
    path_to_file = os.path.join(path_to_folder, file_name)
    if os.path.isfile(path_to_file):
        logging.warn(f"Creating module config '{config_name}': File already exists!\n@ {path_to_file}")
        return
    config = {
        "name": config_name,
        "author": author,
        "interface": {
            "create": True,
            "requests": [
                {
                    "name": "",
                    "description": "",
                    "request parameters": [
                        {
                            "name": "",
                            "type": "",
                            "description": ""
                        },
                        {
                            "name": "",
                            "type": "",
                            "description": ""
                        }
                    ],
                    "use message id": False,
                    "response": True,
                    "response description": "",
                    "response parameters": [
                        {
                            "name": "",
                            "type": "",
                            "description": ""
                        }
                    ]
                }
            ]
        },
        "module": {
            "create": True,
            "kernel": 0,
            "subscribed requests": [
                {
                    "name": "",
                    "description": ""
                }
            ],
            "used requests": [
                {
                    "name": "",
                    "description": ""
                }
            ]
        }
    }
    
    with open(path_to_file, 'w') as outfile:
        yaml.dump(config, outfile, default_flow_style=False)

def create_project_config(
    path_to_folder:str,
    project_name:str
    ) -> None:
    filename = name_to_filename(f"{project_name}_project.yaml")
    path_to_file = os.path.join(path_to_folder, filename)
    if os.path.isfile(path_to_file):
        logging.warn(f"Creating project config '{project_name}': File already exists!\n@ {path_to_file}")
        return
    config = {
        "project name" : project_name,
        "version" : "0.0.1",
        "copyright notice" : ""
    }
    with open(path_to_file, 'w') as outfile:
        yaml.dump(config, outfile, default_flow_style=False)
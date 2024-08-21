"""
OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

import os
import logging
import yaml
import shutil
from typing import Dict, Set, List
import pkg_resources

from . import utils
from . import firmware
from .source_files import RequestsHeader, MainFile, AllModules
from .user_code import CodeParser

def create_project_config(
    path_to_folder:str,
    project_name:str
    ) -> None:
    filename = utils.name_to_filename(f"{project_name}_project.yaml")
    path_to_file = os.path.join(path_to_folder, filename)
    if os.path.isfile(path_to_file):
        logging.warn(f"Creating project config '{project_name}': File already exists! @\n{path_to_file}")
        return
    config = {
        "project name" : project_name,
        "version" : "0.0.1",
        "copyright notice" : ""
    }
    with open(path_to_file, 'w') as outfile:
        yaml.dump(config, outfile, default_flow_style=False)

def create_requests_header(
    fw:firmware.Firmware,
    path_to_config_folder:str
    ) -> None:
    
    request_ids:Dict[str, list] = {}
    filename = "oe_requests.h"
    path_to_file = os.path.join(path_to_config_folder, filename)
    authors:Set = set()

    logging.debug("Looking for request ids...") 
    for config_parser in fw.config_parsers.values():
        authors.add(config_parser.config["author"])
        if len(config_parser.requests) > 0:
            request_ids[config_parser.name] = []
            for request in config_parser.requests.values():
                request_ids[config_parser.name].append(request.RID)

    if os.path.isfile(path_to_file):
        logging.debug(f"Creating oe_requests.h: Found existing file  @\n{path_to_file}")
        parser = CodeParser(path_to_file)
        user_codes = parser.parse_source_code()
    else:
        logging.debug(f"Creating new file 'oe_requests.h'...")
        user_codes = {}

    if len(authors) == 0:
        author_string = "OpenEDOS"
    else:
        author_string = ", ".join(authors)

    requests_header = RequestsHeader(
        author=author_string,
        version=fw.project_config["version"],
        copyright_notice=fw.project_config["copyright notice"],
        request_ids=request_ids,
        user_codes=user_codes)
    
    logging.debug("Generating header file 'oe_requests.h'...")
    with open(path_to_file, 'w') as file:
        file.write(requests_header.get_text())

def create_all_modules_header(
    fw:firmware.Firmware,
    path_to_project_folder:str,
    ) -> None:
    filename = "all_modules.h"
    path_to_file = os.path.join(path_to_project_folder, filename)

    if os.path.isfile(path_to_file):
        logging.debug(f"Creating all_modules.h: Found existing file  @\n{path_to_file}")
        parser = CodeParser(path_to_file)
        user_codes = parser.parse_source_code()
    else:
        logging.debug(f"Creating new file 'all_modules.h'...")
        user_codes = {}

    module_headers = []
    for config_parser in fw.config_parsers.values():
        if config_parser.module is not None:
            if config_parser.module.header is not None:
                module_headers.append(config_parser.module.header)    

    all_modules = AllModules(
        author="OpenEDOS Systembuilder",
        version=fw.project_config["version"],
        copyright_notice=fw.project_config["copyright notice"],
        module_headers=module_headers,
        user_codes=user_codes)  

    logging.debug("Generating header file 'all_modules.h'...")
    with open(path_to_file, 'w') as file:
        file.write(all_modules.get_text())

def create_main(
    fw:firmware.Firmware,
    path_to_project_folder:str,
    ) -> None:

    modules:Dict[int, List[str]] = {}
    filename = "main.c"
    path_to_file = os.path.join(path_to_project_folder, filename)
    authors:Set = set()
    
    if os.path.isfile(path_to_file):
        logging.debug(f"Creating main.c: Found existing file  @\n{path_to_file}")
        parser = CodeParser(path_to_file)
        user_codes = parser.parse_source_code()
    else:
        logging.debug(f"Creating new file 'main.c'...")
        user_codes = {}

    for config_parser in fw.config_parsers.values():
        authors.add(config_parser.config["author"])

    if len(authors) == 0:
        author_string = "OpenEDOS Systembuilder"
    else:
        author_string = ", ".join(authors)

    for kernel_id, kernel in fw.kernels.items():
        if "OE_Core" in kernel.modules.keys():
            modules[kernel_id] = list(kernel.modules.keys()).remove("OE_Core")
        else:
            modules[kernel_id] = list(kernel.modules.keys())

    main = MainFile(
        author=author_string,
        version=fw.project_config["version"],
        copyright_notice=fw.project_config["copyright notice"],
        user_codes=user_codes,
        modules=modules)
    
    logging.debug("Generating source file 'main.c'...")
    with open(path_to_file, 'w') as file:
        file.write(main.get_text())

def create_project(
    path_to_folder:str,
    project_name:str
    ) -> None:
    logging.debug("Checking path to new project...")
    path_to_project = os.path.join(path_to_folder, project_name.replace(" ", "_"))
    if os.path.isdir(path_to_project):
        logging.error(f"Creating project {project_name.replace(' ', '_')}: Project directory already exists! @\n{path_to_project}")
        return
    logging.debug("Creating project directories...")
    path_to_project_config = os.path.join(path_to_project, utils.name_to_filename(f"{project_name}_project.yaml"))
    path_to_module_folder = os.path.join(path_to_project, "Modules")
    path_to_core = os.path.join(path_to_module_folder, "OE_Core")
    path_to_config_folder = os.path.join(path_to_project, "OE_Config")
    
    os.makedirs(path_to_project)
    os.makedirs(path_to_module_folder)
    os.makedirs(path_to_core)
    os.makedirs(path_to_config_folder)
    
    logging.debug("Creating project config...")
    create_project_config(
        path_to_folder=path_to_project,
        project_name=project_name)

    logging.debug("Populating OE Config...")
    shutil.copy(pkg_resources.resource_filename('OE_Config', 'oe_config.h'), path_to_config_folder)
    shutil.copy(pkg_resources.resource_filename('OE_Config', 'oe_port.h'), path_to_config_folder)
    shutil.copy(pkg_resources.resource_filename('OE_Config', 'oe_requests.h'), path_to_config_folder)

    logging.debug("Populating OE Core...")
    # CMakeLists?
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_core_config.yaml'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_core_intf.c'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_core_intf.h'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_core_mod.c'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_core_mod.h'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_defines.h'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_kernel.c'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_kernel.h'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_message_queue.c'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_message_queue.h'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_request_map.c'), path_to_core)
    shutil.copy(pkg_resources.resource_filename('OE_Core', 'oe_request_map.h'), path_to_core)

    fw, _ = firmware.parse_configs(
        path_to_modules=path_to_module_folder,
        path_to_project_config=path_to_project_config)

    logging.debug("Creating oe_requests.h...")
    create_requests_header(
        fw=fw,
        path_to_config_folder=path_to_config_folder)

    logging.debug("Creating main.c...")
    create_main(
        fw=fw,
        path_to_project_folder=path_to_project)
    
    logging.debug("Creating all_modules.h...")
    create_all_modules_header(
        fw=fw,
        path_to_project_folder=path_to_project)
    
    logging.info(f"Generation of project {project_name} completed!")
    
    

    

    
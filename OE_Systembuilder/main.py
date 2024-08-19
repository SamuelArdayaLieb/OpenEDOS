"""
OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

__version__ = "2.0.0"
version_message = '%(prog)s, v%(version)s\n(c) 2022-2024 Samuel Ardaya-Lieb\nMIT license'

import os
import click
import glob
import logging

from . import utils
from . import config
from . import firmware
from . import project

@click.group(no_args_is_help=True)
@click.version_option(version=__version__, prog_name="OpenEDOS", message=version_message)
def openedos():
    pass

@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def parse_modules(
    path:str, 
    debug:bool
    ) -> int:
    """
    Parse module configs located in PATH and subdirectories of PATH.
    """
    utils.set_logging(debug)

    fw, ret = firmware.parse_configs(
        path_to_modules=path, 
        path_to_project_config=None) # not needed for parsing
    
    fw.system_info()

    return ret

@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-p', '--project_config',
    type=click.Path(resolve_path=True, file_okay=True),
    default=None,
    help='The path to the project config file.')
@click.option(
    '-f', '--force',
    is_flag=True, 
    default=False, 
    help='Force generation even if config errors occure.')
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def generate_modules(
    path:str, 
    project_config:str,
    force:bool,
    debug:bool
    ) -> None:
    """
    Generate a code skeleton for module configs located in PATH.
    If PATH contains a subdirectory called 'Modules', this subdirectory is treated as
    the top level module folder. Otherwise PATH is the top level module folder.
    If no path to a project config is given and there is one _project.yaml file in PATH,
    this file is treated as the project config.
    """
    utils.set_logging(debug)

    path_to_project_config = None
    path_to_modules = None

    if project_config is None:
        # Look for project config.
        filelist = glob.glob(f"{path}/*_project.yaml")
        if len(filelist) == 0:
            logging.warn("Coud not find project config and no path was given!")
        elif len(filelist) > 1:
            logging.error("Found more than one project config file!")
            return
        else:
            filename = filelist[0]
            path_to_project_config = os.path.join(path, filename)
            logging.debug(f"Found project config\n@{path_to_project_config}")
    else:
        path_to_project_config = project_config

    # Look for modules folder.
    path_to_modules = os.path.join(path, "Modules")
    if os.path.isdir(path_to_modules):
        logging.debug(f"Found modules folder\n@{path_to_modules}")
    else:
        path_to_modules = path

    fw, ret = firmware.parse_configs(
        path_to_modules=path_to_modules,
        path_to_project_config=path_to_project_config)

    if ret > 0 and not force:
        logging.error("Source code is not generated due to config errors!")
        return
    
    fw.generate()

    logging.info(f"{utils.bcolors.OKGREEN}Source code generation completed.{utils.bcolors.ENDC}")  

    fw.system_info() 

@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-n', '--name',
    default="",
    help='The name of the config.')
@click.option(
    '-N', '--number',
    default=1,
    help='The number of configs to be created.')
@click.option(
    '-a', '--author',
    default="",
    help='The author of the config file.')
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def add_module_config(
    path:str,
    name:str,
    number:int,
    author:str,
    debug:bool
    ) -> None:
    """
    Create new module configs in PATH.
    """
    utils.set_logging(debug)

    if number == 1:
        filename = f"{utils.name_to_filename(name)}_config.yaml"
        logging.debug(f"Creating config file '{filename}'...")
        config.create_module_config(
            path_to_folder=path,
            file_name=filename,
            config_name=name,
            author=author)
        return
    
    for n in range(number):
        filename = f"{utils.name_to_filename(name)}_{n}_config.yaml"
        logging.debug(f"Creating {number} config files...")
        config.create_module_config(
            path_to_folder=path,
            file_name=filename,
            config_name=name,
            author=author)
        
@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-n', '--name',
    help='The name of the request.')
@click.option(
    '-r', '--response',
    is_flag=True, 
    default=False, 
    help='The request has a response.')
@click.option(
    '-u', '--used',
    is_flag=True, 
    default=False, 
    help='Lists added request under used requests.')
@click.option(
    '-U', '--usedonly',
    is_flag=True, 
    default=False, 
    help='Lists under used requests without definition.')
@click.option(
    '-s', '--subscribed',
    is_flag=True, 
    default=False, 
    help='Lists added request under subscribed requests.')
@click.option(
    '-S', '--subscribedonly',
    is_flag=True, 
    default=False, 
    help='Lists under subscribed requests without definition.')
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def add_request(
    path:str, 
    name:str,
    response:bool,
    used:bool,
    usedonly:bool,
    subscribed:bool,
    subscribedonly:bool,
    debug:bool
    ) -> int:
    """
    Add a request to a module config located in PATH.
    There has to be exactly one _config.yaml file in PATH.
    """
    utils.set_logging(debug)
    
    conf, file_name = config.read_config(path)
    if conf is None:
        return

    config.add_request(
        config=conf,
        request_name=name,
        has_response=response,
        used=(used or usedonly),
        subscribed=(subscribed or subscribedonly),
        defined=(not(usedonly or subscribedonly)))

    config.save_config(
        path_to_folder=path,
        file_name=file_name,
        config=conf,
        override=True)  
          
@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-n', '--name',
    default="",
    help='The name of the project.')
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def add_project_config(
    path:str,
    name:str,
    debug:bool
    ) -> None:
    """
    Create a new project config in PATH.
    """
    utils.set_logging(debug)
    logging.debug("Creating project config...")
    project.create_project_config(
        path_to_folder=path,
        project_name=name)
    
@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-n', '--name',
    default="",
    help='The name of the project.')
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def create_project(
    path:str,
    name:str,
    debug:bool):
    """
    Create a new project in PATH.
    A top level project directory is created in PATH. Inside that directory,
    the OpenEDOS Core and OpenEDOS Config folders are placed. Also, an empty
    'Modules' folder and a project config are created. Finally, the files
    oe_requests.h, all_modules.h and main.c are created.
    """
    utils.set_logging(debug)
    project.create_project(
        path_to_folder=path,
        project_name=name)
    
@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def list_request_ids(
    path:str,
    debug:bool) -> None:
    """
    List all request IDs extracted from module configs located in PATH
    and subdirectories.
    """
    utils.set_logging(debug)
    
    fw, _ = firmware.parse_configs(path, path_to_project_config=None)

    message = "Found the following request ids:\n"

    for config_parser in fw.config_parsers.values():
        if len(config_parser.requests) > 0:
            message += f"    /* {config_parser.name} */\n"
            for request in config_parser.requests.values():
                message += f"    {request.RID},\n"
            message += "\n"
    
    logging.info(message)

@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-c', '--config_folder', 
    type=click.Path(resolve_path=True, file_okay=False),
    default=None, 
    help='The path to the OpenEDOS config folder.')
@click.option(
    '-p', '--project_config',
    type=click.Path(resolve_path=True, file_okay=True),
    default=None,
    help='The path to the project config file.')
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def create_request_header(
    path:str,
    config_folder:str,
    project_config:str,
    debug:bool) -> None:
    """
    Write all request IDs to oe_requests.h.
    The request IDs are extracted from all module configs located in PATH
    and subdirectories. If no path to a config folder is given and there is
    a directory 'OE_Config' inside PATH, this directory will be treated as the config folder.
    If no path to a project config is given and one _project.yaml is found inside PATH,
    this file is treated as the project config.
    """
    utils.set_logging(debug)

    path_to_config_folder = None
    path_to_project_config = None

    if config_folder is None:
        # Look for OE_Config/.
        logging.debug("Looking for config folder...")
        if os.path.isdir(os.path.join(path, "OE_Config")):
            path_to_config_folder = os.path.join(path, "OE_Config")
            logging.info(f"Found config folder\n@{path_to_config_folder}")
        else:
            logging.error("Could not find config folder and no path was given!")
            return
    else:
        path_to_config_folder = config_folder

    if project_config is None:
        # Look for _project.yaml.
        filelist = glob.glob(f"{path}/*_project.yaml")
        if len(filelist) == 0:
            logging.warn("Coud not find project config and no path was given!")
        elif len(filelist) > 1:
            logging.error("Found more than one project config file!")
            return
        else:
            filename = filelist[0]
            path_to_project_config = os.path.join(path, filename)
            logging.debug(f"Found project config\n@{path_to_project_config}")
    else:
        path_to_project_config = project_config

    logging.info("Generating oe_requests.h...")
    project.create_requests_header(
        path_to_module_folder=path,
        path_to_config_folder=path_to_config_folder,
        path_to_project_config=path_to_project_config)
    
    logging.info(f"{utils.bcolors.OKGREEN}Generation of oe_requests.h completed.{utils.bcolors.ENDC}") 

@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-m', '--modules', 
    type=click.Path(resolve_path=True, file_okay=False),
    default=None, 
    help='The path to the module folder.')
@click.option(
    '-d', '--debug',           
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def create_all_modules_header(
    path:str,
    modules:str,
    debug:bool) -> None:
    """
    Create all_modules.h in PATH.
    The all_modules.h file will be generated based on all configs found in the given
    modules folder and subdirectories. If no path to a modules folder is given, all
    configs inside PATH are considered. If no path to a modules folder is given and a 
    directory 'Modules' is found inside PATH, only module configs inside that folder 
    are considered. 
    """
    utils.set_logging(debug)

    path_to_module_folder = None

    if modules is None:
        # Look for Modules/.
        logging.debug("Looking for module folder...")
        if os.path.isdir(os.path.join(path, "Modules")):
            path_to_module_folder = os.path.join(path, "Modules")
            logging.info(f"Found module folder\n@{path_to_module_folder}")
        else:
            logging.debug(f"Could not find module folder. Using directory\n@{path}")
            path_to_module_folder = path
    else:
        path_to_module_folder = modules

    logging.info("Generating all_modules.h...")

    fw, _ = firmware.parse_configs(
        path_to_modules=path_to_module_folder,
        path_to_project_config=None)
    
    project.create_all_modules_header(
        fw=fw,
        path_to_project_folder=path)

    logging.info(f"{utils.bcolors.OKGREEN}Generation of all_modules.h completed.{utils.bcolors.ENDC}") 

@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-m', '--modules', 
    type=click.Path(resolve_path=True, file_okay=False),
    default=None, 
    help='The path to the module folder.')
@click.option(
    '-p', '--project_config',
    type=click.Path(resolve_path=True, file_okay=True),
    default=None,
    help='The path to the project config file.')
@click.option(
    '-d', '--debug',           
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def create_main(
    path:str,
    modules:str,
    project_config:str,
    debug:bool) -> None:
    """
    Create main.c in PATH.
    The main.c file will be generated based on all configs found in PATH
    and subdirectories. If no path to a modules folder is given and a directory 'Modules' 
    is found inside PATH, only module configs inside that folder are considered. 
    If no path to a project config is given and one _project.yaml is found inside PATH,
    this file is treated as the project config.
    """
    utils.set_logging(debug)

    path_to_module_folder = None
    path_to_project_config = None

    if modules is None:
        # Look for Modules/.
        logging.debug("Looking for module folder...")
        if os.path.isdir(os.path.join(path, "Modules")):
            path_to_module_folder = os.path.join(path, "Modules")
            logging.info(f"Found module folder\n@{path_to_module_folder}")
        else:
            logging.error("Could not find module folder and no path was given!")
            return
    else:
        path_to_module_folder = modules

    if project_config is None:
        # Look for _project.yaml.
        filelist = glob.glob(f"{path}/*_project.yaml")
        if len(filelist) == 0:
            logging.warn("Coud not find project config and no path was given!")
        elif len(filelist) > 1:
            logging.error("Found more than one project config files!")
            return
        else:
            filename = filelist[0]
            path_to_project_config = os.path.join(path, filename)
            logging.debug(f"Found project config\n@{path_to_project_config}")
    else:
        path_to_project_config = project_config

    fw, _ = firmware.parse_configs(
        path_to_modules=path_to_module_folder,
        path_to_project_config=path_to_project_config)

    logging.info("Generating main.c...")
    project.create_main(
        fw=fw,
        path_to_project_folder=path)
    
    logging.info(f"{utils.bcolors.OKGREEN}Generation of main.c completed.{utils.bcolors.ENDC}") 
    
@openedos.command(no_args_is_help=True)
@click.argument(
    'path', 
    type=click.Path(resolve_path=True, file_okay=False))
@click.option(
    '-f', '--force',
    is_flag=True, 
    default=False, 
    help='Force generation even if config errors occure.')
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def update_project(
    path:str, 
    force:bool,
    debug:bool
    ) -> None:
    """
    Generate code for a project located in PATH.
    PATH is the top level project directory. One _project.yaml file,
    one directory 'Modules' and one directory 'OE_Config' are expected.
    """
    utils.set_logging(debug)

    path_to_project_config = None
    path_to_modules_folder = None
    path_to_config_folder = None

    # Look for project config in given directory.
    filelist = glob.glob(f"{path}/*_project.yaml")
    if len(filelist) == 0:
        logging.warn("Coud not find project config and no path was given!")
    elif len(filelist) > 1:
        logging.error("Found more than one project config files!")
        return
    else:
        filename = filelist[0]
        path_to_project_config = os.path.join(path, filename)
        logging.debug(f"Found project config\n@{path_to_project_config}")

    # Look for Modules folder
    path_to_modules_folder = os.path.join(path, "Modules")
    if os.path.isdir(path_to_modules_folder):
        logging.debug(f"Found 'Modules' folder\n@{path_to_modules_folder}")
    else:
        path_to_modules_folder = path

    # Look for OpenEDOS Config folder.
    path_to_config_folder = os.path.join(path, "OE_Config")
    if os.path.isdir(path_to_config_folder):
        logging.debug(f"Found OpenEDOS Config folder\n@{path_to_config_folder}")
    else:
        logging.warning("Could not find 'OE_Config' folder!")
        path_to_config_folder = None

    fw, ret = firmware.parse_configs(
        path_to_modules=path_to_modules_folder,
        path_to_project_config=path_to_project_config)

    if ret > 0 and not force:
        logging.error("Source code is not generated due to config errors!")
        return
    
    fw.generate()

    if path != path_to_modules_folder:
        project.create_main(
            fw=fw,
            path_to_project_folder=path)
        project.create_all_modules_header(
            fw=fw,
            path_to_project_folder=path)
    
    if path_to_config_folder is not None:
        project.create_requests_header(
            fw=fw,
            path_to_config_folder=path_to_config_folder)
    
    logging.info(f"{utils.bcolors.OKGREEN}Source code generation completed.{utils.bcolors.ENDC}") 
     
    fw.system_info() 
    

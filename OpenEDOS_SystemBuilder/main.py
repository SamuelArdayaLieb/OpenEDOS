"""
This is the OpenEDOS SystemBuilder v0.2.
(c) 2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""
__version__ = "0.1"
version_message = '%(prog)s, v%(version)s\n(c) 2024 Samuel Ardaya-Lieb\nMIT license'

import click
import logging
import utils

@click.group()
@click.version_option(version=__version__, prog_name="OpenEDOS SystemBuilder", message=version_message)
def openedos_systembuilder():
    pass

@openedos_systembuilder.command()
@click.option(
    '-m', '--modules', 
    type=click.Path(resolve_path=True, file_okay=False), 
    help='The path to the top level module folder.')
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def parse_configs(
    modules:str, 
    debug:bool
    ) -> int:
    """Parses module configs."""
    utils.set_logging(debug)
    
    firmware, ret = utils.parse_configs(modules, project=None)

    firmware.system_info()
    
    return ret

@openedos_systembuilder.command()
@click.option(
    '-m', '--modules', 
    type=click.Path(resolve_path=True, file_okay=False), 
    help='The path to the top level module folder.')
@click.option(
    '-p', '--project',
    type=click.Path(resolve_path=True, file_okay=True),
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
def create_firmware(
    modules:str, 
    project:str,
    force:bool,
    debug:bool
    ) -> None:
    """Generates a source code skeleton for modules and interfaces."""
    utils.set_logging(debug)

    firmware, ret = utils.parse_configs(modules, project)

    if ret > 0 and not force:
        logging.warning("Source code is not generated due to config errors!")
        return
    
    firmware.generate()

    logging.info("Source code generation completed")  

    firmware.system_info() 

@openedos_systembuilder.command()
@click.option(
    '-p', '--path',
    type=click.Path(resolve_path=True, file_okay=False),
    help='The path to the folder where the new config file is created.')
@click.option(
    '-n', '--name',
    default="",
    help='The name of the config.')
@click.option(
    '-N', '--number_of_configs',
    default=1,
    help='The number of configs to be created.')
@click.option(
    '-a', '--author',
    default="",
    help='The author of the config file.')
def create_module_config(
    path:str,
    name:str,
    number_of_configs:int,
    author:str
    ) -> None:
    """Creates new module configs."""
    utils.set_logging(debug=False)

    if number_of_configs == 1:
        file_name = f"{utils.name_to_filename(name)}_config.yaml"
        utils.create_module_config(
            path_to_folder=path,
            file_name=file_name,
            config_name=name,
            author=author)
        return
    
    for n in range(number_of_configs):
        file_name = f"{utils.name_to_filename(name)}_{n}_config.yaml"
        utils.create_module_config(
            path_to_folder=path,
            file_name=file_name,
            config_name=name,
            author=author)
        
@openedos_systembuilder.command()
@click.option(
    '-m', '--modules', 
    type=click.Path(resolve_path=True, file_okay=False), 
    help='The path to the top level module folder.')
@click.option(
    '-d', '--debug',
    is_flag=True, 
    default=False, 
    help='Print debug information.')
def list_request_ids(
    modules:str,
    debug:bool) -> None:
    """Lists all discovered request IDs."""
    utils.set_logging(debug)
    
    firmware, _ = utils.parse_configs(modules, project=None)

    message = "Found the following request ids:\n"

    for config_parser in firmware.config_parsers.values():
        if len(config_parser.requests) > 0:
            message += f"    /* {config_parser.name} */\n"
            for request in config_parser.requests.values():
                message += f"    {request.RID},\n"
            message += "\n"
    
    logging.info(message)

@openedos_systembuilder.command()
@click.option(
    '-p', '--path',
    type=click.Path(resolve_path=True, file_okay=False),
    help='The path to the folder where the new project config file is created.')
@click.option(
    '-n', '--name',
    default="",
    help='The name of the project.')
def create_project_config(
    path:str,
    name:str):
    """Creates a new project config."""
    utils.set_logging(debug=False)
    utils.create_project_config(
        path_to_folder=path,
        project_name=name)
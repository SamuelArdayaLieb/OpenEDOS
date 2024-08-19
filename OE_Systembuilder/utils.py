"""
OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

import logging

INDENT = "\t"

OPENEDOS_VERSION = "2.0"

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
        format=f"{bcolors.OKBLUE}OpenEDOS:{bcolors.ENDC} "\
               f"{bcolors.BOLD}%(levelname)s{bcolors.ENDC}: %(message)s", 
        level=logging_level)

# def create_project_config(
#     path_to_folder:str,
#     project_name:str
#     ) -> None:
#     filename = name_to_filename(f"{project_name}_project.yaml")
#     path_to_file = os.path.join(path_to_folder, filename)
#     if os.path.isfile(path_to_file):
#         logging.warn(f"Creating project config '{project_name}': File already exists!\n@ {path_to_file}")
#         return
#     config = {
#         "project name" : project_name,
#         "version" : "0.0.1",
#         "copyright notice" : ""
#     }
#     with open(path_to_file, 'w') as outfile:
#         yaml.dump(config, outfile, default_flow_style=False)
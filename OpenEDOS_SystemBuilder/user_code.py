"""
This is the OpenEDOS SystemBuilder v0.2.
(c) 2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

from typing import Dict

class UserCode():
    def __init__(self,
        identifier:str="",
        description:str="\n",
        code:str="\n",
        indents:int=0
        ) -> None:
        self.identifier = identifier
        self.description = description
        self.code = code
        self.indents = indents

    def get_text(self) -> str:
        text = self.indents*"\t" + f"/* USER CODE {self.identifier} BEGIN */\n"
        text += self.code
        text += self.indents*"\t" + f"/* USER CODE {self.identifier} END */\n"
        return text

class CodeParser():
    def __init__(self,
        path_to_file:str
        ) -> None:
        self.path_to_file = path_to_file

    def get_id(self, line:str) -> str:
        start = "/* USER CODE "
        end = " BEGIN */"
        return line[line.find(start)+len(start):line.rfind(end)]
    
    def parse_source_code(self) -> Dict[str, UserCode]:
        user_codes:Dict[str, UserCode] = {}
        with open(self.path_to_file, 'r') as file:
            lines = file.readlines()
            for i, line in enumerate(lines):
                if ("/* USER CODE " in line) and (" BEGIN */" in line):
                    id = self.get_id(line)
                    code = ""
                    for line in lines[i+1:]:
                        if ("/* USER CODE " in line) and (" END */" in line):
                            user_code = UserCode(
                                identifier=id,
                                code=code)
                            user_codes[id] = user_code
                            break
                        else:
                            code += line
        return user_codes
                    
                    
                    
                        

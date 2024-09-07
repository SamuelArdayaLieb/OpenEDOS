"""
OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

import os
import logging
from typing import List, Dict

from .source_files import ModuleHeader, ModuleSource
from .message_handlers import RequestHandler, ResponseHandler, Request
from .user_code import UserCode

class Module():
    def __init__(self,
        name:str,
        author:str,
        version:str,
        copyright_notice:str,
        kernel_nr:int,
        request_handlers:Dict[str, RequestHandler]={},
        response_handlers:Dict[str, ResponseHandler]={},
        header_user_codes:Dict[str, UserCode]={},
        source_user_codes:Dict[str, UserCode]={},
        path_to_folder:str=""
        ) -> None:

        self.name = name
        self.author = author
        self.version = version
        self.copyright_notice = copyright_notice
        try:
            if self.copyright_notice[-1] != "\n":
                self.copyright_notice += "\n"
        except:
            self.copyright_notice = "\n"
        self.kernel_nr = kernel_nr
        self.request_handlers = request_handlers
        self.response_handlers = response_handlers
        self.header_user_codes:Dict[str, UserCode]={}
        self.source_user_codes:Dict[str, UserCode]={}
        self.path_to_folder = path_to_folder
        self.includes:List[str] = []
        self.header = None
        logging.debug(f"Creating module '{self.name}'...")

        self.add_header_user_codes(header_user_codes)
        self.add_source_user_codes(source_user_codes)
        
    def create_header(self):
        self.header = ModuleHeader(
            name=self.name,
            author=self.author,
            version=self.version,
            copyright_notice=self.copyright_notice,
            user_codes=self.header_user_codes)
        self.header.includes += self.includes
        
    def create_source(self):
        self.source = ModuleSource(
            name=self.name,
            author=self.author,
            version=self.version,
            copyright_notice=self.copyright_notice,
            request_handlers=self.request_handlers,
            response_handlers=self.response_handlers,
            user_codes=self.source_user_codes)

    def add_header_user_codes(self, user_codes:Dict[str, UserCode]):
        logging.debug(f"Module '{self.name}': Adding header user codes...")
        self.header_user_codes.update(user_codes)
        self.create_header()

    def add_source_user_codes(self, user_codes:Dict[str, UserCode]):
        logging.debug(f"Module '{self.name}': Adding source user codes...")
        self.source_user_codes.update(user_codes)
        self.create_source()

    def add_request_handler(self, 
        request:Request, 
        description:str,
        user_code:str
        ) -> None:
        handler = RequestHandler(
            request=request,
            description=description,
            user_code=user_code)
        self.request_handlers[handler.name] = handler
        self.create_source()

    def add_response_handler(self, 
        request:Request, 
        description:str, 
        user_code:str
        ) -> None:
        handler = ResponseHandler(
            request=request,
            description=description,
            user_code=user_code)
        self.response_handlers[handler.name] = handler
        self.create_source()

    def add_include(self, include:str) -> None:
        if include not in self.includes:
            self.includes.append(include)
            self.create_header()

    def get_include(self):
        if self.header is not None:
            return f"#include {self.header.filename}\n"
    
    def print_header(self) -> None:
        logging.debug(f"Module '{self.name}': Generating header file...")
        with open(os.path.join(self.path_to_folder, self.header.filename), 'w') as file:
            file.write(self.header.get_text())

    def print_source(self) -> None:
        logging.debug(f"Module '{self.name}': Generating source file...")
        with open(os.path.join(self.path_to_folder, self.source.filename), 'w') as file:
            file.write(self.source.get_text())
            
    def generate(self) -> None:
        self.print_header()
        self.print_source()
"""
OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

import os
import logging
from typing import Dict

from .source_files import InterfaceHeader, InterfaceSource
from .message_handlers import Request
from .user_code import UserCode


class Interface:
    def __init__(
        self,
        name: str,
        author: str,
        version: str,
        copyright_notice: str,
        requests: Dict[str, Request] = {},
        header_user_codes: Dict[str, UserCode] = {},
        source_user_codes: Dict[str, UserCode] = {},
        path_to_folder: str = "",
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
        self.requests = requests
        self.header_user_codes: Dict[str, UserCode] = {}
        self.source_user_codes: Dict[str, UserCode] = {}
        self.path_to_folder = path_to_folder

        logging.debug(f"Creating interface '{self.name}'...")

        self.add_header_user_codes(header_user_codes)
        self.add_source_user_codes(source_user_codes)

    def add_header_user_codes(self, user_codes: Dict[str, UserCode]):
        logging.debug(f"Interface '{self.name}': Adding header user codes...")
        self.header_user_codes.update(user_codes)
        self.header = InterfaceHeader(
            name=self.name,
            author=self.author,
            version=self.version,
            copyright_notice=self.copyright_notice,
            requests=self.requests,
            user_codes=self.header_user_codes,
        )

    def add_source_user_codes(self, user_codes: Dict[str, UserCode]):
        logging.debug(f"Interface '{self.name}': Adding source user codes...")
        self.source_user_codes.update(user_codes)
        self.source = InterfaceSource(
            name=self.name,
            author=self.author,
            version=self.version,
            copyright_notice=self.copyright_notice,
            requests=self.requests,
            user_codes=self.source_user_codes,
        )

    def get_include(self) -> str:
        return f'"{self.header.filename}"'

    def print_header(self) -> None:
        logging.debug(f"Interface '{self.name}': Generating header file...")
        with open(os.path.join(self.path_to_folder, self.header.filename), "w") as file:
            text = self.header.get_text()
            file.write(text)

    def print_source(self) -> None:
        logging.debug(f"Interface '{self.name}': Generating source file...")
        with open(os.path.join(self.path_to_folder, self.source.filename), "w") as file:
            text = self.source.get_text()
            file.write(text)

    def generate(self) -> None:
        self.print_header()
        self.print_source()

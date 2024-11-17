"""
OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

from typing import Dict

from .module import Module


class Kernel:
    def __init__(self, kernel_number: int = 0) -> None:
        self.kernel_number = kernel_number
        self.modules: Dict[str, Module] = {}

    def add_module(self, module: Module) -> None:
        if module.kernel_nr != self.kernel_number:
            return
        if module.name not in self.modules:
            self.modules[module.name] = module

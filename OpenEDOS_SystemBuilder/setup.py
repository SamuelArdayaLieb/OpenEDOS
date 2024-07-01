"""
This is the OpenEDOS SystemBuilder v0.2.
(c) 2024 Samuel Ardaya-Lieb, MIT license

https://github.com/SamuelArdayaLieb/OpenEDOS
"""

from setuptools import setup

setup(
    name="OpenEDOS-SystemBuilder",
    version="0.2",
    author="Samuel Ardaya-Lieb",
    license="MIT",
    packages=["."],
    install_requires=[
        "click",
        "typing",
        "datetime",
        "pyyaml",
    ],
    entry_points={
        "console_scripts": [
            "openedos_systembuilder = main:openedos_systembuilder"
        ],
    },
)
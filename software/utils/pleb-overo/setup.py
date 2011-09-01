#! /usr/bin/env python

from distutils.core import setup, Extension

module1 = Extension('canbridge',
                    sources = ['canbridgemodule.c', 'microuptime.c', 'heap.c', 'input.c', 'output.c', 'packet.c', 'socket.c'])

setup (name = 'canbridge',
       version = '1.0',
       description = 'Canbridge software',
       ext_modules = [module1])
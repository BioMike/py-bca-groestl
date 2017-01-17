from distutils.core import setup, Extension

groestl_hash_module = Extension('groestl_hash', sources = ['groestlmodule.c', 'groestl.c'])

setup (name = 'groestl_hash',
       version = '1.0',
       ext_modules = [groestl_hash_module])
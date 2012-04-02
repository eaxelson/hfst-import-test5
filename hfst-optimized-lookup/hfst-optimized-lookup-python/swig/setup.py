#!/usr/bin/python

"""
Compile and install HFST's C++ lookup code with SWIG/Python bindings
"""

from distutils.core import setup, Extension

hfst_lookup = Extension('_hfst_lookup',
                        sources = ['transducer.cc', 'HfstFlagDiacritics.cc',
                                   'HfstExceptionDefs.cc', 'HfstSymbolDefs.cc',
                                   'hfst_lookup_wrap.cxx'])

setup(name = 'hfst_lookup',
      version = '1.0',
      author = 'sam.hardwick@iki.fi',
      description = '''HFST's C++ fast lookup''',
      ext_modules = [hfst_lookup],
      py_modules = ['hfst_lookup'])
       

#!/usr/bin/python

"""
setup for HFST-swig
"""

import os, sys, itertools
from distutils.core import setup, Extension

def flatten(lst):
    return itertools.chain.from_iterable(lst)

libhfst_src_path = '../libhfst/src/'

def libsrc_and_wrap(filenames):
    return list(flatten([[os.path.abspath(libhfst_src_path + filename),
                          os.path.abspath(filename.split('.')[0] + \
                                              '_wrap.cxx')]
                         for filename in filenames]))

libhfst_module = Extension('_libhfst',
                           sources=libsrc_and_wrap(('HfstTransducer.cc',
                                                    'HfstInputStream.cc',
                                                    'HfstOutputStream.cc')))

setup(name = 'libhfst_swig',
      version = '3.0.3_beta',
      author = 'HFST team',
      description = 'SWIG-bound libhfst interface',
      ext_modules = [libhfst_module],
      py_modules = ["libhfst"]
      )

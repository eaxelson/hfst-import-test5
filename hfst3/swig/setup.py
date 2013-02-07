#!/usr/bin/python

"""
setup for HFST-swig
"""

import os
from distutils.core import setup, Extension

libhfst_src_path = '../libhfst/src/'
absolute_libhfst_src_path = os.path.abspath(libhfst_src_path)

libhfst_module = Extension('_libhfst',
                           language = "c++",
                           sources = ["libhfst.i"],
                           swig_opts = ["-c++",
                                        "-I" + absolute_libhfst_src_path, "-Wall"],
                           include_dirs = [absolute_libhfst_src_path],
                           library_dirs = [absolute_libhfst_src_path + "/.libs"],
                           libraries = ["hfst"]
                           )

setup(name = 'libhfst_swig',
      version = '3.4.3_beta',
      author = 'HFST team',
      author_email = 'hfst-bugs@helsinki.fi',
      url = 'http://hfst.sourceforge.net',
      description = 'SWIG-bound libhfst interface',
      ext_modules = [libhfst_module],
      py_modules = ["libhfst"]
      )

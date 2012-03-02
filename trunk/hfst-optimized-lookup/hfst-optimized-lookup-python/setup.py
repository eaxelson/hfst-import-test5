import os
from setuptools import setup

# Utility function to read the README file.
def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()

setup(
    name = "hfst_lookup",
    version = "1.0",
    author = "Sam Hardwick",
    author_email = "sam.hardwick@iki.fi",
    description = ("A utility and library function to perform lookup on "
                                   "hfst-optimized-lookup transducers."),
    license = "Apache",
    keywords = "transducer lookup hfst",
    url = "http://hfst.sf.net",
    packages=['hfst_lookup'],
    long_description=read('README')
)


There are about 50 different HFST command line tools installed in this
directory. To be able to use the tools in any location, execute

  start hfst.bat

which will open up a new command window and temporarily add this directory to
the PATH environment variable so that you can freely move between directories.
If you chose to create a shortcut during HFST installation, there should be
one in the Start Menu (All Programs -> HFST -> hfst) which does the same thing.


To get information on the purpose of a tool and how it is used, run

  hfst-some-tool --help

which will print a help message. To test the tools, you can run for example

  echo a:b | hfst-calculate | hfst-invert | hfst-minimize | hfst-fst2strings

that should print

  b:a


Note that if a tool outputs directly to Windows console (i.e. not to the next
tool in a pipe or to a file), you need to use the switch --console or -k to 
make it sure that all unicode symbols are correctly printed.


For more information on HFST tools, see
<http://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstCommandLineTools>


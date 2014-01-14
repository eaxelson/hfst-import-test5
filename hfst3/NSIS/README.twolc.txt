
The tool hfst-twolc is installed in this directory. To be able to use the tool
in any location, execute

  start hfst-twolc.bat

which will open up a new command window and temporarily add this directory to
the PATH environment variable so that you can freely move between directories.
If you chose to create a shortcut during HFST installation, there should be
one in the Start Menu (All Programs -> HFST-TWOLC -> hfst-twolc) which does the
same thing.


To get information on the purpose of the tool, run

  hfst-twolc --help

which will print a help message. To test the tool, you can run for example

  [TODO]

that should print

  [TODO]


Note that if the tool outputs directly to Windows console (i.e. not to the next
tool in a pipe or to a file), you need to use the switch --console or -k to 
make it sure that all unicode symbols are correctly printed. [TODO check]


For more information on hfst-twolc, see
<http://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstTwolC> [TODO check]


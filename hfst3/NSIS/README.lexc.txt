
The tool hfst-lexc is installed in this directory. To be able to use the tool
in any location, execute

  start hfst-lexc-window.bat

which will open up a new command window and temporarily add this directory to
the PATH environment variable so that you can freely move between directories.
If you chose to create a shortcut during HFST installation, there should be
one in the Start Menu (All Programs -> HFST-LEXC -> hfst-lexc) which does the
same thing.


To get information on the purpose of the tool, run

  hfst-lexc --help

which will print a help message. To test the tool, you can run for example

  echo LEXICON Root > tmp.lexc
  echo lexicon # ; >> tmp.lexc
  echo léxico # ; >> tmp.lexc
  echo лексикон # ; >> tmp.lexc
  echo λεξικό # ; >> tmp.lexc  

and then 

  hfst-lexc tmp.lexc -o tmp.hfst
  type tmp.lexc | hfst-lexc > tmp.hfst

both of which write an HFST transducer that contains the four words listed
in file tmp.lexc.

Known bugs:

 - the tool doesn't work in interactive mode, you must either give a
   filename or catenate the file in standard input
   

For more information on hfst-lexc, see
<http://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstLexc> [TODO check]


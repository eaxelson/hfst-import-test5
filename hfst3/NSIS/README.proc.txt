
The tool hfst-proc is installed in this directory. To be able to use the tool
in any location, execute

  start hfst-proc-window.bat

which will open up a new command window and temporarily add this directory to
the PATH environment variable so that you can freely move between directories.
If you chose to create a shortcut during HFST installation, there should be
one in the Start Menu (All Programs -> HFST-PROC -> hfst-proc) which does the
same thing.


To get information on the purpose of the tool, run

  hfst-proc --help

which will print a help message. To test the tool, you can open it in
interactive mode (just writing 'hfst-proc') and write for example

  regex Ä:é;
  print net

which should output something like

  Ss0:    <Ä:é> -> fs1.
  fs1:    (no arcs).

If the unicode characters are not shown correctly, try running

  hfst-proc -k

The option -k (or --output-to-console) tells hfst-proc that the output is not
going to be redirected to a file or given to next command in pipe but printed
directly to the Windows console.

You can also try non-interactive mode:

  echo regex Ä:é; > tmp
  echo print net >> tmp

and then

  type tmp | hfst-proc --pipe-mode (-k)
  hfst-proc -F tmp (-k)  

both of which should print the same thing as above.


For more information on hfst-proc, see
<http://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstProc>


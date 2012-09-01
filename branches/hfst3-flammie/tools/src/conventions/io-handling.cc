//! @file io-handling.cc
//! for hfst files and streams in tools

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <HfstDataTypes.h>

using hfst::HfstInputStream;
using hfst::HfstOutputStream;

#include "commandline.h"
#include "io-handling.h"

// define defaults and globals for i/o handling
hfst_tool_io hfst_iomode = AUTOM_IN_AUTOM_OUT;
hfst_input_count inputs = READ_ONE;
char* outfilename = 0;
FILE* outfile= 0;
hfst::HfstOutputStream* outstream = 0;
bool output_named = false;
char* inputfilename = 0;
FILE* inputfile = 0;
hfst::HfstInputStream* instream = 0;
bool input_named = false;
char* firstfilename = 0;
FILE* firstfile = stdin;
hfst::HfstInputStream* firststream = 0;
bool first_named = false;
char *secondfilename = 0;
FILE *secondfile = stdin;
hfst::HfstInputStream* secondstream = 0;
bool second_named = false;
char** inputfilenames = 0;
FILE** inputfiles = 0;
hfst::HfstInputStream** instreams = 0;
size_t inputs_named = 0;
bool is_input_stdin = true;
bool is_output_stdout = true;
hfst::ImplementationType format = hfst::UNSPECIFIED_TYPE;

void
hfst_open_streams()
  {
    hfst_debug("Opening streams in %d, %d mode\n", hfst_iomode, inputs);
    if ((hfst_iomode == AUTOM_IN_AUTOM_OUT) ||
        (hfst_iomode == AUTOM_IN_FILE_OUT))
      {
        // Reading automata
        try
          {
            switch (inputs)
              {
              case READ_ONE:
                if (strcmp(inputfilename, "<stdin>") != 0)
                  {
                    hfst_verbose("Opening %s for reading automata", inputfilename);
                    instream = new HfstInputStream(inputfilename);
                  }
                else
                  {
                    hfst_verbose("Opening STANDARD INPUT for reading automata");
                    instream = new HfstInputStream();
                  }
                format = instream->get_type();
                break;
              case READ_TWO:
                if (strcmp(firstfilename, "<stdin>") != 0)
                  {
                    hfst_verbose("Opening %s for reading first (set of) "
                                 "automata", firstfilename);
                    firststream = new HfstInputStream(firstfilename);
                  }
                else
                  {
                    hfst_verbose("Opening STANDARD INPUT for first (set of) "
                                 "reading automata");
                    firststream = new HfstInputStream();
                  }
                if (strcmp(secondfilename, "<stdin>") != 0)
                  {
                    hfst_verbose("Opening %s for reading second (set of) "
                                 "automata", secondfilename);
                    secondstream = new HfstInputStream(secondfilename);
                  }
                else
                  {
                    hfst_verbose("Opening STANDARD INPUT for second (set of) "
                                 "reading automata");
                    secondstream = new HfstInputStream();
                  }
                format = firststream->get_type();
                break;
              case READ_MANY:
                for (unsigned int i = 0; i < inputs_named; i++)
                  {
                    if (strcmp(inputfilenames[i], "<stdin>") != 0)
                      {
                        hfst_verbose("Opening %s for reading %u/%u (set of) "
                                     "automata", inputfilenames[i], i + 1,
                                     inputs_named);
                        instreams[i] = new HfstInputStream(inputfilenames[i]);
                      }
                    else
                      {
                        hfst_verbose("Opening STANDARD INPUT for %u/%u (set of)"
                                     " automata", i + 1, inputs_named);
                        instreams[i] = new HfstInputStream();
                      }
                  }
                format = instreams[0]->get_type();
              }
          }
        catch (NotTransducerStreamException ntse)
          {
            if (inputs == READ_ONE)
              {
                hfst_error(EXIT_FAILURE, 0, "Input data in %s are not "
                           "valid HFST automata", inputfilename);
              }
            else if (inputs == READ_TWO)
              {
                hfst_error(EXIT_FAILURE, 0, "Input data in %s and %s are not "
                           "valid HFST automata", 
                           firstfilename, secondfilename);
              }
            else
              {
                hfst_error(EXIT_FAILURE, 0, "Some of the input data is not "
                           "like HFST automaton...");
              }
          }
      }
    else if ((hfst_iomode == FILE_IN_AUTOM_OUT) ||
             (hfst_iomode == FILE_IN_FILE_OUT))
      {
        // reading files
        switch (inputs)
          {
          case READ_ONE:
            if (strcmp(inputfilename, "<stdin>") != 0)
              {
                hfst_verbose("Opening %s for reading data", inputfilename);
                inputfile = hfst_fopen(inputfilename, "r");
              }
            else
              {
                hfst_verbose("Using STANDARD INPUT for reading data");
                inputfile = stdin;
              }
            break;
          case READ_TWO:
            if (strcmp(firstfilename, "<stdin>") != 0)
              {
                hfst_verbose("Opening %s for reading first set of data",
                             firstfilename);
                firstfile = hfst_fopen(firstfilename, "r");
              }
            else
              {
                hfst_verbose("Using STANDARD INPUT for reading first "
                             "set of data");
                firstfile = stdin;
              }
            if (strcmp(secondfilename, "<stdin>") != 0)
              {
                hfst_verbose("Opening %s for reading second set of data",
                             secondfilename);
                secondfile = hfst_fopen(secondfilename, "r");
              }
            else
              {
                hfst_verbose("Using STANDARD INPUT for reading second "
                             "set of data");
                secondfile = stdin;
              }
            break;
          case READ_MANY:
            for (unsigned int i = 0; i < inputs_named; i++)
              {
                if (strcmp(inputfilenames[i], "<stdin>") != 0)
                  {
                    hfst_verbose("Opening %s for reading %u/%u set of data",
                                 inputfilenames[i], i + 1, inputs_named);
                    inputfiles[i] = hfst_fopen(inputfilenames[i], "r");
                  }
                else
                  {
                    hfst_verbose("Using STANDARD INPUT for reading %u/%u "
                                 "set of data", i + 1, inputs_named);
                    inputfiles[i] = stdin;
                  }
              }
          }
      }
    if ((hfst_iomode == AUTOM_IN_AUTOM_OUT) || (hfst_iomode == FILE_IN_AUTOM_OUT))
      {
        if (strcmp(outfilename, "<stdout>") != 0)
          {
            hfst_verbose("Opening %s to write %s automata", outfilename,
                         hfst_strformat(format));
            outstream = 
              new HfstOutputStream(outfilename, format);
          }
        else
          {
            hfst_verbose("Using STANDARD OUTPUT to write %s automata",
                         hfst_strformat(format));
            outstream = new HfstOutputStream(format);
          }
      }
    else if ((hfst_iomode == FILE_IN_FILE_OUT) || (hfst_iomode == AUTOM_IN_FILE_OUT))
      {
        if (!is_output_stdout)
          {
            hfst_verbose("Opening %s for data output", outfilename);
            outfile = hfst_fopen(outfilename, "w");
          }
        else
          {
            hfst_verbose("Using STANDARD OUTPUT for data output");
            outfile = stdout;
          }
      }
  }



    if (firstNamed && secondNamed)
      {
        if ((argc - optind) > 0)
          {
            error(EXIT_FAILURE, 0,
              "No more than two transducer files may be given");
          }
      }
    else if (!firstNamed && !secondNamed)
      {
        // neither input given in options:
        if ((argc - optind) == 2)
          {
            firstfilename = hfst_strdup(argv[optind]);
            firstfile = hfst_fopen(firstfilename, "r");
            secondfilename = hfst_strdup(argv[optind+1]);
            secondfile = hfst_fopen(secondfilename, "r");
            is_input_stdin = false;
          }
        else if ((argc - optind) == 1)
          {
            secondfilename = hfst_strdup(argv[optind]);
            secondfile = hfst_fopen(secondfilename, "r");
            firstfilename = hfst_strdup("<stdin>");
            firstfile = stdin;
            is_input_stdin = true;
          }
        else if ((argc - optind) > 2)
          {
            error(EXIT_FAILURE, 0,
                  "no more than two transducer filenames may be given");
          }
        else
          {
            error(EXIT_FAILURE, 0,
                     "at least one input must be from a named file");
          }
    }
    else if (!firstNamed)
    {
        if ((argc - optind) == 1)
        {
            firstfilename = hfst_strdup(argv[optind]);
            firstfile = hfst_fopen(firstfilename, "r");
            is_input_stdin = false;
        }
        else if ((argc - optind) == 0)
        {
            firstfilename = hfst_strdup("<stdin>");
            firstfile = stdin;
            is_input_stdin = true;
        }
        else 
          {
            error(EXIT_FAILURE, 0,
                  "no more than two transducer filenames may be given");
          }
    }
    else if (!secondNamed)
    {
        if ((argc - optind) == 1)
        {
            secondfilename = hfst_strdup(argv[optind]);
            secondfile = hfst_fopen(secondfilename, "r");
            is_input_stdin = false;
        }
        else if ((argc - optind) == 0)
        {
            secondfilename = hfst_strdup("<stdin>");
            secondfile = stdin;
            is_input_stdin = true;
        }
        else
          {
            error(EXIT_FAILURE, 0,
                  "no more than two transducer filenames may be given");
          }
    }
    else
    {
        error(EXIT_FAILURE, 0,
                "at least one transducer filename must be given");
    }


  // common-unary-cases.h
  case 'i':
  inputfilename = hfst_strdup(optarg);
  if (strcmp(inputfilename, "-") == 0) {
    inputfilename = hfst_strdup("<stdin>");
    inputfile = stdin;
    is_input_stdin = true;
  }
  else {
    inputfile = hfst_fopen(inputfilename, "r");
    is_input_stdin = false;
  }
  break;
  case 'o':
  outfilename = hfst_strdup(optarg);
  if (strcmp(outfilename, "-") == 0) {
    outfilename = hfst_strdup("<stdout>");
    outfile = stdout;
    is_output_stdout = true;
  }
  else {
    outfile = hfst_fopen(outfilename, "w");
    is_output_stdout = false;
  }
  message_out = stdout;
  break;
  case 'R':
  read_symbols_from_filename = hfst_strdup(optarg);
  break;
  case 'D':
  write_symbols = false;
  break;
  case 'W':
  write_symbols_to_filename = hfst_strdup(optarg);
  break;

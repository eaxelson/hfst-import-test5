  case 'd':
  debug = true;
  break;
  case 'h':
  print_usage(argv[0]);
  return EXIT_SUCCESS;
  break;
  case 'V':
  // See, this program name should be static since it’s for bug
  // reports, while the other must be argv[0], as it’s useful
  // for end user.
  print_version(argv[0]);
  return EXIT_SUCCESS;
  break;
  case 'v':
  verbose = true;
  break;
  case 'q':
  verbose = false;
  break;
  case 's':
  verbose = false;
  break;

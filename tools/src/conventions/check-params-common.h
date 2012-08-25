
if (!outputNamed)
  {
    outfilename = hfst_strdup("<stdout>");
    outfile = stdout;
    message_out = stderr;
  }
if (auto_colors)
  {
    if (message_out == stdout)
      {
        if (isatty(1) && isatty(2))
          {
            print_colors = true;
          }
        else
          {
            print_colors = false;
          }
      }
    else
      {
        if (isatty(2))
          {
            print_colors = true;
          }
        else
          {
            print_colors = false;
          }
      }
  }

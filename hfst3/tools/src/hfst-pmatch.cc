#include "hfst-pmatch.h"

std::string input_file_name;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] TRANSDUCER\n"
           "perform matching/lookup on text streams\n"
        "\n", program_name);

    print_common_program_options(message_out);
    fprintf(message_out, 
        "Input/Output options:\n"
        "  -i, --input=INFILE     Read input transducer from INFILE\n"
        "  -o, --output=OUTFILE   Write output to OUTFILE\n");

    fprintf(message_out, "Pmatch options:\n");
//            "  -e, --epsilon-format=EPS         Print epsilon as EPS\n"
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
    fprintf(message_out, "\n");
    fprintf(message_out, "\n");
    print_report_bugs();
    fprintf(message_out, "\n");
    print_more_info();
}

// hfst_ol::PmatchContainer process_transducers(std::ifstream & inputstream)
// {
//     hfst_ol::PmatchContainer container(inputstream);
//     while (inputstream.good()) {
//      try {
//          transducer_name = parse_hfst3_header(inputstream);
//      } catch (TransducerHeaderException & e) {
//          break;
//      }
//         container.add_rtn(
//          new hfst_ol::PmatchTransducer(inputstream, false), transducer_name);
//     }
//     find_rtns(container);
//     return container;
// }

void print_result(std::string res)
{
    std::cout << res << std::endl;;
}

void process_input(hfst_ol::PmatchContainer & container)
{
    std::string input_line;
    while (std::cin) {
        getline(std::cin, input_line);
        print_result(container.match(input_line));
 }

    // std::istream user_input = std::cin;
    // while(!getline(user_input, input_line).failbit) {
    //     print_result(container.match(input_line));
    // }

}


// std::map<std::string, std::string> get_possible_transducers_from_path()
// {
//     return std::map<std::string, std::string>();
// //     char tmp[MAXPATHLEN];
// //     std::string cwd_name = getcwd(tmp, MAXPATHLEN) ?
// //         std::string(tmp) : std::string("");

// }

void find_rtns(hfst_ol::PmatchContainer & cont)
{
    if (cont.has_unsatisfied_rtns()) {
        error(EXIT_FAILURE, 0,
              "Couldn't find referred transducer"); 
    }
    // std::map<std::string, std::string> transducer_library =
    //     get_possible_transducers_from_path();
    // std::map<std::string, std::string>::iterator it;
    // while (cont.has_unsatisfied_rtns()) {
    //     it = transducer_library.find(cont.get_unsatisfied_rtn_name());
    //     if (it != transducer_library.end()) {
    //         HfstInputStream in(it->second);
    //         HfstTransducer rtn(in);
    //         force_ol(rtn);
    //         cont.add_rtn(rtn);
    //     } else {
    //         error(EXIT_FAILURE, 0,
    //               "Couldn't find referred transducer");
    //     }
    // }

}

int parse_options(int argc, char** argv)
{
    int c;
    while (true)
    {
        static const struct option long_options[] =
        {
            // first the hfst-mandated options
          {"help",         no_argument,       0, 'h'},
          {"version",      no_argument,       0, 'V'},
          {"verbose",      no_argument,       0, 'v'},
          {"quiet",        no_argument,       0, 'q'},
          {"silent",       no_argument,       0, 's'},
          {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        c = getopt_long(argc, argv,
                        "hVvqs",
                        long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
        case 'h':
          print_usage();
          return EXIT_SUCCESS;
          break;
        case 'V':
          print_version();
          return EXIT_SUCCESS;
          break;
        default:
            std::cerr << "Invalid option\n\n";
            return EXIT_FAILURE;
            break;
        }
    }

    // no more options, we should now be at the input filename
  if ( (optind + 1) < argc)
    {
      std::cerr << "More than one input file given\n";
      return EXIT_FAILURE;
    }
  else if ( (optind + 1) == argc)
    {
        input_file_name = argv[(optind)];
        return EXIT_CONTINUE;
    }
  else
    {
      std::cerr << "No input file given\n";
      return EXIT_FAILURE;
    }

    return EXIT_CONTINUE;
}


int main(int argc, char ** argv)
{
    hfst_setlocale();
    hfst_set_program_name(argv[0], "0.1", "HfstPmatch");
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE) {
        return retval;
    }
    std::ifstream instream(input_file_name.c_str(),
                           std::ifstream::binary);
    hfst_ol::PmatchContainer container(instream);
                                                        
    process_input(container);
    return EXIT_SUCCESS;
}

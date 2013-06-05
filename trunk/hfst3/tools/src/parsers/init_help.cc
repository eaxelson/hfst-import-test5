// Based on Mans Hulden's foma.
namespace hfst {
  namespace xfst {

    typedef std::vector<std::pair<std::string, std::string> > StringPairVector;
    typedef std::map<hfst::xfst::XfstCommand, StringPairVector > Command2HelpMessages;

    Command2HelpMessages help_messages;

    StringPairVector get_help_messages(hfst::xfst::XfstCommand cmd)
    {
      Command2HelpMessages::const_iterator it = help_messages.find(cmd);
      if (it == help_messages.end())
        {
          StringPairVector retval;
          return retval;
        }
      return it->second;
    }

    void add_help_message(hfst::xfst::XfstCommand cmd, const std::string & name, const std::string & msg)
    {
      help_messages[cmd].push_back(std::pair<std::string, std::string>(name, msg));
    }

    void init_help_messages()
    {
      add_help_message(READ_REGEX_CMD, "read regex", "read a regular expression");
      add_help_message(AMBIGUOUS_CMD, "ambiguous upper", "returns the input words which have multiple paths in a transducer");
      add_help_message(APPLY_UP_CMD, "apply up <string>", "apply <string> up to the top network on stack");
      add_help_message(APPLY_UP_CMD, "apply up", "enter apply up mode (Ctrl-D exits)");
      add_help_message(APPLY_DOWN_CMD, "apply down <string>", "apply <string> down to the top network on stack");
      add_help_message(APPLY_DOWN_CMD, "apply down", "enter apply down mode (Ctrl-D exits)");
      add_help_message(APPLY_MED_CMD, "apply med <string>", "find approximate matches to string in top network by minimum edit distance");
      add_help_message(APROPOS_CMD, "apropos <string>", "search help for <string>");
      add_help_message(CLEAR_CMD, "clear stack", "clears the stack");
      add_help_message(COMPACT_SIGMA_CMD, "compact sigma", "removes redundant symbols from FSM");
      add_help_message(COMPLETE_CMD, "complete net", "completes the FSM");
      add_help_message(COMPOSE_CMD, "compose net", "composes networks on stack");
      add_help_message(CONCATENATE_CMD, "concatenate", "concatenates networks on stack");
      add_help_message(CROSSPRODUCT_CMD, "crossproduct net", "cross-product of top two FSMs on stack");
      add_help_message(DEFINE_CMD, "define <name> <r.e.>", "define a network");
      add_help_message(DEFINE_CMD, "define <fname>(<v1,..,vn>) <r.e.>", "define function");
      add_help_message(DETERMINIZE_CMD, "determinize net", "determinizes top FSM on stack");
      add_help_message(ECHO_CMD, "echo <string>", "echo a string");
      add_help_message(ELIMINATE_FLAG_CMD, "eliminate flag <name>", "eliminate flag <name> diacritics from the top network");
      add_help_message(ELIMINATE_ALL_CMD, "eliminate flags", "eliminate all flag diacritics from the top network");
      //add_help_message("export cmatrix (filename)", "export the confusion matrix as an AT&T transducer");
      add_help_message(EXTRACT_AMBIGUOUS_CMD, "extract ambiguous", "extracts the ambiguous paths of a transducer");
      add_help_message(EXTRACT_AMBIGUOUS_CMD, "extract unambiguous", "extracts the unambiguous paths of a transducer");
      add_help_message(DESCRIBE_CMD, "help", "lists all commands");
      add_help_message(DESCRIBE_CMD, "help <name>", "prints help message of a command");
      add_help_message(DESCRIBE_CMD, "help license", "prints license");
      add_help_message(DESCRIBE_CMD, "help warranty", "prints warranty information");
      add_help_message(IGNORE_CMD, "ignore net", "applies ignore to top two FSMs on stack");
      add_help_message(INTERSECT_CMD, "intersect net", "intersects FSMs on stack");
      add_help_message(INVERT_CMD, "invert net", "inverts top FSM");
      add_help_message(LABEL_CMD, "label net", "extracts all attested symbol pairs from FSM");
      //add_help_message("letter machine", "Converts top FSM to a letter machine");
      add_help_message(LOADS_CMD, "load stack <filename>", "Loads networks and pushes them on the stack");
      add_help_message(LOADD_CMD, "load defined <filename>", "Restores defined networks from file");
      add_help_message(LOWER_SIDE_CMD, "lower-side net", "takes lower projection of top FSM");
      add_help_message(MINIMIZE_CMD, "minimize net", "minimizes top FSM");
      add_help_message(MINUS_CMD, "minus net", "subtracts networks on stack");
      add_help_message(NAME_CMD, "name net <string>", "names top FSM");
      add_help_message(NEGATE_CMD, "negate net", "complements top FSM");
      add_help_message(ONE_PLUS_CMD, "one-plus net", "Kleene plus on top FSM");
      add_help_message(POP_CMD, "pop stack", "remove top FSM from stack");
      //add_help_message("print cmatrix", "prints the confusion matrix associated with the top network in tabular format");
      add_help_message(PRINT_DEFINED_CMD, "print defined", "prints defined symbols and functions");
      add_help_message(SAVE_DOT_CMD, "print dot (>filename)", "prints top FSM in Graphviz dot format");
      add_help_message(PRINT_LOWER_WORDS_CMD, "print lower-words", "prints words on the lower-side of top FSM");
      add_help_message(PRINT_NAME_CMD, "print name", "prints the name of the top FSM");
      add_help_message(PRINT_CMD, "print net", "prints all information about top FSM");
      add_help_message(PRINT_RANDOM_LOWER_CMD, "print random-lower", "prints random words from lower side");
      add_help_message(PRINT_RANDOM_UPPER_CMD, "print random-upper", "prints random words from upper side");
      add_help_message(PRINT_RANDOM_WORDS_CMD, "print random-words", "prints random words from top FSM");
      add_help_message(PRINT_SIGMA_CMD, "print sigma", "prints the alphabet of the top FSM");
      add_help_message(PRINT_SIZE_CMD, "print size", "prints size information about top FSM");
      add_help_message(PRINT_SHORTEST_STRING_CMD, "print shortest-string", "prints the shortest string of the top FSM");
      add_help_message(PRINT_SHORTEST_STRING_SIZE_CMD, "print shortest-string-size", "prints length of shortest string");
      add_help_message(PRUNE_NET_CMD, "prune net", "makes top network coaccessible");
      add_help_message(PUSH_DEFINED_CMD, "push (defined) <name>", "adds a defined FSM to top of stack");
      add_help_message(QUIT_CMD, "quit", "exit foma");
      add_help_message(READ_ATT_CMD, "read att <filename>", "read a file in AT&T FSM format and add to top of stack");
      //add_help_message("read cmatrix <filename>", "read a confusion matrix and associate it with the network on top of the stack");
      add_help_message(READ_PROLOG_CMD, "read prolog <filename>", "reads prolog format file");
      add_help_message(READ_LEXC_CMD, "read lexc <filename>", "read and compile lexc format file");
      add_help_message(READ_SPACED_CMD, "read spaced-text <filename>", "compile space-separated words/word-pairs separated by newlines into a FST");
      add_help_message(READ_TEXT_CMD, "read text <filename>", "compile a list of words separated by newlines into an automaton");
      add_help_message(REVERSE_CMD, "reverse net", "reverses top FSM");
      add_help_message(ROTATE_CMD, "rotate stack", "rotates stack");
      add_help_message(SAVE_DEFINITIONS_CMD, "save defined <filename>", "save all defined networks to binary file");
      add_help_message(SAVE_STACK_CMD, "save stack <filename>", "save stack to binary file");
      add_help_message(SET_CMD, "set <variable> <ON|OFF>", "sets a global variable (see show variables)");
      add_help_message(SHOW_CMD, "show variables", "prints all variable/value pairs");
      add_help_message(SHUFFLE_CMD, "shuffle net", "asynchronous product on top two FSMs on stack");
      add_help_message(SIGMA_CMD, "sigma net", "Extracts the alphabet and creates a FSM that accepts all single symbols in it");
      add_help_message(SOURCE_CMD, "source <file>", "read and compile script file");
      add_help_message(SORT_NET_CMD, "sort net", "sorts arcs topologically on top FSM");
      add_help_message(SORT_IN_CMD, "sort in", "sorts input arcs by sigma numbers on top FSM");
      add_help_message(SORT_OUT_CMD, "sort out", "sorts output arcs by sigma number on top FSM");
      add_help_message(SUBSTITUTE_NAMED_CMD, "substitute defined X for Y", "substitutes defined network X at all arcs containing Y ");
      add_help_message(SUBSTITUTE_SYMBOL_CMD, "substitute symbol X for Y", "substitutes all occurrences of Y in an arc with X");
      add_help_message(SYSTEM_CMD, "system <cmd>", "execute a system command");
      add_help_message(TEST_UNAMBIGUOUS_CMD, "test unambiguous", "test if top FST is unambiguous");
      add_help_message(TEST_EQ_CMD, "test equivalent", "test if the top two FSMs are equivalent");
      add_help_message(TEST_FUNCT_CMD, "test functional", "test if the top FST is functional (single-valued)");
      add_help_message(TEST_ID_CMD, "test identity", "test if top FST represents identity relations only");
      add_help_message(TEST_LOWER_UNI_CMD, "test lower-universal", "test if lower side is Σ*");
      add_help_message(TEST_UPPER_UNI_CMD, "test upper-universal", "test if upper side is Σ*");
      add_help_message(TEST_NONNULL_CMD, "test non-null", "test if top machine is not the empty language");
      add_help_message(TEST_NULL_CMD, "test null", "test if top machine is the empty language");
      //add_help_message("test sequential", "tests if top machine is sequential");
      //add_help_message("test star-free", "test if top FSM is star-free");
      add_help_message(TURN_CMD, "turn stack", "turns stack upside down");
      add_help_message(TWOSIDED_FLAGS_CMD, "twosided flag-diacritics", "changes flags to always be identity pairs");
      add_help_message(UNDEFINE_CMD, "undefine <name>", "remove <name> from defined networks");
      add_help_message(UNION_CMD, "union net", "union of top two FSMs");
      add_help_message(UPPER_SIDE_CMD, "upper-side net", "upper projection of top FSM");
      add_help_message(VIEW_CMD, "view net", "display top network (if supported)");
      add_help_message(ZERO_PLUS_CMD, "zero-plus net", "Kleene star on top fsm");
      /*add_help_message("variable compose-tristate", "use the tristate composition algorithm");
      add_help_message("variable show-flags", "show flag diacritics in `apply'");
      add_help_message("variable obey-flags", "obey flag diacritics in `apply'");
      add_help_message("variable minimal", "minimize resulting FSMs");
      add_help_message("variable print-pairs", "always print both sides when applying");
      add_help_message("variable print-space", "print spaces between symbols");
      add_help_message("variable print-sigma", "print the alphabet when printing network");*/
      //add_help_message("quit-on-fail", "Abort operations when encountering errors");
      /*add_help_message("variable recursive-define", "Allow recursive definitions");
      add_help_message("variable verbose", "Verbosity of interface");
      add_help_message("variable hopcroft-min", "ON = Hopcroft minimization, OFF = Brzozowski minimization");
      add_help_message("variable med-limit", "the limit on number of matches in apply med");
      add_help_message("variable med-cutoff", "the cost limit for terminating a search in apply med");
      add_help_message("variable att-epsilon", "the EPSILON symbol when reading/writing AT&T files");*/
      add_help_message(SAVE_PROLOG_CMD, "write prolog (> filename)", "writes top network to prolog format file/stdout");
      add_help_message(WRITE_ATT_CMD, "write att (> <filename>)", "writes top network to AT&T format file/stdout");
    }
  }
}




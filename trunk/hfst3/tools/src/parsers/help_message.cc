#define COMMAND(x, y) if (command_matches(text, x)) { return help_message(x, y); }
#define CASE_()

bool command_matches(const char * text, const char * commands)
{
  return false;
}

const char * help_message(const char * commands, const char * description)
{
  return NULL;
}

bool get_help_message(const std::string & text, std::string & message, bool all_messages /*=false*/)
{
  COMMAND("ambiguous upper, ambiguous", "returns the input words which have multiple paths in a transducer");
  AMBIGUOUS_COMMAND("apply down, down", "apply down <string>, apply down");
  COMMAND("apply down <string>, down <string>" "apply <string> down to the top network on stack");
  COMMAND("apply down, down" "enter apply down mode (Ctrl-D exits)");
  AMBIGUOUS_COMMAND("apply up, up", "apply up <string>, apply up");
  COMMAND("apply up <string>, up <string>" "apply <string> up to the top network on stack");
  COMMAND("apply up, up" "enter apply up mode (Ctrl-D exits)");
  //COMMAND("apply med <string>, apply med" "find approximate matches to string in top network by minimum edit distance");
  COMMAND("apropos <string>, apropos" "search help for <string>");
  COMMAND("add properties, add", "<no description>");
  COMMAND("alias", "<no description>");
  COMMAND("cleanup net, cleanup", "<no description>");
  COMMAND("clear stack, clear", "clear stack" "clears the stack");
  COMMAND("collect epsilon-loops, epsilon-loops", "<no description>");
  //"compile-replace lower, com-rep lower"
  //"compile-replace upper, com-rep upper"
  COMMAND("compact sigma" "removes redundant symbols from FSM");
  COMMAND("complete net, comlete" "completes the FSM");
  COMMAND("compose net, compose" "composes networks on stack");
  COMMAND("concatenate net, concatenate" "concatenates networks on stack");
  COMMAND("crossproduct net, crossproduct" "cross-product of top two FSMs on stack");
  AMBIGUOUS_COMMAND("define", "define <name> <r.e.>, define <fname>(<v1,..,vn>) <r.e.>)");
  COMMAND("define <name> <r.e.>" "define a network");
  COMMAND("define <fname>(<v1,..,vn>) <r.e.>" "define function");
  COMMAND("determinize net, determinize, determinize net, determinise", "determinizes top FSM on stack"); 
  COMMAND("echo <string>, echo" "echo a string");
  COMMAND("edit properties, edit", "<no description>");
  //"epsilon-remove net, epsilon-remove"
  COMMAND("eliminate flag <name>" "eliminate flag <name> diacritics from the top network");
  COMMAND("eliminate flags" "eliminate all flag diacritics from the top network");
  COMMAND("export cmatrix (filename)" "export the confusion matrix as an AT&T transducer");
  COMMAND("extract ambiguous" "extracts the ambiguous paths of a transducer");
  COMMAND("extract unambiguous" "extracts the unambiguous paths of a transducer");
  COMMAND("hfst", "<no description>");
  COMMAND("ignore net, ignore" "applies ignore to top two FSMs on stack");
  COMMAND("intersect net, intersect, conjunct net, conjunct" "intersects FSMs on stack");
  COMMAND("invert net, invert" "inverts top FSM");
  COMMAND("inspect, inspect net", "<no description>");
  AMBIGUOUS_COMMAND("help", "help, help <name>, help licence, help warranty");
  COMMAND("help", "lists all commands");
  COMMAND("help <name>", "prints help message of a command");
  COMMAND("help license, help licence", "prints license");
  COMMAND("help warranty", "prints warranty information");
  COMMAND("label net" "extracts all attested symbol pairs from FSM");
  COMMAND("letter machine" "Converts top FSM to a letter machine");
  COMMAND("list", "<no description>");
  AMBIGUOUS_COMMAND("load", "load stack, load defined");
  COMMAND("load stack <filename>, load <filename>" "Loads networks and pushes them on the stack");
  COMMAND("load defined <filename>, loadd <filename>" "Restores defined networks from file");
  COMMAND("lower-side net, lower-side" "takes lower projection of top FSM");
  COMMAND("minimize net, minimize, minimise net, minimise", "minimizes top FSM");
  COMMAND("minus net, minus, subtract", "subtracts networks on stack");
  COMMAND("name net <string>, name", "names top FSM");
  COMMAND("negate net, negate", "complements top FSM");
  COMMAND("one-plus net, one-plus", "Kleene plus on top FSM");
  return NULL;
}





"pop, pop stack"
"print aliases, aliases"
"print arc-tally, arc-tally"
"print defined, pdefined"
"write definition, wdef, write definitions, wdefs"
"print directory, directory"
"write dot, wdot, dot"
"write att, att"
"print file-info, file-info"
"print flags, flags"
"print labels, labels"
"print label-maps, label-maps"
"print label-tally, label-tally"
"print list"
"print lists"
"print longest-string, longest-string, pls"
"print longest-string-size, longest-string-size, plz"
"print lower-words, lower-words"
"print name, pname"
"print net"
"print properties, print props, write properties, write props"
"print random-lower, random-lower"
"print random-upper, random-upper"
"print random-words, random-words"
"print shortest-string-size, print shortest-string-length, "
"print shortest-string, shortest-string, pss"
"print sigma, sigma"
"print sigma-tally, sigma-tally, sitally, print sigma-word-tally"
"print size, size"
"print stack, stack"
"print upper-words, upper-words"
"print words, words"
"prune net, prune"
"push, push defined"
"quit, exit, bye, stop, hyvästi, au revoir, näkemiin, viszlát, auf wiedersehen, has"
"lexc, read lexc"
"att, read att"
"read properties, rprops"
"read prolog, rpl"
"regex, read regex"
"rs, read spaced-text"
"rt, read text"
"reverse net, reverse"
"rotate, rotate stack"
"save defined, saved"
"save stack, save, ss"
"set"
"show variables"
"show variable, show"
"shuffle net, shuffle"
"sigma net"
"sort net, sort"
"sort in"
"sort out"
"source"
"substitute defined"
"for"
"substitute label"
"substitute symbol"
"substring net, substring"
"system"
"test equivalent, equivalent, te"
"test functional, functional, tf"
"test identity, identity, ti"
"test lower-bounded, lower-bounded, tlb"
"test lower-universal, lower-universal, tlu"
"test non-null, tnn"
"test null, tnu"
"test overlap, overlap, to"
"test sublanguage, sublanguage, ts"
"test upper-bounded, upper-bounded, tub"
"test upper-universal, upper-universal, tuu"
"test unambiguous"
"turn, turn stack"
"twosided flag-diacritics, tfd"
"undefine"
"unlist"
"union net, union, disjunct"
"upper-side net, upper-side"
"view net"
"wpl, write prolog, wspaced-text"
"write spaced-text"
"wt, write text"
"zero-plus net, zero-plus"


"read regex" "read a regular expression"






"pop stack" "remove top FSM from stack"
"print cmatrix" "prints the confusion matrix associated with the top network in tabular format"
"print defined" "prints defined symbols and functions"
"print dot (>filename)" "prints top FSM in Graphviz dot format"
"print lower-words" "prints words on the lower-side of top FSM"
"print name" "prints the name of the top FSM"
"print net" "prints all information about top FSM"
"print random-lower" "prints random words from lower side"
"print random-upper" "prints random words from upper side"
"print random-words" "prints random words from top FSM"
"print sigma" "prints the alphabet of the top FSM"
"print size" "prints size information about top FSM"
"print shortest-string" "prints the shortest string of the top FSM"
"print shortest-string-size" "prints length of shortest string"
"prune net" "makes top network coaccessible"
"push (defined) <name>" "adds a defined FSM to top of stack"
"quit" "exit foma"
"read att <filename>" "read a file in AT&T FSM format and add to top of stack"
"read cmatrix <filename>" "read a confusion matrix and associate it with the network on top of the stack"
"read prolog <filename>" "reads prolog format file"
"read lexc <filename>" "read and compile lexc format file"
"read spaced-text <filename>" "compile space-separated words/word-pairs separated by newlines into a FST"
"read text <filename>" "compile a list of words separated by newlines into an automaton"
"reverse net" "reverses top FSM"
"rotate stack" "rotates stack"
"save defined <filename>" "save all defined networks to binary file"
"save stack <filename>" "save stack to binary file"
"set <variable> <ON|OFF>" "sets a global variable (see show variables)"
"show variables" "prints all variable/value pairs"
"shuffle net" "asynchronous product on top two FSMs on stack"
"sigma net" "Extracts the alphabet and creates a FSM that accepts all single symbols in it"
"source <file>" "read and compile script file"
"sort net" "sorts arcs topologically on top FSM"
"sort in" "sorts input arcs by sigma numbers on top FSM"
"sort out" "sorts output arcs by sigma number on top FSM"
"substitute defined X for Y" "substitutes defined network X at all arcs containing Y "
"substitute symbol X for Y" "substitutes all occurrences of Y in an arc with X"
"system <cmd>" "execute a system command"
"test unambiguous" "test if top FST is unambiguous"
"test equivalent" "test if the top two FSMs are equivalent"
"test functional" "test if the top FST is functional (single-valued)"
"test identity" "test if top FST represents identity relations only"
"test lower-universal" "test if lower side is Σ*"
"test upper-universal" "test if upper side is Σ*"
"test non-null" "test if top machine is not the empty language"
"test null" "test if top machine is the empty language"
"test sequential" "tests if top machine is sequential"
"test star-free" "test if top FSM is star-free"
"turn stack" "turns stack upside down"
"twosided flag-diacritics" "changes flags to always be identity pairs"
"undefine <name>" "remove <name> from defined networks"
"union net" "union of top two FSMs"
"upper-side net" "upper projection of top FSM"
"view net" "display top network (if supported)"
"zero-plus net" "Kleene star on top fsm"
"variable compose-tristate" "use the tristate composition algorithm"
"variable show-flags" "show flag diacritics in `apply'"
"variable obey-flags" "obey flag diacritics in `apply'"
"variable minimal" "minimize resulting FSMs"
"variable print-pairs" "always print both sides when applying"
"variable print-space" "print spaces between symbols"
"variable print-sigma" "print the alphabet when printing network"
"quit-on-fail" "Abort operations when encountering errors"
"variable recursive-define" "Allow recursive definitions"
"variable verbose" "Verbosity of interface"
"variable hopcroft-min" "ON = Hopcroft minimization, OFF = Brzozowski minimization"
"variable med-limit" "the limit on number of matches in apply med"
"variable med-cutoff" "the cost limit for terminating a search in apply med"
"variable att-epsilon" "the EPSILON symbol when reading/writing AT&T files"
"write prolog (> filename)" "writes top network to prolog format file/stdout"
"write att (> <filename>)" "writes top network to AT&T format file/stdout"

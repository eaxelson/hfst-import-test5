const bool VERBOSE=true;

void verbose_print(const char *msg, 
		   hfst::ImplementationType type=hfst::ERROR_TYPE) {
  if (VERBOSE) {
    fprintf(stderr, "Testing:\t%s", msg);
    fprintf(stderr, " for type ");
    switch (type) 
      {
      case hfst::SFST_TYPE:
	fprintf(stderr, "SFST_TYPE");
	break;
      case hfst::TROPICAL_OFST_TYPE:
	fprintf(stderr, "TROPICAL_OFST_TYPE");
	break;
      case hfst::LOG_OFST_TYPE:
	fprintf(stderr, "LOG_OFST_TYPE");
	break;
      case hfst::FOMA_TYPE:
	fprintf(stderr, "FOMA_TYPE");
	break;
      default:
	fprintf(stderr, "(type undefined)");
	break;
      }
    fprintf(stderr, "...\n");
  }
}

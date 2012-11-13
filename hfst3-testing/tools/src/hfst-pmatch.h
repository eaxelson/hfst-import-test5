#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <cstring>
#include "unistd.h"
#include <map>
#include <getopt.h>

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include <string.h>

#include "HfstExceptionDefs.h"
#include "inc/globals-common.h"
#include "inc/globals-unary.h"
#include "implementations/optimized-lookup/pmatch.h"

using hfst::HfstInputStream;
using hfst::HfstTransducer;
using hfst::HFST_OL_TYPE;
using hfst::HFST_OLW_TYPE;

void print_usage(void);
//hfst_ol::PmatchContainer process_transducers(std::ifstream & inputstream);
void print_result(std::string res);
void process_input(hfst_ol::PmatchContainer & container);
void find_rtns(hfst_ol::PmatchContainer & cont);
int parse_options(int argc, char** argv);


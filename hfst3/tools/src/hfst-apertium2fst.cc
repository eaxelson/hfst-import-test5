//! @file hfst-apertium2fst.cc
//!
//! @brief Apertium (mono)dix compiling command line tool
//!
//! @author HFST Team


//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>

using std::map;
using std::pair;
using std::string;
using std::vector;
using std::set;

typedef pair<string,string> StringPair;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "hfst-tool-metadata.h"


using hfst::HfstTransducer;
using hfst::HfstOutputStream;
using hfst::HfstTokenizer;
using hfst::StringSet;

#include "inc/globals-common.h"
#include "inc/globals-unary.h"
// add tools-specific variables here
static hfst::ImplementationType format = hfst::UNSPECIFIED_TYPE;
static set<string> tags;
static map<string, vector<pair<string,string> > > pardefs;
static map<string, vector<pair<string,string> > > sections;
static map<string, vector<pair<string,string> > > preblanks;
static map<string, vector<pair<string,string> > > postblanks;
static map<string, vector<pair<string,string> > > inconditionals;

static char* alphabets = 0;


void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Convert apertium XML files into a binary transducer\n"
           "\n", program_name);

    print_common_program_options(message_out);
    print_common_unary_program_options(message_out);
    // fprintf(message_out, (tool-specific options and short descriptions)
    fprintf(message_out, "Format options:\n"
            "  -f, --format=FMT    Write result using FMT as backend format\n");
    fprintf(message_out, "\n");
    fprintf(message_out, 
        "If OUTFILE or INFILE is missing or -,"
        "standard streams will be used.\n"
        "If FMT is not given, OpenFst's tropical format will be used.\n"
        "The possible values for FMT are { sfst, openfst-tropical, "
        "openfst-log,\n"
    "foma, optimized-lookup-unweighted, optimized-lookup-weighted }.\n"
        );
    fprintf(message_out, "\n");
    print_report_bugs();
    fprintf(message_out, "\n");
    print_more_info();
}

int
parse_options(int argc, char** argv)
{
    extend_options_getenv(&argc, &argv);
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
        {
        HFST_GETOPT_COMMON_LONG,
        HFST_GETOPT_UNARY_LONG,
          // add tool-specific options here
            {"format", required_argument, 0, 'f'}, 
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "e:nf:",
                             long_options, &option_index);
        if (-1 == c)
        {
break;
        }

        switch (c)
          {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
          // add tool-specific cases here
        case 'f':
            format = hfst_parse_format_name(optarg);
            break;
#include "inc/getopt-cases-error.h"
          }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    if (format == hfst::UNSPECIFIED_TYPE)
      {
        format = hfst::TROPICAL_OPENFST_TYPE;
        verbose_printf("Using default output format OpenFst "
                "with tropical weight class\n");
      }

    return EXIT_CONTINUE;
}

int
process_stream(HfstOutputStream& outstream)
{
  verbose_printf("Reading apertium XML...\n");
  xmlDocPtr doc;
  xmlNodePtr node;
  doc = xmlParseFile(inputfilename);
  node = xmlDocGetRootElement(doc);
  HfstTokenizer tok;
  tok.add_skip_symbol(hfst::internal_epsilon);
  if (NULL == node)
    {
      xmlFreeDoc(doc);
      error(EXIT_FAILURE, 0, "Libxml could not parse %s",
            inputfilename);
    }
  if (xmlStrcmp(node->name, xmlCharStrdup("dictionary")) != 0)
    {
      xmlFreeDoc(doc);
      error(EXIT_FAILURE, 0, "Root element of %s is not monodix",
            inputfilename);
    }
  // this is a veryvery veryvery simple approach of XML parsing in tree
  // order with level per level control structures:
  // dictionary
  // + alphabet
  // + sdefs
  //   + sdef
  //   + ...
  // + pardefs
  //   + pardef
  //     + e
  //       + p
  //         + i
  //         + l
  //         + r
  //         + par
  //         + ...
  //   + ...
  // + section
  //   + e
  //     + (like above)
  // + ...
  // 
  // a + is if statement
  // a ... is while statement
  node = node->xmlChildrenNode;
  while (node != NULL)
    {
      // we're under dictionary; expect
      // alphabet sdefs pardefs section+
      if (xmlStrcmp(node->name, xmlCharStrdup("alphabet")) == 0)
        {
          // in alphabet; expect
          // PCDATA
          verbose_printf("parsing alphabet...\n");
          xmlChar* alpha = xmlNodeListGetString(doc,
                                                node->xmlChildrenNode, 1);
          HfstTransducer alphaTrans(reinterpret_cast<char*>(alpha), tok,
                                    format);
          if (verbose)
            {
              StringSet alphas = alphaTrans.get_alphabet();
              for (StringSet::const_iterator s = alphas.begin();
                   s != alphas.end();
                   ++s)
                {
                  verbose_printf("%s ", s->c_str());
                }
            }
          verbose_printf("\n");
          alphabets = strdup(reinterpret_cast<char*>(alpha));
        }
      else if (xmlStrcmp(node->name, xmlCharStrdup("sdefs")) == 0)
        {
          // in sdefs; expect:
          // sdef+
          verbose_printf("parsing sdefs...\n");
          xmlNodePtr sdef = node->xmlChildrenNode;
          while (sdef != NULL)
            {
              xmlChar* anal_tag = xmlGetProp(sdef, xmlCharStrdup("n"));
              if (anal_tag != NULL)
                {
                  verbose_printf("<%s>\n", reinterpret_cast<char*>(anal_tag));
                  tags.insert(string("<") + 
                          reinterpret_cast<char*>(anal_tag) + ">");
                  tok.add_multichar_symbol(string("<") + 
                          reinterpret_cast<char*>(anal_tag) + ">");
                }
              sdef = sdef->next;
            }
        }
      else if (xmlStrcmp(node->name, xmlCharStrdup("pardefs")) == 0)
        {
          // in pardefs; expect:
          // pardef+
          verbose_printf("parsing pardefs...\n");
          xmlNodePtr pardef = node->xmlChildrenNode;
          while (pardef != NULL)
            {
              if (xmlStrcmp(pardef->name, xmlCharStrdup("pardef")) == 0)
                {
                  // in pardef; expect:
                  // e+
                  unsigned long affixes = 0;
                  xmlChar* parname = xmlGetProp(pardef, xmlCharStrdup("n"));
                  if (parname != NULL)
                    {
                      verbose_printf("%s... ", reinterpret_cast<char*>(parname));
                      tok.add_multichar_symbol(string("@APERTIUM_JOINER.") +
                                               reinterpret_cast<char*>(parname) +
                                               "@");
                    }
                  else
                    {
                      warning(0, 0, "unnamed pardef?");
                      pardef = pardef->next;
                      continue;
                    }
                  xmlNodePtr e = pardef->xmlChildrenNode;
                  vector<pair<string,string> > es;
                  while (e != NULL)
                    {
                      if (xmlStrcmp(e->name, xmlCharStrdup("e")) == 0)
                        {
                          // in e; expect:
                          // [p | i | par | re]+
                          affixes++;
                          string left;
                          string right;
                          xmlChar* oneway = xmlGetProp(e, xmlCharStrdup("r"));
                          if (oneway != NULL)
                            {
                              if (xmlStrcmp(oneway, xmlCharStrdup("LR")))
                                {
                                  left += "@P.LR.TRUE@";
                                  right += "@R.LR.TRUE@";
                                  tok.add_multichar_symbol("@P.LR.TRUE@");
                                  tok.add_multichar_symbol("@R.LR.TRUE@");
                                }
                              else if (xmlStrcmp(oneway, xmlCharStrdup("RL")))
                                {
                                  left += "@R.LR.FALSE@";
                                  right += "@P.LR.FALSE@";
                                  tok.add_multichar_symbol("@P.LR.FALSE@");
                                  tok.add_multichar_symbol("@R.LR.FALSE@");
                                }
                            }
                          xmlNodePtr pair = e->xmlChildrenNode;
                          while (pair != NULL)
                            {
                              if (xmlStrcmp(pair->name, xmlCharStrdup("p")) == 0)
                                {
                                  // in p; expect:
                                  // l r 
                                  xmlNodePtr rl = pair->xmlChildrenNode;
                                  while (rl != NULL)
                                    {
                                      if (xmlStrcmp(rl->name, xmlCharStrdup("l")) == 0)
                                        {
                                          xmlNodePtr l = rl->xmlChildrenNode;
                                          bool emptiness = true;
                                          while (l != NULL)
                                            {
                                              if (xmlStrcmp(l->name, xmlCharStrdup("s")) == 0)
                                                {
                                                  xmlChar* tagname = xmlGetProp(l, xmlCharStrdup("n"));
                                                  left += string("<") + 
                                                    reinterpret_cast<char*>(tagname) +
                                                    ">";
                                                  emptiness = false;
                                                }
                                              else if (xmlNodeIsText(l))
                                                {
                                                  xmlChar* surf = xmlNodeListGetString(doc,
                                                                                       l,
                                                                                       1);
                                                  left += reinterpret_cast<char*>(surf);
                                                  emptiness = false;
                                                }
                                              l = l->next;
                                            } // while l
                                          if (emptiness)
                                            {
                                              left += string("@@EMPTY_MORPH_DONT_LEAK@@");
                                              tok.add_multichar_symbol("@@EMPTY_MORPH_DONT_LEAK@@");
                                            }

                                        } // if l
                                      else if (xmlStrcmp(rl->name, xmlCharStrdup("r")) == 0)
                                        {
                                          xmlNodePtr r = rl->xmlChildrenNode;
                                          bool emptiness = true;
                                          while (r != NULL)
                                            {
                                              if (xmlStrcmp(r->name, xmlCharStrdup("s")) == 0)
                                                {
                                                  xmlChar* tagname = xmlGetProp(r, xmlCharStrdup("n"));
                                                  right += string("<") + 
                                                    reinterpret_cast<char*>(tagname) +
                                                    ">";
                                                  emptiness = false;
                                                }
                                              else if (xmlNodeIsText(r))
                                                {
                                                  xmlChar* surf = xmlNodeListGetString(doc,
                                                                                       r,
                                                                                       1);
                                                  right += reinterpret_cast<char*>(surf);
                                                  emptiness = false;
                                                }
                                              r = r->next;
                                            } // while r
                                          if (emptiness)
                                            {
                                              right += string("@@EMPTY_MORPH_DONT_LEAK@@");
                                              tok.add_multichar_symbol("@@EMPTY_MORPH_DONT_LEAK@@");
                                            }

                                        } // if r
                                      rl = rl->next;
                                    } // while rl
                                } // if p
                              else if (xmlStrcmp(pair->name, xmlCharStrdup("i")) == 0)
                                {
                                  xmlNodePtr i = pair->xmlChildrenNode;
                                  bool emptiness = true;
                                  while (i != NULL)
                                    {
                                      if (xmlStrcmp(i->name, xmlCharStrdup("s")) == 0)
                                        {
                                          xmlChar* tagname = xmlGetProp(i, xmlCharStrdup("n"));
                                          left += string("<") + 
                                            reinterpret_cast<char*>(tagname) +
                                            ">";
                                          right += string("<") +
                                           reinterpret_cast<char*>(tagname) +
                                           ">";
                                          emptiness = false;
                                        }
                                      else if (xmlNodeIsText(i))
                                        {
                                          xmlChar* surf = xmlNodeListGetString(doc,
                                                                               i,
                                                                               1);
                                          left += reinterpret_cast<char*>(surf);
                                          right += reinterpret_cast<char*>(surf);
                                          emptiness = false;
                                        }
                                      i = i->next;
                                    } // while i
                                  if (emptiness)
                                    {
                                      left += string("@@EMPTY_MORPH_DONT_LEAK@@");
                                      right += string("@@EMPTY_MORPH_DONT_LEAK@@");
                                      tok.add_multichar_symbol("@@EMPTY_MORPH_DONT_LEAK@@");
                                    }
                                } // if i
                              else if (xmlStrcmp(pair->name, xmlCharStrdup("par")) == 0)
                                {
                                  xmlChar* parref = xmlGetProp(pair, xmlCharStrdup("n"));
                                  left += string("@APERTIUM_JOINER.") + 
                                    reinterpret_cast<char*>(parref) +
                                    "@";
                                  right += string("@APERTIUM_JOINER.") +
                                    reinterpret_cast<char*>(parref) + 
                                    "@";

                                } // if par
                              else if (xmlStrcmp(pair->name, xmlCharStrdup("re")) == 0)
                                {
                                  xmlChar* re = xmlNodeListGetString(doc, 
                                                                     pair->children,
                                                                     1);
                                  left += string("@ERE<@") + 
                                      reinterpret_cast<char*>(re)
                                      + "@>ERE@";
                                  right += string("@ERE<@") +
                                      reinterpret_cast<char*>(re)
                                      + "@>ERE@";
                                  tok.add_multichar_symbol("@ERE>@");
                                  tok.add_multichar_symbol("@>ERE@");
                                } // if re
                              pair = pair->next;
                            } // while pair
                          if (right == "")
                            {
                              right = hfst::internal_epsilon;
                            }
                          if (left == "")
                            {
                              left = hfst::internal_epsilon;
                            }
                          es.push_back(StringPair(left, right));
                          verbose_printf("%s:%s\n", left.c_str(), right.c_str());
                        } // if e
                      e = e->next;
                    } // while e
                  verbose_printf("%lu\n", affixes);
                  pardefs[reinterpret_cast<char*>(parname)] = es;
                } // if pardef
              pardef = pardef->next;
            } // while pardef
        } // if pardefs
      else if (xmlStrcmp(node->name, xmlCharStrdup("section")) == 0)
        {
          // in section; expect:
          // e+
          xmlChar* secid = xmlGetProp(node, xmlCharStrdup("id"));
          xmlChar* sectype = xmlGetProp(node, xmlCharStrdup("type"));
          verbose_printf("Parsing %s section %s...\n",
                         reinterpret_cast<char*>(sectype),
                         reinterpret_cast<char*>(secid));
          xmlNodePtr e = node->children;
          vector<StringPair> es;
          unsigned long roots = 0;
          while (e != NULL)
            {
              if (xmlStrcmp(e->name, xmlCharStrdup("e")) == 0)
                {
                  // in e; expect:
                  // [p | i | par]+
                  roots++;
                  string left;
                  string right;
                  xmlChar* ignore = xmlGetProp(e, xmlCharStrdup("i"));
                  if (ignore != NULL)
                    {
                      roots--;
                      e = e->next;
                      continue;
                    }
                  xmlChar* oneway = xmlGetProp(e, xmlCharStrdup("r"));
                  if (oneway != NULL)
                    {
                      if (xmlStrcmp(oneway, xmlCharStrdup("LR")))
                        {
                          left += "@P.LR.TRUE@";
                          right += "@R.LR.TRUE@";
                          tok.add_multichar_symbol("@P.LR.TRUE@");
                          tok.add_multichar_symbol("@R.LR.TRUE@");
                        }
                      else if (xmlStrcmp(oneway, xmlCharStrdup("RL")))
                        {
                          left += "@R.LR.FALSE@";
                          right += "@P.LR.FALSE@";
                          tok.add_multichar_symbol("@P.LR.FALSE@");
                          tok.add_multichar_symbol("@R.LR.FALSE@");
                        }
                    }
                  xmlNodePtr pair = e->xmlChildrenNode;
                  while (pair != NULL)
                    {
                      if (xmlStrcmp(pair->name, xmlCharStrdup("p")) == 0)
                        {
                          xmlNodePtr rl = pair->xmlChildrenNode;
                          while (rl != NULL)
                            {
                              if (xmlStrcmp(rl->name, xmlCharStrdup("l")) == 0)
                                {
                                  xmlNodePtr l = rl->xmlChildrenNode;
                                  bool emptiness = true;
                                  while (l != NULL)
                                    {
                                      if (xmlStrcmp(l->name, xmlCharStrdup("s")) == 0)
                                        {
                                          xmlChar* tagname = xmlGetProp(l, xmlCharStrdup("n"));
                                          left += string("<") + 
                                            reinterpret_cast<char*>(tagname) +
                                            ">";
                                          emptiness = false;
                                        }
                                      else if (xmlNodeIsText(l))
                                        {
                                          xmlChar* surf = xmlNodeListGetString(doc,
                                                                               l,
                                                                               1);
                                          left += reinterpret_cast<char*>(surf);
                                          emptiness = false;
                                        }
                                      l = l->next;
                                    } // while l
                                  if (emptiness)
                                    {
                                      left += string("@@EMPTY_MORPH_DONT_LEAK@@");
                                      tok.add_multichar_symbol("@@EMPTY_MORPH_DONT_LEAK@@");
                                    }
                                } // if l
                              else if (xmlStrcmp(rl->name, xmlCharStrdup("r")) == 0)
                                {
                                  xmlNodePtr r = rl->xmlChildrenNode;
                                  bool emptiness = true;
                                  while (r != NULL)
                                    {
                                      if (xmlStrcmp(r->name, xmlCharStrdup("s")) == 0)
                                        {
                                          xmlChar* tagname = xmlGetProp(r, xmlCharStrdup("n"));
                                          right += string("<") + 
                                            reinterpret_cast<char*>(tagname) +
                                            ">";
                                          emptiness = false;
                                        }
                                      else if (xmlNodeIsText(r))
                                        {
                                          xmlChar* surf = xmlNodeListGetString(doc,
                                                                               r,
                                                                               1);
                                          right += reinterpret_cast<char*>(surf);
                                          emptiness = false;
                                        }
                                      r = r->next;
                                    }
                                  if (emptiness)
                                    {
                                      right += string("@@EMPTY_MORPH_DONT_LEAK@@");
                                      tok.add_multichar_symbol("@@EMPTY_MORPH_DONT_LEAK@@");
                                    }
                                } // if r
                              rl = rl->next;
                            } // while rl
                        } // if p
                      else if (xmlStrcmp(pair->name, xmlCharStrdup("i")) == 0)
                        {
                          xmlNodePtr i = pair->xmlChildrenNode;
                          bool emptiness = true;
                          while (i != NULL)
                            {
                              if (xmlStrcmp(i->name, xmlCharStrdup("s")) == 0)
                                {
                                  xmlChar* tagname = xmlGetProp(i, xmlCharStrdup("n"));
                                  left += string("<") + 
                                    reinterpret_cast<char*>(tagname) +
                                    ">";
                                  right += string("<") +
                                   reinterpret_cast<char*>(tagname) +
                                   ">";
                                  emptiness = false;
                                }
                              else if (xmlNodeIsText(i))
                                {
                                  xmlChar* surf = xmlNodeListGetString(doc,
                                                                       i,
                                                                       1);
                                  left += reinterpret_cast<char*>(surf);
                                  right += reinterpret_cast<char*>(surf);
                                  emptiness = false;
                                }
                              i = i->next;
                            } // while i
                          if (emptiness)
                            {
                              left += string("@@EMPTY_MORPH_DONT_LEAK@@");
                              right += string("@@EMPTY_MORPH_DONT_LEAK@@");
                              tok.add_multichar_symbol("@@EMPTY_MORPH_DONT_LEAK@@");
                            }

                        } // if i
                      else if (xmlStrcmp(pair->name, xmlCharStrdup("par")) == 0)
                        {
                          xmlChar* parref = xmlGetProp(pair, xmlCharStrdup("n"));
                          left += string("@APERTIUM_JOINER.") + 
                            reinterpret_cast<char*>(parref) +
                            "@";
                          right += string("@APERTIUM_JOINER.") +
                            reinterpret_cast<char*>(parref) + 
                            "@";
                        } // if pair
                      else if (xmlStrcmp(pair->name, xmlCharStrdup("re")) == 0)
                        {
                          xmlChar* re = xmlNodeListGetString(doc, pair->children, 1);
                          left += string("@ERE<@") + 
                              reinterpret_cast<char*>(re)
                              + "@>ERE@";
                          right += string("@ERE<@") +
                              reinterpret_cast<char*>(re)
                              + "@>ERE@";
                          tok.add_multichar_symbol("@ERE<@");
                          tok.add_multichar_symbol("@>ERE@");
                        } // if re
                      pair = pair->next;
                    } // while pair
                  if (right == "")
                    {
                      right = hfst::internal_epsilon;
                    }
                  if (left == "")
                    {
                      left = hfst::internal_epsilon;
                    }
                  es.push_back(StringPair(left, right));
                  verbose_printf("%s:%s\n", left.c_str(), right.c_str());
                } // if e
              e = e->next;
            } // while e
          verbose_printf("%lu\n", roots);
          if (xmlStrcmp(sectype, reinterpret_cast<const xmlChar*>("standard")) == 0)
            {
              sections[reinterpret_cast<char*>(secid)] = es;
            }
          else if (xmlStrcmp(sectype, reinterpret_cast<const xmlChar*>("preblank")) == 0)
            {
              preblanks[reinterpret_cast<char*>(secid)] = es;
            }
          else if (xmlStrcmp(sectype, reinterpret_cast<const xmlChar*>("postblank")) == 0)
            {
              postblanks[reinterpret_cast<char*>(secid)] = es;
            }
          else if (xmlStrcmp(sectype, reinterpret_cast<const xmlChar*>("inconditional")) == 0)
            {
              inconditionals[reinterpret_cast<char*>(secid)] = es;
            }
        } // if section
      node = node->next;
    } // while node
  // create PARDEF* ROOT PARDEF*
  verbose_printf("Turning parsed string into HFST automaton...\n");
  HfstTransducer t(format);
  HfstTransducer prefixPardefsTrans(format);
  HfstTransducer suffixPardefsTrans(format);
  verbose_printf("Joining pardefs... ");
  for (map<string, vector<StringPair> >::const_iterator par = pardefs.begin();
       par != pardefs.end();
       ++par)
    {
      string parJoinerString("@APERTIUM_JOINER.");
      verbose_printf("%s... ", par->first.c_str());
      parJoinerString.append(par->first);
      parJoinerString.append("@");
      HfstTransducer parJoinerLeft(parJoinerString, parJoinerString, format);
      HfstTransducer parJoinerRight(parJoinerLeft);
      HfstTransducer morphs(format);
      for (vector<StringPair>::const_iterator morph = par->second.begin();
           morph != par->second.end();
           ++morph)
        {
          HfstTransducer morphTrans(morph->first, morph->second, tok, format);
          morphs.disjunct(morphTrans);
        }
      HfstTransducer suffixPardefTrans(parJoinerLeft);
      suffixPardefTrans.concatenate(morphs);
      suffixPardefTrans.minimize();
      suffixPardefsTrans.disjunct(suffixPardefTrans);
      HfstTransducer prefixPardefTrans(morphs);
      prefixPardefTrans.concatenate(parJoinerRight);
      prefixPardefTrans.minimize();
      prefixPardefsTrans.disjunct(prefixPardefTrans);
    }
  verbose_printf("\nRepeating and minimising...\n");
  prefixPardefsTrans.repeat_star().minimize();
  suffixPardefsTrans.repeat_star().minimize();
  if (debug)
    {
      std::cerr << "prefix pardefs:" << std::endl << prefixPardefsTrans << std::endl;
      std::cerr << "suffix pardefs:" << std::endl << suffixPardefsTrans << std::endl;
    }
  verbose_printf("Joining standard sections... ");
  HfstTransducer sectionsTrans(format);
  for (map<string, vector<StringPair> >::const_iterator root = sections.begin();
       root != sections.end();
       ++root)
    {
      verbose_printf("%s... ", root->first.c_str());
      HfstTransducer roots(format);
      for (vector<StringPair>::const_iterator morph = root->second.begin();
           morph != root->second.end();
           ++morph)
        {
          HfstTransducer morphTrans(morph->first, morph->second, tok, format);
          roots.disjunct(morphTrans);
        }
      sectionsTrans.disjunct(roots);
    }
  verbose_printf("\npreblanks... ");
  HfstTransducer preblanksTrans(hfst::internal_epsilon, hfst::internal_epsilon,
                                format);
  for (map<string, vector<StringPair> >::const_iterator root = preblanks.begin();
       root != preblanks.end();
       ++root)
    {
      verbose_printf("%s... ", root->first.c_str());
      HfstTransducer roots(format);
      for (vector<StringPair>::const_iterator morph = root->second.begin();
           morph != root->second.end();
           ++morph)
        {
          HfstTransducer morphTrans(morph->first, morph->second, tok, format);
          roots.disjunct(morphTrans);
        }
      preblanksTrans.disjunct(roots);
    }
  preblanksTrans.optionalize();
  verbose_printf("\npostblanks... ");
  HfstTransducer postblanksTrans(hfst::internal_epsilon, hfst::internal_epsilon,
                                 format);
  for (map<string, vector<StringPair> >::const_iterator root = postblanks.begin();
       root != postblanks.end();
       ++root)
    {
      verbose_printf("%s... ", root->first.c_str());
      HfstTransducer roots(format);
      for (vector<StringPair>::const_iterator morph = root->second.begin();
           morph != root->second.end();
           ++morph)
        {
          HfstTransducer morphTrans(morph->first, morph->second, tok, format);
          roots.disjunct(morphTrans);
        }
      postblanksTrans.disjunct(roots);
    }
  postblanksTrans.optionalize();
  verbose_printf("\ninconditionals... ");
  HfstTransducer inconditionalsTrans(format);
  for (map<string, vector<StringPair> >::const_iterator root = inconditionals.begin();
       root != inconditionals.end();
       ++root)
    {
      verbose_printf("%s... ", root->first.c_str());
      HfstTransducer roots(format);
      for (vector<StringPair>::const_iterator morph = root->second.begin();
           morph != root->second.end();
           ++morph)
        {
          HfstTransducer morphTrans(morph->first, morph->second, tok, format);
          roots.disjunct(morphTrans);
        }
      inconditionalsTrans.disjunct(roots);
    }
  verbose_printf("\nMinimising...\n");
  sectionsTrans.minimize();
  preblanksTrans.minimize();
  postblanksTrans.minimize();
  inconditionalsTrans.minimize();
  if (debug)
    {
      std::cerr << "Standard sections:" << std::endl << sectionsTrans << std::endl;
      std::cerr << "Postblanks:" << std::endl << postblanksTrans << std::endl;
      std::cerr << "Preblanks:" << std::endl << preblanksTrans << std::endl;
      std::cerr << "Inconditionals:" << std::endl << inconditionalsTrans << std::endl;
    }
  verbose_printf("Concatenating...\n");
  HfstTransducer result(prefixPardefsTrans);
  result.concatenate(sectionsTrans);
  result.concatenate(suffixPardefsTrans);
  if (debug)
    {
      std::cerr << "prefixes-root-suffixes:" <<
                    std::endl << result << std::endl;
    }
  verbose_printf("Creating morphotax...");
  HfstTransducer joiners(format);
  HfstTransducer joinerPairs(format);
  for (map<string, vector<StringPair> >::const_iterator pardef = pardefs.begin();
       pardef != pardefs.end();
       ++pardef)
    {
      string parJoinerString("@APERTIUM_JOINER.");
      verbose_printf("%s... ", pardef->first.c_str());
      parJoinerString.append(pardef->first);
      parJoinerString.append("@");
      HfstTransducer parJoiner(parJoinerString, parJoinerString, format);
      joiners.disjunct(parJoiner);
      joinerPairs.disjunct(parJoiner.repeat_n(2));
    }
  HfstTransducer sigmaMinusJoiners(hfst::internal_identity,
                                   hfst::internal_identity,
                                   format);
  sigmaMinusJoiners.subtract(joiners);
  HfstTransducer morphotax(joinerPairs.disjunct(sigmaMinusJoiners));
  morphotax.repeat_star();
  morphotax.minimize();
  morphotax.insert_freely_missing_flags_from(result);
  morphotax.minimize();
  if (debug)
    {
      std::cerr << "morphotax:" << std::endl << morphotax << std::endl;
    }
  verbose_printf("Applying morphotax...\n");
  t = morphotax.compose(result);
  verbose_printf("Minimising...\n");
  t.minimize();
  verbose_printf("Adding other sections");
  t = postblanksTrans.concatenate(t).concatenate(preblanksTrans);
  t.disjunct(inconditionalsTrans);
  verbose_printf("Discarding joiners...\n");
  for (map<string, vector<StringPair> >::const_iterator pardef = pardefs.begin();
       pardef != pardefs.end();
       ++pardef)
    {
      string parJoinerString("@APERTIUM_JOINER.");
      verbose_printf("%s... ", pardef->first.c_str());
      parJoinerString.append(pardef->first);
      parJoinerString.append("@");
      t.substitute(parJoinerString, hfst::internal_epsilon);
    }
  t.substitute("@@EMPTY_MORPH_DONT_LEAK@@", hfst::internal_epsilon);
  verbose_printf("\nMinimising...\n");
  t.minimize();
  if (debug)
    {
      std::cerr << "result:" << std::endl << t << std::endl;
    }
  hfst_set_name(t, inputfilename, "apertium");
  hfst_set_formula(t, inputfilename, "A");
  outstream << t;
  outstream.close();
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) 
{
  hfst_set_program_name(argv[0], "0.1", "HfstTxt2Fst");
    int retval = parse_options(argc, argv);

    if (retval != EXIT_CONTINUE)
    {
        return retval;
    }
    // close buffers, we use streams
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    verbose_printf("Reading from %s, writing to %s\n", 
        inputfilename, outfilename);
    switch (format)
      {
      case hfst::SFST_TYPE:
        verbose_printf("Using SFST as output handler\n");
        break;
      case hfst::TROPICAL_OPENFST_TYPE:
        verbose_printf("Using OpenFst's tropical weights as output\n");
        break;
      case hfst::LOG_OPENFST_TYPE:
        verbose_printf("Using OpenFst's log weight output\n");
        break;
      case hfst::FOMA_TYPE:
        verbose_printf("Using foma as output handler\n");
        break;
      case hfst::HFST_OL_TYPE:
        verbose_printf("Using optimized lookup output\n");
        break;
      case hfst::HFST_OLW_TYPE:
        verbose_printf("Using optimized lookup weighted output\n");
        break;
      default:
        error(EXIT_FAILURE, 0, "Unknown format cannot be used as output\n");
        return EXIT_FAILURE;
      }
    // here starts the buffer handling part
    HfstOutputStream* outstream = (outfile != stdout) ?
                new HfstOutputStream(outfilename, format) :
                new HfstOutputStream(format);
    process_stream(*outstream);
    if (inputfile != stdin)
      {
        fclose(inputfile);
      }
    free(inputfilename);
    free(outfilename);
    return EXIT_SUCCESS;
}


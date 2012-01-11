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

#include <hfst.hpp>

#include "conventions/commandline.h"
#include "conventions/options.h"
#include "conventions/metadata.h"
#include "utils/edit-distance.h"

using hfst::HfstBasicTransducer;
using hfst::HfstTransducer;
using hfst::HfstOutputStream;
using hfst::HfstTokenizer;
using hfst::StringSet;

#include "conventions/globals-common.h"
#include "conventions/globals-unary.h"
// add tools-specific variables here
static hfst::ImplementationType format = hfst::UNSPECIFIED_TYPE;
static set<string> tags;
static map<string, vector<pair<string,string> > > pardefs;
static map<string, vector<pair<string,string> > > sections;
static map<string, vector<pair<string,string> > > preblanks;
static map<string, vector<pair<string,string> > > postblanks;
static map<string, vector<pair<string,string> > > inconditionals;
static HfstTokenizer tok;

static set<string> alphabets;

static char* error_filename = 0;
static FILE* error_file = 0;


void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Convert apertium XML files into a binary transducer\n"
           "\n", program_name);

    print_common_program_options();
    print_common_creational_program_options();
    // fprintf(message_out, (tool-specific options and short descriptions)
    fprintf(message_out, "Format options:\n"
            "  -e, --errmodel=ERR   Write error model for spelller to ERR\n");
    fprintf(message_out, "\n");
    print_common_creational_program_parameter_instructions();
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
            {"errmodel", required_argument, 0, 'e'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "e:f:",
                             long_options, &option_index);
        if (-1 == c)
        {
break;
        }

        switch (c)
          {
#include "conventions/getopt-cases-common.h"
#include "conventions/getopt-cases-unary.h"
          // add tool-specific cases here
        case 'f':
            format = hfst_parse_format_name(optarg);
            break;
        case 'e':
            error_filename = hfst_strdup(optarg);
            error_file = hfst_fopen(error_filename, "w");
            break;
#include "conventions/getopt-cases-error.h"
          }
    }

#include "conventions/check-params-common.h"
#include "conventions/check-params-unary.h"
    if (format == hfst::UNSPECIFIED_TYPE)
      {
        format = hfst::TROPICAL_OPENFST_TYPE;
        verbose_printf("Using default output format OpenFst "
                "with tropical weight class\n");
      }

    return EXIT_CONTINUE;
}

static
set<string>*
parse_alphabet(xmlDocPtr doc, xmlNodePtr alphabet)
{
  // in alphabet; expect
  // PCDATA
  verbose_printf("parsing alphabet...\n");
  xmlChar* alpha = xmlNodeListGetString(doc,
                                        alphabet->xmlChildrenNode, 1);
  if (alpha == NULL)
    {
      error(EXIT_FAILURE, 0, "A transducer collection with empty "
            "alphabet would not be able to analyse anything;\n"
            "refusing to create broken automata");
    }
  set<string>* rv = new set<string>;
  HfstTransducer alphaTrans(reinterpret_cast<char*>(alpha), tok,
                            format);
  unsigned int i = 0;
  StringSet alphas = alphaTrans.get_alphabet();
  for (StringSet::const_iterator s = alphas.begin();
       s != alphas.end();
       ++s)
    {
      if (s->substr(0, 1) != "@")
        {
          verbose_printf("%s ", s->c_str());
          rv->insert(*s);
          i++;
        }
    }
  verbose_printf("\n");
  xmlFree(alpha);
  return rv;
}

static
void
parse_sdefs(xmlDocPtr, xmlNodePtr sdefs)
{
  // in sdefs; expect:
  // sdef+
  verbose_printf("parsing sdefs...\n");
  xmlNodePtr sdef = sdefs->xmlChildrenNode;
  while (sdef != NULL)
    {
      xmlChar* anal_tag = xmlGetProp(sdef, reinterpret_cast<const xmlChar*>("n"));
      if (anal_tag != NULL)
        {
          verbose_printf("<%s>\n", reinterpret_cast<char*>(anal_tag));
          tags.insert(string("<") + 
                  reinterpret_cast<char*>(anal_tag) + ">");
          tok.add_multichar_symbol(string("<") + 
                  reinterpret_cast<char*>(anal_tag) + ">");
          xmlFree(anal_tag);
        }
      sdef = sdef->next;
    }
}

static
void
parse_s(xmlDocPtr, xmlNodePtr s, string& lrs)
{
  // in s; expect:
  // EMPTY
  xmlChar* tagname = xmlGetProp(s, reinterpret_cast<const xmlChar*>("n"));
  lrs += string("<") + 
    reinterpret_cast<char*>(tagname) +
    ">";
  xmlFree(tagname);
}

static
void
parse_b(xmlDocPtr, xmlNodePtr, string& lrs)
{
  // in b; expect:
  // EMPTY
  lrs += string(" ");
}

static
void
parse_j(xmlDocPtr, xmlNodePtr, string& lrs)
{
  // in j; expect:
  // EMPTY
  lrs += string("+");
}

static
void
parse_l(xmlDocPtr doc, xmlNodePtr l, string& left, string&)
{
  // in l; expect:
  // [CDATA | s | b | j | g]+
  xmlNodePtr lc = l->xmlChildrenNode;
  bool emptiness = true;
  while (lc != NULL)
    {
      if (xmlStrcmp(lc->name, reinterpret_cast<const xmlChar*>("s")) == 0)
        {
          parse_s(doc, lc, left);
          emptiness = false;
        }
      else if (xmlStrcmp(lc->name, reinterpret_cast<const xmlChar*>("b")) == 0)
        {
          parse_b(doc, lc, left);
          emptiness = false;
        }
      else if (xmlStrcmp(lc->name, reinterpret_cast<const xmlChar*>("j")) == 0)
        {
          parse_j(doc, lc, left);
          emptiness = false;
        }
      else if (xmlNodeIsText(lc))
        {
          xmlChar* surf = xmlNodeGetContent(lc);
          left += reinterpret_cast<char*>(surf);
          xmlFree(surf);
          emptiness = false;
        }
      else if (!xmlIsBlankNode(lc) && (lc->type != XML_COMMENT_NODE))
        {
          error(0, 0,
                "Unrecognised %s in <l>",
                reinterpret_cast<const char*>(lc->name));
        }
      lc = lc->next;
    } // while lc
  if (emptiness)
    {
      left += string("@@EMPTY_MORPH_DONT_LEAK@@");
      tok.add_multichar_symbol("@@EMPTY_MORPH_DONT_LEAK@@");
    }
}

static
void
parse_r(xmlDocPtr doc, xmlNodePtr r, string&, string& right)
{
  // in r; expect:
  // [CDATA | s | b | j | g]+
  xmlNodePtr rc = r->xmlChildrenNode;
  bool emptiness = true;
  while (rc != NULL)
    {
      if (xmlStrcmp(rc->name, reinterpret_cast<const xmlChar*>("s")) == 0)
        {
          parse_s(doc, rc, right);
          emptiness = false;
        }
      else if (xmlStrcmp(rc->name, reinterpret_cast<const xmlChar*>("b")) == 0)
        {
          parse_b(doc, rc, right);
          emptiness = false;
        }
      else if (xmlStrcmp(rc->name, reinterpret_cast<const xmlChar*>("j")) == 0)
        {
          parse_j(doc, rc, right);
          emptiness = false;
        }
      else if (xmlNodeIsText(rc))
        {
          xmlChar* surf = xmlNodeGetContent(rc);
          right += reinterpret_cast<char*>(surf);
          xmlFree(surf);
          emptiness = false;
        }
      else if (!xmlIsBlankNode(rc) && (rc->type != XML_COMMENT_NODE))
        {
          error(0, 0,
                "Unrecognised %s in <r>",
                reinterpret_cast<const char*>(rc->name));
        }
      rc = rc->next;
    } // while rc
  if (emptiness)
    {
      right += string("@@EMPTY_MORPH_DONT_LEAK@@");
      tok.add_multichar_symbol("@@EMPTY_MORPH_DONT_LEAK@@");
    }
}

static
void
parse_i(xmlDocPtr doc, xmlNodePtr i, string& left, string& right)
{
  // in i; expect:
  // [CDATA | s | b | j | g]+
  xmlNodePtr ic = i->xmlChildrenNode;
  bool emptiness = true;
  while (ic != NULL)
    {
      if (xmlStrcmp(ic->name, reinterpret_cast<const xmlChar*>("s")) == 0)
        {
          parse_s(doc, ic, left);
          parse_s(doc, ic, right);
          emptiness = false;
        }
      else if (xmlStrcmp(ic->name, reinterpret_cast<const xmlChar*>("b")) == 0)
        {
          parse_b(doc, ic, left);
          parse_b(doc, ic, right);
          emptiness = false;
        }
      else if (xmlStrcmp(ic->name, reinterpret_cast<const xmlChar*>("j")) == 0)
        {
          parse_j(doc, ic, left);
          parse_j(doc, ic, right);
          emptiness = false;
        }
      else if (xmlNodeIsText(ic))
        {
          xmlChar* surf = xmlNodeGetContent(ic);
          left += reinterpret_cast<char*>(surf);
          right += reinterpret_cast<char*>(surf);
          xmlFree(surf);
          emptiness = false;
        }
      else if (!xmlIsBlankNode(ic) && (ic->type != XML_COMMENT_NODE))
        {
          error(0, 0,
                "Unrecognised %s in <i>",
                reinterpret_cast<const char*>(ic->name));
        }
      ic = ic->next;
    } // while i
  if (emptiness)
    {
      left += string("@@EMPTY_MORPH_DONT_LEAK@@");
      right += string("@@EMPTY_MORPH_DONT_LEAK@@");
      tok.add_multichar_symbol("@@EMPTY_MORPH_DONT_LEAK@@");
    }
}

static
void
parse_p(xmlDocPtr doc, xmlNodePtr p, string& left, string& right)
{
  // in p; expect:
  // l r 
  xmlNodePtr rl = p->xmlChildrenNode;
  while (rl != NULL)
    {
      if (xmlStrcmp(rl->name, reinterpret_cast<const xmlChar*>("l")) == 0)
        {
          parse_l(doc, rl, left, right);
        } // if l
      else if (xmlStrcmp(rl->name, reinterpret_cast<const xmlChar*>("r")) == 0)
        {
          parse_r(doc, rl, left, right);

        } // if r
      else if (!xmlIsBlankNode(rl) && (rl->type != XML_COMMENT_NODE))
        {
          error(0, 0, "Unrecognised %s in <p>",
                reinterpret_cast<const char*>(rl->name));
        }
      rl = rl->next;
    } // while rl
}

static
void
parse_par(xmlDocPtr, xmlNodePtr par, string& left, string& right)
{
  xmlChar* parref = xmlGetProp(par, reinterpret_cast<const xmlChar*>("n"));
  left += string("@APERTIUM_JOINER.") + 
    reinterpret_cast<char*>(parref) +
    "@";
  right += string("@APERTIUM_JOINER.") +
    reinterpret_cast<char*>(parref) + 
    "@";
  xmlFree(parref);
}

static
void
parse_re(xmlDocPtr doc, xmlNodePtr re, string& left, string& right)
{
  xmlChar* regex = xmlNodeListGetString(doc, 
                                     re->children,
                                     1);
  error(0, 0, "regexps unsupported, %s is left as is",
        reinterpret_cast<char*>(regex));
  left += string("@ERE<@") + 
      reinterpret_cast<char*>(regex)
      + "@>ERE@";
  right += string("@ERE<@") +
      reinterpret_cast<char*>(regex)
      + "@>ERE@";
  tok.add_multichar_symbol("@ERE>@");
  tok.add_multichar_symbol("@>ERE@");
  xmlFree(regex);
}

static
void
parse_e(xmlDocPtr doc, xmlNodePtr e, vector<pair<string,string> >& es)
{
  // in e; expect:
  // [p | i | par | re]+
  string left;
  string right;
  xmlChar* ignore = xmlGetProp(e, reinterpret_cast<const xmlChar*>("i"));
  if (ignore != NULL)
    {
      xmlFree(ignore);
      return;
    }
  xmlChar* oneway = xmlGetProp(e, reinterpret_cast<const xmlChar*>("r"));
  if (oneway != NULL)
    {
      if (xmlStrcmp(oneway, 
                    reinterpret_cast<const xmlChar*>("LR")))
        {
          left += "@P.LR.TRUE@";
          right += "@R.LR.TRUE@";
          tok.add_multichar_symbol("@P.LR.TRUE@");
          tok.add_multichar_symbol("@R.LR.TRUE@");
        }
      else if (xmlStrcmp(oneway, 
                         reinterpret_cast<const xmlChar*>("RL")))
        {
          left += "@R.LR.FALSE@";
          right += "@P.LR.FALSE@";
          tok.add_multichar_symbol("@P.LR.FALSE@");
          tok.add_multichar_symbol("@R.LR.FALSE@");
        }
      else
        {
          error(0, 0, "unrecognised @r in <e>: %s",
                reinterpret_cast<char*>(oneway));
        }
      xmlFree(oneway);
    }
  xmlNodePtr pair = e->xmlChildrenNode;
  while (pair != NULL)
    {
      if (xmlStrcmp(pair->name, 
                    reinterpret_cast<const xmlChar*>("p")) == 0)
        {
          parse_p(doc, pair, left, right);
        } // if p
      else if (xmlStrcmp(pair->name,
                         reinterpret_cast<const xmlChar*>("i")) == 0)
        {
          parse_i(doc, pair, left, right);
        } // if i
      else if (xmlStrcmp(pair->name, 
                         reinterpret_cast<const xmlChar*>("par")) == 0)
        {
          parse_par(doc, pair, left, right);

        } // if par
      else if (xmlStrcmp(pair->name, 
                         reinterpret_cast<const xmlChar*>("re")) == 0)
        {
          parse_re(doc, pair, left, right);
        } // if re
      else if (!xmlIsBlankNode(pair) && (pair->type != XML_COMMENT_NODE))
        {
          error(0, 0, "unrecognised %s in <e>",
                reinterpret_cast<const char*>(pair->name));
        }
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
}

static
void
parse_pardef(xmlDocPtr doc, xmlNodePtr pardef)
{
  // in pardef; expect:
  // e+
  xmlChar* parname = xmlGetProp(pardef, 
                                reinterpret_cast<const xmlChar*>("n"));
  if (parname != NULL)
    {
      verbose_printf("%s... ", reinterpret_cast<char*>(parname));
      tok.add_multichar_symbol(string("@APERTIUM_JOINER.") +
                               reinterpret_cast<char*>(parname) +
                               "@");
    }
  else
    {
      error(0, 0, "unnamed pardef?");
      pardef = pardef->next;
      return;
    }
  xmlNodePtr e = pardef->xmlChildrenNode;
  vector<pair<string,string> > es;
  while (e != NULL)
    {
      if (xmlStrcmp(e->name, reinterpret_cast<const xmlChar*>("e")) == 0)
        {
          parse_e(doc, e, es);
        } // if e
      else if (!xmlIsBlankNode(e) && (e->type != XML_COMMENT_NODE))
        {
          error(0, 0, "Unrecognised %s in pardef",
                reinterpret_cast<const char*>(e->name));
        }
      e = e->next;
    } // while e
  verbose_printf("%lu\n", es.size());
  pardefs[reinterpret_cast<char*>(parname)] = es;
  xmlFree(parname);

}

static
void
parse_pardefs(xmlDocPtr doc, xmlNodePtr pardefs)
{
  // in pardefs; expect:
  // pardef+
  verbose_printf("parsing pardefs...\n");
  xmlNodePtr pardef = pardefs->xmlChildrenNode;
  while (pardef != NULL)
    {
      if (xmlStrcmp(pardef->name, 
                    reinterpret_cast<const xmlChar*>("pardef")) == 0)
        {
          parse_pardef(doc, pardef);
        } // if pardef
      else if (!xmlIsBlankNode(pardef) && (pardef->type != XML_COMMENT_NODE))
        {
          error(0, 0, "Unrecognised %s in pardefs",
                reinterpret_cast<const xmlChar*>(pardef->name));
        }
      pardef = pardef->next;
    } // while pardef
}

static
void
parse_section(xmlDocPtr doc, xmlNodePtr section)
{
  // in section; expect:
  // e+
  xmlChar* secid = xmlGetProp(section,
                              reinterpret_cast<const xmlChar*>("id"));
  xmlChar* sectype = xmlGetProp(section,
                                reinterpret_cast<const xmlChar*>("type"));
  verbose_printf("Parsing %s section %s...\n",
                 reinterpret_cast<char*>(sectype),
                 reinterpret_cast<char*>(secid));
  xmlNodePtr e = section->children;
  vector<StringPair> es;
  while (e != NULL)
    {
      if (xmlStrcmp(e->name, reinterpret_cast<const xmlChar*>("e")) == 0)
        {
          parse_e(doc, e, es);
        } // if e
      else if (!xmlIsBlankNode(e) && (e->type != XML_COMMENT_NODE))
        {
          error(0, 0, "Unrecognised %s in section",
                reinterpret_cast<const char*>(e->name));
        }
      e = e->next;
    } // while e
  verbose_printf("%lu\n", es.size());
  if (xmlStrcmp(sectype, 
                reinterpret_cast<const xmlChar*>("standard")) == 0)
    {
      sections[reinterpret_cast<char*>(secid)] = es;
    }
  else if (xmlStrcmp(sectype, 
                     reinterpret_cast<const xmlChar*>("preblank")) == 0)
    {
      preblanks[reinterpret_cast<char*>(secid)] = es;
    }
  else if (xmlStrcmp(sectype, 
                     reinterpret_cast<const xmlChar*>("postblank")) == 0)
    {
      postblanks[reinterpret_cast<char*>(secid)] = es;
    }
  else if (xmlStrcmp(sectype,
                     reinterpret_cast<const xmlChar*>("inconditional")) == 0)
    {
      inconditionals[reinterpret_cast<char*>(secid)] = es;
    }
  xmlFree(secid);
  xmlFree(sectype);
}
int
process_stream(HfstOutputStream& outstream, HfstOutputStream* errstream)
{
  verbose_printf("Reading apertium XML...\n");
  xmlDocPtr doc;
  xmlNodePtr node;
  doc = xmlParseFile(inputfilename);
  node = xmlDocGetRootElement(doc);
  tok.add_skip_symbol(hfst::internal_epsilon);
  if (NULL == node)
    {
      xmlFreeDoc(doc);
      error(EXIT_FAILURE, 0, "Libxml could not parse %s",
            inputfilename);
    }
  if (xmlStrcmp(node->name, 
                reinterpret_cast<const xmlChar*>("dictionary")) != 0)
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
      // we're under dictionary; expect
      // alphabet sdefs pardefs section+
  node = node->xmlChildrenNode;
  while (node != NULL)
    {
      if (xmlStrcmp(node->name, 
                    reinterpret_cast<const xmlChar*>("alphabet")) == 0)
        {
          alphabets = *(parse_alphabet(doc, node));
        }
      else if (xmlStrcmp(node->name,
                         reinterpret_cast<const xmlChar*>("sdefs")) == 0)
        {
          parse_sdefs(doc, node);
        }
      else if (xmlStrcmp(node->name, 
                         reinterpret_cast<const xmlChar*>("pardefs")) == 0)
        {
          parse_pardefs(doc, node);
        } // if pardefs
      else if (xmlStrcmp(node->name, 
                         reinterpret_cast<const xmlChar*>("section")) == 0)
        {
          parse_section(doc, node);
        } // if section
      else if (!xmlIsBlankNode(node) && (node->type != XML_COMMENT_NODE))
        {
          error(0, 0, "unrecognised %s in dictionary",
                reinterpret_cast<const char*>(node->name));
        }
      node = node->next;
    } // while node
  xmlFreeDoc(doc);
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
      HfstBasicTransducer morphs;
      for (vector<StringPair>::const_iterator morph = par->second.begin();
           morph != par->second.end();
           ++morph)
        {
          hfst::StringPairVector spv = tok.tokenize(morph->first, morph->second);
          morphs.disjunct(spv, 0);
        }
      HfstTransducer suffixPardefTrans(parJoinerLeft);
      suffixPardefTrans.concatenate(HfstTransducer(morphs, format));
      suffixPardefTrans.minimize();
      suffixPardefsTrans.disjunct(suffixPardefTrans);
      HfstTransducer prefixPardefTrans(HfstTransducer(morphs, format));
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
      HfstBasicTransducer roots;
      unsigned long root_count = 0;
      for (vector<StringPair>::const_iterator morph = root->second.begin();
           morph != root->second.end();
           ++morph)
        {
          root_count++;
          if ((root_count % 1000) == 0)
            {
              verbose_printf("%lu... ", root_count);
            }
          hfst::StringPairVector spv = tok.tokenize(morph->first, morph->second);
          roots.disjunct(spv, 0);
        }
      sectionsTrans.disjunct(HfstTransducer(roots, format));
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
  verbose_printf("\nApplying morphotax...\n");
  t = morphotax.compose(result);
  verbose_printf("Minimising...\n");
  t.minimize();
  verbose_printf("Adding other sections...\n");
  t = postblanksTrans.concatenate(t).concatenate(preblanksTrans);
  t.disjunct(inconditionalsTrans);
  verbose_printf("Discarding joiners... ");
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
  if (errstream != 0)
    {
      HfstTransducer* errmodel = create_edit_distance(alphabets, 1, 1.0f,
                                                      format);

      errstream->operator<<(*errmodel);
      errstream->close();
    }
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) 
{
  hfst_set_program_name(argv[0], "0.1", "HfstApertium2Fst");
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
    if ((error_file != 0) && (error_file != stdout))
      {
        fclose(error_file);
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
    HfstOutputStream* errstream = 0;
    if (error_file != 0)
      {
        errstream = (error_file != stdout) ?
            new HfstOutputStream(error_filename, format):
            new HfstOutputStream(format);
      }
    process_stream(*outstream, errstream);
    if (profile_file != 0)
      {
        hfst_print_profile_line();
      }
    if (inputfile != stdin)
      {
        fclose(inputfile);
      }
    free(inputfilename);
    free(outfilename);
    free(profile_file_name);
    return EXIT_SUCCESS;
}


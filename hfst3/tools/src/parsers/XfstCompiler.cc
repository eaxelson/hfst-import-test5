//! @file XfstCompiler.cc
//!
//! @brief Implemetation of class encapsulating yacc and flex parsers for XFST
//!        scripts

//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#if HAVE_CONFIG_H
#   include <config.h>
#endif

#include <string>
#include <map>
#include <list>
#include <queue>
#include <stack>

using std::list;
using std::string;
using std::map;
using std::queue;
using std::stack;

#include <cstdio>
#include <cstdlib>
#include <glob.h>

#include "XfstCompiler.h"
#include "xfst-utils.h"
#include "xfst-parser.h"

#ifndef DEBUG_MAIN
extern FILE* hxfstin;
extern int hxfstparse(void);

extern int hxfstlex(void);
class yy_buffer_state;
typedef yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE hxfst_scan_string(const char*);
extern void hxfst_delete_buffer(YY_BUFFER_STATE);

namespace hfst { 
namespace xfst {

  int UPPER_LEVEL=0;
  int LOWER_LEVEL=1;
  
    XfstCompiler::XfstCompiler() :
        xre_(hfst::TROPICAL_OPENFST_TYPE),
        format_(hfst::TROPICAL_OPENFST_TYPE),
        verbose_(false),
        verbose_prompt_(false)
      {
        xre_.set_expand_definitions(true);
        xre_.set_verbosity(true, stderr);
        variables_["assert"] = "OFF";
        variables_["char-encoding"] = "UTF-8";
        variables_["copyright-owner"] = "Copyleft (c) University of Helsinki";
        variables_["directory"] = "OFF";
        variables_["flag-is-epsilon"] = "OFF";
        variables_["minimal"] = "ON";
        variables_["name-nets"] = "OFF";
        variables_["obey-flags"] = "ON";
        variables_["print-pairs"] = "OFF";
        variables_["print-sigma"] = "ON";
        variables_["print-space"] = "OFF";
        variables_["quit-on-fail"] = "ON";
        variables_["quote-special"] = "OFF";
        variables_["random-seed"] = "ON";
        variables_["recode-cp1252"] = "NEVER";
        variables_["recursive-define"] = "OFF";
        variables_["retokenize"] = "OFF";
        variables_["show-flags"] = "OFF";
        variables_["sort-arcs"] = "MAYBE";
        variables_["use-timer"] = "OFF";
        variables_["verbose"] = "OFF";
        prompt();
      }
        
XfstCompiler::XfstCompiler(hfst::ImplementationType impl) :
        xre_(impl),
        format_(impl),
        verbose_(false),
        verbose_prompt_(false)
      {
        xre_.set_expand_definitions(true);
        xre_.set_verbosity(true, stderr);
        variables_["assert"] = "OFF";
        variables_["char-encoding"] = "UTF-8";
        variables_["copyright-owner"] = "Copyleft (c) University of Helsinki";
        variables_["directory"] = "OFF";
        variables_["flag-is-epsilon"] = "OFF";
        variables_["minimal"] = "ON";
        variables_["name-nets"] = "OFF";
        variables_["obey-flags"] = "ON";
        variables_["print-pairs"] = "OFF";
        variables_["print-sigma"] = "ON";
        variables_["print-space"] = "OFF";
        variables_["quit-on-fail"] = "ON";
        variables_["quote-special"] = "OFF";
        variables_["random-seed"] = "ON";
        variables_["recode-cp1252"] = "NEVER";
        variables_["recursive-define"] = "OFF";
        variables_["retokenize"] = "OFF";
        variables_["show-flags"] = "OFF";
        variables_["sort-arcs"] = "MAYBE";
        variables_["use-timer"] = "OFF";
        variables_["verbose"] = "OFF";
        prompt();
      }

    XfstCompiler&
    XfstCompiler::add_prop_line(char* line)
      {
        char* name = static_cast<char*>(malloc(sizeof(char)*strlen(line)));
        char* p = line;
        char* n = name;
        while ((*p != '\0') && (*p != ':'))
          {
            *n = *p;
            n++;
            p++;
          }
        *n = '\0';
        if (*p == '\0')
          {
            assert(*p != '\0');
            fprintf(stderr, "no colon in line\n");
          }
        p++;
        while (isspace(*p))
          {
            p++;
          }
        char* value = strdup(p);
        properties_[name] =  value;
        return *this;
      }

  static void print_paths(const hfst::HfstOneLevelPaths &paths, FILE* outfile=stdout)
  {
    for (hfst::HfstOneLevelPaths::const_iterator it = paths.begin();
         it != paths.end(); it++)
      {
        hfst::StringVector path = it->second;
        for (hfst::StringVector::const_iterator p = path.begin();
             p != path.end(); p++)
          {
            fprintf(outfile, "%s", p->c_str());
          }
        fprintf(outfile, "\n");
      }
  }

    XfstCompiler&
    XfstCompiler::apply_line(char* line, ApplyDirection direction)
      {
        char* token = strstrip(line);
        HfstTransducer* tmp = stack_.top();
        if (direction == APPLY_DOWN_DIRECTION)
          {
            // lookdown not yet implemented in HFST
            tmp = new HfstTransducer(*(stack_.top()));
            tmp->invert().minimize();
          }
        HfstOneLevelPaths * paths = tmp->lookup_fd(std::string(token));
        print_paths(*paths);
        if (paths->empty())
          fprintf(stdout, "???\n");
        delete paths;
        if (direction == APPLY_DOWN_DIRECTION)
          {
            delete tmp;
          }
        return *this;
      }

    XfstCompiler&
    XfstCompiler::apply_up_line(char* line)
      {
        return this->apply_line(line, APPLY_UP_DIRECTION);
      }

    XfstCompiler&
    XfstCompiler::apply_down_line(char* line)
      {
        return this->apply_line(line, APPLY_DOWN_DIRECTION);
      }

    XfstCompiler&
    XfstCompiler::apply_med_line(char* /*line*/)
      {
        fprintf(stderr, "Missing apply med %s:%d\n", __FILE__, __LINE__);
#if 0
        char* token = strstrip(line);
        HfstTransducer top = stack_.top();
        top.lookmed(token);
        for (each result)
          {
            print result;
          }
#endif
        return *this;
      }

    XfstCompiler&
    XfstCompiler::read_prop_line(char* line)
      {
        char* name = static_cast<char*>(malloc(sizeof(char)*strlen(line)));
        char* p = line;
        char* n = name;
        while ((*p != '\0') && (*p != ':'))
          {
            *n = *p;
            n++;
            p++;
          }
        *n = '\0';
        if (*p == '\0')
          {
            assert(*p != '\0');
            fprintf(stderr, "no colon in line\n");
          }
        p++;
        while (isspace(*p))
          {
            p++;
          }
        char* value = strdup(p);
        properties_[name] = value;
        return *this;
      }
    
    XfstCompiler& 
    XfstCompiler::add_props(FILE* infile)
      {
        char* line = 0;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, infile)) != -1)
          {
            add_prop_line(line);
          }
        free(line);
        prompt();
        return *this;
      }

    XfstCompiler& 
    XfstCompiler::add_props(const char* indata) 
      {
        char* s = strdup(indata);
        char* line = strtok(s, "\n");
        while (line != NULL)
          {
            add_prop_line(line);
            line = strtok(NULL, "\n");
          }
        free(s);
        prompt();
        return *this;
      }

    XfstCompiler&
    XfstCompiler::apply(FILE* infile, ApplyDirection direction)
      {
        char* line = 0;
        size_t len = 0;
        ssize_t read;
        if (infile == stdin)
          {
            if (direction == APPLY_UP_DIRECTION)
              fprintf(stdout, "apply up> ");
            if (direction == APPLY_DOWN_DIRECTION)
              fprintf(stdout, "apply down> ");
          }
        while ((read = getline(&line, &len, infile)) != -1)
          {
            if (direction == APPLY_UP_DIRECTION)
              apply_up_line(line);
            if (direction == APPLY_DOWN_DIRECTION)
              apply_down_line(line);
            
            if (infile == stdin)
              {
                if (direction == APPLY_UP_DIRECTION)
                  fprintf(stdout, "apply up> ");
                if (direction == APPLY_DOWN_DIRECTION)
                  fprintf(stdout, "apply down> ");
              }
          }
        if (infile == stdin)
          {
            fprintf(stdout, "\n");
          }
        prompt();
        return *this;
      }


    XfstCompiler&
    XfstCompiler::apply_up(FILE* infile)
      {
        return this->apply(infile, APPLY_UP_DIRECTION);
        /*        char* line = 0;
        size_t len = 0;
        ssize_t read;
        if (infile == stdin)
          {
            fprintf(stdout, "apply up> ");
          }
        while ((read = getline(&line, &len, infile)) != -1)
          {
            apply_up_line(line);
            if (infile == stdin)
              {
                fprintf(stdout, "apply up> ");
              }
          }
        if (infile == stdin)
          {
            fprintf(stdout, "\n");
          }
        prompt();
        return *this;*/
      }

    XfstCompiler&
    XfstCompiler::apply_up(const char* indata)
      {
        char* s = strdup(indata);
        char* line = strtok(s, "\n");
        while (line != NULL)
          {
            apply_up_line(line);
            line = strtok(NULL, "\n");
          }
        free(s);
        prompt();
        return *this;
      }

    XfstCompiler&
    XfstCompiler::apply_down(FILE* infile)
      {
        return this->apply(infile, APPLY_DOWN_DIRECTION);
        /*char* line = 0;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, infile)) != -1)
          {
            apply_down_line(line);
          }
        prompt();
        return *this;*/
      }

    XfstCompiler&
    XfstCompiler::apply_down(const char* indata)
      {
        char* s = strdup(indata);
        char* line = strtok(s, "\n");
        while (line != NULL)
          {
            apply_down_line(line);
            line = strtok(NULL, "\n");
          }
        free(s);
        prompt();
        return *this;
      }
    XfstCompiler&
    XfstCompiler::apply_med(FILE* infile)
      {
        char* line = 0;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, infile)) != -1)
          {
            apply_med_line(line);
          }
        return *this;
      }

    XfstCompiler&
    XfstCompiler::apply_med(const char* indata)
      {
        char* s = strdup(indata);
        char* line = strtok(s, "\n");
        while (line != NULL)
          {
            apply_med_line(line);
            line = strtok(NULL, "\n");
          }
        free(s);
        prompt();
        return *this;
      }

    XfstCompiler&
    XfstCompiler::define_alias(const char* name, const char* commands)
      {
        aliases_[name] = commands;
        prompt();
        return *this;
      }

    XfstCompiler& 
    XfstCompiler::define_list(const char* name, const char* start,
                              const char* end)
      {
        if ((strlen(start) > 1) || (strlen(end) > 1))
          {
            fprintf(stderr, "unsupported unicode range %s-%s\n", start, end);
          }
        list<string> l;
        for (char c = *start; c < *end; c++)
          {
            char *s = static_cast<char*>(malloc(sizeof(char)*2));
            *s = c;
            *(s+1) = '\0';
            l.push_back(s);
          }
        lists_[name] = l;
        return *this;
      }

    XfstCompiler&
    XfstCompiler::define_list(const char* name, const char* values)
      {
        list<string> l;
        char* p = strdup(values);
        char* token = strtok(p, " ");
        while (token != NULL)
          {
            l.push_back(token);
            token = strtok(NULL, " ");
          }
        free(p);
        lists_[name] = l;
        prompt();
        return *this;
      }

  XfstCompiler& 
  XfstCompiler::define(const char* name, const char* xre)
    {
      HfstTransducer* compiled = xre_.compile(xre);
      xre_.define(name, xre);
      definitions_[name] = compiled;
      prompt();
      return *this;
    }

  XfstCompiler&
  XfstCompiler::define(const char* name, const char* prototype,
                       const char* xre)
    {
      HfstTransducer* compiled = xre_.compile(xre);
      char* signature = static_cast<char*>(malloc(sizeof(char)*strlen(name)+strlen(prototype)+1));
      const char* s = name;
      char* p = signature;
      while (*s != '\0')
        {
          *p = *s;
          s++;
          p++;
        }
      s = prototype;
      while (*s != '\0')
        {
          *p = *s;
          s++;
          p++;
        }
      *p = '\0';
      functions_[signature] = compiled;
      prompt();
      return *this;
    }

  XfstCompiler&
  XfstCompiler::undefine(const char* name_list)
    {
      char* s = strdup(name_list);
      char* name = strtok(s, " ");
      while (name != NULL)
        {
          if (definitions_.find(name) != definitions_.end())
            {
              definitions_.erase(definitions_.find(name));
            }
          name = strtok(NULL, " ");
        }
      free(s);
      prompt();
      return *this;
    }

  XfstCompiler&
  XfstCompiler::unlist(const char* name)
    {
      if (lists_.find(name) != lists_.end())
        {
          lists_.erase(lists_.find(name));
        }
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::name(const char* name)
    {
      names_[name] = stack_.top();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::load_definitions(FILE* /* infile */)
    {
      fprintf(stderr, "Cannot load definitions %s:%d\n", __FILE__,
              __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::apropos(const char* /* text */)
    {
      fprintf(stderr, "Apropoo %s:%d\n",
              __FILE__, __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler&
  XfstCompiler::describe(const char* /* text */)
    {
      fprintf(stderr, "HELP! %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::clear()
    {
      while (!stack_.empty())
        {
          stack_.pop();
        }
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::pop()
    {
      stack_.pop();
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::push(const char* name)
    {
      stack_.push(definitions_[name]);
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::push()
    {
      stack_.push(last_defined_);
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::turn()
    {
      queue<HfstTransducer*> tmp;
      while (!stack_.empty())
        {
          tmp.push(stack_.top());
          stack_.pop();
        }
      while (!tmp.empty())
        {
          stack_.push(tmp.front());
          tmp.pop();
        }
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler&
  XfstCompiler::rotate()
    {
      HfstTransducer* top = stack_.top();
      stack_.pop();
      stack<HfstTransducer*> tmp;
      while (!stack_.empty())
        {
          tmp.push(stack_.top());
          stack_.pop();
        }
      tmp.push(top);
      while (!tmp.empty())
        {
          stack_.push(tmp.top());
          tmp.pop();
        }
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::load_stack(const char* infile)
    {
      HfstInputStream* instream = 0;
      try 
        {
          instream = (infile != 0) ?
            new HfstInputStream(infile):
            new HfstInputStream();
        }
      catch (NotTransducerStreamException ntse)
        {
          fprintf(stderr, "Unable to read transducers from %s\n", 
                  infile);
          return *this;
        }
      while (instream->is_good())
        {
          HfstTransducer* t = new HfstTransducer(*instream);
          stack_.push(t);
        }
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::collect_epsilon_loops()
    {
      fprintf(stderr, "cannot collect epsilon loops %s:%d\n", __FILE__,
              __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::compact_sigma()
    {
      stack_.top()->prune_alphabet();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::eliminate_flag(const char* name)
    {
      fprintf(stderr, "cannot eliminate flag %s %s:%d\n", name,
              __FILE__, __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::eliminate_flags()
    {
      fprintf(stderr, "cannot eliminate flags %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::echo(const char* text)
    {
      fprintf(stdout, "%s\n", text);
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::quit(const char* message)
    {
      if (verbose_ && (strcmp(message, "dodongo") == 0))
        {
          fprintf(stdout, "dislikes smoke.\n");
        }
      else if (verbose_)
        {
          fprintf(stdout, "%s.\n", message);
        }
      else
        ;
      exit(EXIT_SUCCESS);
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::system(const char* command)
    {
      int rv = ::system(command);
      if (rv != 0)
        {
          fprintf(stderr, "system %s returned %d\n", command, rv);
        }
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::set(const char* name, const char* text)
    {
      variables_[name] = text;
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::set(const char* name, unsigned int number)
    {
      char* num = static_cast<char*>(malloc(sizeof(char)*31));
      sprintf(num, "%u", number);
      variables_[name] = num;
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::show(const char* name)
    {
      fprintf(stdout, "variable %s = %s\n", name, variables_[name].c_str());
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::show()
    {
      for (map<string,string>::const_iterator var = variables_.begin();
           var != variables_.end();
           var++)
        {
          fprintf(stderr, "%20s:%5s: <EXPLANATION MISSING>\n", 
                  var->first.c_str(), var->second.c_str());
        }
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::test_eq() 
    {
      if (stack_.size() < 2)
        {
          fprintf(stderr, "Not enough networks on stack. Operation requires at least 2.\n");
          return *this;
        }
      HfstTransducer* first = stack_.top();
      stack_.pop();
      HfstTransducer* second = stack_.top();
      stack_.pop();
      print_bool(first->compare(*second, false));
      stack_.push(second);
      stack_.push(first);
      return *this;
    }

  XfstCompiler&
  XfstCompiler::print_bool(bool value)
  {
    int printval = (value)? 1 : 0; 
    fprintf(stdout, "%i, (1 = TRUE, 0 = FALSE)\n", printval);
    return *this;
  }
  HfstTransducer *
  XfstCompiler::top()
  {
    if (stack_.size() < 1)
      {
        fprintf(stderr, "Empty stack.\n");
        return NULL;
      }
    return stack_.top();
  }  

  XfstCompiler& 
  XfstCompiler::test_funct()
    {
      fprintf(stderr, "test funct missing %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler&
  XfstCompiler::test_id()
    {
      HfstTransducer * tmp = this->top();
      if (NULL == tmp)
        return *this;
      HfstTransducer id(hfst::internal_identity, tmp->get_type());
      this->print_bool(id.compare(*tmp, false));
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::test_upper_bounded()
    {
      fprintf(stderr, "test ub missing %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::test_uni(int level)
    {
      HfstTransducer * temp = this->top();
      if (NULL == temp)
        return *this;
      HfstTransducer tmp(*temp);
      tmp.input_project();
      HfstTransducer id(hfst::internal_identity, tmp.get_type());
      bool value = false;
      if (level == UPPER_LEVEL)
        value = id.compare(tmp, false);
      else if (level == LOWER_LEVEL)
        value = ! id.compare(tmp, false);
      else
        fprintf(stderr, "ERROR: argumnent given to function 'test_uni'\n"
                "not recognized\n");
      this->print_bool(value);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::test_upper_uni()
    {
      return this->test_uni(UPPER_LEVEL);
    }
  XfstCompiler& 
  XfstCompiler::test_lower_bounded()
    {
      fprintf(stderr, "test lb missing %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::test_lower_uni()
    {
      return this->test_uni(LOWER_LEVEL);
    }
  XfstCompiler& 
  XfstCompiler::test_nonnull()
    {
      return this->test_null(true);
    }
  XfstCompiler& 
  XfstCompiler::test_null(bool invert)
    {
      HfstTransducer * tmp = this->top();
      if (NULL == tmp)
        return *this;
      HfstTransducer empty(tmp->get_type());
      bool value = empty.compare(*tmp, false);
      if (invert)
        value = !value;
      this->print_bool(value);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::test_overlap()
    {
      fprintf(stderr, "test overlap missing %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::test_sublanguage()
    {
      fprintf(stderr, "test sublang missing %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::test_unambiguous()
    {
      fprintf(stderr, "test unambiguous missing %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::substitute(const char* src, const char* target)
    {
      HfstTransducer* top = this->top();
      if (top == NULL)
        return *this;
      stack_.pop();
      top->substitute(src, target);
      stack_.push(top);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::substitute(const char** list, const char* target)
    {
      HfstTransducer* top = this->top();
      if (top == NULL)
        return *this;
      stack_.pop();
      for (const char* src = *list; src != 0; src++)
        {
          if (strchr(src, ':') != 0)
            {
              fprintf(stderr, "missing substitute %s:%d\n", __FILE__, __LINE__);
            }
          top->substitute(src, target);
        }
      stack_.push(top);
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::print_aliases(FILE* outfile)
    {
      for (map<string,string>::const_iterator alias = aliases_.begin();
           alias != aliases_.end();
           ++alias)
        {
          fprintf(outfile, "alias %10s %s", 
                  alias->first.c_str(), alias->second.c_str());
        }
      prompt();
      return *this;
    }
          
  XfstCompiler& 
  XfstCompiler::print_arc_count(const char* level, FILE* outfile)
    {
      fprintf(outfile, "missing %s arc count %s:%d\n", level,
              __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_arc_count(FILE* outfile)
    {
      fprintf(outfile, "missing arc count %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_defined(FILE* outfile)
    {
      for (map<string,HfstTransducer*>::const_iterator def = definitions_.begin();
           def != definitions_.end();
           ++def)
        {
          fprintf(outfile, "%10s ? bytes. ? states, ? arcs, ? paths\n",
                  def->first.c_str());
        }
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::print_dir(const char* globdata, FILE* outfile)
    {
      glob_t globbuf;

      int rv = glob(globdata, 0, NULL, &globbuf);
      if (rv == 0)
        {
          for (unsigned int i = 0; i < globbuf.gl_pathc; i++)
            {
              fprintf(outfile, "%s\n", globbuf.gl_pathv[i]);
            }
        }
      else
        {
          fprintf(outfile, "glob(%s) = %d\n", globdata, rv);
        }
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::print_flags(FILE* outfile)
    {
      fprintf(outfile, "missing print flags %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  
  XfstCompiler& 
  XfstCompiler::print_labels(const char* name, FILE* outfile)
    {
      fprintf(outfile, "missing print %s labels %s:%d\n", name,
              __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_labels(FILE* outfile)
    {
      fprintf(outfile, "missing print labels %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_labelmaps(FILE* outfile)
    {
      fprintf(outfile, "missing label-maps %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::print_label_count(FILE* outfile)
    {
      fprintf(outfile, "missing label count %s:%d\n", __FILE__, __LINE__);
      return *this;
      prompt();
    }

  XfstCompiler&
  XfstCompiler::print_list(const char* name, FILE* outfile)
    {
      list<string> l = lists_[name];
      fprintf(outfile, "%10s:", name);
      for (list<string>::const_iterator s = l.begin();
           s != l.end();
           ++s)
        {
          fprintf(outfile, "%s ", s->c_str());
        }
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::print_list(FILE* outfile)
    {
      for (map<string,list<string> >::const_iterator l = lists_.begin();
           l != lists_.end();
           ++l)
        {
          fprintf(outfile, "%10s:", l->first.c_str());
          for (list<string>::const_iterator s = l->second.begin();
               s != l->second.end();
               ++s)
            {
              fprintf(outfile, "%s ", s->c_str());
            }
        }
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::print_shortest_string(FILE* outfile)
    {
      fprintf(outfile, "missing shortest string %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_shortest_string_size(FILE* outfile)
    {
      fprintf(outfile, "missing shortest string size %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_longest_string(FILE* outfile)
    {
      fprintf(outfile, "missing longest string %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_longest_string_size(FILE* outfile)
    {
      fprintf(outfile, "missing longes string size %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }

  static void print_paths(const hfst::HfstTwoLevelPaths &paths, FILE* outfile)
  {
    for (hfst::HfstTwoLevelPaths::const_iterator it = paths.begin();
         it != paths.end(); it++)
      {
        hfst::StringPairVector path = it->second;
        for (hfst::StringPairVector::const_iterator p = path.begin();
             p != path.end(); p++)
          {
            if (p->first == p->second) {
              fprintf(outfile, "%s", p->first.c_str()); }
            else {
              fprintf(outfile, "%s:%s", p->first.c_str(), p->second.c_str()); }
          }
        fprintf(outfile, "\n");
      }
  }

  XfstCompiler& 
  XfstCompiler::print_lower_words(const char* /* name */,
                                  unsigned int number,
                                  FILE* outfile)
    {
      fprintf(outfile, "missing %u lower words %s:%d\n", number,
              __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_random_lower(unsigned int number, FILE* outfile)
    {
      hfst::HfstTwoLevelPaths paths;
      HfstTransducer tmp = hfst::HfstTransducer(*(stack_.top()));
      tmp.output_project();
      tmp.extract_random_paths(paths, number);
      print_paths(paths, outfile);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_upper_words(const char* /* name */, unsigned int number,
                                  FILE* outfile)
    {
      fprintf(outfile, "missing %u upper words %s:%d\n", number,
              __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler&
  XfstCompiler::print_random_upper(unsigned int number, FILE* outfile)
    {
      hfst::HfstTwoLevelPaths paths;
      HfstTransducer tmp = hfst::HfstTransducer(*(stack_.top()));
      tmp.input_project();
      tmp.extract_random_paths(paths, number);
      print_paths(paths, outfile);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_words(const char* /* name */, unsigned int number,
                            FILE* outfile)
    {
      fprintf(outfile, "missing %u words %s:%d\n", number,
              __FILE__, __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::print_random_words(unsigned int number, FILE* outfile)
    {
      hfst::HfstTwoLevelPaths paths;
      stack_.top()->extract_random_paths(paths, number);
      print_paths(paths, outfile);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_name(FILE* outfile)
    {
      fprintf(outfile, "cannot tell name %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_net(FILE* outfile)
    {
      HfstTransducer * tmp = this->top();
      if (tmp == NULL)
        return *this;
      tmp->write_in_att_format(outfile);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_net(const char* /*name*/, FILE* outfile)
    {
      fprintf(outfile, "missing print net %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }

  static bool is_special_symbol(const std::string &s)
  {
    if (s == hfst::internal_epsilon || s == hfst::internal_unknown || 
        s == hfst::internal_identity ) {
      return true;
    }
    return false;
  }

  static void print_alphabet(const StringSet & alpha, FILE* outfile)
  {
    unsigned int sigma_count=0;
    fprintf(outfile, "Sigma: ");
    for (StringSet::const_iterator it = alpha.begin(); it != alpha.end(); it++)
      {
        if (! is_special_symbol(*it)) 
          {
            fprintf(outfile, "%s", it->c_str()); 
            sigma_count++;
            if (++it != alpha.end()) {
              fprintf(outfile, ", ");
            }
            --it;
          }
      }
    fprintf(outfile, "\n");
    fprintf(outfile, "Size: %d\n", sigma_count);
  }

  XfstCompiler& 
  XfstCompiler::print_sigma(FILE* outfile)
    {
      hfst::StringSet alpha = stack_.top()->get_alphabet();
      print_alphabet(alpha, outfile);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_sigma(const char* /*name*/, FILE* outfile)
    {
      fprintf(outfile, "missing print sigma %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_sigma_count(FILE* outfile)
    {
      fprintf(outfile, "missing print sigma count %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_sigma_word_count(const char* level, FILE* outfile)
    {
      fprintf(outfile, "missing %s sigma word count %s:%d\n", level,
              __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_sigma_word_count(FILE* outfile)
    {
      fprintf(outfile, "missing sigma word count %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_size(const char* name, FILE* outfile)
    {
      fprintf(outfile, "%10s: ", name);
      fprintf(outfile, "? bytes. ? states, ? arcs, ? paths.\n");
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_size(FILE* outfile)
    {
      fprintf(outfile, "? bytes. ? states, ? arcs, ? paths.\n");
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::print_stack(FILE* outfile)
    {
      stack<HfstTransducer*> tmp;
      int i = 0;
      while (!stack_.empty())
        {
          fprintf(outfile, "%10d: ? bytes. ? states, ? arcs, ? paths.\n", i);
          tmp.push(stack_.top());
          stack_.pop();
          i++;
        }
      while (!tmp.empty())
        {
          stack_.push(tmp.top());
          tmp.pop();
        }
      prompt();
      return *this;
    }
        
  XfstCompiler& 
  XfstCompiler::write_dot(FILE* outfile)
    {
      fprintf(outfile, "missing write dot %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler&
  XfstCompiler::write_dot(const char* /*name*/, FILE* outfile)
    {
      fprintf(outfile, "missing write dot %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::write_prolog(FILE* outfile)
    {
      fprintf(outfile, "missing write prolog %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::write_spaced(FILE* outfile)
    {
      fprintf(outfile, "missing write spaced %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::write_text(FILE* outfile)
    {
      fprintf(outfile, "missing write text %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::write_function(const char* name, const char* outfile)
    {
      if (outfile == 0)
        {
          fprintf(stdout, "%10s: %p\n", name, functions_[name]);
        }
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::write_definition(const char* name, const char* outfile)
    {
      fprintf(stderr, "Warning: cannot to save name of definition with "
              "transducer: %s:%d\n", __FILE__, __LINE__);
      HfstOutputStream* outstream = (outfile != 0) ?
        new HfstOutputStream(outfile, format_):
        new HfstOutputStream(format_);
      *outstream << *(definitions_[name]);
      outstream->close();
      delete outstream;
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::write_definitions(const char* outfile)
    {
      fprintf(stderr, "Warning: cannot save names of definitions with "
              "transducers: %s:%d", __FILE__, __LINE__);
      HfstOutputStream* outstream = (outfile != 0) ?
        new HfstOutputStream(outfile, format_):
        new HfstOutputStream(format_);
      for (map<string,HfstTransducer*>::iterator def = definitions_.begin();
           def != definitions_.end();
           ++def)
        {
          *outstream << *(def->second);
        }
      outstream->close();
      delete outstream;
      prompt();
      return *this;
    }

  XfstCompiler&
  XfstCompiler::write_stack(const char* filename)
    {
      fprintf(stderr, "cannot save transducer names %s:%d\n", __FILE__,
              __LINE__);
      HfstOutputStream* outstream = (filename != 0)?
        new HfstOutputStream(filename, format_):
        new HfstOutputStream(format_);
      stack<HfstTransducer*> tmp;
      while (!stack_.empty())
        {
          tmp.push(stack_.top());
          stack_.pop();
        }
      while (!tmp.empty())
        {
          *outstream << *(tmp.top());
          stack_.push(tmp.top());
          tmp.pop();
        }
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::read_props(FILE* infile)
    {
        char* line = 0;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, infile)) != -1)
          {
            add_prop_line(line);
          }
        free(line);
        prompt();
        return *this;
    }
  XfstCompiler& 
  XfstCompiler::read_props(const char* indata)
    {
        char* s = strdup(indata);
        char* line = strtok(s, "\n");
        while (line != NULL)
          {
            add_prop_line(line);
            line = strtok(NULL, "\n");
          }
        free(s);
        prompt();
        return *this;
    }
  XfstCompiler& 
  XfstCompiler::read_regex(FILE* infile)
    {
#define MAX_FILE_SIZE 10000000
      size_t read = 0;
      char* file_data = static_cast<char*>(malloc(sizeof(char)*MAX_FILE_SIZE+1));
      read = fread(file_data, sizeof(char), MAX_FILE_SIZE, infile);
      if ((read > 0) && (read < MAX_FILE_SIZE) && (feof(infile)))
        {
          HfstTransducer* compiled = xre_.compile(file_data);
          stack_.push(compiled);
        }
      else if (!feof(infile))
        {
          fprintf(stderr, "regex file longer than buffer :-(\n");
        }
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::read_regex(const char* indata)
    {
      HfstTransducer* compiled = xre_.compile(indata);
      stack_.push(compiled);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::read_prolog(FILE* /* infile */)
    {
      fprintf(stderr, "missing read prolog %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::read_prolog(const char* /* indata */)
    {
      fprintf(stderr, "missing read prolog %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::read_spaced(FILE* /* infile */)
    {
      fprintf(stderr, "missing read spaced %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::read_spaced(const char* /* indata */)
    {
      fprintf(stderr, "missing read spaced %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::read_text(FILE* /* infile */)
    {
      fprintf(stderr, "missing read text %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::read_text(const char* /* indata */)
    {
      fprintf(stderr, "missing read text %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::cleanup_net()
    {
      fprintf(stderr, "cannot cleanup net %s:%d\n", __FILE__, __LINE__);
      if (stack_.size() < 1)
        {
          fprintf(stderr, "Empty stack.\n");
          return *this;
        }
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::complete_net()
    {
      fprintf(stderr, "missing complete net %s:%d\n", __FILE__, __LINE__);
      if (stack_.size() < 1)
        {
          fprintf(stderr, "Empty stack.\n");
          return *this;
        }
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler& 
  XfstCompiler::compose_net()
    {
      return this->apply_binary_operation_iteratively(COMPOSE_NET);
    }

  XfstCompiler& 
  XfstCompiler::concatenate_net()
    {
      return this->apply_binary_operation_iteratively(CONCATENATE_NET);
    }
  XfstCompiler& 
  XfstCompiler::crossproduct_net()
    {
      fprintf(stderr, "cannot crossproduct %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }

  XfstCompiler&
  XfstCompiler::apply_unary_operation(UnaryOperation operation)
  {
    HfstTransducer* result = this->top();
    if (result == NULL)
      return *this;
    this->pop();

    switch (operation)
      {
      case DETERMINIZE_NET:
        result->determinize();
        break;
      case EPSILON_REMOVE_NET:
        result->remove_epsilons();
        break;
      case INVERT_NET:
        result->invert();
        break;
      case LOWER_SIDE_NET:
        result->output_project();
        break;
      case UPPER_SIDE_NET:
        result->input_project();
        break;
      case ZERO_PLUS_NET:
        result->repeat_star();
        break;
      case ONE_PLUS_NET:
        result->repeat_plus();
        break;
      case OPTIONAL_NET:
        result->optionalize();
        break;
      case REVERSE_NET:
        result->reverse();
        break;
      case MINIMIZE_NET:
        result->minimize();
        break;
      default:
        fprintf(stderr, "ERROR: unknown unary operation\n");
        break;
      }

    stack_.push(result);
    print_transducer_info();
    prompt();
    return *this;
  }

  XfstCompiler&
  XfstCompiler::apply_binary_operation(BinaryOperation operation)
  {
      if (stack_.size() < 2)
        {
          fprintf(stderr, "Not enough networks on stack. Operation requires at least 2.\n");
          return *this;
        }
      HfstTransducer * result = stack_.top();
      stack_.pop();
      HfstTransducer * another = stack_.top();
      stack_.pop();

      switch (operation)
        {
        case IGNORE_NET:
          result->insert_freely(*another);
          break;
        case MINUS_NET:
          result->subtract(*another);
          break;
        default:
          fprintf(stderr, "ERROR: unknown binary operation\n");
          break;
        }

      result->minimize();
      delete another;
      stack_.push(result);
      print_transducer_info();
      prompt();
      return *this;
  }

  XfstCompiler&
  XfstCompiler::apply_binary_operation_iteratively(BinaryOperation operation)
  {
    if (stack_.size() < 1)
      {
        fprintf(stderr, "Empty stack.\n");
        return *this;
      }
    HfstTransducer* result = stack_.top();
    stack_.pop();
    while (!stack_.empty())
      {
        HfstTransducer* t = stack_.top();
        switch (operation)
          {
          case INTERSECT_NET:
            result->intersect(*t);
            break;
          case IGNORE_NET:
            result->insert_freely(*t);
            break;
          case COMPOSE_NET:
            result->compose(*t);
            break;
          case CONCATENATE_NET:
            result->concatenate(*t);
            break;
          case UNION_NET:
            result->disjunct(*t);
            break;
          case SHUFFLE_NET:
            result->shuffle(*t);
            break;
          default:
            fprintf(stderr, "ERROR: unknown binary operation\n");
            break;
          }
        stack_.pop();
        delete t;
      }
    stack_.push(result);
    print_transducer_info();
    prompt();
    return *this;
  }

  XfstCompiler& 
  XfstCompiler::determinize_net()
    {
      return this->apply_unary_operation(DETERMINIZE_NET);
    }
  XfstCompiler& 
  XfstCompiler::epsilon_remove_net()
    {
      return this->apply_unary_operation(EPSILON_REMOVE_NET);
    }
  XfstCompiler& 
  XfstCompiler::ignore_net()
    {
      return this->apply_binary_operation(IGNORE_NET);
    }
  XfstCompiler& 
  XfstCompiler::intersect_net()
    {
      return this->apply_binary_operation_iteratively(INTERSECT_NET);
    }
  XfstCompiler& 
  XfstCompiler::invert_net()
    {
      return this->apply_unary_operation(INVERT_NET);
    }
  XfstCompiler& 
  XfstCompiler::label_net()
    {
      fprintf(stderr, "missing label net %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::lower_side_net()
    {
      return this->apply_unary_operation(LOWER_SIDE_NET);
    }
  XfstCompiler& 
  XfstCompiler::upper_side_net()
    {
      return this->apply_unary_operation(UPPER_SIDE_NET);
    }
  XfstCompiler& 
  XfstCompiler::minimize_net()
    {
      return this->apply_unary_operation(MINIMIZE_NET);
    }
  XfstCompiler& 
  XfstCompiler::minus_net()
    {
      return this->apply_binary_operation(MINUS_NET);
    }
  XfstCompiler& 
  XfstCompiler::name_net(const char* s)
    {
      if (stack_.size() < 1)
        {
          fprintf(stderr, "Empty stack.\n");
          return *this;
        }
      HfstTransducer* t = stack_.top();
      names_[s] = t;
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::negate_net()
    {
      if (stack_.size() < 1)
        {
          fprintf(stderr, "Empty stack.\n");
          return *this;
        }

      HfstTransducer * result = new HfstTransducer(hfst::internal_identity, hfst::internal_identity, format_);
      HfstTransducer unk2unk(hfst::internal_unknown, hfst::internal_unknown, format_);
      result->disjunct(unk2unk);
      result->repeat_star();
      result->minimize();

      HfstTransducer* t = stack_.top();
      stack_.pop();
      result->subtract(*t);
      delete t;
      
      stack_.push(result);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::one_plus_net()
    {
      return this->apply_unary_operation(ONE_PLUS_NET);
    }
  XfstCompiler& 
  XfstCompiler::zero_plus_net()
    {
      return this->apply_unary_operation(ZERO_PLUS_NET);
    }
  XfstCompiler&
  XfstCompiler::optional_net()
    {
      return this->apply_unary_operation(OPTIONAL_NET);
    }
  XfstCompiler& 
  XfstCompiler::prune_net()
    {
      fprintf(stderr, "missing prune net %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::reverse_net()
    {
      return this->apply_unary_operation(REVERSE_NET);
    }
  XfstCompiler& 
  XfstCompiler::shuffle_net()
    {
      return this->apply_binary_operation_iteratively(SHUFFLE_NET);
    }
  XfstCompiler& 
  XfstCompiler::sigma_net()
    {
      fprintf(stderr, "missing sigma net %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::sort_net()
    {
      fprintf(stderr, "cannot sort net %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::substring_net()
    {
      fprintf(stderr, "missing substring net %s:%d\n", __FILE__, __LINE__);
      print_transducer_info();
      prompt();
      return *this;
    }
  XfstCompiler&
  XfstCompiler::union_net()
    {
      return this->apply_binary_operation_iteratively(UNION_NET);
    }

  XfstCompiler&
  XfstCompiler::print_file_info(FILE* outfile)
    {
      fprintf(outfile, "file info not implemented (cf. summarize) %s:%d\n",
              __FILE__, __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler&
  XfstCompiler::print_properties(const char* /* name */, FILE* outfile)
    {
      fprintf(outfile, "missing print properties %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler&
  XfstCompiler::inspect_net()
    {
      fprintf(stderr, "missing inspect net %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler&
  XfstCompiler::compile_replace_upper_net()
    {
      fprintf(stderr, "missing compile_replace_upper net %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }
  XfstCompiler&
  XfstCompiler::compile_replace_lower_net()
    {
      fprintf(stderr, "missing compile_replace_lower net %s:%d\n", __FILE__, __LINE__);
      prompt();
      return *this;
    }

  XfstCompiler&
  XfstCompiler::hfst(const char* text)
    {
      fprintf(stderr, "HFST: %s\n", text);
      prompt();
      return *this;
    }
  XfstCompiler&
  XfstCompiler::read_lexc(FILE* infile)
    {
      lexc_.parse(infile);
      prompt();
      return *this;
    }
  const std::stack<HfstTransducer*>&
  XfstCompiler::get_stack() const 
    {
      return stack_;
    }
  int
  XfstCompiler::parse(FILE* infile)
    {
      hxfstin = infile;
      xfst_ = this;
      //YY_BUFFER_STATE bs = hxfst_create_buffer(hxfstin, 32000);
      int rv = hxfstparse();
      //hxfst_delete_buffer(bs);
      return rv;
    }
  int
  XfstCompiler::parse(const char* filename)
    {
      hxfstin = fopen(filename, "r");
      if (hxfstin == NULL)
        {
          fprintf(stderr, "could not open %s for reading\n", filename);
          return -1;
        }
      xfst_ = this;
      //YY_BUFFER_STATE bs = hxfst_create_buffer(hxfstin, 32000);
      int rv = hxfstparse();
      //hxfst_delete_buffer(bs);
      fclose(hxfstin);
      return rv;
    }
  int
  XfstCompiler::parse_line(char line[])
  {
    hxfstin = NULL;
    xfst_ = this;
    YY_BUFFER_STATE bs = hxfst_scan_string(line);
    hxfstparse();
    hxfst_delete_buffer(bs);
    return 0;
  }
  int
  XfstCompiler::parse_line(std::string line)
  {
    hxfstin = NULL;
    xfst_ = this;
    char * line_ = strdup(line.c_str());
    YY_BUFFER_STATE bs = hxfst_scan_string(line_);
    hxfstparse();
    hxfst_delete_buffer(bs);
    free(line_);
    return 0;
  }

  XfstCompiler&
  XfstCompiler::print_properties(FILE* outfile)
    {
      fprintf(outfile, "missiing print properties %s:%d\n", __FILE__, __LINE__);
      return *this;
    }
  XfstCompiler& 
  XfstCompiler::setVerbosity(bool verbosity)
    {
      verbose_ = verbosity;
      xre_.set_verbosity(verbosity, stderr);
      return *this;
    }
  XfstCompiler&
  XfstCompiler::setPromptVerbosity(bool verbosity)
  {
    verbose_prompt_ = verbosity;
    return *this;
  }

  const XfstCompiler&
  XfstCompiler::prompt() const
    {
      if (verbose_prompt_ && verbose_)
        {
          fprintf(stdout, "hfst[" SIZE_T_SPECIFIER "]: ", stack_.size());
        }
      return *this;
    }

  char*
  XfstCompiler::get_prompt() const
  {
    char p[256];
    sprintf(p, "hfst[" SIZE_T_SPECIFIER "]: ", stack_.size());
    return strdup(p);
  }

  const XfstCompiler&
  XfstCompiler::print_transducer_info() const
    {
      if (verbose_ && !stack_.empty())
        {
          HfstTransducer* top = stack_.top();
          fprintf(stdout, "? bytes. %i states, %i arcs, ? paths\n",
                  top->number_of_states(), top->number_of_arcs());
        }
      return *this;
    }
// silly globls
XfstCompiler* xfst_ = 0;

}}

#else
#include <cstdlib>
#include <cassert>

using namespace hfst;
using namespace hfst::xfst;

int
main(int argc, char** argv)
  {
    std::cout << "Unit tests for " __FILE__ ":";
    std::cout << std::endl << "constructors:";
    std::cout << " (default)";
    XfstCompiler defaultXfst();
#if HAVE_SFST
    std::cout << " (SFST)";
    XfstCompiler sfstXfst(SFST_TYPE);
#endif
#if HAVE_OPENFST
    std::cout << " (OpenFst)";
    XfstCompiler ofstXfst(TROPICAL_OPENFST_TYPE);
#endif
#if HAVE_SFST
    std::cout << " (foma)";
    XfstCompiler fomaXfst(FOMA_TYPE);
#endif
    // FIXME: test as implemented.
    return EXIT_SUCCESS;
  }
#endif

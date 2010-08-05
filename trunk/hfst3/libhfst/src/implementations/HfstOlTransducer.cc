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

#include <cstring>
#include "HfstOlTransducer.h"

namespace hfst { namespace implementations
{
  HfstOlInputStream::HfstOlInputStream(bool weighted):
    i_stream(),input_stream(std::cin), weighted(weighted)
  {}
  HfstOlInputStream::HfstOlInputStream(const char * filename, bool weighted):
    filename(filename),i_stream(filename),input_stream(i_stream),weighted(weighted)
  {}
  
  /* Skip the identifier string "HFST_OL_TYPE" or "HFST_OLW_TYPE" */
  void HfstOlInputStream::skip_identifier_version_3_0(void)
  { input_stream.ignore((weighted?14:13)); }

  void HfstOlInputStream::skip_hfst_header(void)
  {
    input_stream.ignore(6);
    skip_identifier_version_3_0();
  }
  
  void HfstOlInputStream::open(void) {}
  void HfstOlInputStream::close(void)
  {
    if (filename != string())
      { i_stream.close(); }
  }
  bool HfstOlInputStream::is_open(void) const
  { 
    if (filename != string())
      { return i_stream.is_open(); }
    return true;
  }
  bool HfstOlInputStream::is_eof(void) const
  {
    return input_stream.peek() == EOF;
  }
  bool HfstOlInputStream::is_bad(void) const
  {
    if (filename == string())
      { return std::cin.bad(); }
    else
      { return input_stream.bad(); }    
  }
  bool HfstOlInputStream::is_good(void) const
  {
    if(is_eof())
      return false;
    
    if (filename == string())
      { return std::cin.good(); }
    else
      { return input_stream.good(); }
  }
  
  bool HfstOlInputStream::is_fst(void) const
  {
    return (is_fst(input_stream)!=0);
  }
  
  int HfstOlInputStream::is_fst(FILE * f)
  {
    if (f == NULL)
      { return false; }
    
    char buffer[24];
    size_t num_read = fread(buffer, 24, 1, f);
    unsigned int weighted = *((int*)(buffer+20));
    int res;
    if(num_read != 24)
      { res = 0; }
    else if(weighted == 0)
      { res = 1; }
    else if(weighted == 1)
      { res = 2; }
    else
      { res = 0; }
    
    for(int i=num_read-1;i>=0;i--)
      { ungetc(buffer[i], f); }
    if(num_read != 24)
      { clearerr(f); }
    
    return res;
  }
  
  int HfstOlInputStream::is_fst(istream &s)
  {
    if(!s.good())
      { return 0; }
    
    char buffer[24];
    s.read(buffer, 24);
    size_t num_read = s.gcount();
    unsigned int weighted = *((int*)(buffer+20));
    int res;
    if(num_read != 24)
      { res = 0; }
    else if(weighted == 0)
      { res = 1; }
    else if(weighted == 1)
      { res = 2; }
    else
      { res = 0; }
    
    for(int i=num_read-1;i>=0;i--)
      { s.putback(buffer[i]); }
    if(num_read != 24)
      { s.clear(); }
    
    return res;
  }

  bool HfstOlInputStream::operator() (void) const
  { return is_good(); }

  hfst_ol::Transducer * HfstOlInputStream::read_transducer(bool has_header)
  {
    if (is_eof())
      { throw FileIsClosedException(); }
    try 
    {
      if (has_header)
        skip_hfst_header();
      
      hfst_ol::Transducer* t = new hfst_ol::Transducer(input_stream);
      //t->display();
      return t;
    }
    catch (TransducerHasWrongTypeException e)
      { throw e; }
  }
  
  
  HfstOlOutputStream::HfstOlOutputStream(bool weighted):
    filename(std::string()), output_stream(std::cout), weighted(weighted)
  {}

  HfstOlOutputStream::HfstOlOutputStream(const char * str, bool weighted):
    filename(str),o_stream(str,std::ios::out),output_stream(o_stream), weighted(weighted)
  {
    if (!output_stream)
      fprintf(stderr, "HfstOlOutputStream: ERROR: failbit set (3).\n");
  }

  void HfstOlOutputStream::write_3_0_library_header(std::ostream &out)
  {
    if (!out)
      fprintf(stderr, "HfstOlOutputStream: ERROR: failbit set (2).\n");
    out.write("HFST3",6);
    if(!weighted)
      out.write("HFST_OL_TYPE",13);
    else
      out.write("HFST_OLW_TYPE",14);
  }
  
  void HfstOlOutputStream::write_transducer(hfst_ol::Transducer * transducer) 
  {
    if (!output_stream)
      fprintf(stderr, "HfstOlOutputStream: ERROR: failbit set (1).\n");
    write_3_0_library_header(output_stream);
    transducer->write(output_stream);
  }
  
  void HfstOlOutputStream::open(void) {}
  void HfstOlOutputStream::close(void) 
  {
    if (filename != string())
      { o_stream.close(); }
  }
  
  hfst_ol::Transducer * HfstOlTransducer::create_empty_transducer(bool weighted)
  { return new hfst_ol::Transducer(weighted); }
  
  bool HfstOlTransducer::is_cyclic(hfst_ol::Transducer* t)
  { return t->get_header().probe_flag(hfst_ol::Cyclic); }
  
  void extract_strings
  (hfst_ol::Transducer* t, hfst_ol::TransitionTableIndex s,
   WeightedPaths<float>::Vector &results, std::set<hfst_ol::TransitionTableIndex> &states_visited,
   std::vector<hfst::FdState<hfst_ol::SymbolNumber> >* fd_state_stack, bool filter_fd)
  {
    if (states_visited.find(s) == states_visited.end())
      states_visited.insert(s);
    else
      throw TransducerIsCyclicException();
    
    // check for finality
    if(hfst_ol::indexes_transition_index_table(s))
    {
      if(t->get_index(s).final())
        results.push_back(WeightedPath<float>("","",t->get_header().probe_flag(hfst_ol::Weighted) ?
                dynamic_cast<const hfst_ol::TransitionWIndex&>(t->get_index(s)).final_weight() : 0.0f));
    }
    else
    {
      if(t->get_transition(s).final())
        results.push_back(WeightedPath<float>("","",t->get_header().probe_flag(hfst_ol::Weighted) ?
                dynamic_cast<const hfst_ol::TransitionW&>(t->get_index(s)).get_weight() : 0.0f));
    }
    
    hfst_ol::TransitionTableIndexSet transitions = t->get_transitions_from_state(s);    
    for(hfst_ol::TransitionTableIndexSet::const_iterator it=transitions.begin();it!=transitions.end();it++)
    {
      const hfst_ol::Transition& transition = t->get_transition(*it);
      hfst_ol::SymbolNumber input = transition.get_input_symbol();
      hfst_ol::SymbolNumber output = transition.get_output_symbol();
      std::string istring;
      std::string ostring;
      bool added_fd_state = false;
      
      if(fd_state_stack != NULL)
      {
        if(fd_state_stack->back().get_table().get_operation(input) != NULL)
        {
          fd_state_stack->push_back(fd_state_stack->back());
          if(fd_state_stack->back().apply_operation(input))
            added_fd_state = true;
          else {
            fd_state_stack->pop_back();
            continue; // don't follow the transition
          }
        }
      }
      
      if(input != 0 && (!filter_fd || fd_state_stack->back().get_table().get_operation(input)==NULL))
        istring = t->get_alphabet().get_symbol_table()[input];
      if(output != 0 && (!filter_fd || fd_state_stack->back().get_table().get_operation(output)==NULL))
        ostring = t->get_alphabet().get_symbol_table()[output];
      WeightedPath<float> arc_string(istring,ostring,t->get_header().probe_flag(hfst_ol::Weighted) ?
                dynamic_cast<const hfst_ol::TransitionW&>(transition).get_weight() : 0.0f);
      
      WeightedPaths<float>::Vector continuations;
      extract_strings(t,transition.get_target(),continuations,states_visited,fd_state_stack,filter_fd);
      WeightedPaths<float>::add(arc_string, continuations);
      results.insert(results.end(),continuations.begin(), continuations.end());
      
      if(added_fd_state)
        fd_state_stack->pop_back();
    }
    
    states_visited.erase(s);
  }
  
  void HfstOlTransducer::extract_strings(hfst_ol::Transducer * t, hfst::WeightedPaths<float>::Set &results, 
            FdTable<hfst_ol::SymbolNumber>* fd, bool filter_fd)
  {
    WeightedPaths<float>::Vector results_vec;
    std::set<hfst_ol::TransitionTableIndex> states_visited;
    std::vector<hfst::FdState<hfst_ol::SymbolNumber> >* fd_state_stack = (fd==NULL) ? NULL : 
        new std::vector<hfst::FdState<hfst_ol::SymbolNumber> >(1, hfst::FdState<hfst_ol::SymbolNumber>(*fd));
    
    hfst::implementations::extract_strings(t,0,results_vec,states_visited,fd_state_stack,filter_fd);
    results.insert(results_vec.begin(),results_vec.end());
  }
  
  FdTable<hfst_ol::SymbolNumber>* HfstOlTransducer::get_flag_diacritics(hfst_ol::Transducer* t)
  {
    FdTable<hfst_ol::SymbolNumber>* table = new FdTable<hfst_ol::SymbolNumber>();
    const hfst_ol::SymbolTable& symbols = t->get_alphabet().get_symbol_table();
    for(size_t i=0; i<symbols.size(); i++)
    {
      if(FdOperation::is_diacritic(symbols[i]))
        table->define_diacritic(i, symbols[i]);
    }
    return table;
  }

} }

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

#include "transducer.h"

namespace hfst_ol {

TransducerAlphabet::TransducerAlphabet(istream& is, SymbolNumber symbol_count)
{
  for(SymbolNumber i=0; i<symbol_count; i++)
  {
    string str;
    std::getline(is, str, '\0');
    symbol_table.push_back(str.c_str());
    if(FdOperation::is_diacritic(str))
      fd_table.define_diacritic(i, str);
  }
  if(!is)
    throw TransducerHasWrongTypeException();
}

TransducerAlphabet::TransducerAlphabet(const SymbolTable& st):
  symbol_table(st)
{
  for(SymbolNumber i=0; i<symbol_table.size(); i++)
  {
    if(FdOperation::is_diacritic(symbol_table[i]))
      fd_table.define_diacritic(i, symbol_table[i]);
  }
}

void TransducerAlphabet::display() const
{
  std::cout << "Transducer alphabet:" << std::endl;
  for(size_t i=0;i<symbol_table.size();i++)
    std::cout << " Symbol " << i << ": " << symbol_table[i] << std::endl;
}

void TransitionIndex::display() const
{
  std::cout << "input_symbol: " << input_symbol << ", target: " << first_transition_index << (final()?" (final)":"") << std::endl;
}
void Transition::display() const
{
  std::cout << "input_symbol: " << input_symbol << ", output_symbol: " << output_symbol << ", target: " << target_index << (final()?" (final)":"") << std::endl;
}
void TransitionW::display() const
{
  std::cout << "input_symbol: " << input_symbol << ", output_symbol: " << output_symbol
            << ", target: " << target_index << ", weight: " << transition_weight << (final()?" (final)":"") << std::endl;
}

bool TransitionIndex::matches(SymbolNumber s) const
{
  return input_symbol != NO_SYMBOL_NUMBER && input_symbol == s;
}
bool Transition::matches(SymbolNumber s) const
{
  return input_symbol != NO_SYMBOL_NUMBER && input_symbol == s;
}

bool TransitionIndex::final() const
{
  return input_symbol == NO_SYMBOL_NUMBER && first_transition_index != NO_TABLE_INDEX;
}
bool Transition::final() const
{
  return input_symbol == NO_SYMBOL_NUMBER && output_symbol == NO_SYMBOL_NUMBER && target_index == 1;
}

Weight TransitionWIndex::final_weight(void) const
{
  union to_weight
  {
    TransitionTableIndex i;
    Weight w;
  } weight;
  weight.i = first_transition_index;
  return weight.w;
}

Transducer::Transducer(): header(NULL), alphabet(NULL), tables(NULL) {}

Transducer::Transducer(istream& is):
  header(new TransducerHeader(is)), alphabet(new TransducerAlphabet(is, header->symbol_count())), tables(NULL)
{
  load_tables(is);
}
Transducer::Transducer(bool weighted):
  header(new TransducerHeader(weighted)), alphabet(new TransducerAlphabet())
{
  if(weighted)
    tables = new TransducerTables<TransitionWIndex,TransitionW>();
  else
    tables = new TransducerTables<TransitionIndex,Transition>();
}

Transducer::Transducer(const TransducerHeader& header, const TransducerAlphabet& alphabet,
                       const TransducerTable<TransitionIndex>& index_table,
                       const TransducerTable<Transition>& transition_table):
  header(new TransducerHeader(header)), alphabet(new TransducerAlphabet(alphabet)), tables(new TransducerTables<TransitionIndex,Transition>(index_table, transition_table)) {}
Transducer::Transducer(const TransducerHeader& header, const TransducerAlphabet& alphabet,
                       const TransducerTable<TransitionWIndex>& index_table,
                       const TransducerTable<TransitionW>& transition_table):
  header(new TransducerHeader(header)), alphabet(new TransducerAlphabet(alphabet)), tables(new TransducerTables<TransitionWIndex,TransitionW>(index_table, transition_table)) {}
Transducer::~Transducer()
{
  delete header;
  delete alphabet;
  delete tables;
}

void Transducer::load_tables(istream& is)
{
  if(header->probe_flag(Weighted))
    tables = new TransducerTables<TransitionWIndex,TransitionW>(is,
                      header->index_table_size(),header->target_table_size());
  else
    tables = new TransducerTables<TransitionIndex,Transition>(is,
                      header->index_table_size(),header->target_table_size());
  if(!is)
    throw TransducerHasWrongTypeException();
}

void Transducer::write(ostream& os) const
{
  header->write(os);
  alphabet->write(os);
  for(size_t i=0;i<header->index_table_size();i++)
    tables->get_index(i).write(os);
  for(size_t i=0;i<header->target_table_size();i++)
    tables->get_transition(i).write(os);
}

void Transducer::display() const
{
  std::cout << "-----Displaying optimized-lookup transducer------" << std::endl;
  header->display();
  alphabet->display();
  tables->display();
  std::cout << "-------------------------------------------------" << std::endl;
}

TransitionTableIndexSet Transducer::get_transitions_from_state(TransitionTableIndex state_index) const
{
  TransitionTableIndexSet transitions;
  
  if(indexes_transition_index_table(state_index))
  {
    // for each input symbol that has a transition from this state
    for(SymbolNumber symbol=0; symbol<header->symbol_count(); symbol++)
    {
      const TransitionIndex& test_transition_index = get_index(state_index+1+symbol);
      if(test_transition_index.matches(symbol))
      {
        // there are one or more transitions with this input symbol, starting at
        // test_transition_index.get_target()
        TransitionTableIndex transition_i = test_transition_index.get_target();
        while(true)
        {
          if(get_transition(transition_i).matches(test_transition_index.get_input_symbol()))
            transitions.insert(transition_i);
          else
            break;
          transition_i++;
        }
      }
    }
  }
  else // indexes transition table
  {
    const Transition& transition = get_transition(state_index);
    if(transition.get_input_symbol() != NO_SYMBOL_NUMBER ||
       transition.get_output_symbol() != NO_SYMBOL_NUMBER)
    {
      std::cerr << "Oops" << std::endl;
      throw;
    }
    
    TransitionTableIndex transition_i = state_index+1;
    while(true)
    {
      if(get_transition(transition_i).get_input_symbol() != NO_SYMBOL_NUMBER)
        transitions.insert(transition_i);
      else
        break;
      transition_i++;
    }
  }
  return transitions;
}

}

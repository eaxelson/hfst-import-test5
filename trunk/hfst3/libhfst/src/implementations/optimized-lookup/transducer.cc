#include "transducer.h"

namespace hfst_ol {

TransducerAlphabet::TransducerAlphabet(istream& is, SymbolNumber symbol_count)
{
  for(SymbolNumber i=0; i<symbol_count; i++)
  {
    string str;
    std::getline(is, str, '\0');
    symbol_table.push_back(str.c_str());
    if(is_flag_diacritic(str))
      flag_diacritics_set.insert(i);
  }
  if(!is)
    throw TransducerHasWrongTypeException();
}

void TransducerAlphabet::display() const
{
  std::cout << "Transducer alphabet:" << std::endl;
  for(size_t i=0;i<symbol_table.size();i++)
    std::cout << " Symbol " << i << ": " << symbol_table[i] << std::endl;
}

void TransitionIndex::display() const
{
  std::cout << "input_symbol: " << input_symbol << ", target: " << first_transition_index << std::endl;
}
void Transition::display() const
{
  std::cout << "input_symbol: " << input_symbol << ", output_symbol: " << output_symbol << ", target: " << target_index << std::endl;
}
void TransitionW::display() const
{
  std::cout << "input_symbol: " << input_symbol << ", output_symbol: " << output_symbol
            << ", target: " << target_index << ", weight: " << transition_weight << std::endl;
}

bool TransitionIndex::matches(SymbolNumber s) const
{
  if(input_symbol == NO_SYMBOL_NUMBER)
    return false;
  if(s == NO_SYMBOL_NUMBER)
    return true;
  return input_symbol == s;
}
bool Transition::matches(SymbolNumber s) const
{
  if(input_symbol == NO_SYMBOL_NUMBER)
    return false;
  if(s == NO_SYMBOL_NUMBER)
    return true;
  return input_symbol == s;
}

Transducer::Transducer(istream& is):
  header(is), alphabet(is, header.symbol_count()), tables(NULL)
{
  if(header.probe_flag(Weighted))
    tables = new TransducerTables<TransitionWIndex,TransitionW>(is,
                      header.index_table_size(),header.target_table_size());
  else
    tables = new TransducerTables<TransitionIndex,Transition>(is,
                      header.index_table_size(),header.target_table_size());
  if(!is)
    throw TransducerHasWrongTypeException();
}
Transducer::Transducer(bool weighted):
  header(weighted), alphabet(), tables(NULL)
{
  if(weighted)
    tables = new TransducerTables<TransitionWIndex,TransitionW>();
  else
    tables = new TransducerTables<TransitionIndex,Transition>();
}
Transducer::~Transducer() {delete tables;}

void Transducer::write(ostream& os) const
{
  header.write(os);
  alphabet.write(os);
  for(size_t i=0;i<header.index_table_size();i++)
    tables->get_index(i).write(os);
  for(size_t i=0;i<header.target_table_size();i++)
    tables->get_transition(i).write(os);
}

TransitionTableIndexSet Transducer::get_transitions_from_state(TransitionTableIndex state_index) const
{
  TransitionTableIndexSet transitions;
  
  if(indexes_transition_index_table(state_index))
  {
    // for each input symbol that has a transition from this state
    for(SymbolNumber symbol=0; symbol<header.symbol_count(); symbol++)
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

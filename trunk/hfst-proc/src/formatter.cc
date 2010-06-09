#include <algorithm>
#include <sstream>
#include "formatter.h"

//////////Function definitions for OutputFormatter

LookupPathVector
OutputFormatter::preprocess_finals(const LookupPathVector& finals) const
{
  LookupPathVector sorted_finals(finals.begin(), finals.end());
  std::sort(sorted_finals.begin(), sorted_finals.end(), LookupPath::compare_pointers);
  
  if(sorted_finals.size() > (unsigned int)maxAnalyses)
    return LookupPathVector(sorted_finals.begin(),sorted_finals.begin()+maxAnalyses);
  else
    return sorted_finals;
}

//////////Function definitions for ApertiumOutputFormatter

std::vector<std::string>
ApertiumOutputFormatter::process_finals(const LookupPathVector& finals) const
{
  std::vector<std::string> results;
  LookupPathVector sorted_finals = preprocess_finals(finals);
  
  for(LookupPathVector::const_iterator it=sorted_finals.begin(); it!=sorted_finals.end(); it++)
  {
    std::ostringstream res;
    res << token_stream.get_alphabet().symbols_to_string((*it)->get_output_symbols());
    if(dynamic_cast<const LookupPathW*>(*it) != NULL && displayWeightsFlag)
      res << '~' << dynamic_cast<const LookupPathW*>(*it)->get_weight() << '~';
    
    results.push_back(res.str());
  }
  return results;
}

void
ApertiumOutputFormatter::print_word(const TokenVector& surface_form, 
                                  std::vector<std::string> const &analyzed_forms) const
{
  // any superblanks in the surface form should not be printed as part of the
  // analysis output, but should be output directly afterwards
  TokenVector output_surface_form;
  std::vector<unsigned int> superblanks;
  for(TokenVector::const_iterator it=surface_form.begin(); it!=surface_form.end(); it++)
  {
    if(it->type == Superblank)
    {
      output_surface_form.push_back(Token::as_symbol(token_stream.to_symbol(*it)));
      superblanks.push_back(it->superblank_index);
    }
    else
      output_surface_form.push_back(*it);
  }

  if(printDebuggingInformationFlag)
    std::cout << "surface_form consists of " << output_surface_form.size() << " tokens" << std::endl;
  
  token_stream.ostream() << '^';
  token_stream.write_escaped(output_surface_form);
  for(std::vector<std::string>::const_iterator it=analyzed_forms.begin(); it!=analyzed_forms.end(); it++)
    token_stream.ostream() << "/" << *it;
  token_stream.ostream() << "$";
  
  for(size_t i=0;i<superblanks.size();i++)
    token_stream.ostream() << token_stream.get_superblank(superblanks[i]);
}
void
ApertiumOutputFormatter::print_unknown_word(const TokenVector& surface_form) const
{
  token_stream.ostream() << '^';
  token_stream.write_escaped(surface_form);
  token_stream.ostream() << "/*";
  token_stream.write_escaped(surface_form);
  token_stream.ostream() << '$';
}


//////////Function definitions for XeroxOutputFormatter

TokenVector
XeroxOutputFormatter::clear_superblanks(const TokenVector& tokens) const
{
  TokenVector output_tokens;
  for(TokenVector::const_iterator it=tokens.begin(); it!=tokens.end(); it++)
  {
    if(it->type == Superblank)
      output_tokens.push_back(Token::as_symbol(token_stream.get_alphabet().get_blank_symbol()));
    else
      output_tokens.push_back(*it);
  }
  return output_tokens;
}

std::vector<std::string>
XeroxOutputFormatter::process_finals(const LookupPathVector& finals) const
{
  std::vector<std::string> results;
  LookupPathVector sorted_finals = preprocess_finals(finals);
  
  for(LookupPathVector::const_iterator it=sorted_finals.begin(); it!=sorted_finals.end(); it++)
  {
    std::ostringstream res;
    res << token_stream.get_alphabet().symbols_to_string((*it)->get_output_symbols());
    if(dynamic_cast<const LookupPathW*>(*it) != NULL && displayWeightsFlag)
      res << "\t" << dynamic_cast<const LookupPathW*>(*it)->get_weight();
    
    results.push_back(res.str());
  }
  return results;
}

void
XeroxOutputFormatter::print_word(const TokenVector& surface_form,
                                std::vector<std::string> const &analyzed_forms) const
{
  std::string surface = token_stream.tokens_to_string(clear_superblanks(surface_form));
  
  for(std::vector<std::string>::const_iterator it=analyzed_forms.begin(); it!=analyzed_forms.end(); it++)
    token_stream.ostream() << surface << "\t" << *it << std::endl;
  token_stream.ostream() << std::endl;
}

void
XeroxOutputFormatter::print_unknown_word(const TokenVector& surface_form) const
{
  token_stream.ostream() << token_stream.tokens_to_string(clear_superblanks(surface_form))
                         << "\t+?" << std::endl << std::endl;
}


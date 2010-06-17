#include <algorithm>
#include <sstream>
#include "formatter.h"

//////////Function definitions for OutputFormatter

TokenVector
OutputFormatter::clear_superblanks(const TokenVector& tokens) const
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

bool
OutputFormatter::is_compound_analysis(const SymbolNumberVector& final) const
{
  for(SymbolNumberVector::const_iterator it=final.begin(); it!=final.end(); it++)
  {
    if(token_stream.get_alphabet().symbol_to_string(*it) == "+")
      return true;
  }
  return false;
}

void
OutputFormatter::remove_compound_analyses(LookupPathSet& finals) const
{
  bool has_noncompounded = false;
  
  // first look to see if there are any non-compounded analyses
  for(LookupPathSet::const_iterator it=finals.begin(); it!=finals.end(); it++)
  {
    if(!is_compound_analysis((*it)->get_output_symbols())) {
      has_noncompounded = true;
      break;
    }
  }
  
  if(has_noncompounded)
  { //there is a non-compounded analysis, check for any compounded ones
    for(LookupPathSet::iterator it=finals.begin(); it!=finals.end();)
    {
      if(is_compound_analysis((*it)->get_output_symbols()))
      {
        LookupPathSet::iterator to_delete = it;
        it++;
        finals.erase(to_delete);
      }
      else
        it++;
    }
  }
}

LookupPathSet
OutputFormatter::preprocess_finals(const LookupPathSet& finals) const
{
  LookupPathSet new_finals = LookupPathSet(finals);
  if(filter_compound_analyses)
  {
    remove_compound_analyses(new_finals);
    if(printDebuggingInformationFlag)
    {
      if(new_finals.size() < finals.size())
        std::cout << "Filtered " << finals.size()-new_finals.size() << " compound analyses" << std::endl;
    }
  }
  
  if(new_finals.size() > (unsigned int)maxAnalyses)
  {
    LookupPathSet clipped_finals(LookupPath::compare_pointers);
    LookupPathSet::const_iterator it=new_finals.begin();
    for(int i=0;i<maxAnalyses;i++,it++)
      clipped_finals.insert(*it);
    return clipped_finals;
  }
  else
    return new_finals;
}

//////////Function definitions for ApertiumOutputFormatter

std::vector<std::string>
ApertiumOutputFormatter::process_finals(const LookupPathSet& finals, CapitalizationState caps) const
{
  std::vector<std::string> results;
  LookupPathSet new_finals = preprocess_finals(finals);
  
  for(LookupPathSet::const_iterator it=new_finals.begin(); it!=new_finals.end(); it++)
  {
    std::ostringstream res;
    res << token_stream.get_alphabet().symbols_to_string((*it)->get_output_symbols(), caps);
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


//////////Function definitions for CGOutputFormatter

std::vector<std::string>
CGOutputFormatter::process_finals(const LookupPathSet& finals, CapitalizationState caps) const
{
  std::vector<std::string> results;
  LookupPathSet new_finals = preprocess_finals(finals);
  
  for(LookupPathSet::const_iterator it=new_finals.begin(); it!=new_finals.end(); it++)
  {
    std::ostringstream res;
    SymbolNumberVector output_symbols = (*it)->get_output_symbols();
    size_t tag_start = output_symbols.size();
    for(size_t i=0; i<output_symbols.size(); i++)
    {
      if(token_stream.get_alphabet().is_tag(output_symbols[i]))
      {
        tag_start = i;
        break;
      }
    }
    
    res << '"' << token_stream.get_alphabet().symbols_to_string(
      SymbolNumberVector(output_symbols.begin(),output_symbols.begin()+tag_start), caps)
        << '"';
    
    if(tag_start != output_symbols.size())
    {
      for(size_t i=tag_start; i<output_symbols.size(); i++)
      {
        std::string tag = token_stream.get_alphabet().symbol_to_string(output_symbols[i]);
        // remove the < and >
        if(tag.size() > 0 && tag[0] == '<')
        {
          tag = tag.substr(1);
          if(tag.size() > 0 && tag[tag.length()-1] == '>')
            tag = tag.substr(0,tag.length()-1);
        }
        
        res << (i==tag_start?"\t":" ") << tag;
      }
    }
    
    results.push_back(res.str());
  }
  return results;
}

void
CGOutputFormatter::print_word(const TokenVector& surface_form,
                                std::vector<std::string> const &analyzed_forms) const
{
  token_stream.ostream() << "\"<"
                         << token_stream.tokens_to_string(clear_superblanks(surface_form))
                         << ">\"" << std::endl;
  
  for(std::vector<std::string>::const_iterator it=analyzed_forms.begin(); it!=analyzed_forms.end(); it++)
    token_stream.ostream() << "\t" << *it << std::endl;
}

void
CGOutputFormatter::print_unknown_word(const TokenVector& surface_form) const
{
  std::string form = token_stream.tokens_to_string(clear_superblanks(surface_form));
  token_stream.ostream() << "\"<" << form << ">\"" << std::endl
                         << "\t\"*" << form << "\"" << std::endl;
}


//////////Function definitions for XeroxOutputFormatter

std::vector<std::string>
XeroxOutputFormatter::process_finals(const LookupPathSet& finals, CapitalizationState caps) const
{
  std::vector<std::string> results;
  LookupPathSet new_finals = preprocess_finals(finals);
  
  for(LookupPathSet::const_iterator it=new_finals.begin(); it!=new_finals.end(); it++)
  {
    std::ostringstream res;
    res << token_stream.get_alphabet().symbols_to_string((*it)->get_output_symbols(), caps);
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


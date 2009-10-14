#ifndef __RULE_STORER_H_
#define __RULE_STORER_H_
#include "CompileUtilities.h"
#include "RuleCompiler.h"
#include <iostream>
#include <hfst2/hfst.h>
#include <map>
#include <cstring>

template<class T> struct NameFstMap
{
  struct CompareStrings
  {
    bool operator() (const char * s1, const char * s2) const
    {
      return strcmp(s1,s2) < 0;
    }
  };
  typedef std::map<char *,T,CompareStrings> Type;
};


template<class T> class RuleStorer 
{
 protected:
  HFST::KeyTable * key_table;
  bool output_defined;
  const char *  output_file_name;
  bool store_names;
  const char * name_file_name;
  bool verbose;
  typename NameFstMap<T>::Type name_rule_map;

 public:
  RuleStorer(typename CompiledRule<T>::Vector * compiled,
	     HFST::KeyTable * kt,
	     bool output_name,
	     const char * output,
	     bool names,
	     const char * names_file,
	     bool verbose_b):
  key_table(kt),
  output_defined(output_name),
  output_file_name(output),
  store_names(names),
  name_file_name(names_file),
  verbose(verbose_b)
  {
    if (output_defined) { assert(output_file_name != NULL); }
    if (store_names) { assert(names_file != NULL); }
    size_t counter = 0;
    if (verbose)
      {
	std::cerr << "Joining subrules into larger rules." << std::endl;
      }
    for (typename CompiledRule<T>::Vector::iterator it = compiled->begin();
	 it != compiled->end();
	 ++it)
      {
	++counter;
	if (verbose)
	  {
	    std::cerr << "Processing subrule " << counter << " of " 
		      << compiled->size() << " subrules.\r";  
	  }
	typename CompiledRule<T>::Pair * p = *it;
	if (name_rule_map.find(p->first) != name_rule_map.end())
	  {
	    name_rule_map[p->first] = 
	      name_rule_map[p->first].intersect(p->second);
	    free(p->first);
	  }
	else
	  {
	    name_rule_map[p->first] = p->second;
	  }	
	delete p;
      }
    delete compiled;
    if (verbose)
      {
	std::cerr << std::endl;
      }
  };
  void operator() (void)
  {
    if (output_defined)
      {
	ofstream output_stream(output_file_name,std::ios::out);
	if (store_names)
	  {
	    ofstream name_stream(output_file_name,std::ios::out);
	    for (typename NameFstMap<T>::Type::iterator it = 
		 name_rule_map.begin();
		 it != name_rule_map.end();
		 ++it)
	      {
		if (verbose)
		  {
		    std::cerr << "Storing rule " << it->first << std::endl;
		  }
		name_stream << it->first << std::endl;
		free(it->first);
		it->second.store(output_stream,key_table);
		it->second.destroy_fst();
	      }
	  }
	else
	  {
	    ofstream name_stream(output_file_name,std::ios::out);
	    for (typename NameFstMap<T>::Type::iterator it = 
		 name_rule_map.begin();
		 it != name_rule_map.end();
		 ++it)
	      {
		if (verbose)
		  {
		    std::cerr << "Storing rule " << it->first << std::endl;
		  }
		free(it->first);
		it->second.store(output_stream,key_table);
		it->second.destroy_fst();
	      }
	  }
      }
    else
      {
	if (store_names)
	  {
	    ofstream name_stream(output_file_name,std::ios::out);
	    for (typename NameFstMap<T>::Type::iterator it = 
		 name_rule_map.begin();
		 it != name_rule_map.end();
		 ++it)
	      {
		if (verbose)
		  {
		    std::cerr << "Storing rule " << it->first << std::endl;
		  }
		name_stream << it->first << std::endl;
		free(it->first);
		it->second.store(std::cout,key_table);
		it->second.destroy_fst();
	      }
	  }
	else
	  {
	    for (typename NameFstMap<T>::Type::iterator it = 
		 name_rule_map.begin();
		 it != name_rule_map.end();
		 ++it)
	      {
		if (verbose)
		  {
		    std::cerr << "Storing rule " << it->first << std::endl;
		  }
		free(it->first);
		it->second.store(std::cout,key_table);
		it->second.destroy_fst();
	      }
	  }
      }
    if (verbose)
      {
	std::cerr << std::endl;
      }
  }
};

#endif

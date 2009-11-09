
#include <algorithm>

#include "hwfst-lookup.h"
#include "hfst.h"

namespace HWFST
{

	static
	KeyVector*
	reverse_key_vector(KeyVector* kv)
	{
	  if (kv == NULL)
	    {
	      return NULL;
	    }
		reverse(kv->begin(), kv->end());
		return kv;
	}

	static
	KeyVectorVector*
	reverse_inner(KeyVectorVector * key_strings)
	{
		if (key_strings == NULL)
			return NULL;
		for_each(key_strings->begin(), key_strings->end(),
		reverse_key_vector);
		return key_strings;
	}

	void add_prefix(Key k, KeyVectorVector * suffixes)
	{

		if (suffixes->empty())
		{
			suffixes->push_back(new KeyVector);
			suffixes->at(0)->push_back(k);
			return;
		}

		for (KeyVectorVector::iterator it = suffixes->begin();
			 it != suffixes->end();
			 ++it)
		{
			(*it)->push_back(k);
		}
	}

	void add_to_continuations(KeyVectorVector * continuations,
							  KeyVectorVector * suffixes)
	{
		for (KeyVectorVector::iterator it = suffixes->begin();
			 it != suffixes->end();
			 ++it)
		{
			continuations->push_back(*it);
		}
		delete suffixes;
	}

	KeyVector * find_first_continuation(StateId n,
					    KeyVector::iterator input_position,
					    KeyVector::iterator input_end_position,
					    Transducer &t,
					    StateSet &states,
					    bool &succeeded,
					    bool preserve_epsilons = false)
	{
	  KeyVector * result = NULL;
	  states.insert(n);
	  if ((input_position == input_end_position))
	    {
	      if (t.Final(n) != Weight::Zero())
		{
		  succeeded = true;
		  return new KeyVector;
		}
	      for (ArcIterator arcs(t, n);
		   not arcs.Done();
		   arcs.Next())
		{
		  Arc a = arcs.Value();
		  if (a.ilabel == 0)
		    {
		      if (states.find(a.nextstate) == states.end())
			{
			  states.insert(a.nextstate);
			  result = find_first_continuation(a.nextstate,
							   input_position,
							   input_end_position,
							   t,
							   states,
							   succeeded);
			  if (succeeded)
			    {
			      if ((a.olabel != 0) or
				  preserve_epsilons)
				result->push_back(a.olabel);
			      return result;
			    }
			  states.erase(a.nextstate);
			}
		    }
		}
	    }
	  else
	    {
	      for (ArcIterator arcs(t, n);
		   not arcs.Done();
		   arcs.Next())
		{
		  Arc a = arcs.Value();
		  if (a.ilabel == 0)
		    {
		      if (states.find(a.nextstate) == states.end())
			{
			  states.insert(a.nextstate);
			  result = find_first_continuation(a.nextstate,
							   input_position,
							   input_end_position,
							   t,
							   states,
							   succeeded);
			  if (succeeded)
			    {
			      if ((a.olabel != 0) or
				  preserve_epsilons)
				result->push_back(a.olabel);
			      return result;
			    }
			  states.erase(a.nextstate);
			}
		    }
		  else if (a.ilabel == *input_position)
		    {
		      StateSet new_states;
		      result = find_first_continuation(a.nextstate,
						       input_position + 1,
						       input_end_position,
						       t,
						       new_states,
						       succeeded);
		      if (succeeded)
			{
			  if ((a.olabel != 0) or
			      preserve_epsilons)
			    result->push_back(a.olabel);
			  return result;
			}
		    }
		}
	    }
	  return result;
	}
  
	KeyVectorVector *
	find_all_continuations(StateId n,
			       KeyVector::iterator input_position,
			       KeyVector::iterator input_end_position,
			       Transducer &t,
			       KeySet * skip_symbols,
			       bool preserve_epsilons = false)
	{
	  
	  KeyVectorVector * continuations = NULL;
	  
	  if (input_position == input_end_position)
	    {
	      for (ArcIterator arcs(t, n);
		   not arcs.Done();
		   arcs.Next())
		{
		  Arc a = arcs.Value();
		  if ((a.ilabel == 0) or 
		      (skip_symbols->find(a.ilabel) != skip_symbols->end()))
		    {
		      KeyVectorVector * suffixes =
			find_all_continuations(a.nextstate,
					       input_position,
					       input_end_position,
					       t,
					       skip_symbols);
		      if (suffixes == NULL)
			continue;
		      else if (continuations == NULL)
			{
			  continuations = new KeyVectorVector;
			}
		      add_prefix(a.olabel, suffixes);
		      add_to_continuations(continuations,
					   suffixes);
		    }
		}
	      if (t.Final(n) == Weight::Zero())
		return continuations;
	      else
		{
		  if (continuations == NULL)
		    {
		      continuations = new KeyVectorVector;
		      continuations->push_back(new KeyVector);
		      return continuations;
		    }
		  else
		    {
		      continuations->push_back(new KeyVector);
		      return continuations;
		    }
		}
	    }
	  
	  for (ArcIterator arcs(t, n);
	       not arcs.Done();
	       arcs.Next())
	    {
	      Arc a = arcs.Value();
	      if (a.ilabel == *input_position)
		{
		  KeyVectorVector * suffixes =
		    find_all_continuations(a.nextstate,
					   input_position + 1,
					   input_end_position,
					   t,
					   skip_symbols);
		  if (suffixes == NULL)
		    continue;
		  else if (continuations == NULL)
		    {
		      continuations = new KeyVectorVector;
		    }
		  add_prefix(a.olabel, suffixes);
		  add_to_continuations(continuations,
				       suffixes);
		}
	      
	      if ((a.ilabel == 0) or
		  skip_symbols->find(a.ilabel) != skip_symbols->end())
		{
		  KeyVectorVector * suffixes =
		    find_all_continuations(a.nextstate,
					   input_position,
					   input_end_position,
					   t,skip_symbols);
		  if (suffixes == NULL)
		    continue;
		  else if (continuations == NULL)
		    {
		      continuations = new KeyVectorVector;
		    }
		  add_prefix(a.olabel, suffixes);
		  add_to_continuations(continuations,
				       suffixes);
		}
	    }
	  
	  return continuations;
	}
  
	KeyVector * find_first_output_string(Transducer &t,
					     KeyVector * input)
	{
		if (input->empty())
			return new KeyVector;
		StateId start = t.Start();
		KeyVector::iterator input_position = input->begin();
		KeyVector::iterator last_input_position = input->end();
		bool succeeded = false;
		StateSet states;
		KeyVector * reversed_output =
			find_first_continuation(start,
						input_position,
						last_input_position,
						t,
						states,
						succeeded);
		return reverse_key_vector(reversed_output);

	}

	KeyVectorVector * find_all_output_strings(Transducer &t,
						  KeyVector * input,
						  KeySet * skip_symbols)
	{
		StateId start = t.Start();
		KeyVector::iterator input_position = input->begin();
		KeyVector::iterator last_input_position = input->end();
		KeyVectorVector * reversed_outputs =
		  find_all_continuations(start,
					 input_position,
					 last_input_position,
					 t,skip_symbols
					 );
		KeyVectorVector * outputs =
		  reverse_inner(reversed_outputs);
		if (outputs == NULL)
		  {
		    return new KeyVectorVector;
		  }
		return outputs;
	}

	inline Key get_token(Transducer &t, StateId n)
	{
		for (ArcIterator arcs(t, n);
			 not arcs.Done();
			 arcs.Next())
		{
			Arc a = arcs.Value();
			if (a.ilabel == 0)
				return a.olabel;
		}
		return USHRT_MAX;
	}

	inline bool input_target(Transducer &t,
							 StateId n,
							 Key input_key,
							 StateId &next_state)
	{
		for (ArcIterator arcs(t, n);
			 not arcs.Done();
			 arcs.Next())
		{
			Arc a = arcs.Value();
			if (a.ilabel == input_key)
			{
				next_state = a.nextstate;
				return true;
			}
		}
		return false;
	}

	Key get_next_maximal_prefix(Transducer &t,
				    StateId n,
				    KeyVector::iterator &input_position,
				    KeyVector::iterator input_end)
	{

		if (input_position == input_end)
			return get_token(t, n);

		StateId next_node = 0;

		if (not input_target(t, n, *input_position, next_node))
			return get_token(t, n);

		++input_position;

		Key next_token = get_next_maximal_prefix(t,
							 next_node,
							 input_position,
							 input_end);
		
		if (next_token == USHRT_MAX)
		{
			--input_position;
			return get_token(t, n);
		}

		return next_token;
	}

	KeyVector * longest_prefix_tokenization(Transducer &t,
						KeyVector * input)
	{

		KeyVector * result = new KeyVector;

		KeyVector::iterator input_position = input->begin();

		while (input_position != input->end())
		{

			Key next_token_key =
				get_next_maximal_prefix(t,
							t.Start(),
							input_position,
							input->end());

			if (next_token_key == USHRT_MAX)
			{
				delete result;
				return NULL;
			}

			result->push_back(next_token_key);
		}

		return result;
	}


// below is flag diacritic code
// this static map holds keys to flag strings mapping
static map<Key, string> _flag_map;

	// flag skipping version
	KeyVectorVector *
	find_all_continuations_obey_flags(StateId n,
						   KeyVector::iterator input_position,
						   KeyVector::iterator input_end_position,
						   Transducer &t,
						   KeySet* flags,
						   bool preserve_epsilons = true)
	{
		KeyVectorVector * continuations = NULL;

		if (input_position == input_end_position)
		{
			for (ArcIterator arcs(t, n);
				 not arcs.Done();
				 arcs.Next())
			{
				Arc a = arcs.Value();
				if ((a.ilabel == 0) || (flags->find(a.ilabel) == flags->end()))
				{
					KeyVectorVector * suffixes =
						find_all_continuations_obey_flags(a.nextstate,
										  input_position,
										  input_end_position,
										  t, flags);
					if (suffixes == NULL)
						continue;
					else if (continuations == NULL)
					{
						continuations = new KeyVectorVector;
					}
					add_prefix(a.olabel, suffixes);
					add_to_continuations(continuations,
							     suffixes);
				}
			}
			if (t.Final(n) == Weight::Zero())
				return continuations;
			else
			{
				if (continuations == NULL)
				{
					continuations = new KeyVectorVector;
					continuations->push_back(new KeyVector);
					return continuations;
				}
				else
				{
					continuations->push_back(new KeyVector);
					return continuations;
				}
			}
		}

		for (ArcIterator arcs(t, n);
			 not arcs.Done();
			 arcs.Next())
		{
			Arc a = arcs.Value();
			if (a.ilabel == *input_position)
			{
				KeyVectorVector * suffixes =
					find_all_continuations_obey_flags(a.nextstate,
									  input_position + 1,
									  input_end_position,
									  t, flags);
				if (suffixes == NULL)
					continue;
				else if (continuations == NULL)
				{
					continuations = new KeyVectorVector;
				}
				add_prefix(a.olabel, suffixes);
				add_to_continuations(continuations,
									 suffixes);
			}

			if ((a.ilabel == 0) || (flags->find(a.ilabel) == flags->end()))
			{
				KeyVectorVector * suffixes =
					find_all_continuations_obey_flags(a.nextstate,
									  input_position,
									  input_end_position,
									  t, flags);
				if (suffixes == NULL)
					continue;
				else if (continuations == NULL)
				{
					continuations = new KeyVectorVector;
				}
				add_prefix(a.olabel, suffixes);
				add_to_continuations(continuations,
						     suffixes);
			}
		}

		return continuations;
	}

class XFlagDiacritic
{
public:
	XFlagDiacritic() : 
		name_("@unset@"),
		value_(""),
		complement_(false),
		unset_(false)
	{}

	XFlagDiacritic(const string& name) :
		name_(name),
		value_(""),
		complement_(false),
		unset_(false)
	{}

	XFlagDiacritic& P(const string& value)
	{
		value_ = value;
		complement_ = false;
		unset_ = false;
		return *this;
	}

	XFlagDiacritic& N(const string& value)
	{
		value_ = value;
		complement_ = true;
		unset_ = false;
		return *this;
	}

	XFlagDiacritic& C()
	{
		value_ = "";
		complement_ = false;
		unset_ = true;
		return *this;
	}

	bool R(const string& value)
	{
		if ((value_ == value) && (!complement_))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool R()
	{
		if (!unset_)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool D(const string& value)
	{
		if ((value_ == value) && (!complement_))
		{
			return false;
		}
		else if ((value_ != value) && (complement_))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool D()
	{
		if (unset_)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool U(const string& value)
	{
		if (unset_)
		{
			value_ = value;
			return true;
		}
		else if ((value_ == value) && (!complement_))
		{
			return true;
		}
		else if ((value_ != value) && (complement_))
		{
			value_ = value;
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	string name_;
	string value_;
	bool complement_;
	bool unset_;
};

	static
	bool
	is_invalid_flag_diacritic_string(KeyVector* kv)
	{
		map<string, XFlagDiacritic> flags;
		for (KeyVector::const_iterator k = kv->begin();
			 k != kv->end(); ++k)
		{
			if (_flag_map.find(*k) == _flag_map.end())
			{
				continue;
			}
			string s = _flag_map[*k];
			size_t ldot = s.find(".");
			size_t rdot = s.rfind(".");
			string op = s.substr(1, 1);
			string varname;
			string varvalue;
			if (ldot == rdot)
			{
				// @OP.NAME@
				varname = s.substr(ldot, s.length() - 4);
			}
			else
			{
				// @OP.NAME.VALUE@
				varvalue = s.substr(rdot);
				varvalue = varvalue.substr(0, varvalue.length() - 1);
				varname = s.substr(3, s.length() - varvalue.length() - 4);
			}
			if (flags.find(varname) == flags.end())
			{
				flags[varname] = XFlagDiacritic(varname);
			}
			if (op == "P")
			{
				flags[varname].P(varvalue);
			}
			else if (op == "N")
			{
				flags[varname].N(varvalue);
			}
			else if (op == "C")
			{
				flags[varname].C();
			}
			else if ((op == "R") && (rdot != ldot))
			{
				if (!flags[varname].R(varvalue))
				{
					return false;
				}
			}
			else if ((op == "R") && (rdot == ldot))
			{
				if (!flags[varname].R())
				{
					return false;
				}
			}
			else if ((op == "D") && (rdot != ldot))
			{
				if (!flags[varname].D(varvalue))
				{
					return false;
				}
			}
			else if ((op == "D") && (rdot == ldot))
			{
				if (!flags[varname].D())
				{
					return false;
				}
			}
			else if (op == "U")
			{
				if (!flags[varname].U(varvalue))
				{
					return false;
				}
			}
		}
		return true;
	}

	KeyVectorVector*
	find_all_output_strings_obey_flags(Transducer &t, KeyVector * input,
		KeyTable* kt)
	{
		KeySet* flags=NULL;
#if		1
		KeyTable* foof = kt;
#elif	0
		KeyTable* foof = flag_keys(kt);
#endif
		for (Key k = 0; k < get_unused_key(foof); ++k)
		{
			if (is_key(k, foof))
			{
				Symbol sy = get_key_symbol(k, foof);
				string s = get_symbol_name(sy);
				_flag_map.insert(pair<Key, string>(k, s));
				flags->insert(k);
			}
		}
		StateId start = t.Start();
		KeyVector::iterator input_position = input->begin();
		KeyVector::iterator last_input_position = input->end();
		KeyVectorVector* reversed_outputs =
			find_all_continuations_obey_flags(start,
								   input_position,
								   last_input_position,
								   t, flags);
		KeyVectorVector* outputs = reverse_inner(reversed_outputs);
		KeyVectorVector::iterator oend = outputs->end();
		oend = remove_if(outputs->begin(), outputs->end(), 
						 is_invalid_flag_diacritic_string);
		return new KeyVectorVector(outputs->begin(), oend);
	}

  /*static
	string
	keyToString(Key k, KeyTable* kt)
	{
		if (is_key(k, kt))
		{
			Symbol sy = get_key_symbol(k, kt);
			return get_symbol_name(sy);
		}
		else
		{
			fprintf(stderr, "MISSING KEY: %d\n", k);
			return string(":::ERRORRR:::");
		}
		}*/

}



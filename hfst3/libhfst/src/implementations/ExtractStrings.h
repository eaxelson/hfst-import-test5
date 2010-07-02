#ifndef _EXTRACT_STRINGS_H_
#define _EXTRACT_STRINGS_H_
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <set>
namespace hfst {
  using std::stringstream;
  using std::ios;

  /** \brief A weighted string pair that represents a path in a transducer.

      @see WeightedPaths */
  template<class W> class WeightedPath
    {
    public:
      /** \brief The input string of the path. */
      std::string istring;
      /** \brief The output string of the path. */
      std::string ostring;
      /** \brief The weight of the path. */
      W weight;
      
      WeightedPath(const std::string &is,const std::string &os,W w)
	{ weight = w; istring = is; ostring = os; }

      bool operator< (const WeightedPath &another) const
	{ if (weight == another.weight)
	    { if (istring == another.istring)
		{ return ostring < another.ostring; }
	      return istring < another.istring; }
	  return weight < another.weight; }
      
      std::string to_string(void) const
	{ stringstream s_stream(ios::out);
	  s_stream << istring << ":" << ostring << "\t" << weight; 
	  s_stream.flush();
	  return s_stream.str();
	}
      
      WeightedPath<W> &reverse(void)
      { for(size_t i = 0; i < (istring.size() / 2); ++i)
	  { char c = istring[i];
	    istring[i] = istring[istring.size() - i - 1];
	    istring[istring.size() - i - 1] = c; }

	for(size_t i = 0; i < (ostring.size() / 2); ++i)
	  { char c = ostring[i];
	    ostring[i] = ostring[ostring.size() - i - 1];
	    ostring[ostring.size() - i - 1] = c; }
	return *this; }

      WeightedPath &add(const WeightedPath &another,bool in_front=true)
	{ if (in_front)
	    {
	      istring = another.istring + istring;
	      ostring = another.ostring + ostring;
	      weight = weight + another.weight;
	      return *this;
	    }
	  else 
	    {
	      istring = istring + another.istring;
	      ostring = ostring + another.ostring;
	      weight = weight + another.weight;
	      return *this;
	    } 
	}
      void operator=(const WeightedPath &another)
	{ if (this == &another) { return; }
	  this->istring = another.istring;
	  this->ostring = another.ostring;
	  this->weight = another.weight; }
    };

  /** \brief A class for storing weighted string pairs that represent paths in a transducer. 

      Iterators to Vectors and Sets return paths in descending weight order
      (the string with the biggest weight is returned first). (check this)

      For an example, see HfstTransducer::extract_strings.

      @see HfstTransducer::extract_strings */
  template<class W> class WeightedPaths
    { public:

      /** \brief A vector of weighted string pairs. */
      typedef std::vector< WeightedPath<W> > Vector; 
      /** \brief A set of weighted string pairs. */
      typedef std::set< WeightedPath<W> > Set; 

      static void add(Vector &v,WeightedPath<W> &s)
      {
	for (typename Vector::iterator it = v.begin(); it != v.end(); ++it)
	  { it->add(s,false); }
      }

      static void add(WeightedPath<W> &s,Vector &v)
      {
	for (typename Vector::iterator it = v.begin(); it != v.end(); ++it)
	  { it->add(s); }
      }

      static void cat(Vector &v, const Vector &another_v)
      {
	v.insert(v.end(),another_v.begin(),another_v.end());
      }
      
      static void reverse_strings(Vector &v)
      {
	for (typename Vector::iterator it = v.begin(); it != v.end(); ++it)
	  { it->reverse(); }
      }
    };
  }  
#endif

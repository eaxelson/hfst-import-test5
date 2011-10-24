#include <map>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>

#define BUFFER_SIZE 50000

typedef std::vector<std::string> StringNGram;
typedef std::map<StringNGram,size_t> StringNGramCountMap;
typedef std::vector<StringNGramCountMap> StringNGramCountMapVector;
typedef std::vector<size_t> CountVector;

typedef std::pair<float,size_t> QuotientAndPosition;
typedef std::vector<QuotientAndPosition> QuotientAndPositionVector;

std::ostream &operator<< (std::ostream &out,const StringNGram &n_gram)
{
  for (StringNGram::const_iterator it = n_gram.begin();
       it != n_gram.end();
       ++it)
    { std::cerr << *it << " "; }
  return out;
}

// Get the count of the m-gram which contains the words between positions 
// initial_skip and initial_skip + length in n_gram. If length is zero return
// the corpus size. Subtrct one from all counts.
size_t get_div(const StringNGram &n_gram, 
	       const StringNGramCountMap &n_gram_counts,
	       size_t corpus_size,
	       size_t initial_skip,
	       size_t length)
{
  assert(length <= n_gram.size());

  if (length == 0)
    { return corpus_size - 1; }
  else
    {
      StringNGram abstracted_n_gram(n_gram.begin() + initial_skip,
				    n_gram.begin() + initial_skip + length);

      if (n_gram_counts.find(abstracted_n_gram) == n_gram_counts.end())
	{
	  std::cerr << abstracted_n_gram << std::endl 
		    << n_gram_counts.begin()->first << std::endl
		    << corpus_size << std::endl
		    << initial_skip << std::endl
		    << length << std::endl;
	}

      assert(n_gram_counts.find(abstracted_n_gram) != n_gram_counts.end());

      return n_gram_counts.find(abstracted_n_gram)->second - 1;
    }
}

float get_quotient(const StringNGram &n_gram,
		   const StringNGramCountMapVector &n_gram_counts,
		   size_t corpus_size,
		   size_t length)
{
  assert(length > 0);
  assert(length <= n_gram.size());

  size_t initial_skip = n_gram.size() - length;
  
  size_t dividend = get_div(n_gram,
			    n_gram_counts[ length - 1 ],
			    corpus_size,
			    initial_skip,
			    length);

  length = length > 1 ? length - 2 : 0;
  initial_skip = n_gram.size() - length;

  size_t divisor = get_div(n_gram,
			   n_gram_counts[ length - 1 ],
			   corpus_size,
			   initial_skip,
			   length);

  if (divisor == 0)
    { return 0.0; }
  else
    { return dividend * 1.0 / divisor; }
}

std::string read_tag(std::istream &training_data_file,size_t line_number)
{
  char line[BUFFER_SIZE];
  training_data_file.getline(line,BUFFER_SIZE);
  std::string word_and_tag(line);
  if (word_and_tag.find('\t') == std::string::npos)
    {
      std::cerr << "ERROR: Line number " << line_number 
		<< " \"" << word_and_tag 
		<< "\" in the training file is erroneous." 
		<< std::endl;
    }
  return word_and_tag.substr(word_and_tag.find('\t')+1);
}

StringNGramCountMapVector get_string_counts(std::istream &training_data_file,
					    size_t n,
					    size_t &corpus_size)
{
  assert(n > 0);

  StringNGramCountMapVector string_counts(n);
  StringNGram n_gram(n,"||");

  corpus_size = 1;
  while (training_data_file.peek() != EOF)
    {
      std::string tag = read_tag(training_data_file,corpus_size);
      n_gram = StringNGram(n_gram.begin() + 1, n_gram.begin() + n);
      n_gram.push_back(tag);
      
      StringNGram m_gram;

      for (size_t i = 0; i < n; ++i)
	{
	  m_gram.push_back(n_gram.at(i));
	  ++string_counts[i][m_gram];
	}

      ++corpus_size;
    }
  return string_counts;
}

int main(int argc, char * argv[])
{
  if (argc != 3)
    {
      std::cerr << "Usage: " << argv[0] << " training_data_file model_order"
		<< std::endl;
      exit(1);
    }

  std::string training_data_filename = argv[1];
  std::string model_order_string     = argv[2];

  // Read the training data file.
  std::ifstream training_data_file(training_data_filename.c_str());
  
  if (training_data_file.bad() or training_data_file.peek() == EOF)
    {
      std::cerr << "ERROR: The file " << training_data_filename << " can't "
		<< "be opened." << std::endl;
      exit(1);
    }

  // Read the model order.
  std::istringstream order_in(model_order_string);
  size_t order;
  order_in >> order;
  if (order_in.fail())
    {
      std::cerr << "ERROR: \"" << model_order_string << "\" can't be "
		<< "interpreted as an integer." << std::endl;
      exit(1);
    }
  size_t n = order + 1;

  size_t corpus_size = 0;

  StringNGramCountMapVector string_counts = 
    get_string_counts(training_data_file,n,corpus_size);

  CountVector n_gram_success_vector(n);

  const StringNGramCountMap &n_gram_count_map = string_counts[n - 1];

  for (StringNGramCountMap::const_iterator it = n_gram_count_map.begin();
       it != n_gram_count_map.end();
       ++it)
    {
      const StringNGram n_gram = it->first;

      QuotientAndPositionVector quotients;

      for (size_t i = 0; i < n; ++i)
	{
	  quotients.push_back
	    (QuotientAndPosition
	     (get_quotient(n_gram,string_counts,corpus_size,i + 1),i));
	}
      std::sort(quotients.begin(),quotients.end());
      n_gram_success_vector.at(quotients.begin()->second) += it->second;
    }

  for (size_t i = 0; i < n_gram_success_vector.size(); ++i)
    {
      std::cout << i + 1 << "-gram model weight: " << ": " 
		<< n_gram_success_vector.at(i) * 1.0 / corpus_size 
		<< std::endl; 
    }
}

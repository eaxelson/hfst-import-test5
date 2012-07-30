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
#include <cstdlib>
#include <sstream>
#include "utils/ProcTransducer.h"
#include "utils/ProcLookupPath.h"
#include "utils/ProcLookupState.h"
#include "utils/ProcTokenizer.h"
#include "utils/ProcFormatters.h"

// from commandline conventions
extern bool verbose;

static LookupPath* create_initial_path(const ProcTransducer& t) { return new LookupPath(t, 0);}
static LookupPath* create_initial_path_fd(const ProcTransducer& t) { return new LookupPathFd(t, 0);}
static LookupPath* create_initial_path_weighted(const ProcTransducer& t) { return new LookupPathW(t, 0);}
static LookupPath* create_initial_path_weighted_fd(const ProcTransducer& t) { return new LookupPathWFd(t, 0);}

//////////Function definitions for ProcTransducer

InitialPathCreator ProcTransducer::initial_path_creators[2][2] =
    {{create_initial_path, create_initial_path_fd},
     {create_initial_path_weighted, create_initial_path_weighted_fd}};

static
bool
skip_hfst3_header(std::istream& is)
{
  const char* header1 = "HFST";
  int header_loc = 0; // how much of the header has been found
  int c;
  for(header_loc = 0; header_loc < strlen(header1) + 1; header_loc++)
  {
    c = is.get();
    if(c != header1[header_loc])
      break;
  }
  if(header_loc == strlen(header1) + 1) // we found it
  {
      unsigned short remaining_header_len;
      is.read(reinterpret_cast<char*>(&remaining_header_len),
          sizeof(remaining_header_len));
      if(is.get() != '\0') {
      HFST_THROW(HfstException);
      return false;
      }
      char * headervalue = new char[remaining_header_len];
      while(remaining_header_len > 0) {
      is.getline(headervalue, remaining_header_len + 1, '\0');
      remaining_header_len -= strlen(headervalue) + 1;
      if (!strcmp(headervalue, "type")) {
          is.getline(headervalue, remaining_header_len + 1, '\0');
          remaining_header_len -= strlen(headervalue) + 1;
          if (strcmp(headervalue, "HFST_OL") and
          strcmp(headervalue, "HFST_OLW")) {
          delete headervalue;
          HFST_THROW(TransducerHasWrongTypeException);
          return false;
          }
      }
      }
      delete headervalue;
      if (remaining_header_len == 0) {
      return true;
      } else {
      HFST_THROW(HfstException);
      return false;
      }
  } else // nope. put back what we've taken
  {
    is.putback(c); // first the non-matching character
    for(int i=header_loc-1; i>=0; i--) // then the characters that did match (if any)
      is.putback(header1[i]);
    
    return false;
  }
}
ProcTransducer::ProcTransducer(std::istream& is): Transducer()
{
  skip_hfst3_header(is);
  header = new TransducerHeader(is);
  alphabet = new ProcTransducerAlphabet(is, header->symbol_count());
  load_tables(is);
  
  if (header->probe_flag(Has_unweighted_input_epsilon_cycles) ||
      header->probe_flag(Has_input_epsilon_cycles))
  {
    std::cerr << "!! Warning: transducer has epsilon cycles                  !!\n"
              << "!! This is currently not handled - if they are encountered !!\n"
              << "!! program *will* segfault.                                !!\n";
  }
  
  if(check_for_blank())
  {
    std::cerr << "!! Warning: transducer accepts input strings consisting of !!\n"
              << "!! just a blank. This is probably a bug in the transducer  !!\n"
              << "!! and will cause strange behavior.                        !!\n";
  }
}

bool
ProcTransducer::check_for_blank() const
{
  if(verbose)
    std::cout << "Checking whether the transducer accepts a single blank as a word..." << std::endl;
  LookupState state(*this);
  state.step(get_alphabet().get_blank_symbol());
  return state.is_final();
}

bool
ProcTransducer::is_epsilon(const Transition& transition) const
{
  return transition.matches(0) || alphabet->is_flag_diacritic(transition.get_input_symbol());
}

LookupPath*
ProcTransducer::get_initial_path() const
{
  return (*initial_path_creators[header->probe_flag(Weighted)][alphabet->has_flag_diacritics()])(*this);
}


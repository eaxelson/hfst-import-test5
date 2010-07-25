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

// common-unary-cases.h
  case 'i':
  inputfilename = hfst_strdup(optarg);
  if (strcmp(inputfilename, "-") == 0) {
    inputfilename = hfst_strdup("<stdin>");
    inputfile = stdin;
    is_input_stdin = true;
  }
  else {
    inputfile = hfst_fopen(inputfilename, "r");
    is_input_stdin = false;
  }
  break;
  case 'o':
  outfilename = hfst_strdup(optarg);
  if (strcmp(outfilename, "-") == 0) {
    outfilename = hfst_strdup("<stdout>");
    outfile = stdout;
    is_output_stdout = true;
  }
  else {
    outfile = hfst_fopen(outfilename, "w");
    is_output_stdout = false;
  }
  message_out = stdout;
  break;
  case 'R':
  read_symbols_from_filename = hfst_strdup(optarg);
  break;
  case 'D':
  write_symbols = false;
  break;
  case 'W':
  write_symbols_to_filename = hfst_strdup(optarg);
  break;

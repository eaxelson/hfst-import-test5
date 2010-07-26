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

// common-binary-cases.h
  case '1':
  firstfilename = hfst_strdup(optarg);
  if (strcmp(firstfilename, "-") == 0) {
    free(firstfilename);
    firstfilename = hfst_strdup("<stdin>");
    firstfile = stdin;
    is_input_stdin = true;
  }
  else {
    firstfile = hfst_fopen(firstfilename, "r");
    is_input_stdin = false;
  }
  break;
  case '2':
  secondfilename = hfst_strdup(optarg);
  if (strcmp(secondfilename, "-") == 0) {
    free(secondfilename);
    secondfilename = hfst_strdup("<stdin>");
    secondfile = stdin;
    is_input_stdin = true;
  }
  else {
    secondfile = hfst_fopen(firstfilename, "r");
    is_input_stdin = false;
  }
  break;
  case 'o':
  outfilename = hfst_strdup(optarg);
  if (strcmp(outfilename, "-") == 0) {
    free(outfilename);
    outfilename = hfst_strdup("<stdout>");
    outfile = stdout;
    is_output_stdout = true;
    message_out = stderr;
  }
  else {
    outfile = hfst_fopen(outfilename, "w");
    is_output_stdout = false;
    message_out = stdout;
  }
  break;

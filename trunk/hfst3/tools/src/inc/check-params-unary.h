//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

    if (is_input_stdin)
      {
        if ((argc - optind) == 1)
          {
            inputfilename = hfst_strdup(argv[optind]);
            if (strcmp(inputfilename, "-") == 0) 
              {
                free(inputfilename);
                inputfilename = hfst_strdup("<stdin>");
                inputfile = stdin;
                is_input_stdin = true;
              }
            else 
              {
                inputfile = hfst_fopen(inputfilename, "r");
                is_input_stdin = false;
              }
          }
        else if ((argc - optind) > 1)
          {
            error(EXIT_FAILURE, 0, 
                  "no more than one transducer file may be given");
          }
        else
          {
            inputfilename = hfst_strdup("<stdin>");
          }
      }
    else 
      {
        if ((argc - optind) > 0)
          {
            error(EXIT_FAILURE, 0, 
                  "no more than one transducer filename may be given");
          }
      }


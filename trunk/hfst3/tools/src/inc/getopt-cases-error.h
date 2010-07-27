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

    case '?':
        error(EXIT_FAILURE, 0, "invalid option --%s\n"
              "Try ``%s --help'' for more information\n",
                long_options[option_index].name, program_name);
        return EXIT_FAILURE;
        break;
    default:
        error(EXIT_FAILURE,0 , "invalid option -%c\n"
              "Try ``%s --help'' for more information\n",
              c, program_name);
        return EXIT_FAILURE;
        break;


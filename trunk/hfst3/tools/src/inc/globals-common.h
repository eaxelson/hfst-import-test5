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

// defaults
bool verbose = false;
bool silent = false;
bool debug = false;
FILE *message_out = stdout;
#if !HAVE_DECL_PROGRAM_NAME
char* program_name = "Someone forgot to set program name";
#endif
char* hfst_tool_version = "Someone forgot to give version";
char* hfst_tool_wikiname = "Someone forgot to set wiki page";

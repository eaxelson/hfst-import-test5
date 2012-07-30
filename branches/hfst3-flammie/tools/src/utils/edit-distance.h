//! @file edit-distance.h
//!
//! @brief create edit distance stuff
//!
//! @author HFST Team


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

#ifndef EDIT_DISTANCE_H
#define EDIT_DISTANCE_H 1

#include <string>
#include <set>
#include <hfst.hpp>

hfst::HfstTransducer* create_edit_distance(std::set<std::string>& alphabets, 
                                     unsigned long length,
                                     float weight,
                                     hfst::ImplementationType format);
#endif


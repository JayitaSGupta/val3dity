/*
  val3dity 

  Copyright (c) 2011-2017, 3D geoinformation research group, TU Delft  

  This file is part of val3dity.

  val3dity is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  val3dity is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with val3dity.  If not, see <http://www.gnu.org/licenses/>.

  For any information or further details about the use of val3dity, contact
  Hugo Ledoux
  <h.ledoux@tudelft.nl>
  Faculty of Architecture & the Built Environment
  Delft University of Technology
  Julianalaan 134, Delft 2628BL, the Netherlands
*/

#include "Feature.h"
#include "input.h"
#include <iostream>

namespace val3dity
{


Feature::Feature() {
}

Feature::~Feature() {
}


std::string Feature::get_id()
{
  return _id;
}


void Feature::set_id(std::string id)
{
  _id = id;
}


bool Feature::is_empty()
{
  return _lsPrimitives.empty();
}


json Feature::get_report_json() 
{
  json j;
  j["id"] = _id;
  j["type"] = _type;
  j["feature_errors"];
  j["primitives"];
  for (auto& err : _errors)
  {
    for (auto& e : _errors[std::get<0>(err)])
    {
      json jj;
      jj["type"] = "Error";
      jj["code"] = std::get<0>(err);
      jj["description"] = errorcode2description(std::get<0>(err));
      jj["id"] = std::get<0>(e);
      jj["info"] = std::get<1>(e);
      j["errors"].push_back(jj);
    }
  }
  for (auto& p : _lsPrimitives)
    j["primitives"].push_back(p->get_report_json()); 
  return j;
}


void Feature::add_error(int code, std::string whichgeoms, std::string info)
{
  _is_valid = 0;
  std::tuple<std::string, std::string> a(whichgeoms, info);
  _errors[code].push_back(a);
  std::clog << "\tERROR " << code << ": " << errorcode2description(code);
  if (whichgeoms.empty() == false)
    std::clog << " (id: " << whichgeoms << ")";
  std::clog << std::endl;
  if (info.empty() == false)
    std::clog << "\t[" << info << "]" << std::endl;
}

std::set<int> Feature::get_unique_error_codes()
{
  std::set<int> errs;
  for (auto& err : _errors)
  {
    errs.insert(std::get<0>(err));
  }
  return errs;
}

} // namespace val3dity

//
//  CompositeSolid.cpp
//  val3dity
//
//  Created by Hugo Ledoux on 25/10/16.
//
//

#include "CompositeSolid.h"


CompositeSolid::CompositeSolid(std::string id)
{
  _id = id;
  _is_valid = -1;
}


CompositeSolid::~CompositeSolid()
{}


std::string CompositeSolid::get_type() 
{
  return "CompositeSolid";
}


bool CompositeSolid::validate(double tol_planarity_d2p, double tol_planarity_normals) 
{
  bool isValid = true;
  for (auto& s : _lsSolids)
  {
    if (s->validate(tol_planarity_d2p, tol_planarity_normals) == false)
      isValid = false;
  }
  if (isValid == true) 
  {
    std::clog << "----- CompositeSolid validation (interaction between Solids) -----" << std::endl;
    std::vector<Nef_polyhedron*> lsNefs;
    for (int i = 0; i < _lsSolids.size(); i++)
      lsNefs.push_back(_lsSolids[i]->get_nef_polyhedron());
//-- 1. check if any 2 are the same? ERROR:502
    std::clog << "--Are two solids duplicated" << std::endl;
    for (int i = 0; i < (lsNefs.size() - 1); i++)
    {
      for (int j = i + 1; j < lsNefs.size(); j++) 
      {
        if (*lsNefs[i] == *lsNefs[j])
        {
          std::stringstream msg;
          msg << _lsSolids[i]->get_id() << "& " << _lsSolids[j]->get_id();
          this->add_error(502, msg.str(), "");
          isValid = false;
        }
      }
    }
    if (isValid == true)
    {
//-- 2. check if their interior intersects ERROR:501
      std::clog << "--Intersections of solids" << std::endl;
      Nef_polyhedron emptynef(Nef_polyhedron::EMPTY);
      for (int i = 0; i < (lsNefs.size() - 1); i++)
      {
        Nef_polyhedron* a = lsNefs[i];
        for (int j = i + 1; j < lsNefs.size(); j++) 
        {
          Nef_polyhedron* b = lsNefs[j];
          if (a->interior() * b->interior() != emptynef)
          {
            std::stringstream msg;
            msg << "Solid " << _lsSolids[i]->get_id() << "& Solid " << _lsSolids[j]->get_id();
            this->add_error(501, msg.str(), "");
            isValid = false;
          }
        }
      }
    }
    if (isValid == true)
    {
//-- 3. check if their union yields one solid ERROR:503
      std::clog << "--Forming one solid (union)" << std::endl;
      Nef_polyhedron unioned(Nef_polyhedron::EMPTY);
      for (auto each : lsNefs)
        unioned = unioned + *each;
      if (unioned.number_of_volumes() != 2)
      {
        std::stringstream msg;
        msg << "CompositeSolid is formed of " << unioned.number_of_volumes() << " parts";
        this->add_error(503, "", msg.str());
        isValid = false;
      }
    } 
    //-- clear the temp Nefs
    for (auto each: lsNefs)
      delete each;
  }
  _is_valid = isValid;
  return isValid;
}


int CompositeSolid::is_valid()
{
  if ( (_is_valid == 1) && (this->is_empty() == false) && (_errors.empty() == true) )
    return 1;
  else
    return _is_valid;
}



bool CompositeSolid::is_empty() {
  return _lsSolids.empty();
}


std::string CompositeSolid::get_report_xml() {
  // TODO: xml report
  return "<EMPTY>";
}

bool CompositeSolid::add_solid(Solid* s) {
  _lsSolids.push_back(s);
  return true;
}

int CompositeSolid::num_solids() {
  return _lsSolids.size();
}

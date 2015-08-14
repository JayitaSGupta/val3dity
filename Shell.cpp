//
//  Shell.cpp
//  val3dity
//
//  Created by Hugo Ledoux on 12/08/15.
//
//

#include "Shell.h"
#include "geomtools.h"
#include "validate_2d.h"
#include "validate_shell.h"


Shell2::Shell2(int id, double tol_snap, cbf cb)
{
  _id = id;
  _tol_snap = tol_snap;
  _cb = cb;
  _is_valid = -1;
  _is_valid_2d = -1;
}

Shell2::~Shell2()
{
  // TODO: clear memory properly
  _lsPts.clear();
}


int Shell2::add_point(Point3 p)
{
  int pos = -1;
  int cur = 0;
  for (auto &itr : _lsPts)
  {
    // std::cout << "---" << itr << std::endl;
    if (cmpPoint3(p, itr, _tol_snap) == true)
    {
      pos = cur;
      break;
    }
    cur++;
  }
  if (pos == -1)
  {
    _lsPts.push_back(p);
    return (_lsPts.size() - 1);
  }
  else
    return pos;
}


void Shell2::add_face(vector< vector<int> > f)
{
  _lsFaces.push_back(f);
}


int Shell2::number_points()
{
  return _lsPts.size();
}


int Shell2::number_faces()
{
  return _lsFaces.size();
}


bool Shell2::is_outer()
{
  return (_id == 0);
}

// bool Shell2::triangulate_shell(Shell& shell, int shellNum, TrShell& tshell, cbf cb)
bool Shell2::triangulate_shell()
{
  //-- read the facets
  size_t num = _lsFaces.size();
  for (int i = 0; i < static_cast<int>(num); i++)
  {
    // These are the number of rings on this facet
    size_t numf = _lsFaces[i].size();
    //-- read oring (there's always one and only one)
    if (numf < 1)
    {
      (*_cb)(999, _id, -1, "surface does not have an outer boundary.");
      return false;
    }
    vector<int> &idsob = _lsFaces[i][0]; // helpful alias for the outer boundary
    int proj = projection_plane(_lsPts, idsob);
    Vector* v0 = polygon_normal(_lsPts, idsob);
    //-- get projected Polygon
    Polygon pgn;
    vector<Polygon> lsRings;
    create_polygon(_lsPts, idsob, pgn, true);
    lsRings.push_back(pgn);
    vector< vector<int> > pgnids;
    pgnids.push_back(idsob);
    
    //-- check for irings
    for (int j = 1; j < static_cast<int>(numf); j++)
    {
      vector<int> &ids2 = _lsFaces[i][j]; // helpful alias for the inner boundary
      //-- get projected Polygon
      Polygon pgn;
      create_polygon(_lsPts, ids2, pgn, true);
      lsRings.push_back(pgn);
      pgnids.push_back(ids2);
    }
    //-- get projected CT
    vector<int*> oneface;
    if (construct_ct(pgnids, lsRings, oneface, i) == false)
    {
      (*_cb)(999, _id, i, "face does not have an outer boundary.");
      return false;
    }
    //-- modify orientation of every triangle if necessary
    bool invert = false;
    if (proj == 2)
    {
      Vector n(0, 0, 1);
      if ( (*v0*n) < 0)
        invert = true;
    }
    else if (proj == 1)
    {
      Vector n(0, 1, 0);
      if ( (*v0*n) > 0)
        invert = true;
    }
    else if(proj == 0)
    {
      Vector n(1, 0, 0);
      if ( (*v0*n) < 0)
        invert = true;
    }
    delete v0;
    if ( invert == true ) //-- invert
    {
      vector<int*>::iterator it3 = oneface.begin();
      int tmp;
      int* id;
      for ( ; it3 != oneface.end(); it3++)
      {
        id = *it3;
        tmp = id[0];
        id[0] = id[1];
        id[1] = tmp;
      }
    }
    _lsTr.push_back(oneface);
  }
  return true;
}


// bool Shell2::construct_ct(const vector< Point3 > &_lsPts, const vector< vector<int> >& pgnids, const vector<Polygon>& lsRings, vector<int*>& oneface, int faceNum)
bool Shell2::construct_ct(const vector< vector<int> >& pgnids, const vector<Polygon>& lsRings, vector<int*>& oneface, int faceNum)
{
  bool isValid = true;
  vector<int> ids = pgnids[0];
  int proj = projection_plane(_lsPts, ids);
  CT ct;
  vector< vector<int> >::const_iterator it = pgnids.begin();
  size_t numpts = 0;
  for ( ; it != pgnids.end(); it++)
  {
    numpts += it->size();
  }
  //  cout << "Polygon has # vertices " << numpts << endl;
  for ( it = pgnids.begin(); it != pgnids.end(); it++)
  {
    vector<Point2> pts2d;
    vector<int>::const_iterator it2 = it->begin();
    for ( ; it2 != it->end(); it2++)
    {
      Point3 p1  = _lsPts[*it2];
      if (proj == 2)
        pts2d.push_back( Point2(p1.x(), p1.y()) );
      else if (proj == 1)
        pts2d.push_back( Point2(p1.x(), p1.z()) );
      else
        pts2d.push_back( Point2(p1.y(), p1.z()) );
    }
    vector<Point2>::const_iterator itPt;
    CT::Vertex_handle v0;
    CT::Vertex_handle v1;
    CT::Vertex_handle firstv;
    itPt = pts2d.begin();
    v0 = ct.insert(*itPt);
    firstv = v0;
    it2 = it->begin();
    v0->info() = *it2;
    itPt++;
    it2++;
    for (; itPt != pts2d.end(); itPt++)
    {
      v1 = ct.insert(*itPt);
      v1->info() = *it2;
      ct.insert_constraint(v0, v1);
      v0 = v1;
      it2++;
    }
    ct.insert_constraint(v0,firstv);
  }
  //-- validation of the face itself
  //-- if the CT introduced new points, then there are irings intersectings either oring or other irings
  //-- which is not allowed
  if (numpts < ct.number_of_vertices())
  {
    std::stringstream ss;
    ss << "Intersection(s) between rings of the face #" << faceNum << ".";
    (*_cb)(0, -1, -1, ss.str());
    isValid = false;
  }  
  //-- fetch all the triangles forming the polygon (with holes)
  CT::Finite_faces_iterator fi = ct.finite_faces_begin();
  for( ; fi != ct.finite_faces_end(); fi++)
  {
    Point2 centre = barycenter( ct.triangle(fi).vertex(0), 1,
                               ct.triangle(fi).vertex(1), 1,
                               ct.triangle(fi).vertex(2), 1);
    bool inside = true;
    if (lsRings[0].has_on_negative_side(centre))
      inside = false;
    else
    {
      vector<Polygon>::const_iterator itpgn = lsRings.begin();
      itpgn++;
      for ( ; itpgn != lsRings.end(); itpgn++)   //-- check irings
      {
        if (itpgn->has_on_positive_side(centre))
        {
          inside = false;
          break;
        }
      }
    }
    if (inside == true)
    {
      //-- add the IDs to the face
      int* tr = new int[3];
      tr[0] = fi->vertex(0)->info();
      tr[1] = fi->vertex(1)->info();
      tr[2] = fi->vertex(2)->info();
      oneface.push_back(tr);
    }
  }
  return isValid;
}


bool Shell2::validate_2d_primitives(double tol_planarity_d2p, double tol_planarity_normals)
{
  (*_cb)(0, -1, -1, "Validating surfaces in 2D (their projection)");
  bool isValid = true;
  size_t num = _lsFaces.size();
  for (int i = 0; i < static_cast<int>(num); i++)
  {
    //-- test for too few points (<3 for a ring)
    if (has_face_rings_toofewpoints(_lsFaces[i]) == true)
    {
      (*_cb)(101, _id, i, "");
      isValid = false;
      continue;
    }
    //-- test for 2 repeated consecutive points
    if (has_face_2_consecutive_repeated_pts(_lsFaces[i]) == true)
    {
      (*_cb)(102, _id, i, "");
      isValid = false;
      continue;
    }
    size_t numf = _lsFaces[i].size();
    vector<int> &ids = _lsFaces[i][0]; // helpful alias for the outer boundary
    vector< Point3 > allpts;
    vector<int>::const_iterator itp = ids.begin();
    for ( ; itp != ids.end(); itp++)
    {
      allpts.push_back(_lsPts[*itp]);
    }
    //-- irings
    for (int j = 1; j < static_cast<int>(numf); j++)
    {
      vector<int> &ids2 = _lsFaces[i][j]; // helpful alias for the inner boundary
      vector<int>::const_iterator itp2 = ids2.begin();
      for ( ; itp2 != ids2.end(); itp2++)
      {
        allpts.push_back(_lsPts[*itp2]);
      }
    }
    double value;
    if (false == is_face_planar_distance2plane(allpts, value, tol_planarity_d2p))
    {
      std::stringstream msg;
      msg << "distance to fitted plane: " << value << " (tolerance=" << tol_planarity_d2p << ")";
      (*_cb)(203, _id, i, msg.str());
      isValid = false;
      continue;
    }
    //-- get projected oring
    Polygon pgn;
    vector<Polygon> lsRings;
    if (false == create_polygon(_lsPts, ids, pgn, false))
    {
      (*_cb)(104, _id, i, " outer ring self-intersects or is collapsed to a point or a line");
      isValid = false;
      continue;
    }
    lsRings.push_back(pgn);
    //-- check for irings
    for (int j = 1; j < static_cast<int>(numf); j++)
    {
      vector<int> &ids2 = _lsFaces[i][j]; // helpful alias for the inner boundary
      //-- get projected iring
      Polygon pgn;
      if (false == create_polygon(_lsPts, ids2, pgn, false))
      {
        (*_cb)(104, _id, i, "Inner ring self-intersects or is collapsed to a point or a line");
        isValid = false;
        continue;
      }
      lsRings.push_back(pgn);
    }
    //-- use GEOS to validate projected polygon
    if (!validate_polygon(lsRings, _id, num, _cb, tol_planarity_d2p))
      isValid = false;
  }
  if (isValid)
  {
    //-- triangulate faces of the shell
    triangulate_shell();
    //-- check planarity by normal deviation method (of all triangle)
//    (*_cb)(0, -1, -1, "\nChecking the planarity of surfaces (with normals deviation)");
    for (unsigned int i = 0; i < _lsTr.size(); i++)
    {
      vector< vector<int*> >::iterator it = _lsTr.begin();
      int j = 0;
      double deviation;
      for ( ; it != _lsTr.end(); it++)
      { 
        if (is_face_planar_normals(*it, _lsPts, deviation, tol_planarity_normals) == false)
        //-- second with normals deviation method
        {
          std::ostringstream msg;
          msg << "deviation normals: " << deviation << " (tolerance=" << tol_planarity_normals << ")";
          (*_cb)(204, _id, j, msg.str());
          isValid = false;
        }
        j++;
      }
    }
  }
  _is_valid_2d = isValid;
  std::cout << "so is it valid? " << isValid << std::endl;
  return isValid;
}


bool Shell2::validate_as_multisurface(double tol_planarity_d2p, double tol_planarity_normals)
{
  (*_cb)(0, -1, -1, "--- MultiSurface validation ---\n");
  if (_is_valid_2d == -1)
    return validate_2d_primitives(tol_planarity_d2p, tol_planarity_normals);
  else
  {
    if (_is_valid_2d == 1)
      return true;
    else
      return false;
  }
}


bool Shell2::validate_as_compositesurface(double tol_planarity_d2p, double tol_planarity_normals)
{
  // TODO: not working yet
  (*_cb)(0, -1, -1, "--- MultiSurface validation ---\n");
  return validate_2d_primitives(tol_planarity_d2p, tol_planarity_normals);
}


// CgalPolyhedron* validate_triangulated_shell_solid(TrShell& tshell, int _id, cbf cb, double TOL_PLANARITY_normals)
bool Shell2::validate_as_shell(double tol_planarity_d2p, double tol_planarity_normals)
{
  bool isValid = true;
  CgalPolyhedron *P = new CgalPolyhedron;
//-- 1. minimum number of faces = 4
  if (_lsTr.size() < 4) 
  {
    (*_cb)(301, _id, -1, "");
    isValid = false;
    return false;
  }
//-- 2. Combinatorial consistency
  //-- TODO: construct the CgalPolyhedron with batch operator (not used anymore)
  // P = get_CgalPolyhedron_DS(tshell.faces, tshell.lsPts);
  // std::cout << P->empty() << std::endl;
  // std::cout << P->is_valid() << std::endl;
  // std::cout << P->is_closed() << std::endl;

  //-- construct the CgalPolyhedron incrementally
  (*_cb)(0, -1, -1, "-----Combinatorial consistency");
  ConstructShell<HalfedgeDS> s(&(_lsTr), &(_lsPts), _id, false, _cb);
  P->delegate(s);
  isValid = s.isValid;
  if (isValid == true)
  {
    if (P->is_valid() == true) //-- combinatorially valid that is
    {
      if (P->is_closed() == false)
      {
        P->normalize_border();
        //-- check for unconnected faces
        if (P->keep_largest_connected_components(1) > 0)
        {
          //TODO: how to report what face is not connected? a bitch of a problem...
          (*_cb)(305, _id, -1, "");
          isValid = false;
        }
        else
        {
          //-- check if there are holes in the surface
          if (P->is_closed() == false)
          {
            std::stringstream st;
            P->normalize_border();
            while (P->size_of_border_edges() > 0) {
              CgalPolyhedron::Halfedge_handle he = ++(P->border_halfedges_begin());
              st << "Location hole: " << he->vertex()->point();
              (*_cb)(302, _id, -1, st.str());
              st.str("");
              // P->fill_hole(he);
              // P->normalize_border();
            }
            isValid = false;
          }
        }
      }
      else //-- check if >1 connected components exist (both valid)
      {
        if (P->keep_largest_connected_components(1) > 0) 
        {
          (*_cb)(305, _id, -1, "More than one connected components.");
          isValid = false;
        }
      }
    }
    else 
    {
      (*_cb)(300, _id, -1, "Something went wrong during construction of the shell, reason is unknown.");
      isValid =  false;
    }
  }
//-- 3. Geometrical consistency (aka intersection tests between faces)
  if (isValid == true)
  {
    (*_cb)(0, -1, -1, "\tyes");
    (*_cb)(0, -1, -1, "-----Geometrical consistency");
    isValid = is_polyhedron_geometrically_consistent(P, _id, cb);
  }
  if (isValid)
  {
  (*_cb)(0, -1, -1, "\tyes");
  }
//-- 4. orientation of the normals is outwards or inwards
  if (isValid == true)
  {
    (*_cb)(0, -1, -1, "-----Orientation of normals");
    isValid = check_global_orientation_normals_rev2(P, this->is_outer(), cb);
    if (isValid == false)
      (*_cb)(308, _id, -1, "");
    else
      (*_cb)(0, -1, -1, "\tyes");
  }
  if (isValid == false)
  {
    delete P;
    _is_valid = 0;
  }
  else
  {
    _polyhedron = P;
    _is_valid = 1;
  }
  return isValid;
}


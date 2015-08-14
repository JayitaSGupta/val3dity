
/*
 val3dity - Copyright (c) 2011-2015, Hugo Ledoux.  All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the authors nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL HUGO LEDOUX BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*/

#include "validate.h"
#include <CGAL/intersections.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/intersections.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_polyhedron_triangle_primitive.h>
//#include <CGAL/AABB_face_graph_triangle_primitive.h>

#include<set>
#include<list>





template <class HDS>
class ConstructShell : public CGAL::Modifier_base<HDS> {
  vector< vector<int*> > *faces;
  vector<Point3> *lsPts;
  int shellID;
  int bRepair;
  int _width;
  cbf cb;
public:
  bool isValid;
  ConstructShell(vector< vector<int*> > *faces, vector<Point3> *lsPts, int shellID, bool bRepair, cbf cb)
    :faces(faces), lsPts(lsPts), shellID(shellID), cb(cb), bRepair(bRepair), isValid(true), _width(static_cast<int>(lsPts->size()))
  {
  }
  void operator()( HDS& hds);
  void construct_faces_order_given(CGAL::Polyhedron_incremental_builder_3<HDS>& B, cbf cb);
  int m2a(int m, int n);
  void construct_faces_flip_when_possible(CGAL::Polyhedron_incremental_builder_3<HDS>& B, cbf cb);
  bool try_to_add_face(CGAL::Polyhedron_incremental_builder_3<HDS>& B, list<int*>& trFaces, bool* halfedges, bool bMustBeConnected);
  bool is_connected(int* tr, bool* halfedges);
  void add_one_face(CGAL::Polyhedron_incremental_builder_3<HDS>& B, int i0, int i1, int i2, int faceID, cbf cb) ;
};


CgalPolyhedron*   get_CgalPolyhedron_DS(const vector< vector<int*> >&shell, const vector<Point3>& lsPts);
bool              check_global_orientation_normals(CgalPolyhedron* p, bool bOuter, cbf cb);
bool              check_global_orientation_normals_rev(CgalPolyhedron* p, bool bOuter, cbf cb);
bool              check_global_orientation_normals_rev2(CgalPolyhedron* p, bool bOuter, cbf cb);

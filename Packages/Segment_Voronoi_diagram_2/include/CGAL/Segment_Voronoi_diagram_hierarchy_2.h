// Copyright (c) 2003,2004  INRIA Sophia-Antipolis (France) and
// Notre Dame University (U.S.A.).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Menelaos Karavelas <mkaravel@cse.nd.edu>



#ifndef CGAL_SEGMENT_VORONOI_DIAGRAM_HIERARCHY_2_H
#define CGAL_SEGMENT_VORONOI_DIAGRAM_HIERARCHY_2_H

#include <map>

#include <CGAL/Segment_Voronoi_diagram_short_names_2.h>

#include <CGAL/Random.h>
#include <CGAL/Segment_Voronoi_diagram_2.h>
#include <CGAL/Segment_Voronoi_diagram_data_structure_2.h>
#include <CGAL/Segment_Voronoi_diagram_vertex_base_2.h>
#include <CGAL/Segment_Voronoi_diagram_hierarchy_vertex_base_2.h>
#include <CGAL/Triangulation_face_base_2.h>


CGAL_BEGIN_NAMESPACE

//--------------------------------------------------------------------
//--------------------------------------------------------------------

// parameterization of the hierarchy
#ifdef CGAL_SVD_HIERARCHY_DEMO
const unsigned int svd_hierarchy_2__ratio    = 3;
const unsigned int svd_hierarchy_2__minsize  = 5;
#else
const unsigned int svd_hierarchy_2__ratio    = 30;
const unsigned int svd_hierarchy_2__minsize  = 20;
#endif
const unsigned int svd_hierarchy_2__maxlevel = 5;
// maximal number of points is 30^5 = 24 millions !

//--------------------------------------------------------------------
//--------------------------------------------------------------------

template < class Gt, class STag = Tag_false,
	   class DS = Segment_Voronoi_diagram_data_structure_2<
              Segment_Voronoi_diagram_hierarchy_vertex_base_2<
                 Segment_Voronoi_diagram_vertex_base_2<Gt,
			     typename Gt::Intersections_tag> >,
              Triangulation_face_base_2<Gt> >,
	   class LTag = Tag_false>
class Segment_Voronoi_diagram_hierarchy_2
  : public Segment_Voronoi_diagram_2<Gt,DS,LTag>
{
public:
  // PUBLIC TYPES
  //-------------
  typedef Segment_Voronoi_diagram_2<Gt,DS,LTag>  Base;

  typedef typename Base::Geom_traits        Geom_traits;

  typedef typename Geom_traits::Point_2     Point_2;
  typedef typename Geom_traits::Site_2      Site_2;

  typedef typename Base::Vertex_handle      Vertex_handle;
  typedef typename Base::Face_handle        Face_handle;
  typedef typename Base::Edge               Edge;

  typedef typename Base::Vertex_circulator         Vertex_circulator;
  typedef typename Base::Edge_circulator           Edge_circulator;
  typedef typename Base::Face_circulator           Face_circulator;

  typedef typename Base::All_faces_iterator        All_faces_iterator;
  typedef typename Base::Finite_faces_iterator     Finite_faces_iterator;
  typedef typename Base::All_vertices_iterator     All_vertices_iterator;
  typedef typename Base::Finite_vertices_iterator  Finite_vertices_iterator;
  typedef typename Base::All_edges_iterator        All_edges_iterator;
  typedef typename Base::Finite_edges_iterator     Finite_edges_iterator;

private:
  typedef std::list<Point_2>                       PC;

public:
  typedef typename Base::Point_container           Point_container;
  typedef typename Base::size_type                 size_type;

  typedef typename Base::Intersections_tag         Intersections_tag;

  typedef STag                            Insert_segments_in_hierarchy_tag;
  typedef STag                            Segments_in_hierarchy_tag;

protected:
  // LOCAL TYPES
  //------------
  typedef typename Base::Storage_site_2            Storage_site_2;
  typedef typename Base::List                      List;
  typedef typename Base::Face_map                  Face_map;
  typedef typename Base::Vertex_triple             Vertex_triple;

protected:
  // LOCAL VARIABLES
  //----------------
  static const int UNDEFINED_LEVEL;

  // here is the stack of triangulations which form the hierarchy
  Base*   hierarchy[svd_hierarchy_2__maxlevel];
  Random random; // random number generator

public:
  // CONSTRUCTORS
  //-------------
  Segment_Voronoi_diagram_hierarchy_2
  (const Geom_traits& traits = Geom_traits());
  Segment_Voronoi_diagram_hierarchy_2
  (const Segment_Voronoi_diagram_hierarchy_2& svd);

  Segment_Voronoi_diagram_hierarchy_2 &operator=
  (const  Segment_Voronoi_diagram_hierarchy_2& svd);

  // DESTRUCTOR
  //-----------
  ~Segment_Voronoi_diagram_hierarchy_2();

public:
  // ACCESS METHODS
  //---------------
  const Base& diagram(unsigned int i) const  {
    CGAL_precondition( i < svd_hierarchy_2__maxlevel );
    return *hierarchy[i];
  }

public:
  // INSERTION METHODS
  //------------------
  template<class Input_iterator>
  size_type insert(Input_iterator first, Input_iterator beyond) {
    return insert(first, beyond, Tag_false());
  }

  template<class Input_iterator>
  size_type insert(Input_iterator first, Input_iterator beyond,	Tag_true)
  {
    std::vector<Site_2> site_vec;
    for (Input_iterator it = first; it != beyond; ++it) {
      site_vec.push_back(Site_2(*it));
    }
    std::random_shuffle(site_vec.begin(), site_vec.end());
    return insert(site_vec.begin(), site_vec.end(), Tag_false());
  }

  template<class Input_iterator>
  size_type insert(Input_iterator first, Input_iterator beyond,	Tag_false)
  {
    // do it the obvious way: insert them as they come;
    // one might think though that it might be better to first insert
    // all end points and then all segments, or a variation of that.
    size_type n_before = number_of_vertices();
    for (Input_iterator it = first; it != beyond; ++it) {
      insert(*it);
    }
    size_type n_after = number_of_vertices();
    return n_after - n_before;
  }

  Vertex_handle  insert(const Point_2& p) {
    return insert_point(p, UNDEFINED_LEVEL);
  }

  Vertex_handle  insert(const Point_2& p0, const Point_2& p1) {
    return insert_segment(p0, p1, UNDEFINED_LEVEL);
  }

  Vertex_handle insert(const Point_2& p, Vertex_handle) {
    return insert(p);
  }

  Vertex_handle insert(const Point_2& p1, const Point_2& p2,
		       Vertex_handle) {
    return insert(p1, p2);
  }

  Vertex_handle  insert(const Site_2& t);

  Vertex_handle  insert(const Site_2& t, Vertex_handle) {
    return insert(t);
  }

protected:
  Vertex_handle insert_point(const Point_2& p, int level);
  void          insert_point(const Point_2& p, int level,
			     Vertex_handle* vertices);

  void          insert_point(const Site_2& t, const Storage_site_2& ss,
			     int low, int high, Vertex_handle vbelow,
			     Vertex_handle* vertices);
#if 0
  // not implemented yet
  void          insert_point(const Site_2& p, int level,
			     Vertex_handle* vertices);
#endif

  Vertex_handle insert_segment(const Point_2& p0, const Point_2& p1,
			       int level); 

  Vertex_handle insert_segment_interior(const Site_2& t,
					const Storage_site_2& ss,
					const Vertex_handle* vertices0,
					int level);

  void insert_segment_in_upper_levels(const Site_2& t,
				      const Storage_site_2& ss,
				      Vertex_handle vbelow,
				      const Vertex_handle* vertices0,
				      int level, Tag_true);

  void insert_segment_in_upper_levels(const Site_2& t,
				      const Storage_site_2& ss,
				      Vertex_handle vbelow,
				      const Vertex_handle* vertices,
				      int level, Tag_false) {}

  template<class Tag>
  Vertex_handle
  insert_intersecting_segment_with_tag(const Storage_site_2& ss,
				       const Site_2& t,
				       Vertex_handle v,
				       int level, Tag_false itag, Tag) {
    static int i = 0;
    if ( i == 0 ) {
      i = 1;
      print_error_message();
    }
    return Vertex_handle();
  }

  Vertex_handle
  insert_intersecting_segment_with_tag(const Storage_site_2& ss,
				       const Site_2& t,
				       Vertex_handle v,
				       int level,
				       Tag_true itag, Tag_false stag);

  Vertex_handle
  insert_intersecting_segment_with_tag(const Storage_site_2& ss,
				       const Site_2& t,
				       Vertex_handle v,
				       int level,
				       Tag_true itag, Tag_true stag);

public:
  // NEAREST NEIGHBOR LOCATION
  //--------------------------
  Vertex_handle  nearest_neighbor(const Point_2& p,
				  bool force_point = false) const;

  Vertex_handle  nearest_neighbor(const Point_2& p, Vertex_handle)
  {
    return nearest_neighbor(p);
  }

protected:
  void nearest_neighbor(const Site_2& p,
			Vertex_handle vnear[svd_hierarchy_2__maxlevel],
			bool force_point) const; 

public:
  // MISCELLANEOUS
  //--------------
  void copy_triangulation
  (const Segment_Voronoi_diagram_hierarchy_2& svdh);

  void swap(Segment_Voronoi_diagram_hierarchy_2& svdh);
  void clear();

public:
  // VALIDITY CHECK
  //---------------
  bool is_valid(bool verbose = true, int level = 1) const;

protected:
  // LOCAL HELPER METHODS
  //---------------------
  int random_level();

  size_type find_level(Vertex_handle v) const;

  void print_error_message() const;
};


template<class Gt, class STag, class DS, class LTag>
const int
Segment_Voronoi_diagram_hierarchy_2<Gt,STag,DS,LTag>::UNDEFINED_LEVEL = -1;


CGAL_END_NAMESPACE



#ifdef CGAL_CFG_NO_AUTOMATIC_TEMPLATE_INCLUSION
#  include <CGAL/Segment_Voronoi_diagram_hierarchy_2.C>
#endif



#endif // CGAL_SEGMENT_VORONOI_DIAGRAM_HIERARCHY_2_H


// Copyright (c) 2005, 2006 Fernando Luis Cacciola Carballal. All rights reserved.
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
// $URL$
// $Id$
//
// Author(s)     : Fernando Cacciola <fernando_cacciola@ciudad.com.ar>
//
#ifndef CGAL_STRAIGHT_SKELETON_VERTEX_BASE_2_H
#define CGAL_STRAIGHT_SKELETON_VERTEX_BASE_2_H 1

#ifndef CGAL_HALFEDGEDS_VERTEX_BASE_H
#include <CGAL/HalfedgeDS_vertex_base.h>
#endif

#ifndef CGAL_STRAIGHT_SKELETON_HALFEDGE_BASE_2_H
#include <CGAL/Straight_skeleton_halfedge_base_2.h>
#endif

#ifndef CGAL_CIRCULATOR_H
#include <CGAL/circulator.h>
#endif

#include <boost/iterator/iterator_facade.hpp>

CGAL_BEGIN_NAMESPACE

template < class Refs, class P, class N >
class Straight_skeleton_vertex_base_base_2
  : public HalfedgeDS_vertex_base<Refs, Tag_true, P >
{
protected :

  class Halfedge_circulator_around_vertex_access_policy
  {
  public:
    template<class Impl>
    static typename Impl::reference access ( Impl* aImpl )
    {
      return aImpl->mHandle;
    }
  } ;

  class Halfedge_circulator_across_incident_faces_access_policy
  {
  public:
    template<class Impl>
    static typename Impl::reference access ( Impl* aImpl )
    {
      return aImpl->mHandle->face()->halfedge();
    }
  } ;
  
  template<class HalfedgeHandle, class AccessPolicy >
  class Halfedge_circulator_base
    : public boost::iterator_facade< Halfedge_circulator_base< HalfedgeHandle, AccessPolicy >
                                   ,HalfedgeHandle
                                   ,Bidirectional_circulator_tag
                                   ,HalfedgeHandle
                                  >
  {
    public:

      typedef HalfedgeHandle value_type ;
      typedef HalfedgeHandle reference ;

      Halfedge_circulator_base () : mHandle() {}

      explicit Halfedge_circulator_base ( value_type aHandle ) : mHandle(aHandle) {}

      template < class OtherHalfedgeHandle, class OtherAccessPolicy >
      Halfedge_circulator_base
        ( Halfedge_circulator_base<OtherHalfedgeHandle,OtherAccessPolicy> const& aOther )
        : mHandle(aOther.mHandle) {}

    private :

      typedef Halfedge_circulator_base<HalfedgeHandle,AccessPolicy> Self ;
      
      friend class boost::iterator_core_access ;
      template <class,class> friend class Halfedge_circulator_base;

      template < class OtherHalfedgeHandle, class OtherAccessPolicy >
      bool equal( Halfedge_circulator_base<OtherHalfedgeHandle,OtherAccessPolicy> const& aOther ) const
      {
        return mHandle == aOther.mHandle;
      }

      void increment() { mHandle = mHandle->opposite()->prev(); }

      void decrement() { mHandle = mHandle->next()->opposite() ; }

      reference dereference() const { return AccessPolicy::access(const_cast<Self*>(this)) ; }

    private :
    
      friend class Halfedge_circulator_around_vertex_access_policy ;
      friend class Halfedge_circulator_across_incident_faces_access_policy ;

      value_type mHandle ;
  } ;


public:

  typedef HalfedgeDS_vertex_base<Refs, Tag_true, P> Base ;

  typedef P Point_2;
  typedef N FT ;

  typedef typename Refs::Vertex_handle         Vertex_handle ;
  typedef typename Refs::Vertex_const_handle   Vertex_const_handle ;
  typedef typename Refs::Halfedge_handle       Halfedge_handle ;
  typedef typename Refs::Halfedge_const_handle Halfedge_const_handle ;

  typedef Halfedge_circulator_base< Halfedge_const_handle
                                   ,Halfedge_circulator_around_vertex_access_policy
                                  >
            Halfedge_around_vertex_const_circulator ;

  typedef Halfedge_circulator_base< Halfedge_handle
                                   ,Halfedge_circulator_around_vertex_access_policy
                                  >
            Halfedge_around_vertex_circulator ;

  typedef Halfedge_circulator_base< Halfedge_const_handle
                                   ,Halfedge_circulator_across_incident_faces_access_policy
                                  >
            Halfedge_across_incident_faces_const_circulator ;

  typedef Halfedge_circulator_base< Halfedge_handle
                                   ,Halfedge_circulator_across_incident_faces_access_policy
                                  >
            Halfedge_across_incident_faces_circulator ;

protected:

  Straight_skeleton_vertex_base_base_2() : mID(-1) {}
  Straight_skeleton_vertex_base_base_2 ( int aID, Point_2 const& aP )
    :
      Base(aP)
    , mID(aID)
    , mTime(0.0)
  {
  }

  Straight_skeleton_vertex_base_base_2 ( int aID, Point_2 const& aP, FT aTime )
    :
      Base(aP)
    , mID(aID)
    , mTime(aTime)
 {
 }

public:

  int id() const { return mID ; }

  FT time() const { return mTime ; }

  Halfedge_const_handle primary_bisector() const { return this->halfedge()->next(); }

  Halfedge_handle primary_bisector() { return this->halfedge()->next(); }

  Halfedge_around_vertex_const_circulator incident_edges_begin() const
  {
    return Halfedge_around_vertex_const_circulator(this->halfedge());
  }

  Halfedge_around_vertex_circulator incident_edges_begin()
  {
    return Halfedge_around_vertex_circulator(this->halfedge());
  }
  Halfedge_across_incident_faces_const_circulator defining_contour_edges_begin() const
  {
    return Halfedge_across_incident_faces_const_circulator(this->halfedge());
  }

  Halfedge_across_incident_faces_circulator defining_contour_edges_begin()
  {
    return Halfedge_across_incident_faces_circulator(this->halfedge());
  }

  bool is_skeleton() const { return  this->halfedge()->is_bisector() ; }
  bool is_contour () const { return !this->halfedge()->is_bisector() ; }

private:

  int mID ;
  FT  mTime ;
};

template < class Refs, class P, class N >
class Straight_skeleton_vertex_base_2
  : public Straight_skeleton_vertex_base_base_2<Refs,P,N>
{

public:

  typedef Straight_skeleton_vertex_base_base_2<Refs,P,N> Base ;

  typedef typename Base::Base            Base_base ;
  typedef typename Base::Point_2         Point_2;
  typedef typename Base::FT              FT;
  typedef typename Base::Halfedge_handle Halfedge_handle ;

public:

  Straight_skeleton_vertex_base_2() {}
  Straight_skeleton_vertex_base_2 ( int aID, Point_2 const& aP )
    :
    Base(aID,aP)
  {}

  Straight_skeleton_vertex_base_2 ( int aID, Point_2 const& aP, FT aTime )
    :
    Base(aID,aP,aTime)
 {}
protected:
//  void set_halfedge( Halfedge_handle h ) { this->set_halfedge(h) ; }
};
CGAL_END_NAMESPACE

#endif // CGAL_STRAIGHT_SKELETON_VERTEX_BASE_2_H //
// EOF //


/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2011-2012 Ricardo Fernandes - All Rights Reserved       *
 *   Email: rjf@dcc.fc.up.pt or rikardojfernandez@gmail.com                *
 *                                                                         *
 *   This file is part of VNS - Vehicular Networks Simulator.              *
 *                                                                         *
 *   For information about the licensing and copyright of this code        *
 *   please contact the author.                                            *
 *                                                                         *
 ***************************************************************************/

#ifndef VNS_POLYGON_H
#define VNS_POLYGON_H

#include "datastream.h"
#include "bbox.h"
#include "core.h"
#include "vec.h"
#include "vector.h"

namespace vns {

/*! \cond PRIVATE */

class Polygon : public BBoxItem {

private:

    vns::BBox bb;
    vns::Vector<vns::Vec> v;
    double a;

    void initialize();
    bool snip(const int u,int i,int w,int n,int *V) const;
    bool insidetriangle(double Ax, double Ay,double Bx, double By,double Cx, double Cy,double Px, double Py) const;
    Polygon* clone() const;

    friend DataStream& operator <<(DataStream& f, const Polygon* p);
    friend DataStream& operator >>(DataStream& f, Polygon* p);

public:
    
    Polygon();
    Polygon(int32 n, double* xs, double* ys, double* zs);
    Polygon(const vns::Vector<vns::Vec>& pv);
    ~Polygon();

    void setPoints(int32 n, double* xs, double* ys, double* zs);
    void setPoints(const vns::Vector<vns::Vec>& pv);
    void removePoint(int32 i);
    inline void clear(){v.clear(); bb.clear();};

    inline int32 getNumberOfPoints() const { return v.size(); }
    inline const Vec& getPoint(int32 i) const { return v[i]; };
    inline const BBox& getBBox() const { return bb; };
    inline double area() const { return a; };

    double distanceTo(const vns::Vec& p) const;

    bool intersects(const vns::Vec& p1,const vns::Vec& p2);
    bool intersects(const vns::BBox& bb);

    bool triangulate(vns::Vector<vns::Vec>& triangles);

};

/*! \endcond */
}

#endif

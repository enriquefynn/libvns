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

#ifndef VNS_POLYLINE_H
#define VNS_POLYLINE_H

#include "core.h"
#include "datastream.h"
#include "bbox.h"
#include "vec.h"
#include "vector.h"
#include "list.h"

namespace vns {

class Road;

class PolyLine : public BBoxItem {

private:

    double len;
    vns::BBox bb;
    vns::Vector<vns::Vec> v;
    vns::Vector<double> T;
    vns::Vector<vns::Vec> normal;

    void initialize();
    int seg(double p) const;
    PolyLine* clone() const;


    friend DataStream& operator <<(DataStream& f, const PolyLine* p);
    friend DataStream& operator >>(DataStream& f, PolyLine* p);

    friend class Road;

public:
    
    PolyLine();
    PolyLine(int32 n, vns::Vec *pv);
    PolyLine(int32 n, double* xs, double* ys, double* zs);
    PolyLine(const vns::Vector<vns::Vec>& pv);
    PolyLine(const vns::List<vns::Vec>& pv);
    virtual ~PolyLine();

    inline const vns::BBox& getBBox() const { return bb; };

    void setPoints(int32 n, double* xs, double* ys, double* zs);
    void setPoints(int32 n, Vec* pv);
    void setPoints(const vns::Vector<vns::Vec>& pv);
    void setPoints(const vns::List<vns::Vec>& pv);
    void updatePoint(int32 i, double x, double y, double z);
    void insertPoint(int32 i, const vns::Vec& point);
    void removePoint(int32 i);
    void reverse();
    void clear();
    inline double getLength() const { return len; }

    inline int32 size() const { return v.size(); }
    inline int32 getNumberOfPoints() const { return v.size(); }
    inline const Vec& getPoint(int32 i) const { return v[i]; };
    inline const Vec& getFirstPoint() const { return v[0]; };
    inline const Vec& getFirstPointNormal() const { return normal[0]; };
    inline double getFirstPointT() const { return T[0]; };
    inline const Vec& getLastPoint() const { return v[v.size()-1]; };
    inline const Vec& getLastPointNormal() const { return normal[v.size()-1]; };
    inline double getLastPointT() const { return T[v.size()-1]; };
    inline const Vec& getNormal(int32 i) const { return normal[i]; };
    inline double getT(int32 i) const { return T[i]; };
    inline Vec getLastPointDirection() const { return (v[v.size()-1]-v[v.size()-2]).normalized(); };
    inline Vec getFirstPointDirection() const { return (v[1]-v[0]).normalized(); };

    double distanceTo(const vns::Vec& p) const;
    void pointDirFromFirstPoint(vns::Vec& pt, vns::Vec& dir, vns::Vec& perp, double pos, double offset=0) const;
    void pointDirFromLastPoint(vns::Vec& pt, vns::Vec& dir, vns::Vec& perp, double pos, double offset=0) const;
    int32 getPointWithin(const vns::Vec&pt, double radius) const ;
    int32 getNearestPoint(const vns::Vec& pt, double& d) const ;
    int32 getNearestSegment(const vns::Vec&pt, double& d, Vec& spt) const;
    bool distanceToPointIsLessThan(const vns::Vec& pt, double distance) const ;

    // TODO: maybe remove lastSeg and getPosition
    void lastSeg(bool start, double d, vns::Vec& v0, vns::Vec& v1);

    double getPosition(const vns::Vec& p, bool forward = true);

    vns::Vec getDirection(int32 i) const;

    vns::Vec getPerpendicular(int32 i) const;

    int32 getIndexPosition(double p) const;
    int32 getIndexPositionX(double p,int32 i=0) const;
};

}

#endif

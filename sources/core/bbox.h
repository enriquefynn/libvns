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

#ifndef VNS_BBOX_H
#define VNS_BBOX_H

#include <iostream>
#include "datastream.h"
#include "core.h"
#include "vec.h"

namespace vns {

class BBox {
public:

	inline BBox(double xmin=vns::MAX_DOUBLE, double ymin=vns::MAX_DOUBLE, double xmax=vns::MIN_DOUBLE, double ymax=vns::MIN_DOUBLE);
	inline BBox(const Vec& vmin, const Vec& vmax):minX(vmin.getX()),minY(vmin.getY()),maxX(vmax.getX()),maxY(vmax.getY()){};
	inline BBox(const BBox& bb):minX(bb.minX),minY(bb.minY),maxX(bb.maxX),maxY(bb.maxY){};
	inline virtual ~BBox(){};
    inline void setBox(double xmin, double ymin, double xmax, double ymax);
    inline void setBox(const BBox& box);
    inline void clear();
    inline void addPoint(const Vec& pt);
    inline void addBBox(const BBox& box);
    inline bool contains( const Vec& pt ) const;
    inline bool intersects( const BBox& bb ) const;
    inline bool intersects( const Vec& p1, const Vec& p2 ) const;
    inline double getMinX() const { return minX; };
    inline double getMinY() const { return minY; };
    inline double getMaxX() const { return maxX; };
    inline double getMaxY() const { return maxY; };
    inline double getHeight() const { return maxY-minY; };
    inline double getWidth() const { return maxX-minX; };
    inline Vec getLeftTop() const { return Vec(minX,maxY); };
    inline Vec getLeftBottom() const { return Vec(minX,minY); };
    inline Vec getRightTop() const { return Vec(maxX,maxY); };
    inline Vec getRightBottom() const { return Vec(maxX,minY); };
    inline Vec getCenter() const;
	inline double getArea() const { return (maxX-minX)*(maxY-minY); };
	inline double getIntersectionArea(const BBox& bb) const;
	inline double getDistanceFromCenter(const BBox& bb) const;

	inline bool operator==(const BBox &bb) const { return maxX==bb.maxX && maxY==bb.maxY && minX==bb.minX && minY==bb.minY; };
	inline bool operator!=(const BBox &bb) const { return maxX!=bb.maxX || maxY!=bb.maxY || minX!=bb.minX || minY!=bb.minY; };

private:
    double minX, minY, maxX, maxY;

    friend DataStream& operator<<(DataStream& f, const BBox& b) {f<<b.minX;f<<b.minY;f<<b.maxX;f<<b.maxY;return f;};
    friend DataStream& operator>>(DataStream& f, BBox& b){f>>b.minX;f>>b.minY;f>>b.maxX;f>>b.maxY;return f;};
	friend std::ostream& operator<<(std::ostream& out, const BBox& bb){out<<"("<<bb.minX<<","<<bb.minY<<","<<bb.maxX<<","<<bb.maxY<<")";return out;};
};

class BBoxItem {
public:
	virtual ~BBoxItem(){};
	virtual const BBox& getBBox() const = 0;
};

inline BBox::BBox(double x0, double y0, double x1, double y1) {
	minX=x0;
	minY=y0;
	maxX=x1;
	maxY=y1;
}

inline void BBox::setBox( double x0, double y0, double x1, double y1 ) {
	minX=x0;
	minY=y0;
	maxX=x1;
	maxY=y1;
}

inline void BBox::setBox(const BBox& box){
	minX=box.minX;
	minY=box.minY;
	maxX=box.maxX;
	maxY=box.maxY;
}

inline void BBox::clear(){
	minX=vns::MAX_DOUBLE;
	minY=vns::MAX_DOUBLE;
	maxX=vns::MIN_DOUBLE;
	maxY=vns::MIN_DOUBLE;
}

inline Vec BBox::getCenter() const {
	return Vec((minX+maxX)*0.5,(minY+maxY)*0.5);
};

inline bool BBox::intersects(const BBox& b) const {
    if (b.maxX<minX || b.minX>maxX)
        return false;
    if (b.maxY<minY || b.minY>maxY)
        return false;
    return true;
}

inline bool BBox::intersects( const Vec& p1, const Vec& p2 ) const {
	Vec lowerLeft( minX, minY );
	Vec upperRight( maxX, maxY );
	Vec upperLeft( minX, maxY );
	Vec lowerRight( maxX, minY );

	if (p1.getX() > lowerLeft.getX() && p1.getX() < upperRight.getX()
			&& p1.getY() < lowerLeft.getY() && p1.getY() > upperRight.getY()
			&& p2.getX() > lowerLeft.getX() && p2.getX() < upperRight.getX()
			&& p2.getY() < lowerLeft.getY() && p2.getY() > upperRight.getY() ){
		return true;
	}
	if( vns::intersects(p1,p2, upperLeft, lowerLeft ) ) return true;
	if( vns::intersects(p1,p2, lowerLeft, lowerRight) ) return true;
	if( vns::intersects(p1,p2, upperLeft, upperRight) ) return true;
	if( vns::intersects(p1,p2, upperRight, lowerRight) ) return true;
	return false;
}

inline bool BBox::contains(const Vec& pt) const {
    return pt.getX()>=minX && pt.getX()<=maxX && pt.getY()>=minY && pt.getY()<=maxY;
}

inline void BBox::addPoint(const Vec& pt) {
    if(pt.getX()<=minX) minX = pt.getX();
    if(pt.getX()>=maxX) maxX = pt.getX();
    if(pt.getY()<=minY) minY = pt.getY();
    if(pt.getY()>=maxY) maxY = pt.getY();
}

// stretch
inline void BBox::addBBox(const BBox& box){
	addPoint( Vec(box.minX,box.minY) );
	addPoint( Vec(box.maxX,box.maxY) );
}

inline double BBox::getIntersectionArea(const BBox& bb) const {
	if(intersects(bb)){
		return BBox((minX>bb.minX?minX:bb.minX),(minY>bb.minY?minY:bb.minY),(maxX<bb.maxX?maxX:bb.maxX),(maxY<bb.maxY?maxY:bb.maxY)).getArea();
	}
	return 0.0;
}

inline double BBox::getDistanceFromCenter(const BBox& bb) const {
	double distance = 0, t;
	t = (minX+maxX+bb.minX+bb.maxX)*0.5;
	distance += t*t;
	t = (minY+maxY+bb.minY+bb.maxY)*0.5;
	distance += t*t;
	return distance;
}

}

#endif

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

#ifndef VNS_VEC_H
#define VNS_VEC_H

#include "datastream.h"
#include <iostream>
#include "maths.h"

namespace vns {

class Vec {
private:
    double mx, my, mz;

public:
    inline Vec() { mx=my=mz=0; };
    inline Vec(double xv, double yv, double zv=0){ mx=xv;my=yv;mz=zv; };
    inline Vec(const Vec& v){ mx=v.mx; my=v.my; mz=v.mz; };
    inline ~Vec(){};
    
    inline Vec& operator=(const Vec& v){ mx=v.mx; my=v.my; mz=v.mz; return *this; };
    inline Vec operator+(const Vec &v) const { return Vec(mx+v.mx,my+v.my,mz+v.mz); };
    inline bool operator==(const Vec &v) const { return mx==v.mx && my==v.my && mz==v.mz; };
    inline bool operator!=(const Vec &v) const { return mx!=v.mx || my!=v.my || mz!=v.mz; };
    inline Vec operator-(const Vec &v) const { return Vec(mx-v.mx,my-v.my,mz-v.mz); };
    inline Vec operator-() const { return Vec(-mx,-my,-mz); };
    inline double operator*(const Vec& p) const { return mx*p.mx+my*p.my+mz*p.mz; };

    inline Vec operator*(const double a) const { return Vec(mx*a,my*a,mz*a); };
    inline friend Vec operator*(const double a, const Vec& v) { return Vec(v.mx*a,v.my*a,v.mz*a); };
    inline Vec operator/(double fact) const { double xf = 1.0/fact; return Vec(mx*xf, my*xf, mz*xf); };
    inline Vec& operator*=(double fact){ mx*=fact; my*=fact; mz*=fact; return *this; };


    inline Vec perpendicular() const { return Vec(-my,mx,0); };
    inline Vec normalized() const { double d = isqrt(mx*mx+my*my+mz*mz); return Vec(mx*d,my*d,mz*d); };
    inline void normalize(){ double d = isqrt(mx*mx+my*my+mz*mz); mx*=d;my*=d;mz*=d; };
    inline double norm() const { return fsqrt(mx*mx+my*my+mz*mz); };
    inline double normSquared() const { return mx*mx+my*my+mz*mz; };
    
    inline double dot(const Vec& p) const { return mx*p.mx + my*p.my + mz*p.mz; };
    inline Vec cross(const Vec& p) const { return Vec(my*p.mz-mz*p.my,mz*p.mx-mx*p.mz,mx*p.my-my*p.mx); };

    inline double dir_deg() const { return atan2(my,mx)*180.0/M_PI; };
    inline double dir_rad() const { return atan2(my,mx); };

    inline double x() const { return mx; };
    inline double y() const { return my; };
    inline double z() const { return mz; };
    inline double getX() const { return mx; };
    inline double getY() const { return my; };
    inline double getZ() const { return mz; };
    inline void setX(double mx){ this->mx=mx; };
    inline void setY(double my){ this->my=my; };
    inline void setZ(double mz){ this->mz=mz; };

    inline double& operator[](int dim){ return (dim==0?mx:(dim==1?my:mz)); };
    inline double operator[](int dim) const { return (dim==0?mx:(dim==1?my:mz)); };

    inline double angleTo(Vec v) const;
    inline double angleDiffTo(Vec v) const;
    inline double distanceTo(const Vec& p) const;
    inline double distanceToLessThan(const Vec& p, double d) const;
    inline double distanceToLine(const Vec& p1,const Vec& p2) const;
    inline Vec nearPointInTheLine(const Vec& p1,const Vec& p2) const;

    friend DataStream& operator<<(DataStream& ds, const Vec& v){ds<<v.mx; ds<<v.my; ds<<v.mz;return ds;}
    friend DataStream& operator>>(DataStream& ds, Vec& v){ds>>v.mx; ds>>v.my; ds>>v.mz;return ds;}
    friend inline uint hash(const Vec& p){ return uint(p.mx)*31+uint(p.my); };

    friend std::ostream& operator<<(std::ostream& out, const Vec& p ){out<<"("<<p.mx<<","<<p.my<<","<<p.mz<<")";return out;};
};

class VecItem {
public:
	virtual ~VecItem(){};
	virtual const Vec& getPosition() const = 0;
};

inline double Vec::angleTo(Vec v) const {
	double sp =mx*v.mx+my*v.my+mz*v.mz;
	double r = acos(sp/(norm()*v.norm())) * 180 / M_PI;
	if(sp<0){
		r*=-1;
	}
	return r;
};

inline double Vec::angleDiffTo(Vec v) const {
	double a = dir_deg() - v.dir_deg();
	if(a <= -180){
		a += 360;
	}
	if(a>=180){
		a-=360;
	}
	return a;
};

inline double Vec::distanceTo(const Vec& p) const {
	double dx = p.mx-mx;
	double dy = p.my-my;
	double dz = p.mz-mz;
	return fsqrt((dx*dx)+(dy*dy)+(dz*dz));
};

inline double Vec::distanceToLessThan(const Vec& p, double d) const {
	double dx = p.mx-mx;
	double dy = p.my-my;
	double dz = p.mz-mz;
	return (dx*dx)+(dy*dy)+(dz*dz) < d*d;
};

inline double Vec::distanceToLine(const Vec& p1,const Vec& p2) const {
	return distanceTo( nearPointInTheLine(p1,p2) );
};

inline Vec Vec::nearPointInTheLine(const Vec& p1,const Vec& p2) const {
	Vec c = p2-p1;
	double u = ((mx-p1.mx)*(p2.mx-p1.mx) + (my-p1.my)*(p2.my-p1.my))/c.norm();
	return Vec( p1.mx + u*(p2.mx-p1.mx) , p1.my + u*(p2.my-p1.my) );
};

}

#endif

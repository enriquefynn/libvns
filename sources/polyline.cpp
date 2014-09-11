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

#include "polyline.h"

namespace vns {



Vec PolyLine::getDirection(int32 i) const {
	if(i>=v.size()-1){
		return (v[v.size()-1]-v[v.size()-2]).normalized();
	}
	return (v[i+1]-v[i]).normalized();
}

Vec PolyLine::getPerpendicular(int32 i) const {
	if(i>=v.size()-1){
		return (v[v.size()-1]-v[v.size()-2]).perpendicular().normalized();
	}
	return (v[i+1]-v[i]).perpendicular().normalized();
}

PolyLine::PolyLine(){
	len = 0.0;
}

PolyLine* PolyLine::clone() const {
	PolyLine* line = new PolyLine();
	int32 n = v.size();
    line->v.resize( n );
    for (int32 i=0; i<n; ++i) {
        line->v[i] = v[i];
    }
    line->initialize();
    return line;
}

PolyLine::PolyLine(int32 n, Vec *pv) {
	setPoints(n,pv);
}

PolyLine::PolyLine(int32 n, double* xs, double* ys, double *zs) {
	setPoints(n,xs,ys,zs);
}

PolyLine::PolyLine(const vns::Vector<Vec>& pv) {
	setPoints(pv);
}

PolyLine::PolyLine(const vns::List<Vec>& pv) {
	setPoints(pv);
}

PolyLine::~PolyLine(){}

void PolyLine::setPoints(int32 n, double* xs, double* ys, double* zs) {
    v.resize(n);
    for (int32 i=0; i<n; ++i) {
        v[i].setX( *xs++ );
        v[i].setY( *ys++ );
        v[i].setZ( *zs++ );
    }
    initialize();
}

void PolyLine::setPoints(int32 n, Vec *pv) {
    v.resize(n);
    for (int32 i=0; i<n; ++i) {
        v[i] = *pv++;
    }
    initialize();
}

void PolyLine::setPoints(const vns::Vector<Vec>& pv) {
	v.resize(pv.size());
    for (int32 i=0; i<pv.size(); ++i) {
    	v[i] = pv[i];
    }
    initialize();
}

void PolyLine::setPoints(const vns::List<Vec>& pv) {
	v.resize(pv.size());
	vns::List<Vec>::ConstIterator it = pv.begin();
	int i=0;
    while( it!=pv.end() ) {
    	v[i] = *it;
    	it++;
    	i++;
    }
    initialize();
}


void PolyLine::updatePoint(int32 i, double x, double y, double z){
	v[i].setX(x);
	v[i].setY(y);
	v[i].setZ(z);
	initialize();
}

void PolyLine::insertPoint(int32 i, const Vec& p){
	v.insert(i,p);
	initialize();
}

void PolyLine::removePoint(int32 i){
	v.remove(i);
	initialize();
}


void PolyLine::reverse(){
	int32 n = v.size();
	for(int32 i=0;i<n/2;i++){
		Vec tmp = v[i];
		v[i] = v[n-1-i];
		v[n-1-i] = tmp;
	}
	initialize();
}

void PolyLine::initialize() {
	bb.clear();
    int32 n = v.size();
	T.resize(n);
	normal.resize(n);
    VNS_ASSERT( n > 1 ,"PolyLine","*** PolyLine size is less than 2");
    len = 0;
    T[0] = 0;
    bb.addPoint(v[0]);
    for(int32 i=1; i<n; ++i) {
    	bb.addPoint(v[i]);
        Vec dp = v[i]-v[i-1];
        len += dp.norm();
        normal[i] = dp.perpendicular().normalized();
        T[i] = len;
    }
    normal[0] = normal[1];
    for(int32 i=1; i<n-1; ++i) {
    	normal[i] = (1.0/(1.0+normal[i]*normal[i+1]))*(normal[i]+normal[i+1]);
    }

}

void PolyLine::clear(){
	bb.clear();
	v.clear();
	normal.clear();
	T.clear();
	len = 0;
}

int32 PolyLine::seg(double p) const {
	int32 n = v.size();
    int32 l=0, h=n, m;
    while (l+1<h) {
        m = (l+h)/2;
        if(p>T[m])
            l = m;
        else
            h = m;
    }
    if(l==n-1) --l;
    return l;
}

int32 PolyLine::getIndexPosition(double p) const {
	int32 n = v.size();
    int32 l=0, h=n, m;
    while(l+1<h) {
        m = (l+h)/2;
        if(p>T[m])
            l = m;
        else
            h = m;
    }
    if(l==n-1) --l;
    return l;
}

int32 PolyLine::getIndexPositionX(double p,int32 i) const {
	int32 n = v.size();

	if(i<0) i = 0;
	if(i>n-1) i = n-1;

    while(i<n-1 && T[i+1]<p) {
    	i++;
    }
    while(i>0 && T[i]>p) {
    	i--;
    }
    if(i>=n-1) i = n-2;
    return i;
}


double PolyLine::distanceTo(const Vec& p) const {
    double d = vns::MAX_DOUBLE;
    int32 n = v.size()-1;
    for( int32 i=0; i < n ; i++ ) {
        Vec dif = v[i+1]-v[i];
        dif.normalize();
        Vec n = dif.perpendicular();
        double d1 = fabs(n*(p-v[i]));
        //double c = dir*(p-v[i]);
        if (d1<d){
            d=d1;
        }
    }
    return d;
}

void PolyLine::pointDirFromFirstPoint(Vec& pt, Vec& dir, Vec& perp, double pos, double offset) const {
    int32 i = seg(pos);
    Vec dp = v[i+1]-v[i];
    double a = (pos-T[i])/(T[i+1]-T[i]);
    dir = dp.normalized();
    perp = (normal[i]*(1-a)+normal[i+1]*a).normalized();
    pt = v[i]+dp*a+perp*offset;
}

void PolyLine::pointDirFromLastPoint(Vec& pt, Vec& dir, Vec& perp, double pos, double offset) const {
    pos = len-pos;
    pointDirFromFirstPoint(pt, dir, perp, pos, offset);
    dir = -dir;
    perp = -perp;
}

int32 PolyLine::getPointWithin(const Vec& pt, double radius) const {
	int32 n = v.size();
    for(int32 i=0; i<n; i++) {
        double pd = (v[i]-pt).norm();
        if(pd < radius){
        	return i;
        }
    }
    return -1;
}

bool PolyLine::distanceToPointIsLessThan(const Vec& pt, double radius) const {
	double a,pa;
	bool done;
	int32 i,k;
	int32 n = v.size();
	Vec p[4];
    for(i=0; i<n-1; i++) {
    	p[0] = v[i]-normal[i]*radius;
    	p[1] = v[i]+normal[i]*radius;
    	p[2] = v[i+1]+normal[i+1]*radius;
    	p[3] = v[i+1]-normal[i+1]*radius;
    	pa = (pt.getY()-p[0].getY())*(p[1].getX()-p[0].getX()) - (pt.getX()-p[0].getX())*(p[1].getY()-p[0].getY());
    	done = true;
    	for(k=1;k<4;k++){
    		a = (pt.getY()-p[k].getY())*(p[(k+1)%4].getX()-p[k].getX()) - (pt.getX()-p[k].getX())*(p[(k+1)%4].getY()-p[k].getY());
    		if( (pa < 0 &&  a > 0) || (pa > 0 && a < 0) ){
    			done = false;
    			break;
    		}
    		pa = a;
    	}
    	if(done) return true;
    }
    return false;
}


int32 PolyLine::getNearestPoint(const Vec& pt, double& d) const {
    int32 n = v.size();
    d = 1e6;
    int32 np = 0;
    for(int32 i=0; i<n; ++i) {
        double pd = (v[i]-pt).norm();
        if(pd<d){
            d = pd;
            np = i;
        }
    }
    return np;   
}

int32 PolyLine::getNearestSegment(const Vec& p, double& d, Vec& spt) const {
    int32 n = v.size()-1;
    d = vns::MAX_DOUBLE;
    int32 ns = 0;
    double ip;
    double sd;
    for(int32 i=0; i<n; ++i) {
        Vec u = v[i+1]-v[i];
        u.normalize();
        ip = (p-v[i])*u;
        sd = (p-v[i]-u*ip).norm();
        if(sd<d) {
            d = sd;
            ns = i;
            spt = v[i]+u*ip;
        }
    }
    return ns;
}

void PolyLine::lastSeg(bool start, double d, Vec& v0, Vec& v1) {
    if(start) {
        v0 = v[0]+d*normal[0];
        v1 = v[1]+d*normal[1];
    } else {
    	int32 n = v.size();
        v0 = v[n-2]+d*normal[n-2];
        v1 = v[n-1]+d*normal[n-1];
    }
}

double PolyLine::getPosition(const Vec& p,bool forward){
	double d;
	Vec cp;
	int32 i = getNearestSegment(p,d,cp);
	double dt = T[i]+(cp-v[i]).norm();
	if(dt<0) return 0.0;
	if(dt>len) return len;
	if(forward){
		return dt;
	}else{
		return len-dt;
	}
}

DataStream& operator <<(DataStream&f, const PolyLine* p) {
    f << p->v;
    return f;
}

DataStream& operator >>(DataStream& f, PolyLine* p) {
    f >> p->v;
    p->initialize();
    return f;
}


}

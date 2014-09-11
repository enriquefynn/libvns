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

#include "polygon.h"

namespace vns {

DataStream& operator <<(DataStream&f, const Polygon* p) {
    f << p->v;
    return f;
}

DataStream& operator >>(DataStream& f, Polygon* p) {
    f >> p->v;
    p->initialize();
    return f;
}

Polygon::Polygon(){

}

Polygon* Polygon::clone() const {
	Polygon* pol = new Polygon();
	int32 n = v.size();
    pol->v.resize( n );
    for (int32 i=0; i<n; ++i) {
        pol->v[i] = v[i];
    }
    pol->initialize();
    return pol;
}


Polygon::Polygon(int32 n, double* xs, double* ys, double *zs) {
	setPoints(n,xs,ys,zs);
}

Polygon::Polygon(const vns::Vector<Vec>& pv) {
	setPoints(pv);
}

Polygon::~Polygon(){}

void Polygon::setPoints(int32 n, double* xs, double* ys, double* zs) {
    v.resize(n);
    for (int32 i=0; i<n; ++i) {
        v[i].setX( *xs++ );
        v[i].setY( *ys++ );
        v[i].setZ( *zs++ );
    }
    initialize();
}

void Polygon::setPoints(const vns::Vector<Vec>& pv) {
	v.resize(pv.size());
    for (int32 i=0; i<pv.size(); ++i) {
    	v[i] = pv[i];
    }
    initialize();
}


void Polygon::removePoint(int32 i){
	v.remove(i);
	initialize();
}


void Polygon::initialize() {
    int32 n = v.size();
    VNS_ASSERT( n > 1 ,"Polygon","*** Polygon size is less than 2");
    bb.clear();
    for(int32 i=0; i<n; ++i) {
        bb.addPoint(v[i]);
    }
    a=0.0f;
    for(int p=n-1,q=0; q<n; p=q++){
  	  a += v[p].getX()*v[q].getY() - v[q].getX()*v[p].getY();
    }
    a = a*0.5;
}

bool Polygon::intersects(const vns::Vec& p1,const vns::Vec& p2){
	//if( !bb.intersects(p1,p2) ) return false;
	for(int i=1;i<v.size();i++){
		if( vns::intersects(p1,p2,v[i-1],v[i]) ){
			return true;
		}
	}
	return false;
}

bool Polygon::intersects(const vns::BBox& bbox){
	if( !bb.intersects(bbox) ) return false;
	for(int i=1;i<v.size();i++){
		if( bbox.intersects(v[i-1],v[i]) ){
			return true;
		}
	}
	return false;
}

double Polygon::distanceTo(const Vec& p) const {
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

bool Polygon::insidetriangle(double Ax, double Ay,double Bx, double By,double Cx, double Cy,double Px, double Py) const{
  double ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
  double cCROSSap, bCROSScp, aCROSSbp;

  ax = Cx - Bx;  ay = Cy - By;
  bx = Ax - Cx;  by = Ay - Cy;
  cx = Bx - Ax;  cy = By - Ay;
  apx= Px - Ax;  apy= Py - Ay;
  bpx= Px - Bx;  bpy= Py - By;
  cpx= Px - Cx;  cpy= Py - Cy;

  aCROSSbp = ax*bpy - ay*bpx;
  cCROSSap = cx*apy - cy*apx;
  bCROSScp = bx*cpy - by*cpx;
  return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
};


bool Polygon::snip(const int u,int i,int w,int n,int *V) const{
	const double EPSILON=0.0000000001f;
	int p;
	double Ax, Ay, Bx, By, Cx, Cy, Px, Py;

	Ax = v[V[u]].getX();
	Ay = v[V[u]].getY();

	Bx = v[V[i]].getX();
	By = v[V[i]].getY();

	Cx = v[V[w]].getX();
	Cy = v[V[w]].getY();

	if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

	for (p=0;p<n;p++){
		if( (p == u) || (p == i) || (p == w) ) continue;
		Px = v[V[p]].getX();
		Py = v[V[p]].getY();
		if (insidetriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
	}
	return true;
}

bool Polygon::triangulate(vns::Vector<vns::Vec>& result){

	int n = v.size();
	if ( n < 3 ) return false;

	int *V = new int[n];

	/* we want a counter-clockwise polygon in V */

	if( 0.0f < area() ){
	    for (int i=0; i<n; i++){
	    	V[i] = i;
	    }
	}else{
	    for(int i=0; i<n; i++){
	    	V[i] = (n-1)-i;
	    }
	}

	int nv = n;

	/*  remove nv-2 Vertices, creating 1 triangle every time */
	int count = 2*nv;   /* error detection */

	for(int m=0, i=nv-1; nv>2; ){
		/* if we loop, it is probably a non-simple polygon */
	    if (0 >= (count--)){
	      //** Triangulate: ERROR - probable bad polygon!
	    	return false;
	    }

	    /* three consecutive vertices in current polygon, <u,v,w> */
	    int u = i; if (nv <= u) u = 0;     /* previous */
	    i = u+1; if (nv <= i) i = 0;     /* new v    */
	    int w = i+1; if (nv <= w) w = 0;     /* next     */

	    if ( snip(u,i,w,nv,V) ){
	    	int a,b,c,s,t;

	    	/* true names of the vertices */
	    	a = V[u]; b = V[i]; c = V[w];

	    	/* output Triangle */
	    	result.push_back( v[a] );
	    	result.push_back( v[b] );
	    	result.push_back( v[c] );

	    	m++;

	    	/* remove v from remaining polygon */
	    	for(s=i,t=i+1;t<nv;s++,t++){
	    		V[s] = V[t];
	    		nv--;
	    	}
	    	/* resest error detection counter */
	    	count = 2*nv;
	    }
	}

	delete V;

	return true;
}

}

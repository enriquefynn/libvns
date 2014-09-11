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

#include "maths.h"
#include "vec.h"
#include "bbox.h"
#include <stdlib.h>
#include <omp.h>

static unsigned int* tseed[50];

namespace vns {


int __rand_r(unsigned int *seed)
{
    unsigned int next = *seed;
    int result;

    next *= 1103515245;
    next += 12345;
    result = (unsigned int) (next / 65536) % 2048;

    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;

    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;

    *seed = next;

    return result;
}

void setRandomSeed(unsigned int* seed){
	//srand(seed);
	tseed[omp_get_thread_num()] = seed;
}

int randomInt(){
	/*
	unsigned int* g_seed = tseed[omp_get_thread_num()];
	*g_seed = (214013*(*g_seed)+2531011);
	return (*g_seed>>16)&0x7FFF;*/
	return __rand_r(tseed[omp_get_thread_num()]);
}

unsigned int getRandomSeed(){
	return *tseed[omp_get_thread_num()];
}


Vec closestPoint(const Vec& p,const Vec& a,const Vec& b){
	Vec ap = p-a;
	Vec ab = b-a;

	double magnitudeab = ab.normSquared();
	double abapprod = ab*ap;
	double d = abapprod/magnitudeab;
	if(d<0){
		return Vec(a.getX(),a.getY());
	}else{
		if(d>magnitudeab){
			return Vec(b.getX(),b.getY());
		}else{
			return a+(ab*d);
		}
	}
}

bool intersects(const Vec& p1,const Vec& p2,const Vec& p3,const Vec& p4){
	if(p1==p3) return true;
	if(p2==p4) return true;
	double s1x = (p2.getX()-p1.getX()), s1y = (p2.getY()-p1.getY());
	double s2x = (p4.getX()-p3.getX()), s2y = (p4.getY()-p3.getY());

	double d = (-s2x*s1y) + (s1x*s2y);
	if (d == 0) return false;

	double s = (-s1y*(p1.getX()-p3.getX()) + s1x*(p1.getY()-p3.getY())) / d;
	double t = (s2x*(p1.getY()-p3.getY()) - s2y*(p1.getX()-p3.getX())) / d;

	if( s>=0.0 && s<=1.0 && t>=0.0 && t<=1.0){
		return true;
	}
	return false;
}

bool intersects(const Vec& p1,const Vec& p2,const Vec& p3,const Vec& p4, Vec& ip){
	if(p1==p3){
		ip = p1;
		return true;
	}
	if(p2==p4){
		ip = p2;
		return true;
	}
	double s1x = (p2.getX()-p1.getX()), s1y = (p2.getY()-p1.getY());
	double s2x = (p4.getX()-p3.getX()), s2y = (p4.getY()-p3.getY());

	double d = (-s2x*s1y) + (s1x*s2y);
	if (d == 0) return false;

	double s = (-s1y*(p1.getX()-p3.getX()) + s1x*(p1.getY()-p3.getY())) / d;
	double t = (s2x*(p1.getY()-p3.getY()) - s2y*(p1.getX()-p3.getX())) / d;

	if( s>=0.0 && s<=1.0 && t>=0.0 && t<=1.0){
        ip.setX( p1.getX() + (t*s1x) );
        ip.setY( p1.getY() + (t*s1y) );
        ip.setZ( 0.0 );
		return true;
	}
	return false;
}

}

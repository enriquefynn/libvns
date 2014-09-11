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

#ifndef VNS_MATHS_H_
#define VNS_MATHS_H_

#include "core.h"
#include "lock.h"
#include <cmath>
#include <cstdlib>

#define vns_MIN(a,b) ((a)<(b)?(a):(b))
#define vns_MAX(a,b) ((a)>(b)?(a):(b))

namespace vns {

class Vec;
class BBox;

void setRandomSeed(unsigned int* seed);
int randomInt();
unsigned int getRandomSeed();

inline double randomValue(double lo, double hi) {
	return lo+(hi-lo)*(randomInt()/((double)RAND_MAX));
}

inline float randomValue(float lo, float hi) {
	return lo+(hi-lo)*(randomInt()/((float)RAND_MAX));
}

inline double fpow(double a, double b) {
  union {
    double d;
    int x[2];
  } u = { a };
  u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
}

// Quake 3
inline float fsqrt(const float x) {
	const float xhalf = 0.5f*x;
	union { float x; int i; } u;
	u.x = x;
	u.i = 0x5f3759df - (u.i >> 1);
	return x*u.x*(1.5f - xhalf*u.x*u.x);
}

inline double fsqrt(const double x) {
	const double xhalf = 0.5f*x;
	union { double x; long i; } u;
	u.x = x;
	u.i = 0x5fe6ec85e7de30da - (u.i >> 1);
	return x*u.x*(1.5F - xhalf*u.x*u.x);
}

inline float isqrt(const float x) {
	const float xhalf = 0.5f*x;
	union { float x; int i; } u;
	u.x = x;
	u.i = 0x5f3759df - (u.i >> 1);
	return u.x*(1.5f - xhalf*u.x*u.x);
}

inline double isqrt(const double x) {
	const double xhalf = 0.5f*x;
	union { double x; long i; } u;
	u.x = x;
	u.i = 0x5fe6ec85e7de30da - (u.i >> 1);
	return u.x*(1.5F - xhalf*u.x*u.x);
}

inline double round(double c,int p){
	int u = int(c);
	double d = c-u;
	return double(u)+(int(d*::pow(10,p))/double(::pow(10,p)));
}

inline float round(float c,int p){
	int u = int(c);
	float d = c-u;
	return float(u)+(int(d*::pow(10,p))/float(::pow(10,p)));
}


inline double toRadians(double deg){
	return deg * (M_PI/180.0);
}

inline double toDegrees(double rad){
	return rad * (180.0/M_PI);
}

bool intersects(const Vec& p1,const Vec& p2,const Vec& p3,const Vec& p4);
bool intersects(const Vec& p1,const Vec& p2,const Vec& p3,const Vec& p4, Vec& ip);
Vec closestPoint(const Vec& p, const Vec& a,const Vec& b);

}

#endif /* VNS_MATHS_H_ */

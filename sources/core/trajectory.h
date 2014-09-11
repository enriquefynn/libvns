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

#ifndef VNS_TRAJECTORY_H_
#define VNS_TRAJECTORY_H_

#include "vec.h"

namespace vns {

/*! \cond PRIVATE */

class Trajectory {
public:
    Trajectory(){};

    Trajectory(const Vec& from, const Vec& fromDir, const Vec& to, const Vec& toDir) {
    	create(from, fromDir, to, toDir);
    };

    inline void create(const Vec& from, const Vec& fromDir, const Vec& to, const Vec& toDir){
        a = from;
        computeCoefs(fromDir, to, toDir, 1.0*(from-to).norm());
        computeLen(5);
        computeCoefs(fromDir, to, toDir, len);
        //computeLen(5);

    };

    inline double length() { return len; };
    inline void interpolate(double p, Vec& pos, Vec& dir){
    	pos = a + p*(b + p*(c + p*d));
    	dir = b + p*(2*c + (3*p)*d);
    };

private:
    Vec a;
    Vec b;
    Vec c;
    Vec d;
    double len;

    void computeLen(int k) {
        double step = 1.0/k;
        int i;
        Vec p, p0, dp;
        interpolate(0.0,p0,dp);
        len = 0;
        for(i=1; i<=k; ++i) {
            interpolate(i*step, p, dp);
            len += (p-p0).norm();
            p0 = p;
        }
    };

    inline void computeCoefs(const Vec& fromDir, const Vec& to, const Vec& toDir, double l) {
        b = l*fromDir.normalized();
        c = l*toDir.normalized();
        d = to - a - b;
        c = 3*d -(c-b);
        d = d - c;
    };

};

/*! \endcond */

}

#endif /* TRAJECTORY_H_ */

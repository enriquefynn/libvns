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

#ifndef SRTM_H_
#define SRTM_H_

#include "core.h"
#include "vec.h"
#include "roadnetwork.h"

namespace vns {

class SRTM {
public:
	SRTM(const char* filename);
	virtual ~SRTM();

	float getZ(const vns::Vec& p);

	inline int width(){ return w; };
	inline int height(){ return h; };
	inline double dx(){ return xres; };
	inline double dy(){ return yres; };
	inline double xMin(){ return xmin; };
	inline double yMin(){ return ymin; };
	inline double xMax(){ return xmax; };
	inline double yMax(){ return ymax; };
	bool contains(const vns::Vec& p) const;
	inline float get(int row,int col){ return data[row*h+col]; };
	vns::Vec getNormal(int row,int col);
	inline vns::Vec getPoint(int row,int col){ return vns::Vec( xmin+xres*col, ymin+yres*row,data[row*h+col]); };

	void applyRoadNetwork(vns::RoadNetwork* network);
private:
	double xmin;
	double ymin;
	double xmax;
	double ymax;
	int32 h,w;
	double xres;
	double yres;
	float* data;
};

}

#endif /* SRTM_H_ */

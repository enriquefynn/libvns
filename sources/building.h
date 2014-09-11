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

#ifndef VNS_BUILDING_H_
#define VNS_BUILDING_H_

#include "polygon.h"

namespace vns {

/*! \cond PRIVATE */

class Building : public Polygon {
private:
	int8 type;
public:
	Building(int8 typ=-1):Polygon(){ type=typ; };
	Building(int32 n, double* xs, double* ys, double* zs,int8 typ=-1):Polygon(n,xs,ys,zs){ type=typ; };
	Building(const vns::Vector<vns::Vec>& pv, int8 typ=-1):Polygon(pv){ type=typ; };
	virtual ~Building(){};

	inline int8 getBuildingType(){ return type; };
	inline void setBuildingType(int8 t){ type=t; };
};

/*! \endcond */

}

#endif /* BUILDING_H_ */

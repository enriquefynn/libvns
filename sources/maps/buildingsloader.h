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

#ifndef VNS_BUILDINGSLOADER_H_
#define VNS_BUILDINGSLOADER_H_

#include "vec.h"
#include "buildings.h"

namespace vns {

/*! \cond PRIVATE */

class BuildingsLoader {

public:

	static vns::Buildings* fromShapefile(const char* filename,const vns::Vec& offset=vns::Vec(0,0,0));
};

/*! \endcond */

}
#endif

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

#ifndef VNS_SUMOROUTESFILE_H_
#define VNS_SUMOROUTESFILE_H_

#include "routesfile.h"

namespace vns {

class SUMORoutesFile : public RoutesFile {
public:
	SUMORoutesFile();
	virtual ~SUMORoutesFile();
	const char* getFileType() { return "SUMO Routes (XML)"; };
	const char* getFileExtension() { return "SUMO Routes (*.xml)"; };
	bool load(Routes* routes, const char* fileName);
	bool save(const Routes* routes, const char* fileName);
};

}

#endif

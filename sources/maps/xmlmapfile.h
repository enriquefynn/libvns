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

#ifndef VNS_XMLMAPFILE_H_
#define VNS_XMLMAPFILE_H_

#include "mapfile.h"

namespace vns {

class XmlMapFile : public MapFile {
public:
	XmlMapFile();
	virtual ~XmlMapFile();
	const char* getFileType() const { return "VNS (XML)"; };
	const char* getFileExtension() const { return "VNS XML (*.xml)"; };
	bool load(vns::RoadNetwork* network, const char* fileName);
	bool save(const vns::RoadNetwork* network, const char* fileName);

};

}

#endif

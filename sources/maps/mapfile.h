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

#ifndef VNS_MAPFILE_H_
#define VNS_MAPFILE_H_

namespace vns {

class RoadNetwork;

class MapFile {
public:
	MapFile(){};
	virtual inline ~MapFile(){};
	virtual const char* getFileType() const { return "VNS Binary (RNW)"; };
	virtual const char* getFileExtension() const { return "VNS Binary (*.rnw)"; };
	virtual bool load(RoadNetwork* network, const char* fileName);
	virtual bool save(const RoadNetwork* network, const char* fileName);
	RoadNetwork* createRoadNetwork(const char* fileName);
};

}

#endif /* MAPFILE_H_ */

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

#ifndef VNS_TRACKSFILE_H_
#define VNS_TRACKSFILE_H_

namespace vns {

class Tracker;
class RoadNetwork;

class TracksFile {
public:
	TracksFile(){};
	virtual inline ~TracksFile(){};
	virtual const char* getFileType() { return "VNS Tracks File (trk)"; };
	virtual const char* getFileExtension() { return "VNS Tracks File (*.trk)"; };
	virtual bool save(const vns::Tracker* tracker, const RoadNetwork* network, const char* fileName) = 0;
};

}

#endif /* MAPFILE_H_ */

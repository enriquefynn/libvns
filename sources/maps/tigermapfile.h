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

#ifndef VNS_TIGERMAPFILE_H_
#define VNS_TIGERMAPFILE_H_

#include "shpmapfile.h"
#include "hashtable.h"
#include "list.h"

class TigerRoad;

namespace vns {


class TigerMapFile : public ShpMapFile {
public:
	TigerMapFile();
	virtual ~TigerMapFile();
	const char* getFileType() const { return "Tiger 2012 (SHP)"; };
	const char* getFileExtension() const { return "Tiger2012 Shapefile (*.shp)"; };
	bool save(const vns::RoadNetwork* network, const char* fileName);

protected:

	void initializeLoad(vns::RoadNetwork* network);
	void finalizeLoad(vns::RoadNetwork* network);
	bool hasValidNumberOfFields(int numberOfFields );
	bool loadRoad(DBFHandle& dbfHandle, int id, vns::RoadNetwork* network, vns::PolyLine* line);

private:
	vns::HashTable< vns::Vec, int > pointsRoads;
	vns::List< TigerRoad* > tigerRoads;

};

}

#endif

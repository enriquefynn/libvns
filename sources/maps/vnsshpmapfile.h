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

#ifndef VNS_VNSSHPMAPFILE_H_
#define VNS_VNSSHPMAPFILE_H_

#include "shpmapfile.h"
#include <string>
#include <map>
#include <vector>

namespace vns {

class VNSShpMapFile : public ShpMapFile {
public:
	VNSShpMapFile();
	virtual ~VNSShpMapFile();
	const char* getFileType() const { return "VNS (SHP)"; };
	const char* getFileExtension() const { return "VNS Shapefile (*.shp)"; };

protected:

	virtual bool hasValidNumberOfFields(int numberOfFields );
	virtual void initializeLoad(vns::RoadNetwork* network);
	virtual void finalizeLoad(vns::RoadNetwork* network);
	virtual bool loadRoad(DBFHandle& dbfHandle, int id, vns::RoadNetwork* network, vns::PolyLine* line);

	void createDBFFields(DBFHandle& dbf);
	void saveDBFRoad(DBFHandle& dbf,int id,const vns::Road* road);

private:
	std::map< int, std::string > roadConnections;

	std::vector<int> getFlagsList(const char* text);
	std::vector< std::vector<int> > getConnectionsList(const char* text,unsigned int nlanes);
	std::vector<bool> getBoolList(const char* text);
	std::vector<double> getDoubleList(const char* text);

};

}

#endif

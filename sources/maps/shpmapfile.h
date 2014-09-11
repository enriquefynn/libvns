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

#ifndef VNS_SHPMAPFILE_H_
#define VNS_SHPMAPFILE_H_

#include "shapefil.h"
#include "mapfile.h"

namespace vns {

class Vec;
class BBox;
class Road;
class PolyLine;

class ShpMapFile : public MapFile {
public:
	ShpMapFile();
	virtual ~ShpMapFile();
	virtual const char* getFileType() const { return "Shapefile (SHP)"; };
	virtual const char* getFileExtension() const { return "Shapefile (*.shp)"; };
	virtual bool load(vns::RoadNetwork* network, const char* fileName);
	virtual bool save(const vns::RoadNetwork* network, const char* fileName);

protected:

	virtual bool hasValidNumberOfFields(int numberOfFields );
	virtual vns::BBox getExtents(double xmin, double ymin, double xmax, double ymax);
	virtual void initializeLoad(vns::RoadNetwork* network);
	virtual void finalizeLoad(vns::RoadNetwork* network);
	virtual vns::Vec loadPoint(double x,double y);
	virtual vns::Vec savePoint(double x,double y);
	virtual bool loadRoad(DBFHandle& dbfHandle, int id, vns::RoadNetwork* network, vns::PolyLine* line);

	virtual void createDBFFields(DBFHandle& dbf);
	virtual void saveDBFRoad(DBFHandle& dbf,int id,const vns::Road* road);

};

}

#endif

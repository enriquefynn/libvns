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

#ifndef VNS_INFOPTMAPFILE_H_
#define VNS_INFOPTMAPFILE_H_

#include "shpmapfile.h"

namespace vns {

/*! \cond PRIVATE */

class InfoPTMapFile : public ShpMapFile {
public:
	InfoPTMapFile();
	virtual ~InfoPTMapFile();
	const char* getFileType() const { return "InfoPortugal (SHP)"; };
	const char* getFileExtension() const { return "InfoPortugal Shapefile (*.shp)"; };
protected:

	bool hasValidNumberOfFields(int numberOfFields );
	bool loadRoad(DBFHandle& dbfHandle, int id, vns::RoadNetwork* network, vns::PolyLine* line);

	void createDBFFields(DBFHandle& dbf);
	void saveDBFRoad(DBFHandle& dbf,int id,const vns::Road* road);

};

/*! \endcond */

}

#endif

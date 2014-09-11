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

#ifndef VNS_KMLTRACKSFILE_H_
#define VNS_KMLTRACKSFILE_H_

#include "tracksfile.h"

namespace vns {

class KMLTracksFile : public TracksFile {
private:
	const char* modelFile;
	const char* iconFile;
	const char* lineColor;
	int lineWidth;
	float modelHeading;
public:
	KMLTracksFile();
	virtual ~KMLTracksFile();
	const char* getFileType() { return "KML Tracks File (kml)"; };
	const char* getFileExtension() { return "KML Tracks File (*.kml)"; };
	inline void setModelFile(const char* file){ modelFile = file; };
	inline void setModelHeading(float heading){ modelHeading = heading; };
	inline void setIconFile(const char* file){ iconFile = file; };
	inline void setTrackColor(const char* color){ lineColor = color; };
	inline void setTrackWidth(int width){ lineWidth=width; };
	bool save(const vns::Tracker* tracker, const RoadNetwork* network, const char* fileName);
};

}

#endif

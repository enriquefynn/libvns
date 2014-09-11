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

#ifndef VNS_XMLROUTESFILE_H_
#define VNS_XMLROUTESFILE_H_

#include "routesfile.h"

namespace vns {

class XmlRoutesFile : public RoutesFile {
public:
	XmlRoutesFile();
	virtual ~XmlRoutesFile();
	const char* getFileType() { return "VNS Routes (XML)"; };
	const char* getFileExtension() { return "VNS Routes (*.xml)"; };
	bool load(Routes* routes, const char* fileName);
	bool save(const Routes* routes, const char* fileName);

};

}

#endif

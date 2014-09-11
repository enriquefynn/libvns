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

#ifndef VNS_H_
#define VNS_H_

#include "core.h"
#include "datastream.h"
#include "list.h"
#include "vector.h"
#include "hashtable.h"
#include "mapgrid.h"
#include "vec.h"
#include "bbox.h"
#include "polyline.h"
#include "polygon.h"
#include "building.h"
#include "buildings.h"
#include "trigger.h"
#include "vehicle.h"
#include "taxi.h"
#include "vehiclefactory.h"
#include "modulesfactory.h"
#include "travelstatsmodule.h"
#include "roadnetwork.h"
#include "mapfile.h"
#include "manhattanmap.h"
#include "mapfilters.h"

#include "pathstop.h"
#include "pathnode.h"
#include "path.h"
#include "routingtable.h"
#include "membasedroutingtable.h"
#include "pathfinderhandler.h"
#include "pathfinder.h"
#include "astarhandler.h"
#include "dijkstrahandler.h"
#include "tablehandler.h"
#include "route.h"
#include "scheduledroute.h"
#include "routes.h"
#include "routesfile.h"
#include "trafficlightcontroller.h"
#include "fixedtimetrafficlightcontroller.h"
#include "pretimedtrafficlightcontroller.h"
#include "trafficlightsgenerator.h"
#include "simplenetworkmodule.h"

#include "generationfunction.h"
#include "trafficgenerationmodel.h"
#include "observer.h"
#include "simulator.h"
#include "parallelsimulator.h"

#include "shpmapfile.h"
#include "vnsshpmapfile.h"
#include "tigermapfile.h"
#include "buildingsloader.h"

#include "routesgenerator.h"

#include "sumomapfile.h"
#include "osmmapfile.h"
#include "xmlmapfile.h"
#include "xmlroutesfile.h"
#include "sumoroutesfile.h"

#include "inputeventhandler.h"
#include "panhandler.h"
#include "visualizer.h"

#endif /* VNS_H_ */

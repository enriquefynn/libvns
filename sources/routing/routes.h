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

#ifndef VNS_ROUTES_H
#define VNS_ROUTES_H

#include "datastream.h"
#include "roadnetwork.h"
#include "route.h"
#include "scheduledroute.h"
#include "driverinfo.h"
#include "vector.h"

namespace vns {

class Routes {

private:
	Vector<DriverInfo*> defaultDrivers;
	Vector<DriverInfo*> drivers;
	Vector<Path*> paths;
    Vector<Route*> routes;
    Vector<ScheduledRoute*> scheduledRoutes;
    RoadNetwork* network;

    friend DataStream& operator <<(DataStream&f, const Routes* routes);
    friend DataStream& operator >>(DataStream& f, Routes* routes);
    friend class Path;

public:

    Routes(RoadNetwork* network);
    Routes(RoadNetwork* network,const char* fileName);
    ~Routes();

    int32 registerDriver(const DriverInfo& driver);
    inline const DriverInfo* getDriver(int32 driverID) const { return drivers.at(driverID); };
    inline const DriverInfo* getDefaultDriver(VehicleType vehicleID) const { return defaultDrivers.at(vehicleID); };

    int32 addPath(Path* path);
    inline int32 getNumberOfPaths() const { return paths.size(); };
    inline Path* getPath(int32 pathID){ return paths.at(pathID); };
    inline const Path* getPath(int32 pathID) const { return paths.at(pathID); };

    inline Route* getRoute(int32 routeID){ return routes.at(routeID); };
    inline const Route* getRoute(int32 routeID) const { return routes.at(routeID); };
    int32 addRoute(int32 pathID, double interval,VehicleType vType=vns::CAR,int32 driverID=-1,double startTime=0.0,double endTime=0.0);
    void deleteRoute(int32 routeID);
    inline int32 getNumberOfRoutes() const { return routes.size(); };
    void clearRoutes();

    int32 addScheduledRoute(int32 pathID,double depart,VehicleType vType=vns::CAR,int32 driverID=-1);
    void deleteScheduledRoute(int32 routeID);
    int32 getNumberOfScheduledRoutes() const { return scheduledRoutes.size(); };
    ScheduledRoute* getScheduledRoute(int32 routeID){ return scheduledRoutes.at(routeID); };
    const ScheduledRoute* getScheduledRoute(int32 routeID) const { return scheduledRoutes.at(routeID); };
    void clearScheduledRoutes();
    void clear();

    bool load(const char* fileName);
    bool save(const char* fileName);

    inline RoadNetwork* getRoadNetwork() { return network; };
    inline const RoadNetwork* getRoadNetwork() const { return network; };

};

}

#endif

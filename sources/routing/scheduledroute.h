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

#ifndef VNS_SCHEDULEDROUTE_H_
#define VNS_SCHEDULEDROUTE_H_

#include "datastream.h"
#include "core.h"
#include "maths.h"
#include "path.h"
#include "vector.h"


namespace vns {

class Routes;

class ScheduledRoute {
private:
	VehicleType vehicleType;
	double scheduledTime;
    Path* path;
    int32 driver;

	ScheduledRoute(Path* path, double scheduledTime,VehicleType vType=0,int32 driverID=0);
	~ScheduledRoute();

	static vns::ScheduledRoute* read(DataStream& f, Routes* routes);
	static void write(DataStream& f, ScheduledRoute* route);
    friend DataStream& operator <<(DataStream&f, const Routes* routes);
    friend DataStream& operator >>(DataStream& f, Routes* routes);
    friend class Routes;

public:

	inline double getScheduledTime() const { return scheduledTime; };
	inline Path* getPath(){ return path; };
	inline const Path* getPath() const { return path; };
    inline VehicleType getVehicleType() const { return vehicleType; };
    inline void setVehicleType(VehicleType vType){ vehicleType = vType; };
    inline int32 getDriver() const { return driver; };
    inline void setDriver(int32 driverID){ driver = driverID; };
};

}

#endif /* SCHEDULEDROUTE_H_ */

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

#ifndef VNS_ROUTE_H_
#define VNS_ROUTE_H_

#include "datastream.h"
#include "core.h"
#include "path.h"
#include "vector.h"

namespace vns {

class Routes;

class Route {
private:
	VehicleType vehicleType;
	int32 driver;
    Path* path;
    double period;
    double startTime;
    double endTime;
    double t; // simulation

    Route(Path* path, double period,VehicleType vehicleType,int32 driverID, double startTime,double endTime);
	~Route();

	static void write(DataStream& f, const Route* route);
	static vns::Route* read(DataStream& f, Routes* routes);
	friend class Routes;
    friend DataStream& operator <<(DataStream& f, const Routes* routes);
    friend DataStream& operator >>(DataStream& f, Routes* routes);

public:

    inline void setT(double nt){ t = nt; };
    inline double getT() const { return t; };

	inline Path* getPath(){ return path; };
    inline const Path* getPath() const { return path; };
    inline VehicleType getVehicleType() const { return vehicleType; };
    inline void setVehicleType(VehicleType vType){ vehicleType = vType; };
    inline double getInterval() const { return period; };
    inline double getStartTime() const { return startTime; };
    inline double getEndTime() const { return endTime; };
    inline void setStartTime(double tt){ startTime = tt; };
    inline void setEndTime(double tt){ endTime = tt; };
    inline void setInterval(double interval){  period = (interval<=0.0?0.05:interval); };
    inline int32 getDriver() const { return driver; };
    inline void setDriver(int32 driverID){ driver = driverID; };

    bool operator<(const Route& rt) const { return t < rt.t; };
    bool operator<(const Route* rt) const { return t < rt->t; };
};

}

#endif

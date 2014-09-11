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

#ifndef VNS_TAXI_H_
#define VNS_TAXI_H_

#include "drivermodel.h"
#include "vehicle.h"

namespace vns {

class Taxi : public Vehicle {

public:
	enum TaxiState { Busy, Free, Picking, Stand, Pause };

	Taxi(uint64 id,VehicleModel model,const Path::Iterator& path,float width, float length,DriverModel* driver);
	virtual ~Taxi();
	inline void setServiceState(Taxi::TaxiState st){ taxiState = st; };
	inline Taxi::TaxiState getServiceState() const { return taxiState; };

private:

	Taxi::TaxiState taxiState;

};

}

#endif /* TAXI_H_ */

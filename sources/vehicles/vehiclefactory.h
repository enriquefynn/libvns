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

#ifndef VNS_VEHICLEFACTORY_H_
#define VNS_VEHICLEFACTORY_H_


#include "vehicle.h"
#include "taxi.h"
#include "train.h"
#include "vector.h"

namespace vns {

class DriverModel;
class DriverInfo;
class Simulator;

class VehicleFactory {
private:
	class VehicleInfo {
	public:
		VehicleInfo(){};
		VehicleInfo(float w,float h):w(w),h(h){};
	public:
		float w;
		float h;
		friend class VehicleFactory;
	};
	Vector< VehicleInfo > vehicles;
	Vehicle* newVehicle(uint64 id,VehicleType vehicleType,const DriverInfo* driver,const Path::Iterator& path);

protected:

	virtual Vehicle* createVehicle(uint64 id,VehicleType type,VehicleModel model,const Path::Iterator& path,float width, float length,DriverModel* driver);
public:

	VehicleFactory();
	virtual ~VehicleFactory();

	static int32 getNumberOfVehicleTypes();
	static const char* fromVehicleType(VehicleType type);
	static VehicleType fromVehicleTypeName(const char* typeName);

private:
	void registerVehicle(VehicleType type, const VehicleInfo& vinfo);
	friend class Simulator;
};

}

#endif

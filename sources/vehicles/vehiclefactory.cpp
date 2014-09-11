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

#include "vehiclefactory.h"
#include "drivers.h"
#include "drivermodel.h"
#include <cstring>

#define VEHICLE_TYPES 6
#define VEHICLE_MODELS 20

namespace vns {

VehicleFactory::VehicleFactory(){
	vehicles.resize(VEHICLE_TYPES);

	registerVehicle(vns::CAR, VehicleInfo(2.0,4.7) ); // 0

	// BIKES
	registerVehicle(vns::BIKE, VehicleInfo(1.0,3) ); // 0

	// TAXIS
	registerVehicle(vns::TAXI, VehicleInfo(2.0,4.7) ); // 0

	// BUS
	registerVehicle(vns::BUS, VehicleInfo(2.2,7.0) ); // 0

	// TRUCK
	registerVehicle(vns::TRUCK, VehicleInfo(2.6,8.0) ); // 0

	// TRAIN
	registerVehicle(vns::TRAIN, VehicleInfo(2.5,12.0) ); // 0
}

VehicleFactory::~VehicleFactory(){

}

Vehicle* VehicleFactory::createVehicle(uint64 id,VehicleType type,VehicleModel model,const Path::Iterator& path,float w, float len,DriverModel* driver){
	switch(type){
	case vns::CAR: return new Vehicle(id,vns::CAR,model,path,w,len,driver);
	case vns::BIKE: return new Vehicle(id,vns::BIKE,model,path,w,len,driver);
	case vns::TAXI: return new Taxi(id,model,path,w,len,driver);
	case vns::BUS: return new Vehicle(id,vns::BUS,model,path,w,len,driver);
	case vns::TRUCK: return new Vehicle(id,vns::TRUCK,model,path,w,len,driver);
	case vns::TRAIN:return new Train(id,vns::TRAIN,model,path,w,len,driver);
	}
	return 0;
}

Vehicle* VehicleFactory::newVehicle(uint64 id,VehicleType vehicleType, const DriverInfo* driver, const Path::Iterator& path){
	if( vehicleType<0 || vehicleType>=vehicles.size() ){ // Random
		vehicleType = vns::randomInt() % vehicles.size();
	}
	int32 vehicleModel = vns::randomInt() % VEHICLE_MODELS;
	VehicleInfo& info = vehicles[vehicleType];
	DriverModel* driverModel = vns::DriverModel::newDriverModel(driver);
	return createVehicle(id,vehicleType,vehicleModel,path,info.w,info.h,driverModel);
};

void VehicleFactory::registerVehicle(VehicleType type, const VehicleInfo& vinfo){
	if( type >=0 && type < vehicles.size() ){
		vehicles[type]= vinfo;
	}
}

int32 VehicleFactory::getNumberOfVehicleTypes(){
	return VEHICLE_TYPES;
}

const char* VehicleFactory::fromVehicleType(VehicleType type){
	switch(type){
	case vns::CAR: return "car";
	case vns::BIKE: return "bike";
	case vns::TAXI: return "taxi";
	case vns::BUS: return "bus";
	case vns::TRUCK: return "truck";
	case vns::TRAIN: return "train";
	}
	return "";
}

VehicleType VehicleFactory::fromVehicleTypeName(const char* typeName){
	if( strcmp(typeName,"car") == 0 ) {
		return vns::CAR;
	} else if( strcmp(typeName,"bike") == 0 ) {
		return vns::BIKE;
	} else if( strcmp(typeName,"taxi") == 0 ) {
		return vns::TAXI;
	} else if( strcmp(typeName,"bus") == 0 ) {
		return vns::BUS;
	} else if( strcmp(typeName,"truck") == 0 ) {
		return vns::TRUCK;
	} else if( strcmp(typeName,"train") == 0 ) {
		return vns::TRAIN;
	}
	return -1;
}

}

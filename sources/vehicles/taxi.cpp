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

#include "taxi.h"

namespace vns {

Taxi::Taxi(uint64 id,VehicleModel model,const Path::Iterator& path,float width, float length,DriverModel* driver)
	: Vehicle(id,vns::TAXI,model,path,width,length,driver){
	taxiState = Taxi::Stand;
}

Taxi::~Taxi() {

}

}

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

#ifndef VNS_DRIVERS_H_
#define VNS_DRIVERS_H_

#include "driverinfo.h"
#include "idm.h"
#include "gipps.h"
#include "krauss.h"
#include "newell.h"
#include "ovm.h"
#include "fvdm.h"
#include "kkw.h"
#include "eidm.h"
#include "nagel.h"
#include "pipes.h"

namespace vns {

/*! \cond PRIVATE */

class CarDriver : public DriverInfo {
public:
	CarDriver();
};

class BikeDriver : public DriverInfo {
public:
	BikeDriver();
};

class BusDriver : public DriverInfo {
public:
	BusDriver();
};

class TaxiDriver : public DriverInfo {
public:
	TaxiDriver();
};

class TruckDriver : public DriverInfo {
public:
	TruckDriver();
};

class TrainDriver : public DriverInfo {
public:
	TrainDriver();
};

/*! \endcond */

}

#endif /* VNS_DRIVERS_H_ */

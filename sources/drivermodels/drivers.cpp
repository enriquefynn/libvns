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

#include "drivers.h"

namespace vns {

CarDriver::CarDriver(){
	model = vns::DriverModel::IDM;
	T_min = 1.0;	T_max = 1.5;
	v0_min = 25.0; v0_max = 33.3;
	s0_min = 1.5; s0_max = 2.0;
	s1_min = 5.0; s1_max = 5.0;
	a_min = 4.0;	a_max = 5.0;
	b_min = 2.0;	b_max = 3.0;

	politiness_min = 0.1; politiness_max = 0.1;
	deltab_min = 0.3; deltab_max = 0.3;
	bsave_min = 4.0; bsave_max = 5.0;
	athr_min = 0.2; athr_max = 0.2;

	/* Memory */
	mtau_min = mtau_max = 600;
	malphaA_min = malphaA_max = 1.0;
	malphaV0_min = malphaV0_max = 1.0;
	malphaT_min = malphaT_max = 1.7;
}

BikeDriver::BikeDriver(){
	model = vns::DriverModel::IDM;
	T_min = 0.5;	T_max = 1.5;
	v0_min = 25.0; v0_max = 33.3;
	s0_min = 0.5; s0_max = 2.0;
	s1_min = 0.0; s1_max = 0.0;
	a_min = 1.0;	a_max = 2.0;
	b_min = 1.0;	b_max = 2.0;
	politiness_min = 0.0; politiness_max = 0.5;
	deltab_min = a_min; deltab_max = 0.5;
	bsave_min = 2.0; bsave_max = 5.0;
	athr_min = 0.2; athr_max = 0.5;

	/* Memory */
	mtau_min = mtau_max = 600;
	malphaA_min = malphaA_max = 1.0;
	malphaV0_min = malphaV0_max = 1.0;
	malphaT_min = malphaT_max = 1.7;
}

BusDriver::BusDriver(){
	model = vns::DriverModel::IDM;
	T_min = 1.2;	T_max = 2.0;
	v0_min = 10.0; v0_max = 20.3;
	s0_min = 1.0; s0_max = 2.5;
	s1_min = 0.0; s1_max = 0.0;
	a_min = 0.5;	a_max = 1.5;
	b_min = 1.0;	b_max = 2.0;
	politiness_min = 0.0; politiness_max = 0.5;
	deltab_min = a_min; deltab_max = 0.5;
	bsave_min = 2.0; bsave_max = 5.0;
	athr_min = 0.2; athr_max = 0.5;

	/* Memory */
	mtau_min = mtau_max = 600;
	malphaA_min = malphaA_max = 1.0;
	malphaV0_min = malphaV0_max = 1.0;
	malphaT_min = malphaT_max = 1.7;
}

TaxiDriver::TaxiDriver(){
	model = vns::DriverModel::IDM;
	T_min = 0.8;	T_max = 1.5;
	v0_min = 20.0; v0_max = 33.3;
	s0_min = 0.5; s0_max = 2.0;
	s1_min = 0.0; s1_max = 0.0;
	a_min = 0.5;	a_max = 2.0;
	b_min = 1.0;	b_max = 3.0;
	politiness_min = 0.0; politiness_max = 0.5;
	deltab_min = a_min; deltab_max = 0.5;
	bsave_min = 2.0; bsave_max = 5.0;
	athr_min = 0.2; athr_max = 0.5;

	/* Memory */
	mtau_min = mtau_max = 600;
	malphaA_min = malphaA_max = 1.0;
	malphaV0_min = malphaV0_max = 1.0;
	malphaT_min = malphaT_max = 1.7;
};

TruckDriver::TruckDriver(){
	model = vns::DriverModel::IDM;
	T_min = 1.2;	T_max = 2.0;
	v0_min = 10.0; v0_max = 20.3;
	s0_min = 1.0; s0_max = 2.5;
	s1_min = 0.0; s1_max = 0.0;
	a_min = 0.5;	a_max = 1.5;
	b_min = 1.0;	b_max = 2.0;
	politiness_min = 0.0; politiness_max = 0.5;
	deltab_min = a_min; deltab_max = 0.5;
	bsave_min = 2.0; bsave_max = 5.0;
	athr_min = 0.2; athr_max = 0.5;

	/* Memory */
	mtau_min = mtau_max = 600;
	malphaA_min = malphaA_max = 1.0;
	malphaV0_min = malphaV0_max = 1.0;
	malphaT_min = malphaT_max = 1.7;
}

TrainDriver::TrainDriver(){
	model = vns::DriverModel::IDM;
	T_min = 1.2;	T_max = 2.0;
	v0_min = 10.0; v0_max = 20.3;
	s0_min = 1.0; s0_max = 2.5;
	s1_min = 0.0; s1_max = 0.0;
	a_min = 0.5;	a_max = 1.5;
	b_min = 1.0;	b_max = 2.0;
	politiness_min = 0.0; politiness_max = 0.5;
	deltab_min = a_min; deltab_max = 0.5;
	bsave_min = 2.0; bsave_max = 5.0;
	athr_min = 0.2; athr_max = 0.5;

	/* Memory */
	mtau_min = mtau_max = 600;
	malphaA_min = malphaA_max = 1.0;
	malphaV0_min = malphaV0_max = 1.0;
	malphaT_min = malphaT_max = 1.7;
}


}

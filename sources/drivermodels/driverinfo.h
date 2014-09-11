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

#ifndef VNS_DRIVERINFO_H_
#define VNS_DRIVERINFO_H_

namespace vns {

class DataStream;

class DriverInfo {
public:
	DriverInfo();
	DriverInfo(const DriverInfo& driver);
	virtual ~DriverInfo();

public:
	int model;
	float T_min;
	float T_max;
	float a_min;
	float a_max;
	float b_min;
	float b_max;
	float v0_min;
	float v0_max;
	float s0_min;
	float s0_max;
	float s1_min;
	float s1_max;
	float delta_min;
	float delta_max;
	float coolness_min;
	float coolness_max;

	float beta_min;
	float beta_max;
	float lambda_min;
	float lambda_max;
	float tau_min;
	float tau_max;
	float tWidth_min;
	float tWidth_max;

	//KKW
	float k_min;
	float k_max;
	float pb0_min;
	float pb0_max;
	float pb1_min;
	float pb1_max;
	float pa1_min;
	float pa1_max;
	float pa2_min;
	float pa2_max;
	float vp_min;
	float vp_max;

	//Nagel
	float pSlowDown_min;
	float pSlowDown_max;
	float pSlowToStart_min;
	float pSlowToStart_max;

	// Krauss
	float epsilon_min;
	float epsilon_max;

	// Memory parameters
	float mtau_min;
	float mtau_max;
	float malphaV0_min;
	float malphaV0_max;
	float malphaT_min;
	float malphaT_max;
	float malphaA_min;
	float malphaA_max;

	// MOBIL parameters - Lane Change
	float athr_min;
	float athr_max;
	float politiness_min;
	float politiness_max;
	float deltab_min;
	float deltab_max;
	float bsave_min;
	float bsave_max;

	static void write(DataStream& f, const DriverInfo* driver);
	static DriverInfo* read(DataStream& f);
};



}

#endif /* VNS_DRIVERINFO_H_ */

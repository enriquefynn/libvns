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

#include "driverinfo.h"
#include "drivermodel.h"
#include "datastream.h"

namespace vns {

void DriverInfo::write(DataStream&f, const DriverInfo* d) {
	f << d->model;
	f << d->T_min;
	f << d->T_max;
	f << d->a_min;
	f << d->a_max;
	f << d->b_min;
	f << d->b_max;
	f << d->v0_min;
	f << d->v0_max;
	f << d->s0_min;
	f << d->s0_max;
	f << d->s1_min;
	f << d->s1_max;
	f << d->delta_min;
	f << d->delta_max;
	f << d->coolness_min;
	f << d->coolness_max;

	f << d->beta_min;
	f << d->beta_max;
	f << d->lambda_min;
	f << d->lambda_max;
	f << d->tau_min;
	f << d->tau_max;
	f << d->tWidth_min;
	f << d->tWidth_max;

	//KKW
	f << d->k_min;
	f << d->k_max;
	f << d->pb0_min;
	f << d->pb0_max;
	f << d->pb1_min;
	f << d->pb1_max;
	f << d->pa1_min;
	f << d->pa1_max;
	f << d->pa2_min;
	f << d->pa2_max;
	f << d->vp_min;
	f << d->vp_max;

	//Nagel
	f << d->pSlowDown_min;
	f << d->pSlowDown_max;
	f << d->pSlowToStart_min;
	f << d->pSlowToStart_max;

	// Krauss
	f << d->epsilon_min;
	f << d->epsilon_max;

	// Memory parameters
	f << d->mtau_min;
	f << d->mtau_max;
	f << d->malphaV0_min;
	f << d->malphaV0_max;
	f << d->malphaT_min;
	f << d->malphaT_max;
	f << d->malphaA_min;
	f << d->malphaA_max;

	// MOBIL parameters - Lane Change
	f << d->athr_min;
	f << d->athr_max;
	f << d->politiness_min;
	f << d->politiness_max;
	f << d->deltab_min;
	f << d->deltab_max;
	f << d->bsave_min;
	f << d->bsave_max;
}


DriverInfo* DriverInfo::read(DataStream& f) {
	DriverInfo* d = new DriverInfo();
	f >> d->model;
	f >> d->T_min;
	f >> d->T_max;
	f >> d->a_min;
	f >> d->a_max;
	f >> d->b_min;
	f >> d->b_max;
	f >> d->v0_min;
	f >> d->v0_max;
	f >> d->s0_min;
	f >> d->s0_max;
	f >> d->s1_min;
	f >> d->s1_max;
	f >> d->delta_min;
	f >> d->delta_max;
	f >> d->coolness_min;
	f >> d->coolness_max;

	f >> d->beta_min;
	f >> d->beta_max;
	f >> d->lambda_min;
	f >> d->lambda_max;
	f >> d->tau_min;
	f >> d->tau_max;
	f >> d->tWidth_min;
	f >> d->tWidth_max;

	//KKW
	f >> d->k_min;
	f >> d->k_max;
	f >> d->pb0_min;
	f >> d->pb0_max;
	f >> d->pb1_min;
	f >> d->pb1_max;
	f >> d->pa1_min;
	f >> d->pa1_max;
	f >> d->pa2_min;
	f >> d->pa2_max;
	f >> d->vp_min;
	f >> d->vp_max;

	//Nagel
	f >> d->pSlowDown_min;
	f >> d->pSlowDown_max;
	f >> d->pSlowToStart_min;
	f >> d->pSlowToStart_max;

	// Krauss
	f >> d->epsilon_min;
	f >> d->epsilon_max;

	// Memory parameters
	f >> d->mtau_min;
	f >> d->mtau_max;
	f >> d->malphaV0_min;
	f >> d->malphaV0_max;
	f >> d->malphaT_min;
	f >> d->malphaT_max;
	f >> d->malphaA_min;
	f >> d->malphaA_max;

	// MOBIL parameters - Lane Change
	f >> d->athr_min;
	f >> d->athr_max;
	f >> d->politiness_min;
	f >> d->politiness_max;
	f >> d->deltab_min;
	f >> d->deltab_max;
	f >> d->bsave_min;
	f >> d->bsave_max;
    return d;
}

DriverInfo::DriverInfo(){
	model = vns::DriverModel::IDM;
	T_min = 1.5;	T_max = 2.0;
	v0_min = 20.0; v0_max = 33.3;
	s0_min = 2.0; s0_max = 5.0;
	s1_min = 0.0; s1_max = 0.0;
	a_min = 0.5;	a_max = 2.0;
	b_min = 1.0;	b_max = 3.0;
	delta_min = delta_max = 4.0;
	coolness_min = 1.0; coolness_max = 1.0;

	beta_min = beta_max = 1.5;
	lambda_min = lambda_max = 0.0;
	tau_min = tau_max = 0.65;
	tWidth_min = tWidth_max = 15;

	// KKW
    k_min = k_max = 2.55;
    pb0_min = pb0_max = 0.425;
    pb1_min = pb1_max = 0.04;
    pa1_min = pa1_max = 0.2;
    pa2_min = pa2_max = 0.052;
    vp_min = vp_max = 14;

    // NAGEL
    pSlowDown_min = pSlowDown_max = 0.1;
    pSlowToStart_min = pSlowToStart_max = 0.1;

    // Krauss
    epsilon_min = epsilon_max = 0.4;

	/* MOBIL */
	politiness_min = 0.1; politiness_max = 0.1;
	deltab_min = 0.3; deltab_max = 0.3;
	bsave_min = 3.0; bsave_max = 4.0;
	athr_min = 0.2; athr_max = 0.2;

	/* Memory */
	mtau_min = mtau_max = 600;
	malphaA_min = malphaA_max = 1.0;
	malphaV0_min = malphaV0_max = 1.0;
	malphaT_min = malphaT_max = 1.7;
}

DriverInfo::DriverInfo(const DriverInfo& d){
	model = d.model;
	T_min = d.T_min;	T_max = d.T_max;
	v0_min = d.v0_min; v0_max = d.v0_max;
	s0_min = d.s0_min; s0_max = d.s0_max;
	s1_min = d.s1_min; s1_max = d.s1_max;
	a_min = d.a_min;	a_max = d.a_max;
	b_min = d.b_min;	b_max = d.b_max;
	delta_min = d.delta_min; delta_max = d.delta_max;
	coolness_min = d.coolness_min; coolness_max = d.coolness_max;

	beta_min = d.beta_min; beta_max = d.beta_max;
	lambda_min = d.lambda_min; lambda_max = d.lambda_max;
	tau_min = d.tau_min; tau_max = d.tau_max;
	tWidth_min = d.tWidth_min; tWidth_max = d.tWidth_max;

	// KKW
    k_min = d.k_min; k_max = d.k_max;
    pb0_min = d.pb0_min; pb0_max = d.pb0_max;
    pb1_min = d.pb1_min; pb1_max = d.pb1_max;
    pa1_min = d.pa1_min; pa1_max = d.pa1_max;
    pa2_min = d.pa2_min; pa2_max = d.pa2_max;
    vp_min = d.vp_min; vp_max = d.vp_max;

    // NAGEL
    pSlowDown_min = d.pSlowDown_min; pSlowDown_max = d.pSlowDown_max;
    pSlowToStart_min = d.pSlowToStart_min; pSlowToStart_max = d.pSlowToStart_max;

    // Krauss
    epsilon_min = d.epsilon_min; epsilon_max = d.epsilon_max;

	/* MOBIL */
	politiness_min = d.politiness_min; politiness_max = d.politiness_max;
	deltab_min = d.deltab_min; deltab_max = d.deltab_max;
	bsave_min = d.bsave_min; bsave_max = d.bsave_max;
	athr_min = d.athr_min; athr_max = d.athr_max;

	/* Memory */
	mtau_min = d.mtau_min; mtau_max = d.mtau_max;
	malphaA_min = d.malphaA_min; malphaA_max = d.malphaA_max;
	malphaV0_min = d.malphaV0_min; malphaV0_max = d.malphaV0_max;
	malphaT_min = d.malphaT_min; malphaT_max = d.malphaT_max;
}

DriverInfo::~DriverInfo(){

}

}


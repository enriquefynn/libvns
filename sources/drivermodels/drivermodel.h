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

#ifndef VNS_DRIVERMODEL_H_
#define VNS_DRIVERMODEL_H_

#include "driverinfo.h"

namespace vns {

class LaneObject;
class Vehicle;
class Lane;
class Simulator;

class DriverModel {
public:

	/*! \cond PRIVATE */
	static const float NUMBER_OF_MODULES = 10;
	enum Type {INVALID=-1,IDM=0,EIDM=1,GIPPS=2,KRAUSS=3,NEWELL=4,OVM=5,FVDM=6,KKW=7,NAGEL=8,PIPES=9};

	static const float DT = 0.1;
	/*! \endcond */

public:
	DriverModel();
	DriverModel(const DriverInfo* driver);
	virtual ~DriverModel();

	enum LaneChangeDecision {None,StayInLane,OptionalChangeToLeft,OptionalChangeToRight,MandatoryChangeToLeft,MandatoryChangeToRight,MandatoryToStayInLane};

	/*! \cond PRIVATE */
protected:

	/* Memory */
	float malphaA;
	float malphaV0;
	float malphaT;
	float mtau;
	float mresignationMinAlphaA;
	float mresignationMinAlphaV0;
	float mresignationMaxAlphaT;

	void updateMemory(double dt, float v, float v0);

    /* Other Parameters */
    float yield;
    float patience;


    // MOBIL parameters - Lane Change
    float gapMin;
    float politeness;
    float deltab;
    float bSafe;
    float threshold;
    float biasRight;
    //---------------------

    float speedLimit;
    /*! \endcond */

public:

    inline void setSpeedLimit(float speed){ speedLimit = speed; };

    virtual float getDesiredSpeed() const = 0;
    virtual float accel(const LaneObject* v, const LaneObject* fv=0) = 0;
    virtual float accel(const LaneObject* v, float stopPos) = 0;
    virtual float accel(const LaneObject* v, float dist, float speed, float accel) = 0;
    virtual float updatePositionAndSpeed(float dt, float lanePos, float& speed, float& accel) = 0;

    virtual bool isLaneChangeAllowed(){ return true; };
    LaneChangeDecision makeLaneChangeDecision( Vehicle* me, Simulator* simulator);
    virtual float laneChangeAccelerationBalance(Vehicle* vehicle, Lane* newLane);
    bool isSafeToChangeLane(Vehicle* me, Lane* newLane );
    LaneChangeDecision makeDecisionForEnteringVehicles(Vehicle* me);
    LaneChangeDecision checkForMandatoryLaneChanges(Vehicle* me);
    LaneChangeDecision checkForOptionalLaneChanges(Vehicle* me);
    bool isLaneAllowed(Vehicle* me, Lane* lane);


    virtual bool canInvertWay(LaneObject* car,Lane* reverseLane);

    static DriverModel* newDriverModel( const DriverInfo* info );
    static vns::DriverModel::Type getDriverModel(const char* modelName);
    static const char* getDriverModelName(vns::DriverModel::Type type);

};

}

#endif /* VNS_DRIVERMODEL_H_ */

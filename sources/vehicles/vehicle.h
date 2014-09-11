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

#ifndef VNS_VEHICLE_H
#define VNS_VEHICLE_H

#include "core.h"
#include "list.h"
#include "bitset.h"
#include "laneobject.h"
#include "drivermodel.h"
#include "trajectory.h"
#include "routes.h"
#include "trafficlightcontroller.h"
#include "path.h"
#include "vehiclemodule.h"

namespace vns {

class GhostVehicle;

class Vehicle : public LaneObject {

public:
	static const uint8 MovingInLane = 0;
	static const uint8 ApprochingJunction = 1;
	static const uint8 MovingInJunction = 2;
	static const uint8 ChangingLane = 3;
	static const uint8 Parked = 4;
	static const uint8 Removed = 5;
	static const uint8 Stopped = 6;
	static const uint8 StoppedInTaxiStand = 7;
	static const uint8 ChangingWay = 8;
	static const uint8 Initialising = 9;
	static const uint8 LeavingParking = 10;
	static const uint8 EnteringParking = 11;

protected:
	/*! \cond PRIVATE */
    VehicleType vehicleType;
    VehicleModel vehicleModel;
    DriverModel* model;
    float speed;
    float accel;
    float newAccel;
	uint8 numberOfPassengers;
	uint8 limitOfPassengers;


	uint8 state;
    bool usingTrajectory;
	bool brakeLights;
	bool yield;
	bool isLaneChangeRequired;
	bool invertWay;
    bool INeedNewRoute;
    bool crossing;
    uint8 hasPriority;

	vns::Lane* laneToChange;

	//bool usingTrajectory;
	vns::List<VehicleModule*> modules;
	void (Vehicle::*stateFunction)(Simulator* sim);
	float (Vehicle::*accelFunction)(Simulator* sim);
	void (Vehicle::*checkFunction)(Simulator* sim);

	/* Think time */
	double optionalLaneChangeThinkTime;
    double nextThinkTime;

    /* Sensors */
    Lane::SensorIterator sensorIterator;

    GhostVehicle* ghost;
    Path::Iterator path;
    PathNode::StopsIterator stops;
    Path* tmpPath;



    float dirToNextLane;

    /*
     * Trajectory
     */

    Trajectory trajectory;
    float trajStart;
    float trajEnd;
    /******************/

    double junctionEnteringTime;
    double endStopTime;
    /*! \endcond */

private:
    uint64 id;
    void* userdata;
    void* networkNode;
    DriverModel::LaneChangeDecision currentLaneDecision;
	float laneStopPosition;

public:

	Vehicle(uint64 id, VehicleType type, VehicleModel model, const Path::Iterator& path, float width, float length, DriverModel* driver);
    virtual ~Vehicle();

    inline uint64 getID() const { return id; };
    inline VehicleModel getVehicleModel() const { return vehicleModel; };
    inline VehicleType getVehicleType() const { return vehicleType; };
    inline float getSpeed() const { return speed; };
    inline float getAccel() const { return accel; };
    inline void setUserData(void* data){ userdata = data; };
    inline void* getUserData() const { return userdata; };
    inline void* getNetworkNode() const { return networkNode; };
    inline void setNetworkNode(void* node){ networkNode = node; };
    inline DriverModel* getDriverModel(){ return model; };
    inline const DriverModel* getDriverModel() const { return model; };
    inline bool isBraking() const { return brakeLights; };
	inline uint8 getNumberOfPassengers() const { return numberOfPassengers; };
	inline void setNumberOfPassengers(uint8 p){ numberOfPassengers = p; };
	inline void setLimitOfPassengers(uint8 l){ limitOfPassengers = l; };
	inline uint8 getLimitOfPassengers() const { return limitOfPassengers; };
    float getAccelTo(const LaneObject* obj) const;
    inline uint32 getState() const { return state; };

	inline bool isInProcessOfLaneChange() const { return state == ChangingLane; };
	inline bool isGivingWay() const { return yield; };
	inline bool isCrossing() const { return crossing; };
	inline double getJunctionEnteringTime() const { return junctionEnteringTime; };

	Vehicle* getVehicleInFront();
    const Vehicle* getVehicleInFront() const;
    Vehicle* getVehicleBehind();
    const Vehicle* getVehicleBehind() const;
    Road* getNextRoad();
    const Road* getNextRoad() const;
    inline bool requiresChangeWay() const { return invertWay; };
    const PathStop* getRoadStop() const;

    virtual vns::Light getTrafficLightColor() const;
    inline const Lane* getNextLane() const { return nextLane; };

    /* Path */
    inline const vns::Path* getPath() const { return path.getPath(); };
    inline const Path::Iterator& getPathIterator() const { return path;};
    bool updatePath(Path* path);

    static Vehicle* fromObject(Object*);

protected:
    /*! \cond PRIVATE */
    virtual void initialize(Simulator* sim);
    virtual void simulationStep(Simulator* simulator);
    virtual void updateStep(Simulator* simulator);

    /*
     * Check Methods
     */

    virtual void check_leavingParking(Simulator* sim);
    virtual void check_enteringParking(Simulator* sim);
    virtual void check_movingInLane(Simulator* sim);
    virtual void check_changingLane(Simulator* sim);
    virtual void check_stoppedInLane(Simulator* sim);
    virtual void check_movingInJunction(Simulator* sim);
    virtual void check_changingWay(Simulator* sim);

    /*
     * State Methods
     */
    virtual void leavingParking(Simulator* sim);
    virtual void enteringParking(Simulator* sim);
    virtual void movingInLane(Simulator* sim);
    virtual void changingLane(Simulator* sim);
    virtual void stoppedInLane(Simulator* sim);
    virtual void movingInJunction(Simulator* sim);
    virtual void changingWay(Simulator* sim);

    /*
     * Acceleration Methods
     */
    virtual float acc_leavingParking(Simulator* sim);
    virtual float acc_enteringParking(Simulator* sim);
    virtual float acc_movingInLane(Simulator* sim);
    virtual float acc_changingLane(Simulator* sim);
    virtual float acc_stoppedInLane(Simulator* sim);
    virtual float acc_movingInJunction(Simulator* sim);
    virtual float acc_changingWay(Simulator* sim);

    void crossToRoad(Simulator* sim,Lane* nextLane);
    void changeWay(Simulator* sim,Lane* nextLane);
    void changeToLane(Simulator* sim,Lane* destLane);

    virtual int getPriority();
    /*! \endcond */
private:

    /* Processing Modules */
    void installModule(const Simulator* sim,VehicleModule* module);
    void uninstallAllModules(const Simulator* sim);
    void runModules(const Simulator* simulator);
    /* ****************** */

    bool willBlockIntersection() const;
    float getAccelToEnd() const;
    float getAccelToCross() const;
    float accelToJunction(Simulator* sim,Junction* junction);

    vns::Lane* considerLaneChanges(Simulator* simulator);
    float facilitateLaneChanges();

    bool lossPriorityAgainst(const Vehicle* v) const;
    bool hasTrajectoryConflictWith(const Vehicle* vehicle ) const;
    bool hasTrajectoryConflictWithInside(const Vehicle* vehicle ) const;


    float acceltoVehicle( const Vehicle* v ) const;
    vns::Lane* getBestNextLane() const;




    void tryToFindOtherPath(Simulator* sim);


    void enterLane(Simulator* sim);
    void enterRoad(Simulator* sim);

    friend class Painter;
    friend class Road;
    friend class Simulator;
    friend class DriverModel;
    friend class ModulesFactory;

};

}


#endif /* VEHICLE_H_ */

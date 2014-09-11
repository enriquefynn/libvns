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

#ifndef VNS_SIMULATOR_H
#define VNS_SIMULATOR_H

#include "core.h"
#include "lock.h"
#include "roadnetwork.h"
#include "networkmodule.h"
#include "vehiclefactory.h"
#include "modulesfactory.h"
#include "observer.h"
#include "trafficgenerationmodel.h"
#include "astarhandler.h"
#include "vector.h"
#include "list.h"
#include "eventscheduler.h"

namespace vns {

class Lane;
class RoadNetwork;

class Simulator {

public:
    static double DT;

	static const int32 SmoothLaneChanges = 1<<0;
	static const int32 RouteChanges = 1<<1;
	static const int32 TrafficLights = 1<<2;
	static const int32 RealisticParking = 1<<3;

	inline void addFlags(int32 flgs){ flags |= flgs; };
	inline void removeFlags(int32 flgs){ flags &= (~flgs); };
	inline bool hasFlag(int32 flg) const { return flags & flg; };
	inline bool hasFlags(int32 flgs) const { return (flags & flgs) == flgs; };

    Simulator();
    Simulator(vns::RoadNetwork* network);
    virtual ~Simulator();

protected:

    static Simulator* instance;
    unsigned int randomSeed;
    unsigned int seed;
    int32 flags;

    bool running;
    vns::Lock nVehiclesLock;
    uint32 nVehicles;
    uint32 nVehiclesInQueue;
    uint32 simstep;
    double finishTime;
    double time;
    uint32 dateTime;
    double stepTime;
    double stepExcutionTime;

    RoadNetwork* network;

    vns::List<Observer*> observers;
    VehicleFactory* vehicleFactory;
    ModulesFactory* modulesFactory;
    NetworkModule* networkModule;
    TrafficGenerationModel* trafficGenerationModel;
    EventScheduler* scheduler;

    List<Vehicle*> externalVehicles;


    TravelTimeFunction* astarfunction;
    AStarHandler* astar;

public:

    void setRoadNetwork( RoadNetwork* network );
    inline RoadNetwork* getRoadNetwork(){ return network; };
    inline const RoadNetwork* getRoadNetwork() const { return network; };
    inline void setRandomSeed(unsigned int s){ randomSeed = s; };
    inline unsigned int getRandomSeed() const { return randomSeed; };
    void setStepTime( double stepTime );
    void setFinishTime( double endTime );
    inline void setDateTime( uint32 baseTime ){ dateTime = baseTime; };
    inline double getFinishTime() const { return finishTime; };
    inline double getSimulationTime() const { return time; };
    inline uint32 getDateTime() const { return dateTime; };
    inline double getStepTime() const { return stepTime; };
    inline uint32 getStep() const { return simstep; };
    inline bool isRunning() const { return running; };
    inline double getExecutionTimePerStep() const { return stepExcutionTime; };
    inline uint32 getNumberOfVehicles() const { return nVehicles; };
    inline uint32 getNumberOfVehiclesInQueue() const { return nVehiclesInQueue; };
    void addObserver(Observer* observer);
    void setVehicleFactory(VehicleFactory* factory);
    void setModulesFactory(ModulesFactory* factory);
    void setTrafficGenerationModel(TrafficGenerationModel* model);
    void setNetworkModule(NetworkModule* module);
    inline const VehicleFactory* getVehicleFactory() const { return vehicleFactory; };
    inline EventScheduler* getEventScheduler(){ return scheduler; };

    void start();
    void step();
    void stop();
    void run();


    void addExternalVehicle(Vehicle* vehicle);
    const List<Vehicle*>& getExternalVehicles() const;
    Vehicle* createVehicle(VehicleType vehicleType, const DriverInfo* driver, const Path::Iterator& path);

protected:
    virtual void onSimulationStart();
    virtual void onSimulationStep();
    virtual void onSimulationStop();
    virtual void addObjectToLane(LaneObject *obj, Lane* lane, double position);
    virtual void removeObjectFromLane(LaneObject *obj);

    void deleteVehicle(Vehicle* vehicle);
    void initializeTrafficLights();

private:

    void addVehicle(Vehicle *vehicle, Lane* lane, double position);

    void simulationStep();

    Lane* chooseLaneForVehicle(float length,const Path::Iterator& path, double& lanePos);

    Path* calculatePath(Road* from, Way wayFrom, Road* to, Way wayTo) const;


    friend class TrafficLights;
    friend class Vehicle;
    friend class TrainCar;
    friend class Lane;
    friend class Road;
    friend class NS3NetworkModule;
    friend class Viewer;

};

}

#endif

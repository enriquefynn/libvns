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

#ifndef VNS_TRAIN_H_
#define VNS_TRAIN_H_

#include "vehicle.h"
#include "vector.h"

namespace vns {

/*! \cond PRIVATE */

class Train;

class TrainCar : public Vehicle {

public:

	TrainCar(Train* train);
	virtual ~TrainCar();

	static TrainCar* fromVehicle(Vehicle* vehicle);

protected:
    void simulationStep(Simulator* simulator);
    void updateStep(Simulator* simulator);

	void initialize(Simulator* sim);
    void updateLanePosition(Simulator* sim);
    void movingInLane(Simulator* sim);
    void crossToRoad(Simulator* sim,Lane* nextLane);
    void movingInJunction(Simulator* sim);

private:
	Train* leader;
	Lane* nextLane;
	void (TrainCar::*stateFunction)(Simulator* sim);
	friend class Simulator;
	friend class Train;
};


class Train : public Vehicle {

public:

	Train(uint64 id,int nCars,int32 model,const Path::Iterator& path,float width, float length,DriverModel* driver);
	virtual ~Train();

	int getNumberOfCars(){ return cars.size(); };
	TrainCar* getCar(int i){ return cars.at(i); };

	static Train* fromVehicle(Vehicle* vehicle);

protected:
    void simulationStep(Simulator* simulator);
    void updateStep(Simulator* simulator);
	void initialize(Simulator* sim);

private:
	vns::Vector<TrainCar*> cars;
};

/*! \endcond */

}

#endif /* TAXI_H_ */

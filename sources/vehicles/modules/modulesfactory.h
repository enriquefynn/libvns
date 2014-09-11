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

#ifndef VNS_MODULESFACTORY_H_
#define VNS_MODULESFACTORY_H_

namespace vns {

class Simulator;
class Vehicle;
class VehicleModule;

class ModulesFactory {
private:
	Simulator* simulator;

public:
	ModulesFactory();
	virtual ~ModulesFactory();

	virtual void installModules(Vehicle* vehicle) = 0;

protected:
	void install(Vehicle*,VehicleModule*);
	void setSimulator(Simulator* simulator);

	friend class Simulator;
};

}

#endif /* VNS_MODULESFACTORY_H_ */

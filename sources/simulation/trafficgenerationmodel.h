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

#ifndef VNS_TRAFFICGENERATIONMODEL_H_
#define VNS_TRAFFICGENERATIONMODEL_H_

#include "routes.h"
#include "generationfunction.h"

namespace vns {

class TrafficGenerationModel {
	/*! \cond PRIVATE */
protected:
	GenerationFunction* genFunction;
	GenerationFunction* defaultGenFunction;
	Routes* routes;
	EventScheduler* scheduler;
	/*! \endcond */
public:

	TrafficGenerationModel( Routes* routes = 0 );
	virtual ~TrafficGenerationModel();

	void setGenerationFunction(GenerationFunction* function);

	virtual void onSimulationStart( Simulator* simulator );
	virtual void onSimulationStep( Simulator* simulator );
	virtual void onSimulationFinished( Simulator* simulator );

protected:
	void addRoute(Simulator* sim, Route* route);
	void addScheduledRoute(Simulator* sim, ScheduledRoute* route);

};

}

#endif /* VNS_TRAFFICGENERATIONMODEL_H_ */

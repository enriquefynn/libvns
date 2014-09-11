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

#ifndef VNS_PARALLELSIMULATOR_H
#define VNS_PARALLELSIMULATOR_H

#include "simulator.h"

namespace vns {

class ParallelSimulator : public Simulator {

public:

    ParallelSimulator();
    ParallelSimulator(vns::RoadNetwork* network);
    ~ParallelSimulator();


protected:
    void onSimulationStart();
    void onSimulationStep();
};

}

#endif

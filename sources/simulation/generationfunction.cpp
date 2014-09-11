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

#include "generationfunction.h"
#include "maths.h"

namespace vns {

double NegativeExponentialDistribution::calcT(double t,double meanHeadway) const {
	return t+meanHeadway*(-log(vns::randomValue(0.0001,1.0)));
}

double PoissonDistribution::calcT(double t,double meanHeadway) const {
	return t+meanHeadway*(-log(vns::randomValue(0.0001,1.0)));
}

double UniformDistribution::calcT(double t,double meanHeadway) const {
	return t+meanHeadway;
}

}

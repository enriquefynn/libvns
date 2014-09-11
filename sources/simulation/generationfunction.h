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

#ifndef VNS_GENERATIONFUNCTION_H_
#define VNS_GENERATIONFUNCTION_H_

namespace vns {

class GenerationFunction {
public:
	virtual ~GenerationFunction(){};
	virtual double calcT(double t,double meanHeadway) const = 0;
};

class NegativeExponentialDistribution : public GenerationFunction {
public:
	double calcT(double t,double meanHeadway) const;
};

class PoissonDistribution : public GenerationFunction {
public:
	double calcT(double t,double meanHeadway) const;
};

class UniformDistribution : public GenerationFunction {
public:
	double calcT(double t,double meanHeadway) const;
};

}

#endif

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

#include <omp.h>

namespace vns {

void setNumberOfThreads(int nThreads){
	omp_set_num_threads(nThreads);
}

int getNumberOfThreads(){
	return omp_get_num_threads();
}

int getThreadID(){
	return omp_get_thread_num();
}

double getWTime(){
	return omp_get_wtime();
}


}

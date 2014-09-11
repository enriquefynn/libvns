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
#include "lock.h"

namespace vns {

Lock::Lock(){
	mlock = new omp_lock_t;
	omp_init_lock((omp_lock_t*)mlock);
}

Lock::~Lock(){
	omp_destroy_lock((omp_lock_t*)mlock);
	delete (omp_lock_t*)mlock;
}

bool Lock::testLock(){
	return omp_test_lock((omp_lock_t*)mlock);
}

void Lock::lock(){
	omp_set_lock((omp_lock_t*)mlock);
}

void Lock::unlock() {
	omp_unset_lock((omp_lock_t*)mlock);
}

}

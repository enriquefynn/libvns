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

#ifndef VNS_MUTEX_H_
#define VNS_MUTEX_H_

namespace vns {
/*! \cond PRIVATE */

class Lock {
private:
	void* mlock;
public:
	Lock();
	~Lock();
	bool testLock();
	void lock();
	void unlock();
};


/*! \endcond */
}

#endif

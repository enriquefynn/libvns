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

#ifndef VNS_TRACKER_H_
#define VNS_TRACKER_H_

#include "vec.h"
#include "hashtable.h"
#include "vector.h"
#include "list.h"
#include "observer.h"

namespace vns {

class Simulator;
class Vehicle;

class Tracker : public Observer {
public:
	class Position {
	private:
		Vec pos;
		Vec dir;
		uint32 t;
	public:
		inline Vec getPosition(){ return pos; };
		inline Vec getDirection(){ return dir; };
		inline uint32 getTime(){ return t; };
		Position(){};
		Position(const Vec& pos,const Vec&dir,uint32 t):pos(pos),dir(dir),t(t){};
	};
	typedef Vector<Position> Track;
	Tracker();
	virtual ~Tracker();
	void onVehicleRemoved(const Simulator* , Vehicle* );
	void onVehicleInjected(const Simulator* , Vehicle* );
	void onSimulationStart(const Simulator* );
	void onSimulationStep(const Simulator* );
	void onSimulationStop(const Simulator* );
	void setStepInterval(double dt){ stepInterval=dt; };

	inline const List<Track*>& getTracks() const { return tracks; };
	void clear();

private:
	HashTable<Vehicle*,Track*>* vehicles;
	List<Track*> tracks;
	double nextStep;
	double stepInterval;
};

}

#endif /* TRACKER_H_ */

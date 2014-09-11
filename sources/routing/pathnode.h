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

#ifndef VNS_PATHNODE_H_
#define VNS_PATHNODE_H_

#include "datastream.h"
#include "core.h"
#include "pathstop.h"
#include "vector.h"

namespace vns {

class Routes;
class Road;

class PathNode {
private:
	Road* road;
	Way way;
	PathNode* next;
	PathNode* prev;
	Vector< PathStop* > stops;

	PathNode(Road* road, Way way);
	~PathNode();

	static void write(DataStream& f,const PathNode* node);
	static PathNode* read(DataStream& f, Routes* routes);

public:
	inline Road* getRoad() { return road; };
	inline const Road* getRoad() const { return road; };
	inline Way getWay() const { return way; };
	inline int32 getNumberOfStops() const { return stops.size(); };
	bool addStop(PathStop::Type type, double position, float duration);
	void clearStops();
	inline bool hasPrevious() const { return prev!=0; };
	inline bool hasNext() const { return next!=0; };
	inline PathNode* getPrevious(){ return prev; };
	inline PathNode* getNext(){ return next; };
	inline const PathNode* getPrevious() const { return prev; };
	inline const PathNode* getNext() const { return next; };

	class StopsIterator {
	private:
		int i;
		const PathNode* node;
		StopsIterator(const PathNode* node,int i=0):i(i),node(node){};

	public:
		StopsIterator():i(0),node(0){};
		inline bool isValid() const { return node && i>=0 && i<node->stops.size(); };
		inline void next(){ i++; };
		inline void previous(){ i--; };
		inline const PathStop* get() const { return node->stops.at(i); };
	    inline float getPosition() const { return node->stops.at(i)->lanePos; };
	    inline float getDuration() const { return node->stops.at(i)->time; };
	    inline PathStop::Type getType() const { return node->stops.at(i)->type; };
		friend class PathNode;
	};

	inline PathNode::StopsIterator getStopsIterator() { return PathNode::StopsIterator(this); };
	inline PathNode::StopsIterator getStopsIterator() const { return PathNode::StopsIterator(this); };

	friend class Path;
};

}

#endif /* PATHNODE_H_ */

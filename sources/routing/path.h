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

#ifndef VNS_PATH_H_
#define VNS_PATH_H_

#include "datastream.h"
#include "core.h"
#include "pathnode.h"

namespace vns {

class Routes;
class Road;

class Path {
private:
	int32 id;
	PathNode* first;
	PathNode* last;
	int32 size;
	bool loop;

	static void write(DataStream& f,const vns::Path* path);
	static vns::Path* read(DataStream& f, vns::Routes* routes);

    friend DataStream& operator <<(DataStream&f, const Routes* routes);
    friend DataStream& operator >>(DataStream& f, Routes* routes);

	friend class Routes;
	friend class Route;
	friend class ScheduledRoute;

public:
	Path();
	Path(Road* road,Way way);
	virtual ~Path();
	int32 getID() const { return id; };
	PathNode* appendNode(Road* road, Way way);
	PathNode* prependNode(Road* road, Way way);
	inline int32 getNumberOfNodes() const { return size; };
	inline bool isLoop() const { return loop; };
	inline PathNode* getFirstNode(){ return first; };
	inline PathNode* getLastNode(){ return last; };
	inline const PathNode* getFirstNode() const { return first; };
	inline const PathNode* getLastNode() const { return last; };
	void printPath();
	void setLoop(bool v);
	Path* clone();
	bool append(Path* path);
	void join(Path* path);

	class Iterator {
	private:
		PathNode* i;
		const Path* path;
		Iterator(PathNode* node,const Path* path):i(node),path(path){};

	public:
		Iterator():i(0),path(0){};
		inline bool isValid() const { return i!=0; };
		inline void next(){ i = i->next; };
		inline void previous(){ i = i->prev; };
		inline const Path* getPath() const { return path; };
		inline PathNode* getFirst() const { return path->first; };
		inline PathNode* getLast() const { return path->last; };
		inline Road* getFirstRoad() const { return path->first->road; };
		inline Road* getLastRoad() const { return path->last->road; };
		inline Way getFirstRoadWay() const { return path->first->way; };
		inline Way getLastRoadWay() const { return path->last->way; };
		inline PathNode* get() const { return i; };
		inline Road* getRoad() const { return i->road; };
		inline Way getWay() const { return i->way; };
		inline bool hasNext() const { return i!=0 && i->next!=0; };
		inline PathNode* getNext() const { return i->next; };
		inline Road* getNextRoad() const { return i->next->road; };
		inline Way getNextRoadWay() const { return i->next->way; };
		inline bool hasPrevious() const { return i!=0 && i->prev!=0; };
		inline PathNode* getPrevious() const { return i->prev; };
		inline Road* getPreviousRoad() const { return i->prev->road; };
		inline Way getPreviousRoadWay() const { return i->prev->way; };
		inline PathNode::StopsIterator getStopsIterator() { return i->getStopsIterator(); };

		friend class Path;
	};

	class ConstIterator {
	private:
		const PathNode* i;
		const Path* path;
		ConstIterator(const PathNode* node,const Path* path):i(node),path(path){};

	public:
		ConstIterator():i(0),path(0){};
		inline bool isValid() const { return i!=0; };
		inline void next(){ i = i->next; };
		inline void previous(){ i = i->prev; };
		inline const Path* getPath() const { return path; };
		inline const PathNode* getFirst() const { return path->first; };
		inline const PathNode* getLast() const { return path->last; };
		inline const Road* getFirstRoad() const { return path->first->road; };
		inline const Road* getLastRoad() const { return path->last->road; };
		inline Way getFirstRoadWay() const { return path->first->way; };
		inline Way getLastRoadWay() const { return path->last->way; };
		inline const PathNode* get() const { return i; };
		inline const Road* getRoad() const { return i->road; };
		inline Way getWay() const { return i->way; };
		inline bool hasNext() const { return i!=0 && i->next!=0; };
		inline const PathNode* getNext() const { return i->next; };
		inline const Road* getNextRoad() const { return i->next->road; };
		inline Way getNextRoadWay() const { return i->next->way; };
		inline bool hasPrevious() const { return i!=0 && i->prev!=0; };
		inline const PathNode* getPrevious() const { return i->prev; };
		inline const Road* getPreviousRoad() const { return i->prev->road; };
		inline Way getPreviousRoadWay() const { return i->prev->way; };
		inline PathNode::StopsIterator getStopsIterator() const { return i->getStopsIterator(); };

		friend class Path;
	};

	inline Path::Iterator getFirstNodeIterator(){ return Path::Iterator(first,this); };
	inline Path::Iterator getLastNodeIterator(){ return Path::Iterator(last,this); };
	inline Path::ConstIterator getFirstNodeConstIterator() const { return Path::ConstIterator(first,this); };
	inline Path::ConstIterator getLastNodeConstIterator() const { return Path::ConstIterator(last,this); };

    static bool isValidPath(const Path* path);
    static bool equalPaths(const Path* path1, const Path* path2);

};

}

#endif /* VNS_PATH_H_ */

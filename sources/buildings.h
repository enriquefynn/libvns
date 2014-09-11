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

#ifndef VNS_BUILDINGS_H_
#define VNS_BUILDINGS_H_

#include "building.h"
#include "rtree.h"
#include "vector.h"

namespace vns {

/*! \cond PRIVATE */

class Buildings {
private:
	vns::Vector<vns::Building*> buildings;
	vns::RTree rtree;
	vns::BBox extents;

	static Buildings* staticInstance;

	class IntersectionVisitor : public RTree::Visitor {
	private:
		vns::Vec p1;
		vns::Vec p2;
		vns::BBox bb;
		bool intersects;
	public:
		IntersectionVisitor(const vns::Vec& p1,const vns::Vec& p2){
			this->p1 = p1;
			this->p2 = p2;
			bb.addPoint(p1);
			bb.addPoint(p2);
			intersects = false;
		};
		bool accept(BBoxItem* item) {
			if(intersects) return false;
			if( bb.intersects(item->getBBox()) ){
				return true;
			}
			return false;
		};
		void visit(BBoxItem* item){
			vns::Building* building = dynamic_cast<vns::Building*>(item);
			if(building){
				if( building->intersects(p1,p2) ){
					intersects = true;
				}
			}
		};

		inline bool hasIntersection(){ return intersects; };
	};

public:
	Buildings();
	virtual ~Buildings();

	static Buildings* getInstance(){ return staticInstance; };

	void addBuilding(vns::Building* building);
	inline int getNumberOfBuildings(){ return buildings.size(); };
	inline vns::Building* getBuilding(int i){ return buildings.at(i); };
	inline void removeBuilding(int i){ buildings.remove(i); };
	bool intersects(const vns::Vec& p1,const vns::Vec& p2);
	void clear();
};

/*! \endcond */

}
#endif /* BUILDINGS_H_ */

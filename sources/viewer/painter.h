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

#ifndef VNS_PAINTER_H_
#define VNS_PAINTER_H_

#include "simulator.h"
#include "roadnetwork.h"
#include "trafficlightcontroller.h"
#include "routes.h"
#include "vehicle.h"
#include "vector.h"
#include "trajectory.h"

namespace vns {

class SceneViewer;

#define LINE_WIDTH 0.12

class Painter {
public:
	Painter();
	virtual ~Painter();
	void initialize(SceneViewer* w);

	void drawRailAsphalt(SceneViewer* w,const Road* road,uint8 lod);
	void drawRailTrack(SceneViewer* w,const Road* road,uint8 lod);
	void drawRoadAsphalt(SceneViewer* w,const Road* road,uint8 lod);
	void drawRoadMarkers(SceneViewer* w,const Road* road,uint8 lod);
	void drawJunctionAsphalt(SceneViewer* w,const Junction* junction,uint8 lod);
	void drawJunctionMarkers(SceneViewer* w,const Junction* junction,uint8 lod);
	void drawLane(SceneViewer* w,const Lane* lane,uint8 lod);
	void drawStopSign(const Vec& pt, const Vec& dir, const Vec& perp);
	void drawYieldSign(const Lane* lane);
	void drawLaneArrows(const Vec& pt, const Vec& dir, const Vec& perp, const Lane* lane);
	void drawContinuousLine(const PolyLine* line, double offset, double start, double end, double lineWidth=0.12);
	void drawDiscontinuousLine(const PolyLine* line, double offset, double start, double end, double lineWidth=0.12,double spacing=2.0);
	void drawCrossWalk(const Road* road, Way way, float width=3.0, float interval=0.50);
	void drawRoadPoints(SceneViewer* w,const Road* road,uint8 lod);
	void drawJunctionBorder(const Vec& p1,const Vec& p2, const Vec& p3, const Vec& p4, double offset=0.0);
	void drawCircularPoint(const Vec& pos, const double radius);
	void drawCircularPoint(double x,double y, const double radius);
	void drawHighlightedRoad(const Road* road,uint8 lod);
	void drawRoutePath(SceneViewer* w, const Path* path);
	virtual void drawVehicle(SceneViewer* w, const Vehicle* vehicle, uint8 lod);
	void drawLaneConnections(SceneViewer* w, const Lane* lane, uint8 lod);
	void drawLaneNextLanes(SceneViewer* w, const Lane* lane, uint8 lod);
	void drawNumbersOfJunctionExitLanes(SceneViewer* w, const Junction* junction, uint8 lod);
	void drawLights(const Lane* lane);
	virtual void draw(SceneViewer*,const BBox&, uint8){};

	virtual void setColor3f(float r,float g,float b);
	virtual void setColor4f(float r,float g,float b,float a);
	virtual void setPointSize(float s);
	virtual void setLineWidth(float w);
	virtual void drawLine(const vns::Vec& p1,const vns::Vec& p2);
	virtual void drawPoint(const vns::Vec&);
	virtual void drawPoint(double x, double y, double z=0.0);
	virtual void drawBBox(const BBox& bbox);
	virtual void drawTrajectory(const vns::Vec& p1,const vns::Vec& d1,const vns::Vec& p2,const vns::Vec& d2,float width);
	void drawRoadInfo(SceneViewer* viewer,const Road* road);

	void drawDebugArrows(const Vec& pt, const Vec& dir, const Vec& perp);
	void drawDebugRoad(const Road* road);
	void drawDebugJunction(const Junction* junction);

public:
	inline void setDrawJunctionAsphalt(bool v){ draw_junctionAsphalt=v; };
	inline void setDrawJunctionMarkers(bool v){ draw_junctionMarkers=v; };
	inline void setDrawRoadAsphalt(bool v){ draw_roadAsphalt=v; };
	inline void setDrawRoadMarkers(bool v){ draw_roadMarkers=v; };
	inline void setDrawRoadObjects(bool v){ draw_roadObjects=v; };
	inline void setDrawRoadsEnabled(bool v){draw_roads = v;};
	inline bool isDrawRoadsEnabled(){ return draw_roads; };

private:
	uint32 circleList;
	uint32 stopTID;
	uint32 busTID;
	uint32 yieldTID;
	uint32 redLightTID;
	uint32 greenLightTID;
	uint32 yellowLightTID;
	uint32 offLightTID;

	uint32 carTID;
	Vector< Vector<const char*> > vehiclesIMG;
	Vector< Vector<uint32> > vehicleTIDs;

	Trajectory trajectory;

	double lineWidth;

	bool draw_junctionAsphalt;
	bool draw_junctionMarkers;
	bool draw_roadAsphalt;
	bool draw_roadMarkers;
	bool draw_roadObjects;
	bool draw_roads;
};

}

#endif /* PAINTER_H_ */

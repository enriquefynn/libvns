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

#ifndef VNS_PATHSTOP_H_
#define VNS_PATHSTOP_H_

#include "datastream.h"

namespace vns {

class PathStop {
public:
    typedef enum { LeftParking, RightParking, LeftMostLane, RightMostLane, LeftTaxiStand, RightTaxiStand } Type;

private:
	PathStop::Type type;
	float lanePos;
	float time;

	PathStop(PathStop::Type type, float lanePosition, float duration);
    static void write(DataStream& f, const PathStop* stop);
    static PathStop* read(DataStream& f);
    friend class PathNode;

public:

    inline float getPosition() const { return lanePos; };
    inline float getDuration() const { return time; };
    inline PathStop::Type getType() const { return type; };

    static PathStop::Type fromTypeName(const char* typeName);
    static const char* fromType(PathStop::Type type);
};

}

#endif /* VNS_PATHSTOP_H_ */

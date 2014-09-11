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

#ifndef VNS_DATASTREAM_H_
#define VNS_DATASTREAM_H_

#include "core.h"
#include <fstream>

namespace vns {

class DataStream {
private:
	std::fstream* file;
public:
	DataStream(std::fstream* f):file(f){};

    DataStream& operator>>(int8& v);
    DataStream& operator>>(uint8& v);
    DataStream& operator>>(int16& v);
    DataStream& operator>>(uint16& v);
    DataStream& operator>>(int32& v);
    DataStream& operator>>(uint32& v);
    DataStream& operator>>(int64& v);
    DataStream& operator>>(uint64& v);
    DataStream& operator>>(bool& v);
    DataStream& operator>>(float& v);
    DataStream& operator>>(double& v);
    DataStream& operator>>(char*& str);

    DataStream& operator<<(int8 v);
    DataStream& operator<<(uint8 v);
    DataStream& operator<<(int16 v);
    DataStream& operator<<(uint16 v);
    DataStream& operator<<(int32 v);
    DataStream& operator<<(uint32 v);
    DataStream& operator<<(int64 v);
    DataStream& operator<<(uint64 v);
    DataStream& operator<<(bool v);
    DataStream& operator<<(float v);
    DataStream& operator<<(double v);
    DataStream& operator<<(const char* str);

    DataStream& readBytes(char*&, uint32& len);
    void readRawData(char*, uint32 len);
    DataStream& writeBytes(const char*, uint32 len);
    void writeRawData(const char*, uint32 len);
};


}

#endif

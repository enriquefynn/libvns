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

#include <cstring>
#include "datastream.h"

namespace vns {

DataStream& DataStream::operator<<(int8 v){ file->put(v); return *this; }
DataStream& DataStream::operator<<(int16 v){ file->write((char*)&v,2); return *this; }
DataStream& DataStream::operator<<(int32 v){ file->write((char*)&v,4); return *this; }
DataStream& DataStream::operator<<(int64 v){ file->write((char*)&v,8); return *this; }
DataStream& DataStream::operator<<(uint8 i){ return *this << int8(i); }
DataStream& DataStream::operator<<(uint16 i){ return *this << int16(i); }
DataStream& DataStream::operator<<(uint32 i){ return *this << int32(i); }
DataStream& DataStream::operator<<(uint64 i){ return *this << int64(i); }
DataStream& DataStream::operator<<(bool v){ file->put(char(v)); return *this; }
DataStream& DataStream::operator<<(float v){ file->write((char*)&v,4); return *this; }
DataStream& DataStream::operator<<(double v){ file->write((char*)&v,8); return *this; }

DataStream& DataStream::operator<<(const char *s){
	if (!s) {
		*this << (uint32)0;
        return *this;
    }
    uint32 len = strlen(s) + 1;
    *this << (uint32)len;
    writeRawData(s, len);
    return *this;
}

DataStream& DataStream::writeBytes(const char* s, uint32 len){
    *this << (uint32)len;
    if(len) writeRawData(s, len);
    return *this;
}

void DataStream::writeRawData(const char* s, uint32 len){
	file->write(s, len);
}

DataStream& DataStream::operator>>(int8& v){
	char c;
	file->get(c);
	v = int8(c);
    return *this;
}

DataStream& DataStream::operator>>(int16& v){
	v = int32(0);
	file->read((char *)&v, 2);
    return *this;
}

DataStream& DataStream::operator>>(int32& v){
	v = int32(0);
	file->read((char *)&v, 4);
    return *this;
}

DataStream& DataStream::operator>>(int64& v){
	v = int64(0);
	file->read((char *)&v, 8);
    return *this;
}

DataStream& DataStream::operator>>(uint8& v){ return *this >> reinterpret_cast<int8&>(v); }
DataStream& DataStream::operator>>(uint16& v){ return *this >> reinterpret_cast<int16&>(v); }
DataStream& DataStream::operator>>(uint32& v){ return *this >> reinterpret_cast<int32&>(v); }
DataStream& DataStream::operator>>(uint64& v){ return *this >> reinterpret_cast<int64&>(v); }

DataStream& DataStream::operator>>(bool& v){
	char c;
	file->get(c);
	v = !!c;
    return *this;
}

DataStream& DataStream::operator>>(float& v){
	v = 0.0f;
	file->read((char *)&v, 4);
    return *this;
}

DataStream& DataStream::operator>>(double& v){
	v = 0.0;
	file->read((char *)&v, 8);
    return *this;
}

DataStream& DataStream::operator>>(char*& s){
    uint32 len = 0;
    return readBytes(s, len);
}

DataStream& DataStream::readBytes(char*& s, uint32 &l){
    s = 0;
    l = 0;
    uint32 len;
    *this >> len;
    if (len == 0)
        return *this;

    const uint32 step = 1024 * 1024;
    uint32 allocated = 0;
    char *prevBuf = 0;
    char *curBuf = 0;
    do {
        int blockSize = (step<len - allocated?step:len-allocated);
        prevBuf = curBuf;
        curBuf = new char[allocated + blockSize + 1];
        if (prevBuf) {
            memcpy(curBuf, prevBuf, allocated);
            delete [] prevBuf;
        }
        file->read(curBuf + allocated, blockSize);
        allocated += blockSize;
    } while (allocated < len);
    s = curBuf;
    s[len] = '\0';
    l = (uint32)len;
    return *this;
}

void DataStream::readRawData(char* s, uint32 len){
	file->read(s, len);
}

}

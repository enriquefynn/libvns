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

#ifndef VNS_CORE_H_
#define VNS_CORE_H_


#include <limits>
#include <iostream>
#include <stdint.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define WINDOWS
#else
#define LINUX
#endif

#define VNS_Debug std::cerr
#define VNS_Output std::cout



#define VNS_ASSERT(v,type,error) ((v)?(void)0:(fprintf(stderr,"%s: %s\n",(type),(error)),abort()))


typedef unsigned uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char byte;
typedef signed char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;

typedef int64_t int64;           /* 64 bit signed */
typedef uint64_t uint64; /* 64 bit unsigned */

namespace vns {

/*! \cond PRIVATE */

template <class T> inline void vns_Swap(T& a, T& b){ T t = a;a = b;b = t; };

template <int> struct IntegerForSize;
template <>	struct IntegerForSize<1> { typedef uint8 Unsigned; typedef int8 Signed; };
template <> struct IntegerForSize<2> { typedef uint16 Unsigned; typedef int16 Signed; };
template <> struct IntegerForSize<4> { typedef uint32 Unsigned; typedef int32 Signed; };
template <> struct IntegerForSize<8> { typedef uint64 Unsigned; typedef int64 Signed; };
template <class T> struct IntegerForSizeof: IntegerForSize<sizeof(T)> { };

typedef IntegerForSizeof<void*>::Unsigned uintptr;
typedef IntegerForSizeof<void*>::Signed ptrdiff;

/*! \endcond */



static const int8 MAX_INT8 = std::numeric_limits<int8>::max();
static const int16 MAX_INT16 = std::numeric_limits<int16>::max();
static const int32 MAX_INT32 = std::numeric_limits<int32>::max();
static const int64 MAX_INT64 = std::numeric_limits<int64>::max();
static const uint8 MAX_UINT8 = std::numeric_limits<uint8>::max();
static const uint16 MAX_UINT16 = std::numeric_limits<uint16>::max();
static const uint32 MAX_UINT32 = std::numeric_limits<uint32>::max();
static const uint64 MAX_UINT64 = std::numeric_limits<uint64>::max();
static const double MAX_DOUBLE = std::numeric_limits<double>::max();
static const float MAX_FLOAT = std::numeric_limits<float>::max();

static const int8 MIN_INT8 = std::numeric_limits<int8>::min();
static const int16 MIN_INT16 = std::numeric_limits<int16>::min();
static const int32 MIN_INT32 = std::numeric_limits<int32>::min();
static const int64 MIN_INT64 = std::numeric_limits<int64>::min();
static const uint8 MIN_UINT8 = 0;
static const uint16 MIN_UINT16 = 0;
static const uint32 MIN_UINT32 = 0;
static const uint64 MIN_UINT64 = 0;
static const double MIN_DOUBLE = -std::numeric_limits<double>::max();
static const float MIN_FLOAT = -std::numeric_limits<float>::max();

typedef enum {Backward=false,Forward=true} Way;
typedef int8 VehicleType;
typedef int8 VehicleModel;
typedef enum { NoLight, RedLight, GreenLight, YellowLight, OffLight } Light;

static const VehicleType CAR = 0;
static const VehicleType BIKE = 1;
static const VehicleType TAXI = 2;
static const VehicleType BUS = 3;
static const VehicleType TRUCK = 4;
static const VehicleType TRAIN = 5;

void setNumberOfThreads(int nThreads);
int getNumberOfThreads();
int getThreadID();
double getWTime();

}

#endif /* VNS_CORE_H_ */

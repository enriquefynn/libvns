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

#ifndef VNS_MAPGRID_H_
#define VNS_MAPGRID_H_

#include "vec.h"
#include "bbox.h"

namespace vns {

template <typename T>
class MapGrid {
private:
	int ROWS;
	int COLS;
	double xmin;
	double ymin;
	double xmax;
	double ymax;
	int dx;
	int dy;
	T** data;

public:
	MapGrid(const BBox& extents,int ddy=10,int ddx=10);
	virtual ~MapGrid();
	inline void add(const Vec& pt,const T& v);
	inline void set(const Vec& pt,const T& v);
	void fill(const T& v);

	inline int getRows(){ return ROWS; };
	inline int getCols(){ return COLS; };
	inline double getXMin(){ return xmin; };
	inline double getXMax(){ return xmax; };
	inline double getYMin(){ return ymin; };
	inline double getYMax(){ return ymax; };
	inline int getDx(){ return dx; };
	inline int getDy(){ return dy; };
	inline const T& get(int y,int x){ return data[y][x]; };
	const T& get(const Vec& pos);
};

template <typename T>
MapGrid<T>::MapGrid(const BBox& bb,int ddy,int ddx){

	dx = ddx;
	dy = ddy;

	xmin = bb.getMinX();
	ymin = bb.getMinY();
	xmax = bb.getMaxX();
	ymax = bb.getMaxY();

	COLS = ((xmax-xmin)/dx)+1;
	ROWS = ((ymax-ymin)/dy)+1;

	data = new T*[ROWS];
	for(int i=0;i<ROWS;i++){
		data[i] = new T[COLS];
	}

};

template <typename T>
MapGrid<T>::~MapGrid(){
	for(int i=0;i<ROWS;i++){
		delete[] data[i];
	}
	delete[] data;
	data = 0;
}

template <typename T>
void MapGrid<T>::fill(const T& v){
	for(int i=0;i<ROWS;i++){
		for(int j=0;j<COLS;j++){
			data[i][j] = v;
		}
	}
}

template <typename T>
void MapGrid<T>::add(const Vec& pt,const T& v){
	int x = int((pt.getX()-xmin)/dx);
	int y = int((pt.getY()-ymin)/dy);
	if(y<ROWS && x<COLS){
		data[y][x] += v;
	}
}

template <typename T>
void MapGrid<T>::set(const Vec& pt,const T& v){
	int x = int((pt.getX()-xmin)/dx);
	int y = int((pt.getY()-ymin)/dy);
	if(y<ROWS && x<COLS){
		data[y][x] = v;
	}
}

template <typename T>
const T& MapGrid<T>::get(const Vec& pt){
	int x = int((pt.getX()-xmin)/dx);
	int y = int((pt.getY()-ymin)/dy);
	return data[y][x];
}

}

#endif /* MAPGRID_H_ */

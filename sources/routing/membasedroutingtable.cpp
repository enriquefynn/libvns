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

#include "membasedroutingtable.h"
#include "roadnetwork.h"
#include <time.h>
#include <fstream>

#define INFINITE vns::MAX_INT16

namespace vns {

MemBasedRoutingTable::MemBasedRoutingTable(vns::RoadNetwork* net) : RoutingTable(net) {
	NR = net->getNumberOfRoads();
	NR2 = NR*2;

	dist = new int16*[NR2];
	for(int i=0;i<NR2;i++){
		dist[i] = new int16[NR2];
		for(int j=0;j<NR2;j++){
			dist[i][j] = INFINITE;
		}
	}

	path = new int16*[NR2];
	for(int i=0;i<NR2;i++){
		path[i] = new int16[NR2];
		for(int j=0;j<NR2;j++){
			path[i][j] = -1;
		}
	}
}

MemBasedRoutingTable::~MemBasedRoutingTable() {
	for(int i=0;i<NR2;i++){
		delete[] dist[i];
	}
	delete[] dist;
	for(int i=0;i<NR2;i++){
		delete[] path[i];
	}
	delete[] path;
}

void MemBasedRoutingTable::load(const char* fname){
	std::fstream file;
	file.open(fname, std::fstream::in | std::fstream::binary );
	if(!file.is_open()){
		fprintf(stderr,"MemBasedRoutingTable::load - Error: Cannot Open File!\n");
		exit(1);
	}
	DataStream in(&file);
	in >> NR;
	in >> NR2;
	for(int i=0;i<NR2;i++){
		for(int j=0;j<NR2;j++){
			in >> dist[i][j];
		}
	}
	for(int i=0;i<NR2;i++){
		for(int j=0;j<NR2;j++){
			in >> path[i][j];
		}
	}
	file.close();
}

void MemBasedRoutingTable::save(const char* fname){
	std::fstream file;
	file.open(fname, std::fstream::out | std::fstream::binary | std::fstream::trunc );
	if(!file.is_open()){
		fprintf(stderr,"MemBasedRoutingTable::save - Error: Cannot Open File!\n");
		exit(1);
	}
	DataStream out(&file);
	out << NR;
	out << NR2;
	for(int i=0;i<NR2;i++){
		for(int j=0;j<NR2;j++){
			out << (int16)dist[i][j];
		}
	}
	for(int i=0;i<NR2;i++){
		for(int j=0;j<NR2;j++){
			out << (int16)path[i][j];
		}
	}
	file.flush();
	file.close();
}

int32 MemBasedRoutingTable::getWeight(vns::Road* src,vns::Way srcWay,vns::Road* dst,vns::Way dstWay){
	return getWeight(src->getID(),srcWay,dst->getID(),dstWay);
}

int32 MemBasedRoutingTable::getWeight(int32 srcID,vns::Way srcWay,int32 dstID,vns::Way dstWay){
	int32 d = (int32)dist[ (srcWay==vns::Forward?srcID:srcID+NR) ][ (dstWay==vns::Forward?dstID:dstID+NR) ];
	if(d>=INFINITE){
		return -1;
	}
	return d;
}

bool MemBasedRoutingTable::hasPath(vns::Road* src,vns::Way srcWay,vns::Road* dst,vns::Way dstWay){
	return hasPath(src->getID(),srcWay,dst->getID(),dstWay);

}

bool MemBasedRoutingTable::hasPath(int32 srcID,vns::Way srcWay,int32 dstID,vns::Way dstWay){
	return dist[ (srcWay==vns::Forward?srcID:srcID+NR) ] [(dstWay==vns::Forward?dstID:dstID+NR) ] < INFINITE;
}

vns::Path* MemBasedRoutingTable::getPath(vns::Road* src,vns::Way srcWay,vns::Road* dst,vns::Way dstWay){
	return getPath(src->getID(),srcWay,dst->getID(),dstWay);

}

vns::Path* MemBasedRoutingTable::getPath(int32 srcID,vns::Way srcWay,int32 dstID,vns::Way dstWay){
	return getPath( (srcWay==vns::Forward?srcID:srcID+NR) , (dstWay==vns::Forward?dstID:dstID+NR) );
}

vns::Path* MemBasedRoutingTable::getPath(int32 i,int32 j){
	if( dist[i][j] == INFINITE ){
		return 0;
	}
	vns::Path* path = reconstructPath(i,j);
	path->prependNode( network->getRoad( (i<NR?i:i-NR) ) , (i<NR?vns::Forward:vns::Backward) );
	path->appendNode( network->getRoad( (j<NR?j:j-NR) ) , (j<NR?vns::Forward:vns::Backward) );
	return path;
}

vns::Path* MemBasedRoutingTable::reconstructPath(int32 i,int32 j){
	if( path[i][j] < 0 ){
		return new vns::Path();
	}
	int32 k = path[i][j];
	vns::Path* path = reconstructPath(i,k);
	path->appendNode( network->getRoad( (k<NR?k:k-NR) ) , (k<NR?vns::Forward:vns::Backward) );
	path->join( reconstructPath(k,j) );
	return path;
}


int16 MemBasedRoutingTable::getPrevious(int16 a, int16 j) {
	int32 id = (j<NR?j:j-NR);
	vns::Way way = (j<NR?vns::Forward:vns::Backward);
	const vns::Road* road = network->getRoad(id);
	const vns::Junction* start = road->getStartJunction(way);
	if(start==0) return -1;
	int32 nr = start->getNumberOfRoads();
	int16 minD = INFINITE;
	int16 k = -1;
	for(int r=0;r<nr;r++){
		const vns::Road* rn = start->getRoad(r);
		if(rn==road) continue;
		if(rn->hasConnectionTo(road)){
			vns::Way rw = (rn->getEndJunction()==start?vns::Forward:vns::Backward);
			int16 i = (rw==vns::Forward?rn->getID():rn->getID()+NR);
			int16 tmp = dist[a][i];
			if( tmp<INFINITE && tmp<minD ){
				minD = tmp;
				k = i;
			}
		}
	}
	return k;
}

vns::Path* MemBasedRoutingTable::getPathByConnections(int32 i,int32 j){
	if( dist[i][j]>=INFINITE ){
		return 0;
	}

	vns::Path* p = new vns::Path();
	p->prependNode( network->getRoad( (j<NR?j:j-NR) ) , (j<NR?vns::Forward:vns::Backward) );
	int16 k = getPrevious(i,j);
	while( k!=i && k>=0 ){
		p->prependNode( network->getRoad( (k<NR?k:k-NR) ) , (k<NR?vns::Forward:vns::Backward) );
		k = getPrevious(i,k);
	}
	p->prependNode( network->getRoad( (i<NR?i:i-NR) ) , (i<NR?vns::Forward:vns::Backward) );
	return p;
}

void MemBasedRoutingTable::printDistTable(){
	printf("DISTANCES:\n\n");
	printf("\t");
	for(int i=0;i<NR2;i++){
		printf("%d%c\t",(i<NR?i:i-NR),(i<NR?'f':'b'));
	}
	printf("\n");
	for(int i=0;i<NR2;i++){
		printf("%d%c\t",(i<NR?i:i-NR),(i<NR?'f':'b'));
		for(int j=0;j<NR2;j++){
			if(dist[i][j]>=INFINITE){
				printf("-\t");
			}else{
				printf("%d\t",dist[i][j]);
			}
		}
		printf("\n");
	}
}

void MemBasedRoutingTable::printPathTable(){
	printf("PATHS:\n\n");
	printf("\t");
	for(int i=0;i<NR2;i++){
		printf("%d%c\t",(i<NR?i:i-NR),(i<NR?'f':'b'));
	}
	printf("\n");
	for(int i=0;i<NR2;i++){
		printf("%d%c\t",(i<NR?i:i-NR),(i<NR?'f':'b'));
		for(int j=0;j<NR2;j++){
			int16 v = path[i][j];
			if( v >= 0 ){
				printf("%d%c\t",(v<NR?v:v-NR),(v<NR?'f':'b'));
			}else{
				printf("-\t");
			}
		}
		printf("\n");
	}
}

int16 MemBasedRoutingTable::connection(int16 i ,int16 j, WeightFunction* function,const VehicleType& vType){
	if(i==j) return 0;
	int32 s = (i<NR?i:i-NR);
	int32 d = (j<NR?j:j-NR);
	vns::Way sw = (i<NR?vns::Forward:vns::Backward);
	vns::Way dw = (j<NR?vns::Forward:vns::Backward);
	const vns::Road* src = network->getRoad(s);
	const vns::Road* dst = network->getRoad(d);

	if( function->isPossibleToCross(src,sw,dst,dw,vType) ){
		return function->getG(src,sw);
	}
	return 0;
}


void MemBasedRoutingTable::computeAllPaths(WeightFunction* function,const VehicleType& vType){
	/*
	 * initialize
	 */
	for(int i=0;i<NR2;i++){
		for(int j=0;j<NR2;j++){
			path[i][j] = -1;
			if(i==j){
				dist[i][j] = 0;
				continue;
			}
			int32 w = connection(i,j,function, vType);
			if( w > 0){
				dist[i][j] = w;
			}else{
				dist[i][j] = INFINITE;
			}
		}
	}

	int16** ddist = dist;
	int16** dpath = path;

	for(int k=0;k<NR2;k++){
		printf("Computing %d of %d.\n",k+1,NR2);
		clock_t tStart = clock();
		{
			for(int i=0;i<NR2;i++){
				for(int j=0;j<NR2;j++){
					if( ddist[i][k]<INFINITE && ddist[k][j]<INFINITE && ddist[i][k]+ddist[k][j] < ddist[i][j]){
						ddist[i][j] = ddist[i][k]+ddist[k][j];
						dpath[i][j] = k;
					}
				}
			}
		}
		printf("Time taken: %.4fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
	}
}

}


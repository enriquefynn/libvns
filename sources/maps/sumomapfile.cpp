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

#include "sumomapfile.h"
#include "roadnetwork.h"
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtCore/QHash>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>


namespace vns {

SUMOMapFile::SUMOMapFile() : vns::MapFile() {

}

SUMOMapFile::~SUMOMapFile() {

}

bool SUMOMapFile::load(vns::RoadNetwork*, const char*){
	return false;
}

bool SUMOMapFile::save(const vns::RoadNetwork* network, const char* fileName){
	QHash<int,bool> junctionsWithLights;
	for(int t=0;t<network->getNumberOfTrafficLights();t++){
		const vns::FixedTimeTrafficLightController* c = dynamic_cast<const vns::FixedTimeTrafficLightController*>( network->getTrafficLightController(t) );
		const vns::Junction* endJunction = c->getLane(0)->getEndJunction();
		if(!junctionsWithLights.contains(endJunction->getID()) ){
			junctionsWithLights.insert(endJunction->getID(),true);
		}
	}

	QList<QDomElement> nodesList;
	QDomDocument edgesdoc;
	QDomElement edges = edgesdoc.createElement("edges");
	edges.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	edges.setAttribute("xsi:noNamespaceSchemaLocation","http://sumo.sf.net/xsd/edges_file.xsd");
	edgesdoc.appendChild(edges);
	for(int r=0;r<network->getNumberOfRoads();r++){
		const vns::Road* road = network->getRoad(r);
		QList<const vns::Lane*> forward;
		QList<const vns::Lane*> backward;
		float fspeed = 0.0;
		float bspeed = 0.0;
		for(int l=0;l<road->getNumberOfLanes();l++){
			const vns::Lane* lane = road->getLane(l);
			if(lane->getWay()==vns::Forward){
				forward.push_front( lane );
				fspeed = vns_MAX(fspeed,lane->getMaximumSpeed());
			}else{
				backward.push_back( lane );
				bspeed = vns_MAX(bspeed,lane->getMaximumSpeed());
			}
		}
		if (forward.size()>0) {
			QDomElement edge = edgesdoc.createElement("edge");
			edge.setAttribute("id", QString("e_%1_i").arg(road->getID()));
			edge.setAttribute("numlanes", QString("%1").arg(forward.size()));
			edge.setAttribute("speed", QString("%1").arg(fspeed));
			//edge.setAttribute("length", QString("%1").arg(road->getLength()));
			edge.setAttribute("priority", "1");
			const vns::Junction* startJunction = road->getStartJunction(vns::Forward);
			if (startJunction) {
				edge.setAttribute("from", QString("j_%1").arg(startJunction->getID()));
			} else {
				edge.setAttribute("from", QString("sj_%1_i").arg(road->getID()) );
				QDomElement node = edgesdoc.createElement("node");
				node.setAttribute("id",QString("sj_%1_i").arg(road->getID()));
				node.setAttribute("x",QString("%1").arg(road->getFirstPoint(vns::Forward).getX()));
				node.setAttribute("y",QString("%1").arg(road->getFirstPoint(vns::Forward).getY()));
				node.setAttribute("type",QString("unregulated"));
				nodesList.append( node );
			}
			const vns::Junction* endJunction = road->getEndJunction(vns::Forward);
			if (endJunction) {
				edge.setAttribute("to", QString("j_%1").arg(endJunction->getID()));
			} else {
				edge.setAttribute("to", QString("ej_%1_i").arg(road->getID()) );
				QDomElement node = edgesdoc.createElement("node");
				node.setAttribute("id",QString("ej_%1_i").arg(road->getID()));
				node.setAttribute("x",QString("%1").arg(road->getLastPoint(vns::Forward).getX()));
				node.setAttribute("y",QString("%1").arg(road->getLastPoint(vns::Forward).getY()));
				node.setAttribute("type",QString("unregulated"));
				nodesList.append( node );
			}
			QString shape;
			for (int i = 0; i < road->getNumberOfPoints(); i++) {
				if (i > 0)
					shape.append(" ");
				vns::Vec p = road->getPoint(i);
				shape.append(QString("%1,%2").arg(p.getX()).arg(p.getY()));
			}
			edge.setAttribute("shape", shape);
			edges.appendChild(edge);
		}
		if (backward.size()>0) {
			QDomElement edge = edgesdoc.createElement("edge");
			edge.setAttribute("id", QString("e_%1_o").arg(road->getID()));
			edge.setAttribute("numlanes", QString("%1").arg(backward.size()));
			edge.setAttribute("speed", QString("%1").arg(bspeed));
			edge.setAttribute("priority", "1");
			const vns::Junction* startJunction = road->getStartJunction(vns::Backward);
			if (startJunction) {
				edge.setAttribute("from", QString("j_%1").arg(startJunction->getID()));
			} else {
				edge.setAttribute("from", QString("sj_%1_o").arg(road->getID()) );
				QDomElement node = edgesdoc.createElement("node");
				node.setAttribute("id",QString("sj_%1_o").arg(road->getID()));
				node.setAttribute("x",QString("%1").arg(road->getFirstPoint(vns::Backward).getX()));
				node.setAttribute("y",QString("%1").arg(road->getFirstPoint(vns::Backward).getY()));
				node.setAttribute("type",QString("unregulated"));
				nodesList.append( node );
			}
			const vns::Junction* endJunction = road->getEndJunction(vns::Backward);
			if (endJunction) {
				edge.setAttribute("to", QString("j_%1").arg(endJunction->getID()));
			} else {
				edge.setAttribute("to", QString("ej_%1_o").arg(road->getID()) );
				QDomElement node = edgesdoc.createElement("node");
				node.setAttribute("id",QString("ej_%1_o").arg(road->getID()));
				node.setAttribute("x",QString("%1").arg(road->getLastPoint(vns::Backward).getX()));
				node.setAttribute("y",QString("%1").arg(road->getLastPoint(vns::Backward).getY()));
				node.setAttribute("type",QString("unregulated"));
				nodesList.append( node );
			}
			QString shape;
			for (int i = road->getNumberOfPoints()-1; i >= 0; i--) {
				if (i < road->getNumberOfPoints()-1)
					shape.append(" ");
				vns::Vec p = road->getPoint(i);
				shape.append(QString("%1,%2").arg(p.getX()).arg(p.getY()));
			}
			edge.setAttribute("shape", shape);
			edges.appendChild(edge);
		}
	}
	QString edgesfilename = QString("%1.edg.xml").arg(fileName);
	QFile edgesfile(edgesfilename);
	if( edgesfile.open(QIODevice::WriteOnly) ){
		QTextStream out(&edgesfile);
		edgesdoc.save(out, 4);
		edgesfile.close();
	}else{
		return false;
	}


	QDomDocument nodesdoc;
	QDomElement nodes = nodesdoc.createElement("nodes");
	nodes.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	nodes.setAttribute("xsi:noNamespaceSchemaLocation","http://sumo.sf.net/xsd/nodes_file.xsd");
	nodesdoc.appendChild(nodes);
	for(int j=0;j<network->getNumberOfJunctions();j++){
		const vns::Junction* junction = network->getJunction(j);
		vns::Vec p = junction->getPosition();
		QDomElement node = nodesdoc.createElement("node");
		node.setAttribute("id",QString("j_%1").arg(junction->getID()));
		node.setAttribute("x",QString("%1").arg(p.getX()));
		node.setAttribute("y",QString("%1").arg(p.getY()));
		if(junctionsWithLights.contains(junction->getID()) ){
			node.setAttribute("type",QString("traffic_light"));
		}
		node.setAttribute("type",QString("unregulated"));
		nodes.appendChild( node );
	}

	for(int j=0;j<nodesList.size();j++){
		nodes.appendChild( nodesList.at(j) );
	}

	QString nodesfilename = QString("%1.nod.xml").arg(fileName);
	QFile nodesfile(nodesfilename);
	if( nodesfile.open(QIODevice::WriteOnly) ){
		QTextStream out(&nodesfile);
		nodesdoc.save(out, 4);
		nodesfile.close();
	}else{
		return false;
	}

	QDomDocument connsdoc;
	QDomElement conns = connsdoc.createElement("connections");
	conns.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	conns.setAttribute("xsi:noNamespaceSchemaLocation","http://sumo.sf.net/xsd/connections_file.xsd");
	connsdoc.appendChild(conns);
	for(int r=0;r<network->getNumberOfRoads();r++){
		const vns::Road* road = network->getRoad(r);
		const vns::Junction* junction = road->getEndJunction(vns::Forward);
		if(junction && road->hasLanes(vns::Forward) ){
			for(int i=0;i<junction->getNumberOfRoads();i++){
				const vns::Road* dest = junction->getRoad(i);
				if(dest==road) continue;
				if(road->hasConnectionTo(dest)){
					QDomElement conn = connsdoc.createElement("connection");
					conn.setAttribute("from",QString("e_%1_i").arg(road->getID()));
					if( dest->getStartJunction(vns::Forward) == junction ){
						conn.setAttribute("to",QString("e_%1_i").arg(dest->getID()));
					}else{
						conn.setAttribute("to",QString("e_%1_o").arg(dest->getID()));
					}
					conns.appendChild( conn );
				}
			}
		}
		junction = road->getEndJunction(vns::Backward);
		if(junction && road->hasLanes(vns::Backward)){
			for(int i=0;i<junction->getNumberOfRoads();i++){
				const vns::Road* dest = junction->getRoad(i);
				if(dest==road) continue;
				if(road->hasConnectionTo(dest)){
					QDomElement conn = connsdoc.createElement("connection");
					conn.setAttribute("from",QString("e_%1_o").arg(road->getID()));
					if( dest->getStartJunction(vns::Forward) == junction ){
						conn.setAttribute("to",QString("e_%1_i").arg(dest->getID()));
					}else{
						conn.setAttribute("to",QString("e_%1_o").arg(dest->getID()));
					}
					conns.appendChild( conn );
				}
			}
		}
	}
	QString connsfilename = QString("%1.con.xml").arg(fileName);
	QFile connsfile(connsfilename);
	if( connsfile.open(QIODevice::WriteOnly) ){
		QTextStream out(&connsfile);
		connsdoc.save(out, 4);
		connsfile.close();
	}else{
		return false;
	}

	// TRAFFIC LIGHTS

	/*

	QDomDocument tlsdoc;
	QDomElement tls = tlsdoc.createElement("additional");
	tlsdoc.appendChild(tls);

	for(int t=0;t<network->getNumberOfTrafficLights();t++){
		vns::FixedTimeTrafficLightController* c = dynamic_cast<vns::FixedTimeTrafficLightController*>( network->getTrafficLightController(t) );
		const vns::Junction* endJunction = c->getLane(0)->getEndJunction();
		QDomElement tl = tlsdoc.createElement("tlLogic");
		tl.setAttribute("id",QString("j_%1").arg(endJunction->getID()));
		tl.setAttribute("type","static");
		tl.setAttribute("programID","1");
		tl.setAttribute("offset",QString("%1").arg(c->getTimeOffset()));
		tls.appendChild( tl );

		QDomElement st = tlsdoc.createElement("phase");
		st.setAttribute("duration","20");
		st.setAttribute("state","GGGrrrGGGrrr");
		tl.appendChild( st );

		st = tlsdoc.createElement("phase");
		st.setAttribute("duration","5");
		st.setAttribute("state","yyyrrryyyrrr");
		tl.appendChild( st );

		st = tlsdoc.createElement("phase");
		st.setAttribute("duration","20");
		st.setAttribute("state","rrrGGGrrrGGG");
		tl.appendChild( st );

		st = tlsdoc.createElement("phase");
		st.setAttribute("duration","5");
		st.setAttribute("state","rrryyyrrryyy");
		tl.appendChild( st );
	}



	QString tlsfilename = QString("%1.tls.xml").arg(name);
	QFile tlsfile(tlsfilename);
	if( tlsfile.open(QIODevice::WriteOnly) ){
		QTextStream out(&tlsfile);
		tlsdoc.save(out, 4);
		tlsfile.close();
	}else{
		return false;
	}*/

	return true;
}

}

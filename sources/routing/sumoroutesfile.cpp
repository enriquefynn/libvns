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

#include "sumoroutesfile.h"
#include "routes.h"
#include "vehiclefactory.h"
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>
#include <QtCore/QtCore>

namespace vns {

SUMORoutesFile::SUMORoutesFile() : vns::RoutesFile() {

}

SUMORoutesFile::~SUMORoutesFile() {

}

bool SUMORoutesFile::load(vns::Routes* , const char* ) {
	return false;
}

bool SUMORoutesFile::save(const vns::Routes* routes, const char* fileName) {
	QDomDocument doc;
	QDomElement docElem = doc.createElement("routes");
	docElem.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
	docElem.setAttribute("xsi:noNamespaceSchemaLocation","http://sumo.sf.net/xsd/routes_file.xsd");
	doc.appendChild(docElem);
	QDomElement xmlvtype = doc.createElement("vType");
	xmlvtype.setAttribute("id","vA");
	xmlvtype.setAttribute("accel","3.0");
	xmlvtype.setAttribute("decel","6.0");
	xmlvtype.setAttribute("length","5.0");
	xmlvtype.setAttribute("minGap","4.0");
	xmlvtype.setAttribute("maxSpeed","50");
	xmlvtype.setAttribute("sigma","0.5");
	docElem.appendChild(xmlvtype);

	for(int r=0;r<routes->getNumberOfScheduledRoutes();r++){
		const vns::ScheduledRoute* route = routes->getScheduledRoute(r);
		QDomElement xmlroute = doc.createElement("route");
		xmlroute.setAttribute("id",QString("rs%1").arg(r));
		/*QString types;
		for(int i = 0; i < route->getNumberOfVehicleTypes(); i++) {
			if (i > 0)
				types.append(",");
			types.append(QString("%1").arg(vns::VehicleFactory::getVehicleNameByType(route->getVehicleType(i))));
		}
		if( types.size()>0){
			xmlroute.setAttribute("vehicles", types);
		}*/
		QString edges;
		const vns::Path* path = route->getPath();
		vns::Path::ConstIterator it = path->getFirstNodeConstIterator();
		int i=0;
		while( it.isValid() ){
			const vns::Road* road = it.getRoad();
			vns::Way way = it.getWay();
			QString str = QString("e_%1_%2").arg(road->getID()).arg(way==vns::Forward?"i":"o");
			vns::PathNode::StopsIterator st = it.getStopsIterator();
			while(st.isValid()){
				QDomElement xmlstop = doc.createElement("stop");
				xmlstop.setAttribute("lane",str);
				xmlstop.setAttribute("endPos",st.getPosition());
				xmlstop.setAttribute("duration",st.getDuration());
				xmlroute.appendChild(xmlstop);
				st.next();
			}
			if (i > 0)
				edges.append(" ");
			edges.append( str );
			it.next();
			i++;
		}
		xmlroute.setAttribute("edges", edges);
		docElem.appendChild(xmlroute);
		QDomElement xmlvehicle = doc.createElement("vehicle");
		xmlvehicle.setAttribute("type","vA");
		xmlvehicle.setAttribute("id",QString("veh%1").arg(r));
		xmlvehicle.setAttribute("route",QString("rs%1").arg(r));
		xmlvehicle.setAttribute("depart", QString("%1").arg(route->getScheduledTime()) );
		docElem.appendChild(xmlvehicle);
	}

	for(int r=0;r<routes->getNumberOfRoutes();r++){
		const vns::Route* route = routes->getRoute(r);
		QDomElement xmlroute = doc.createElement("route");
		xmlroute.setAttribute("id",QString("rd%1").arg(r));
		QString edges;
		const vns::Path* path = route->getPath();
		vns::Path::ConstIterator it = path->getFirstNodeConstIterator();
		int i=0;
		while( it.isValid() ){
			const vns::Road* road = it.getRoad();
			vns::Way way = it.getWay();
			QString str = QString("e_%1_%2").arg(road->getID()).arg(way==vns::Forward?"i":"o");
			vns::PathNode::StopsIterator st = it.getStopsIterator();
			while(st.isValid()){
				QDomElement xmlstop = doc.createElement("stop");
				xmlstop.setAttribute("lane",str);
				xmlstop.setAttribute("endPos",st.getPosition());
				xmlstop.setAttribute("duration",st.getDuration());
				xmlroute.appendChild(xmlstop);
				st.next();
			}
			if (i > 0)
				edges.append(" ");
			edges.append( str );
			it.next();
			i++;
		}
		xmlroute.setAttribute("edges", edges);
		docElem.appendChild(xmlroute);
		QDomElement xmlflow = doc.createElement("flow");
		xmlflow.setAttribute("type","vA");
		xmlflow.setAttribute("id",QString("f%1").arg(r));
		xmlflow.setAttribute("begin",QString("%1").arg(route->getStartTime()));
		if(route->getEndTime()>0){
			xmlflow.setAttribute("end",QString("veh%1").arg(route->getEndTime()));
		}
		xmlflow.setAttribute("period",QString("%1").arg(route->getInterval()) );
		xmlflow.setAttribute("route",QString("rd%1").arg(r));
		docElem.appendChild(xmlflow);
	}

	QString routesfilename = QString("%1.rou.xml").arg(fileName);
	QFile file(routesfilename);
	if( file.open(QIODevice::WriteOnly) ){
		QTextStream out(&file);
		doc.save(out, 4);
		file.close();
	}else{
		return false;
	}
	return true;
}


}

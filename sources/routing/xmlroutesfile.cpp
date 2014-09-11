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

#include "xmlroutesfile.h"
#include "routes.h"
#include "vehiclefactory.h"
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>
#include <QtCore/QtCore>

namespace vns {

XmlRoutesFile::XmlRoutesFile() : vns::RoutesFile() {

}

XmlRoutesFile::~XmlRoutesFile() {

}

QString createXMLValue(float* v_min, float* v_max){
        VNS_ASSERT( *v_min<=*v_max ,"XML Error","Invalid range value in <drivers>");
        if(*v_min==*v_max){
                return QString("%1").arg(*v_min);
        }
        return QString("%1-%2").arg(*v_min).arg(*v_max);
}

void parseXMLValue(QDomElement& e, QString attr, float* v_min, float* v_max){
        if(!e.hasAttribute(attr)) return;
        QString invalid = QString("Invalid value for '%1' attribute in <driver>").arg(attr);

        bool oka,okb;
        QStringList list = e.attribute(attr).trimmed().split("-");
        VNS_ASSERT( list.size()>0 || list.size()<=2 ,"XML Error", invalid.toAscii().constData());
        double a = list.at(0).trimmed().toDouble(&oka);
        double b = a;
        VNS_ASSERT( oka,"XML Error",invalid.toAscii().constData());
        if(list.size()==2){
                b = list.at(1).trimmed().toDouble(&okb);
                VNS_ASSERT( okb,"XML Error",invalid.toAscii().constData());
        }
        *v_min = a; *v_max = b;
        VNS_ASSERT( *v_min<=*v_max ,"XML Error","Invalid range value in <drivers>");
}

bool XmlRoutesFile::load(vns::Routes* routes, const char* fileName) {
	bool ok;
	vns::RoadNetwork* network = routes->getRoadNetwork();

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}
	QDomDocument doc;
	if (!doc.setContent(&file)) {
		return false;
	}

	routes->clear();

	QDomElement docElem = doc.documentElement();
	QString rootTag = docElem.tagName();
	VNS_ASSERT( rootTag=="routes" ,"XML Error","Root tag should be <routes>");

	/* DRIVERS */

	QHash<QString,int32> driversIDs;

	QDomNodeList driverList = docElem.elementsByTagName("driver");
	for(int i = 0;i < driverList.count(); i++){
		QDomElement e = driverList.at(i).toElement();
		vns::DriverInfo driver;

		VNS_ASSERT( e.hasAttribute("id") ,"XML Error","Missing attribute 'id' in <driver>");
		QString driverKey = e.attribute("id").toLower();

		VNS_ASSERT( driversIDs.contains(driverKey)==false ,"XML Error","Attribute 'id' already exists - <driver>");
		VNS_ASSERT( e.hasAttribute("model") ,"XML Error","Missing attribute 'model' in <driver>");


		QString model = e.attribute("model").toLower();
		driver.model = vns::DriverModel::getDriverModel(model.toAscii().data());
		VNS_ASSERT( driver.model!=vns::DriverModel::INVALID ,"XML Error","Invalid attribute 'model' in <driver>");

		parseXMLValue(e,"a",&driver.a_min,&driver.a_max);
		parseXMLValue(e,"b",&driver.b_min,&driver.b_max);
		parseXMLValue(e,"v0",&driver.v0_min,&driver.v0_max);
		parseXMLValue(e,"s0",&driver.s0_min,&driver.s0_max);
		parseXMLValue(e,"s1",&driver.s1_min,&driver.s1_max);
		parseXMLValue(e,"T",&driver.T_min,&driver.T_max);
		parseXMLValue(e,"delta",&driver.delta_min,&driver.delta_max);

		parseXMLValue(e,"coolness",&driver.coolness_min,&driver.coolness_max);

		parseXMLValue(e,"beta",&driver.beta_min,&driver.beta_max);
		parseXMLValue(e,"tau",&driver.tau_min,&driver.tau_max);
		parseXMLValue(e,"lambda",&driver.lambda_min,&driver.lambda_max);
		parseXMLValue(e,"tWidth",&driver.tWidth_min,&driver.tWidth_max);
		//KKW
		parseXMLValue(e,"k",&driver.k_min,&driver.k_max);
		parseXMLValue(e,"pb0",&driver.pb0_min,&driver.pb0_max);
		parseXMLValue(e,"pb1",&driver.pb1_min,&driver.pb1_max);
		parseXMLValue(e,"pa1",&driver.pa1_min,&driver.pa1_max);
		parseXMLValue(e,"pa2",&driver.pa2_min,&driver.pa2_max);
		parseXMLValue(e,"vp",&driver.vp_min,&driver.vp_max);
		//NAGEL
		parseXMLValue(e,"pSlowDown",&driver.pSlowDown_min,&driver.pSlowDown_max);
		parseXMLValue(e,"pSlowToStart",&driver.pSlowToStart_min,&driver.pSlowToStart_max);
		//KRAUSS
		parseXMLValue(e,"epsilon",&driver.epsilon_min,&driver.epsilon_max);
		/* MOBIL */
		parseXMLValue(e,"politiness",&driver.politiness_min,&driver.politiness_max);
		parseXMLValue(e,"bsave",&driver.bsave_min,&driver.bsave_max);
		parseXMLValue(e,"athr",&driver.athr_min,&driver.athr_max);
		parseXMLValue(e,"deltab",&driver.deltab_min,&driver.deltab_max);

		int32 driverID = routes->registerDriver( driver );
		driversIDs.insert(driverKey,driverID);
	}

	/* */

	QHash<QString,int32> paths;

	QDomNodeList pathList = docElem.elementsByTagName("path");
	for (int i = 0; i < pathList.count(); i++) {
		QDomElement e = pathList.at(i).toElement();
		VNS_ASSERT( e.hasAttribute("id") ,"XML Error", "Missing 'nodes' attribute in <path>");
		VNS_ASSERT( e.hasAttribute("nodes") ,"XML Error", "Missing 'nodes' attribute in <path>");
		QString pathID = e.attribute("id");
		QString nodes = e.attribute("nodes");

		bool loop = false;
		if( e.hasAttribute("loop") ){
			loop = (e.attribute("loop","false")=="true"?true:false);
		}

		QStringList list = nodes.trimmed().split(",");
		VNS_ASSERT( list.size()>0 ,"XML Error","Invalid value for 'nodes' attribute in <path>");
		vns::Path* path = new vns::Path();
		vns::Road* proad = 0;
		vns::Way pway = vns::Forward;
		for(int i=0;i<list.size();i++){
			QString n = list.at(i).trimmed();
			QStringList ln = n.split("-");
			int32 rid = ln.at(0).trimmed().toInt(&ok);
			QString w = ln.at(1).trimmed();
			VNS_ASSERT( ok && rid>=0 && rid<network->getNumberOfRoads() && (w=="f" || w=="b"),"xXML Error","Invalid value for 'nodes' attribute in <path>");
			vns::Way way = (w=="f"?vns::Forward:vns::Backward);
			vns::Road* road = network->getRoad(rid);
			if(!road->hasLanes(way)){
				fprintf(stderr,"XML Error: Invalid 'path' -> road %d has no '%s' lanes.\n",road->getID(),(way==vns::Forward?"f":"b"));
				exit(-1);
			}
			if(proad!=0){
				if(proad==road && pway!=way){

				}else
				if(!proad->hasConnectionTo(pway,road)){
					fprintf(stderr,"XML Error: Invalid 'path' -> %d-%s does not have connection to %d-%s.\n",proad->getID(),(pway==vns::Forward?"f":"b"),road->getID(),(way==vns::Forward?"f":"b"));
					exit(-1);
				}
			}
			path->appendNode(road, way);
			proad = road;
			pway = way;
		}
		VNS_ASSERT( path->getNumberOfNodes()>0 ,"XML Error","Invalid 'nodes' attribute in <path>");

		/* Stops */
		QDomNodeList stopsList = e.elementsByTagName("stop");
		for(int s = 0;s < stopsList.count(); s++){
			QDomElement estop = stopsList.at(s).toElement();
			vns::RoadNetwork* network = routes->getRoadNetwork();
			VNS_ASSERT( estop.hasAttribute("node") ,"XML Error", "Missing 'node' attribute in <stop>");
			VNS_ASSERT( estop.hasAttribute("duration") ,"XML Error", "Missing 'duration' attribute in <stop>");
			VNS_ASSERT( estop.hasAttribute("position") ,"XML Error", "Missing 'position' attribute in <stop>");

			double duration = estop.attribute("duration","").trimmed().toLower().toDouble(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid value for 'duration' attribute in <stop>");
			double position = estop.attribute("position","").trimmed().toLower().toDouble(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid value for 'position' attribute in <stop>");

			vns::PathStop::Type stopType = vns::PathStop::RightMostLane;
			if( estop.hasAttribute("type") ){
				const char* typeName = estop.attribute("type").trimmed().toLower().toAscii().constData();
				stopType = vns::PathStop::fromTypeName(typeName);
				VNS_ASSERT( stopType>=0 ,"XML Error","Invalid value for 'type' attribute in <stop>");
			}

			QString n = estop.attribute("node").trimmed().toLower();
			QStringList ln = n.split("-");
			VNS_ASSERT( ln.size()==2 ,"XML Error","Invalid value for 'node' attribute in <stop>");
			int32 rid = ln.at(0).trimmed().toInt(&ok);
			VNS_ASSERT( ok && rid>=0 && rid<network->getNumberOfRoads(),"XML Error","Invalid value for 'node' attribute in <stop>");
			QString w = ln.at(1).trimmed();
			VNS_ASSERT( w=="f" || w=="b","XML Error","Invalid value for 'node' attribute in <stop>");
			vns::Way way = (w=="f"?vns::Forward:vns::Backward);
			vns::Road* road = network->getRoad(rid);

			vns::Path::Iterator it = path->getFirstNodeIterator();
			while( it.isValid() ){
				vns::PathNode* node = it.get();
				if( node->getRoad()==road && node->getWay()==way ){
					bool add = node->addStop(stopType,position,duration);
					VNS_ASSERT( add ,"XML Error","Invalid <stop> attribute in <path> (roadStart < position < roadEnd)");
				}
				it.next();
			}
		}
		path->setLoop(loop);
		int32 pid = routes->addPath( path );
		paths.insert(pathID,pid);
	}

	QDomNodeList routeList = docElem.elementsByTagName("route");
	for (int i = 0; i < routeList.count(); i++) {
		QDomElement eroute = routeList.at(i).toElement();

		VNS_ASSERT( eroute.hasAttribute("path") ,"XML Error","Missing attribute 'path' - <route>");
		QString pid = eroute.attribute("path").trimmed();
		VNS_ASSERT( paths.contains(pid) ,"XML Error","Invalid 'path' attribute - <route>");
		int32 pathID = paths.value(pid);

		VehicleType vType = vns::CAR;
		if( eroute.hasAttribute("vehicle") ){
			const char* vTypeName = eroute.attribute("vehicle").trimmed().toLower().toAscii().constData();
			vType = vns::VehicleFactory::fromVehicleTypeName(vTypeName);
			VNS_ASSERT( vType>=0 ,"XML Error","Invalid 'vehicle' attribute - <route>");
		}
		int32 driverID = -1;
		if( eroute.hasAttribute("driver") ){
			QString driverKey = eroute.attribute("driver").toLower();
			VNS_ASSERT( driversIDs.contains(driverKey) ,"XML Error","Invalid 'driver' attribute - <route>");
			driverID = driversIDs.value(driverKey);
		}

		if( eroute.hasAttribute("depart") ){
			bool ok;
			double depart = eroute.attribute("depart","").trimmed().toLower().toDouble(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid value for 'depart' attribute in <route>");
			routes->addScheduledRoute(pathID,depart,vType,driverID);
		}else{
			double interval = -1.0;
			if( eroute.hasAttribute("frequency") ){
				bool oka,okb;
				int32 cars = 0;
				double seconds = 0.0;
				QStringList list = eroute.attribute("frequency","0/0").trimmed().toLower().split("/");
				VNS_ASSERT( list.size()==2 ,"XML Error","Invalid value for 'frequency' attribute in <route>");
				cars = list.at(0).trimmed().toLower().toInt(&oka);
				seconds = list.at(1).trimmed().toLower().toDouble(&okb);
				VNS_ASSERT( oka && okb && cars>0 && seconds>0.0,"XML Error","Invalid value for 'frequency' attribute in <route>");
				interval = double(cars)/seconds;
				VNS_ASSERT( interval>0.0 ,"XML Error","Invalid value for 'frequency' attribute in <route>");
			}else{
				if( eroute.hasAttribute("interval") ){
					bool ok;
					interval = eroute.attribute("interval","").trimmed().toLower().toDouble(&ok);
					VNS_ASSERT( ok && interval>0.0 ,"XML Error","Invalid value for 'interval' attribute in <route>");
				}else{
					VNS_ASSERT( false ,"XML Error","Missing 'frequency','interval' or 'depart' attribute in <route>");
				}
			}

			bool startTime = 0.0;
			bool endTime = 0.0;
			if( eroute.hasAttribute("starttime") ){
				bool ok;
				startTime = eroute.attribute("starttime","").trimmed().toLower().toDouble(&ok);
				VNS_ASSERT( ok && startTime>=0.0,"XML Error","Invalid value for 'startTime' attribute in <route>");
			}
			if( eroute.hasAttribute("endtime") ){
				bool ok;
				endTime = eroute.attribute("endTime","").trimmed().toLower().toDouble(&ok);
				VNS_ASSERT( ok  && endTime>=0.0,"XML Error","Invalid value for 'endTime' attribute in <route>");
			}
			routes->addRoute(pathID,interval,vType,driverID,startTime,endTime);
		}

	}
	return true;
}

bool XmlRoutesFile::save(const vns::Routes* routes, const char* fileName) {
	QDomDocument doc;
	QDomElement docElem = doc.createElement("routes");
	doc.appendChild(docElem);

	for(int p=0;p<routes->getNumberOfPaths();p++){
		const vns::Path* path = routes->getPath(p);
		QDomElement xmlpath = doc.createElement("path");
		xmlpath.setAttribute("id", QString("%1").arg(path->getID()) );
		if( path->isLoop() ) xmlpath.setAttribute("loop", "true");
		vns::Path::ConstIterator it = path->getFirstNodeConstIterator();
		int i=0;
		QString nodes;
		while( it.isValid() ){
			const vns::Road* road = it.getRoad();
			vns::Way way = it.getWay();
			if (i > 0)
				nodes.append(",");
			nodes.append(QString("%1-%2").arg(road->getID()).arg((way==vns::Forward?"f":"b")));
			vns::PathNode::StopsIterator st = it.getStopsIterator();
			while( st.isValid() ){
				QDomElement xmlstop = doc.createElement("stop");
				xmlstop.setAttribute("position", QString("%1").arg(st.getPosition()) );
				xmlstop.setAttribute("duration", QString("%1").arg(st.getDuration()) );
				xmlstop.setAttribute("type", QString("%1").arg(vns::PathStop::fromType(st.getType())) );
				xmlstop.setAttribute("node", QString("%1-%2").arg(road->getID()).arg((way==vns::Forward?"f":"b")) );
				xmlpath.appendChild(xmlstop);
				st.next();
			}
			it.next();
			i++;
		}
		xmlpath.setAttribute("nodes", nodes );
		docElem.appendChild(xmlpath);
	}


	for(int r=0;r<routes->getNumberOfScheduledRoutes();r++){
		const vns::ScheduledRoute* route = routes->getScheduledRoute(r);
		const char* vTypeName = vns::VehicleFactory::fromVehicleType(route->getVehicleType());
		QDomElement xmlroute = doc.createElement("route");
		xmlroute.setAttribute("path", QString("%1").arg(route->getPath()->getID()) );
		xmlroute.setAttribute("depart", QString("%1").arg(route->getScheduledTime()) );
		xmlroute.setAttribute("vehicle", QString("%1").arg(vTypeName) );
		docElem.appendChild(xmlroute);
	}

	for(int r=0;r<routes->getNumberOfRoutes();r++){
		const vns::Route* route = routes->getRoute(r);
		const char* vTypeName = vns::VehicleFactory::fromVehicleType(route->getVehicleType());
		QDomElement xmlroute = doc.createElement("route");
		xmlroute.setAttribute("path", QString("%1").arg(route->getPath()->getID()) );
		xmlroute.setAttribute("interval", QString("%1").arg(route->getInterval()) );
		if(route->getStartTime()>0.0) xmlroute.setAttribute("starttime", QString("%1").arg(route->getStartTime()) );
		if(route->getEndTime()>0.0) xmlroute.setAttribute("endtime", QString("%1").arg(route->getEndTime()) );
		xmlroute.setAttribute("vehicle", QString("%1").arg(vTypeName) );
		docElem.appendChild(xmlroute);
	}

	QFile file(fileName);
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

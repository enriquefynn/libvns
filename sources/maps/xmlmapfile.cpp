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

#include "xmlmapfile.h"
#include "roadnetwork.h"
#include "mercartor.h"
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>
#include <QtCore/QtCore>

class XMLLane {
    public:
    	QString id;
    	bool forward;
    	float maxSpeed;
    	float minSpeed;
    	double width;
    	double startoffset;
    	double endoffset;
    	bool buses;
    	bool cars;
    	bool bikes;
    	bool trucks;
    	bool rails;
    	bool pedestrians;
    	bool stop;
    	bool yield;
    	bool changeleft;
    	bool changeright;
    	bool access;
    	QList< QString > connections;
};

bool getBoolValue(QDomElement&e, const QString& attr, bool defaultValue,const char* error){
	if(!e.hasAttribute(attr)){
		return defaultValue;
	}
	QString v = e.attribute(attr);
	if(v=="false" || v=="no" || v=="0"){
		return false;
	}
	if(v=="true" || v=="yes" || v=="1"){
		return true;
	}
	VNS_ASSERT( false ,"OSM Error",error);
	return false;
}

void parseXMLFixedTrafficLight(vns::RoadNetwork* network,QDomElement& e, QHash<QString,vns::Road*>& roadIDs){
	vns::FixedTimeTrafficLightController* ctl = dynamic_cast<vns::FixedTimeTrafficLightController*>(network->createTrafficLightController(vns::TrafficLightController::FixedTime));
	bool ok;
	double offset = e.attribute("offset","0.0").toDouble(&ok);
	VNS_ASSERT( ok ,"XML Error","Invalid 'offset' attribute in <tl>");
	ctl->setTimeOffset(offset);
	QDomNodeList lightsList = e.elementsByTagName("light");
	for(int i = 0;i < lightsList.count(); i++){
		QDomElement lighte = lightsList.at(i).toElement();
		VNS_ASSERT( lighte.hasAttribute("road") ,"XML Error","Missing attribute 'road' in <light>");
		VNS_ASSERT( lighte.hasAttribute("lane") ,"XML Error","Missing attribute 'lane' in <light>");
		QString rid = lighte.attribute("road");
		VNS_ASSERT( roadIDs.contains(rid) ,"XML Error","Invalid 'road' attribute in <light>");
		int lid = lighte.attribute("lane").toInt(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'lane' attribute in <light>");
		vns::Road* road = roadIDs.value(rid);
		VNS_ASSERT( lid>= 0 && lid<road->getNumberOfLanes() ,"XML Error","Invalid 'lane' attribute in <light>");
		vns::Lane* lane = road->getLane(lid);
		ctl->installLight(lane);
	}
	QDomNodeList statesList = e.elementsByTagName("state");
	for(int i = 0;i < statesList.count(); i++){
		QDomElement ste = statesList.at(i).toElement();
		VNS_ASSERT( ste.hasAttribute("duration") ,"XML Error","Missing attribute 'duration' in <state>");
		VNS_ASSERT( ste.hasAttribute("lights") ,"XML Error","Missing attribute 'lights' in <state>");
		float duration = ste.attribute("duration").toFloat(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'duration' attribute in <state>");
		QString lights = ste.attribute("lights");
		VNS_ASSERT( lights.size()== ctl->getNumberOfLights() ,"XML Error","Invalid number of 'lights' in <state>");
		vns::Vector<vns::Light> vl;
		for(int l=0;l<lights.size();l++){
			if(lights.at(l)=='R') vl.append(vns::RedLight);
			else if(lights.at(l)=='Y') vl.append(vns::YellowLight);
			else if(lights.at(l)=='G') vl.append(vns::GreenLight);
			else VNS_ASSERT( false ,"XML Error","Invalid 'lights' attribute in <state>");
		}
		ctl->addState(duration,vl);
	}
}


vns::Road* parseXMLRoad(vns::RoadNetwork* network,QDomElement& e, QHash<QString,vns::Road*>& roadIDs, QHash<vns::Lane*,QStringList>& laneConnections){
	VNS_ASSERT( e.hasAttribute("id") ,"XML Error", "Root tag should be <roadnetwork>");
	VNS_ASSERT( e.hasAttribute("type") ,"XML Error","Root tag should be <roadnetwork>");
	VNS_ASSERT( e.hasAttribute("polyline") ,"XML Error","Root tag should be <roadnetwork>");
	QString id = e.attribute("id");
	VNS_ASSERT( !roadIDs.contains(id) ,"XML Error","Value for 'id' already exists - <road>");

	vns::Road::Type roadType = vns::Road::Undefined;
	if(e.hasAttribute("type")){
		roadType = vns::Road::fromTypeName( e.attribute("type").toAscii().data() );
	}

	bool lparking = getBoolValue(e,"leftparking",false,"Invalid 'leftparking' attribute in <road>");
	bool rparking = getBoolValue(e,"rightparking",false,"Invalid 'rightparking' attribute in <road>");
	bool lstand = getBoolValue(e,"lefttaxistand",false,"Invalid 'lefttaxistand' attribute in <road>");
	bool rstand = getBoolValue(e,"righttaxistand",false,"Invalid 'righttaxistand' attribute in <road>");
	bool scross = getBoolValue(e,"startcrosswalk",false,"Invalid 'startcrosswalk' attribute in <road>");
	bool ecross = getBoolValue(e,"endcrosswalk",false,"Invalid 'endcrosswalk' attribute in <road>");
	float maxSpeed = 50.0;
	float minSpeed = 0.0;
	float fl = 1;
	float bl = 0;
	if( e.hasAttribute("maxspeed") ){
		bool ok;
		float v = e.attribute("maxspeed").toFloat(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'maxspeed' attribute in <road>");
		maxSpeed = v;
	}
	if( e.hasAttribute("minspeed") ){
		bool ok;
		float v = e.attribute("minspeed").toFloat(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'minspeed' attribute in <road>");
		minSpeed = v;
	}
	if( e.hasAttribute("lanes:forward") ){
		bool ok;
		int v = e.attribute("lanes:forward").toInt(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'lanes:forward' attribute in <road>");
		VNS_ASSERT( v>0 ,"XML Error","Attribute 'lanes:forward' must be > 0 in <road>");
		fl = v;
	}
	if( e.hasAttribute("lanes:backward") ){
		bool ok;
		int v = e.attribute("lanes:backward").toInt(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'lanes:backward' attribute in <road>");
		bl = v;
	}
	VNS_ASSERT( bl>0 || fl>0 ,"XML Error","value 0 for 'lanes:backward' and 'lanes:forward' attributes in <road>");

	QStringList pts = e.attribute("polyline").split(' ');
	vns::Vector<vns::Vec> points;
	for(int c=0;c<pts.size();c++){
		QStringList pt = pts.at(c).split(',');
		VNS_ASSERT( pt.size()==2 || pt.size()==3,"XML Error","Invalid 'polyline' attribute in <road>");
		bool ok;
		double x =  pt.at(0).toDouble(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'polyline' attribute in <road>");
		double y =  pt.at(1).toDouble(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'polyline' attribute in <road>");
		double z = 0;
		if( pt.size()==3 ){
			z =  pt.at(2).toDouble(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid 'polyline' attribute in <road>");
		}
		points.append( vns::Vec(x,y,z) );
	}
	VNS_ASSERT( points.size()>=2 ,"XML Error","Invalid 'polyline' attribute in <road>");

	QDomNodeList laneList = e.elementsByTagName("lane");
	QList< XMLLane > flanes;
	QList< XMLLane > blanes;
	for(int j = 0;j < laneList.count(); j++){
		QDomElement el = laneList.at(j).toElement();
		XMLLane lane;
		lane.forward = getBoolValue(el,"forward",true,"Invalid 'forward' attribute in <road>");
		float width = 3.0;
		float start = 0.0;
		float end = vns::MAX_FLOAT;

		if( el.hasAttribute("maxspeed") ){
			bool ok;
			float v = el.attribute("maxspeed").toFloat(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid 'maxspeed' attribute in <lane>");
			maxSpeed = v;
		}
		lane.maxSpeed = maxSpeed;
		if( el.hasAttribute("minspeed") ){
			bool ok;
			float v = el.attribute("minspeed").toFloat(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid 'minspeed' attribute in <lane>");
			minSpeed = v;
		}
		lane.minSpeed = minSpeed;
		if( el.hasAttribute("width") ){
			bool ok;
			float v = el.attribute("width").toFloat(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid 'width' attribute in <lane>");
			width = v;
		}
		lane.width = width;
		if( el.hasAttribute("startoffset") ){
			bool ok;
			float v = el.attribute("startoffset").toFloat(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid 'startoffset' attribute in <lane>");
			start = v;
		}
		lane.startoffset = start;
		if( el.hasAttribute("endoffset") ){
			bool ok;
			float v = el.attribute("endoffset").toFloat(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid 'endoffset' attribute in <lane>");
			end = v;
		}
		lane.endoffset = end;

		lane.buses = getBoolValue(el,"buses",true,"Invalid 'buses' attribute in <road>");
		lane.cars = getBoolValue(el,"cars",true,"Invalid 'cars' attribute in <road>");
		lane.bikes = getBoolValue(el,"bikes",true,"Invalid 'bikes' attribute in <road>");
		lane.trucks = getBoolValue(el,"trucks",true,"Invalid 'trucks' attribute in <road>");
		lane.rails = getBoolValue(el,"rails",false,"Invalid 'rails' attribute in <road>");
		lane.pedestrians = getBoolValue(el,"pedestrians",false,"Invalid 'pedestrians' attribute in <road>");
		lane.stop = getBoolValue(el,"stop",false,"Invalid 'stop' attribute in <road>");
		lane.yield = getBoolValue(el,"yield",false,"Invalid 'yield' attribute in <road>");
		lane.changeleft = getBoolValue(el,"changeleft",true,"Invalid 'changeleft' attribute in <road>");
		lane.changeright = getBoolValue(el,"changeright",true,"Invalid 'changeright' attribute in <road>");
		lane.access = getBoolValue(el,"access",true,"Invalid 'access' attribute in <road>");

		if( el.hasAttribute("connections") ){
			QStringList conns = el.attribute("connections").split(' ');
			for(int c=0;c<conns.size();c++){
				QString rid = conns.at(c).trimmed();
				if( rid.size()>0 ){
					lane.connections.append( rid );
				}
			}
		}
		if(lane.forward){
			flanes.append(lane);
		}else{
			blanes.append(lane);
		}
	}
	vns::PolyLine* line = new vns::PolyLine(points);
	vns::Road* road = 0;
	if( flanes.size()==0 && blanes.size()==0 ){
		road = network->createRoad(line,fl,bl);
		road->setMaximumSpeed(maxSpeed/3.6);
		road->setMinimumSpeed(minSpeed/3.6);
	}else{
		road = network->createRoad(line,flanes.size(),blanes.size());
		for(int l=0;l<blanes.size();l++){
			vns::Lane* ln = road->getLane(l);
			ln->setMaximumSpeed(blanes.at(l).maxSpeed/3.6);
			ln->setMinimumSpeed(blanes.at(l).minSpeed/3.6);
			ln->setWidth(blanes.at(l).width);
			ln->setLaneStartOffset(blanes.at(l).startoffset);
			ln->setLaneEndOffset(blanes.at(l).endoffset);
			laneConnections.insert(ln,blanes.at(l).connections);

			if( blanes.at(l).buses == true ) ln->addFlags(vns::Lane::Buses); else ln->removeFlags(vns::Lane::Buses);
			if( blanes.at(l).cars == true ) ln->addFlags(vns::Lane::Cars); else ln->removeFlags(vns::Lane::Cars);
			if( blanes.at(l).bikes == true ) ln->addFlags(vns::Lane::Bikes); else ln->removeFlags(vns::Lane::Bikes);
			if( blanes.at(l).trucks == true ) ln->addFlags(vns::Lane::Trucks); else ln->removeFlags(vns::Lane::Trucks);
			if( blanes.at(l).rails == true ) ln->addFlags(vns::Lane::Rails); else ln->removeFlags(vns::Lane::Rails);
			if( blanes.at(l).pedestrians == true ) ln->addFlags(vns::Lane::Pedestrians); else ln->removeFlags(vns::Lane::Pedestrians);
			if( blanes.at(l).stop == true ) ln->addFlags(vns::Lane::StopSign); else ln->removeFlags(vns::Lane::StopSign);
			if( blanes.at(l).yield == true ) ln->addFlags(vns::Lane::YieldSign); else ln->removeFlags(vns::Lane::YieldSign);
			if( blanes.at(l).changeleft == true ) ln->addFlags(vns::Lane::ChangeLaneToLeft); else ln->removeFlags(vns::Lane::ChangeLaneToLeft);
			if( blanes.at(l).changeright == true ) ln->addFlags(vns::Lane::ChangeLaneToRight); else ln->removeFlags(vns::Lane::ChangeLaneToRight);
			if( blanes.at(l).access == false ) ln->addFlags(vns::Lane::NoTrafficLane); else ln->removeFlags(vns::Lane::NoTrafficLane);

		}
		for(int l=0;l<flanes.size();l++){
			vns::Lane* ln = road->getLane(blanes.size()+l);
			ln->setMaximumSpeed(flanes.at(l).maxSpeed/3.6);
			ln->setMinimumSpeed(flanes.at(l).minSpeed/3.6);
			ln->setWidth(flanes.at(l).width);
			ln->setLaneStartOffset(flanes.at(l).startoffset);
			ln->setLaneEndOffset(flanes.at(l).endoffset);
			laneConnections.insert(ln,flanes.at(l).connections);

			if( flanes.at(l).buses == true ) ln->addFlags(vns::Lane::Buses); else ln->removeFlags(vns::Lane::Buses);
			if( flanes.at(l).cars == true ) ln->addFlags(vns::Lane::Cars); else ln->removeFlags(vns::Lane::Cars);
			if( flanes.at(l).bikes == true ) ln->addFlags(vns::Lane::Bikes); else ln->removeFlags(vns::Lane::Bikes);
			if( flanes.at(l).trucks == true ) ln->addFlags(vns::Lane::Trucks); else ln->removeFlags(vns::Lane::Trucks);
			if( flanes.at(l).rails == true ) ln->addFlags(vns::Lane::Rails); else ln->removeFlags(vns::Lane::Rails);
			if( flanes.at(l).pedestrians == true ) ln->addFlags(vns::Lane::Pedestrians); else ln->removeFlags(vns::Lane::Pedestrians);
			if( flanes.at(l).stop == true ) ln->addFlags(vns::Lane::StopSign); else ln->removeFlags(vns::Lane::StopSign);
			if( flanes.at(l).yield == true ) ln->addFlags(vns::Lane::YieldSign); else ln->removeFlags(vns::Lane::YieldSign);
			if( flanes.at(l).changeleft == true ) ln->addFlags(vns::Lane::ChangeLaneToLeft); else ln->removeFlags(vns::Lane::ChangeLaneToLeft);
			if( flanes.at(l).changeright == true ) ln->addFlags(vns::Lane::ChangeLaneToRight); else ln->removeFlags(vns::Lane::ChangeLaneToRight);
			if( flanes.at(l).access == false ) ln->addFlags(vns::Lane::NoTrafficLane); else ln->removeFlags(vns::Lane::NoTrafficLane);
		}
	}
	if(road){
		road->setType(roadType);
		road->setLeftParking(lparking || lstand);
		if(lstand) road->getLeftParking()->addFlags(vns::Lane::TaxiStand);
		road->setRightParking(rparking || rstand);
		if(rstand) road->getRightParking()->addFlags(vns::Lane::TaxiStand);
		road->setStartCrossWalk(scross);
		road->setEndCrossWalk(ecross);
		roadIDs.insert(id,road);
	}
	return road;
}

QDomElement createXMLFixedTimeController(QDomDocument& doc,const vns::FixedTimeTrafficLightController* c){
	QDomElement xmlc = doc.createElement("tl");
	xmlc.setAttribute("id", QString("%1").arg(c->getID()));
	xmlc.setAttribute("type", "fixed");
	xmlc.setAttribute("offset", QString("%1").arg(c->getTimeOffset()));
	for(int l=0;l<c->getNumberOfLights();l++){
		const vns::Lane* lane = c->getLane(l);
		QDomElement xmllight = doc.createElement("light");
		xmllight.setAttribute("road", QString("%1").arg(lane->getRoad()->getID()));
		xmllight.setAttribute("lane", QString("%1").arg(lane->getID()));
		xmlc.appendChild(xmllight);
	}
	for(int s=0;s<c->getNumberOfStates();s++){
		const vns::TrafficLightState* st = c->getState(s);
		QDomElement xmlstate = doc.createElement("state");
		xmlstate.setAttribute("duration", QString("%1").arg(st->getDuration()));
		QString lights;
		for(int l=0;l<st->getNumberOfLights();l++){
			if(st->getLight(l)==vns::RedLight) lights.append('R');
			if(st->getLight(l)==vns::YellowLight) lights.append('Y');
			if(st->getLight(l)==vns::GreenLight) lights.append('G');
		}
		xmlstate.setAttribute("lights", lights);
		xmlc.appendChild(xmlstate);
	}
	return xmlc;
}


namespace vns {

XmlMapFile::XmlMapFile() : vns::MapFile() {

}

XmlMapFile::~XmlMapFile() {

}

bool XmlMapFile::load(vns::RoadNetwork* network, const char* fileName) {

	QFile file(fileName);
	if( !file.open(QIODevice::ReadOnly | QIODevice::Text) ){
		return  false;
	}
	QDomDocument doc;
	if( !doc.setContent(&file) ){
		return false;
	}

	network->setAutomaticConnectivityGeneration(false);
	QDomElement docElem = doc.documentElement();
	QString rootTag = docElem.tagName();
	VNS_ASSERT( rootTag=="roadnetwork" ,"XML Error","Root tag should be <roadnetwork>");
	if (docElem.hasAttribute("offset")) {
		QStringList pt = docElem.attribute("offset").split(',');
		VNS_ASSERT( pt.size()==2 || pt.size()==3,"XML Error","Invalid 'offset' attribute in <roadnetwork>");
		bool ok;
		double x = pt.at(0).toDouble(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'offset' attribute in <roadnetwork>");
		double y = pt.at(1).toDouble(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'offset' attribute in <roadnetwork>");
		double z = 0;
		if (pt.size() == 3) {
			z = pt.at(2).toDouble(&ok);
			VNS_ASSERT( ok ,"XML Error","Invalid 'offset' attribute in <roadnetwork>");
		}
		network->setOffset( vns::Vec(x,y,z) );
	}

	QDomNodeList roadList = docElem.elementsByTagName("road");

	QHash<QString,vns::Road*> roadIDs;

	QHash<vns::Lane*,QStringList> laneConnections;
	for(int i = 0;i < roadList.count(); i++){
		QDomElement e = roadList.at(i).toElement();
		parseXMLRoad(network, e, roadIDs, laneConnections);
	}
	network->generateRoadsConnectivity();
	network->setAutomaticConnectivityGeneration(true);
	QHash<vns::Lane*,QStringList>::iterator it = laneConnections.begin();
	QHash<vns::Lane*,QStringList>::iterator end = laneConnections.end();
	while( it != end ) {
		vns::Lane* lane = it.key();
		lane->clearConnections();
	    QStringList ids = it.value();
	    for(int i=0;i<ids.size();i++){
	    	 QString id = ids.at(i);
	    	 VNS_ASSERT( roadIDs.contains(id) ,"XML Error","Invalid roadID in 'connections' - <lane> ");
	    	 vns::Road* nroad = roadIDs.value(id);
	    	 if(nroad){
	    		 lane->addConnectionTo(nroad);
	    	 }
	    }
	    ++it;
	}

	QDomNodeList tlList = docElem.elementsByTagName("tl");
	for(int i = 0;i < tlList.count(); i++){
		QDomElement e = tlList.at(i).toElement();
		VNS_ASSERT( e.hasAttribute("type") ,"XML Error","Missing attribute 'type' in <tl>");
		QString type = e.attribute("type").toLower().trimmed();
		if(type=="fixed") parseXMLFixedTrafficLight(network, e, roadIDs);
	}

	file.close();
	return true;
}

bool XmlMapFile::save(const vns::RoadNetwork* network, const char* fileName) {
	vns::Vec offset = network->getOffset();
	vns::BBox extents = network->getExtents();
	const char* version = network->getVersion();
	QDomDocument doc;
	QDomElement docElem = doc.createElement("roadnetwork");
	docElem.setAttribute("extents",QString("%1,%2,%3,%4").arg(extents.getMinX()).arg(extents.getMinY()).arg(extents.getMaxX()).arg(extents.getMaxY()));
	docElem.setAttribute("offset",QString("%1,%2,%3").arg(offset.getX()).arg(offset.getY()).arg(offset.getX()));
	docElem.setAttribute("version", version );
	doc.appendChild(docElem);
	for (int r = 0; r < network->getNumberOfRoads(); r++) {
		const vns::Road* road = network->getRoad(r);

		QDomElement xmlroad = doc.createElement("road");
		xmlroad.setAttribute("id", QString("%1").arg(road->getID()));
		xmlroad.setAttribute("type", vns::Road::fromType(road->getType()));
		if(road->getRoadOffset() != 0) xmlroad.setAttribute("offset", QString("%1").arg(road->getRoadOffset()));
		if(road->hasLeftParking()){
			if(road->getLeftParking()->hasFlag(vns::Lane::TaxiStand)){
				xmlroad.setAttribute("leftparking", "yes");
			}else{
				xmlroad.setAttribute("lefttaxistand", "yes");
			}
		}
		if(road->hasRightParking()){
			if(road->getRightParking()->hasFlag(vns::Lane::TaxiStand)){
				xmlroad.setAttribute("rightparking", "yes");
			}else{
				xmlroad.setAttribute("righttaxistand", "yes");
			}
		}
		if(road->hasStartCrossWalk()) xmlroad.setAttribute("startcrosswalk", "yes");
		if(road->hasEndCrossWalk()) xmlroad.setAttribute("endcrosswalk", "yes");
		QString shape;
		for (int i = 0; i < road->getNumberOfPoints(); i++) {
			if (i > 0)
				shape.append(" ");
			vns::Vec p = road->getPoint(i);
			shape.append(QString("%1,%2,%3").arg(p.getX(),0,'g',15).arg(p.getY(),0,'g',15).arg(p.getZ(),0,'g',15));
		}
		xmlroad.setAttribute("polyline", shape);

		for (int l = 0; l < road->getNumberOfLanes(); l++) {
			const vns::Lane* lane = road->getLane(l);
			QDomElement xmllane = doc.createElement("lane");
			xmllane.setAttribute("forward",(lane->getWay() == Forward ? "yes" : "no"));
			xmllane.setAttribute("maxspeed",QString("%1").arg(lane->getMaximumSpeed()*3.6));
			if(lane->getMinimumSpeed()>0.0) xmllane.setAttribute("minspeed",QString("%1").arg(lane->getMinimumSpeed()*3.6));
			xmllane.setAttribute("width", QString("%1").arg(lane->getWidth()));
			if(lane->getLaneStartOffset()>0.0) xmllane.setAttribute("startoffset", QString("%1").arg(lane->getLaneStartOffset()));
			if(lane->getLaneEndOffset()>0.0) xmllane.setAttribute("endoffset", QString("%1").arg(lane->getLaneEndOffset()));

			if (!lane->hasFlag(Lane::Buses)) xmllane.setAttribute("buses", "no");
			if (!lane->hasFlag(Lane::Cars)) xmllane.setAttribute("cars", "no");
			if (!lane->hasFlag(Lane::Bikes)) xmllane.setAttribute("bikes", "no");
			if (!lane->hasFlag(Lane::Trucks)) xmllane.setAttribute("trucks", "no");
			if (lane->hasFlag(Lane::Rails)) xmllane.setAttribute("rails", "yes");
			if (lane->hasFlag(Lane::Pedestrians)) xmllane.setAttribute("pedestrians", "yes");
			if (lane->hasFlag(Lane::StopSign)) xmllane.setAttribute("stop", "yes");
			if (lane->hasFlag(Lane::YieldSign)) xmllane.setAttribute("yield", "yes");
			if (!lane->hasFlag(Lane::ChangeLaneToLeft)) xmllane.setAttribute("changeleft", "no");
			if (!lane->hasFlag(Lane::ChangeLaneToRight)) xmllane.setAttribute("changeright", "no");
			if (lane->hasFlag(Lane::NoTrafficLane)) xmllane.setAttribute("access", "no");

			QString conns;
			for (int c = 0; c < lane->getNumberOfConnections(); c++) {
				if (c > 0)
					conns.append(" ");
				const Road* nroad = lane->getConnection(c);
				conns.append(QString("%1").arg(nroad->getID()));
			}
			xmllane.setAttribute("connections", conns);
			xmlroad.appendChild(xmllane);
		}
		docElem.appendChild(xmlroad);
	}
	for (int t = 0; t < network->getNumberOfTrafficLights(); t++) {
			const vns::TrafficLightController* c = network->getTrafficLightController(t);
			if(c->getType()==vns::TrafficLightController::FixedTime){
				QDomElement xmlc = createXMLFixedTimeController(doc,dynamic_cast<const vns::FixedTimeTrafficLightController*>(c));
				docElem.appendChild(xmlc);
			}
	}


	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream out(&file);
		doc.save(out, 4);
		file.close();
	} else {
		return false;
	}
	return true;
}


}

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

#include "osmmapfile.h"
#include "roadnetwork.h"
#include "mercartor.h"
#include <QtCore/QHash>
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <sstream>


class RoadOsm {
public:
	uint64 globalid;
	QString name;
	bool oneway;
	int nlanes;
	int minspeed;
	int maxspeed;
	QList<vns::Vec> points;
	RoadOsm(qint32 i,bool ow,int nl,int vm,int vx,QList<vns::Vec> p){
		globalid = i;
		oneway = ow;
		nlanes = nl;
		minspeed = vm;
		maxspeed = vx;
		points = p;
	};
	~RoadOsm(){};
};

double getDoubleValue(const QString& text,const char* error){
	bool ok;
	double v = text.toDouble(&ok);
	VNS_ASSERT( ok ,"OSM Error",error);
	return v;
}

unsigned int getUInt32Value(const QString& text,const char* error){
	bool ok;
	unsigned int v = text.toUInt(&ok);
	VNS_ASSERT( ok ,"OSM Error",error);
	return v;
}

bool getBoolValue(const QString& text,const char*){
	if( text=="true" || text=="yes" || text=="1"){
		return true;
	}
	if( text=="false" || text=="no" || text=="0"){
		return false;
	}
	return false;
}

void parseDoubleList(const QString& text,const char* error, QList<double>& list){
	QStringList textList = text.split('|');
	for(int i=0;i<textList.size();i++){
		QString s = textList.at(i);
		double v = getDoubleValue(s,error);
		list.append(v);
	}
}

void parseBoolList(const QString& text,const char* error, QList<bool>& list){
	QStringList textList = text.split('|');
	for(int i=0;i<textList.size();i++){
		bool v = getBoolValue( textList.at(i) , error);
		list.append(v);
	}
}




namespace vns {

OsmMapFile::OsmMapFile() : vns::MapFile() {

}

OsmMapFile::~OsmMapFile() {

}

bool OsmMapFile::load(vns::RoadNetwork* network, const char* fileName) {

	QFile file(fileName);
	if( !file.open(QIODevice::ReadOnly | QIODevice::Text) ){
		return false;
	}
	QDomDocument doc;
	if( !doc.setContent(&file) ){
		return false;
	}

	QDomElement osm = doc.documentElement();
	VNS_ASSERT( osm.tagName()=="osm" ,"XML Error","Root tag should be <osm>.");

	QDomNodeList nodesList = osm.elementsByTagName("node");
	QHash<QString,vns::Vec> nodes;
	bool ok;
	vns::BBox bb;
	for(int i = 0;i < nodesList.count(); i++){
		QDomElement e = nodesList.at(i).toElement();
		VNS_ASSERT( e.hasAttribute("id") ,"XML Error","'id' attribute is missing in <node>.");
		VNS_ASSERT( e.hasAttribute("lat") ,"XML Error","'lat' attribute is missing in <node>.");
		VNS_ASSERT( e.hasAttribute("lon") ,"XML Error","'lon' attribute is missing in <node>.");
		QString id = e.attribute("id");
		double lat = e.attribute("lat").toDouble(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'lat' attribute in <node>.");
		double lon = e.attribute("lon").toDouble(&ok);
		VNS_ASSERT( ok ,"XML Error","Invalid 'lon' attribute in <node>.");
		vns::Vec p = vns::Mercartor::toXY(lon,lat);
		bb.addPoint(p);
		nodes.insert(id,p);
	}

	vns::Vec offset( (bb.getMaxX()+bb.getMinX())*0.5, (bb.getMaxY()+bb.getMinY())*0.5, 0);

	network->setOffset( offset );
	network->setAutomaticConnectivityGeneration(false);

	QDomNodeList wayList = osm.elementsByTagName("way");

	QHash<QString,int> nodesWays;
	for(int i=0;i<wayList.count();i++){
			QDomElement way = wayList.at(i).toElement();
			VNS_ASSERT( way.hasAttribute("id") ,"OSM Error","'id' attribute is missing in <way>.");
			QString roadID = way.attribute("id");
			QDomNodeList ndList = way.elementsByTagName("nd");
			for(int n=0;n<ndList.count();n++){
				QDomElement nd = ndList.at(n).toElement();
				VNS_ASSERT( nd.hasAttribute("ref") ,"OSM Error","'ref' attribute is missing in <nd>.");
				QString ref = nd.attribute("ref");
				VNS_ASSERT( nodes.contains(ref) ,"OSM Error","'ref' node does not exist.");
				nodesWays.insert(ref, nodesWays.value(ref,0)+1 );
			}
	}

	QHash< vns::Road* , QString > roadConnections;
	QHash< QString , vns::Road* > roads;

	for(int i=0;i<wayList.count();i++){
		QDomElement way = wayList.at(i).toElement();
		QString roadID = way.attribute("id");
		VNS_ASSERT( !roads.contains(roadID) ,"OSM Error","'id' attribute in <way> already exists.");
		QDomNodeList ndList = way.elementsByTagName("nd");
		QList<QString> points;
		for(int n=0;n<ndList.count();n++){
			QDomElement nd = ndList.at(n).toElement();
			QString ref = nd.attribute("ref");
			points.append(ref);
		}
		if(points.size()<2) continue;

		bool isRoad = false;
		bool isReverse = false;
		int lanes = 1;
		int lanesforward = 1;
		int lanesbackward = 0;
		bool oneway = true;
		bool hasLanesForward = false;
		bool hasLanesBackward = false;
		vns::Road::Type roadType = vns::Road::Undefined;
		double offset = 0.0;
		bool leftparking = false;
		bool rightparking = false;
		bool startcrosswalk = false;
		bool endcrosswalk = false;
		uint32 backlogforward = vns::MAX_UINT32;
		uint32 backlogbackward = vns::MAX_UINT32;
		QString connections = "";
		/*
		bool sourceforward = true;
		bool sourcebackward = true;
		bool sinkforward = true;
		bool sinkbackward = true;
		*/

		QList<double> width;
		QList<double> maxspeed;
		QList<double> minspeed;
		QList<double> start;
		QList<double> end;
		QList<bool> bus;
		QList<bool> cars;
		QList<bool> bikes;
		QList<bool> trucks;
		QList<bool> rails;
		QList<bool> pedestrians;
		QList<bool> yield;
		QList<bool> stop;
		QList<bool> changeleft;
		QList<bool> changeright;
		QList<bool> access;

		QDomNodeList tagList = way.elementsByTagName("tag");
		for(int n=0;n<tagList.count();n++){
			QDomElement tag = tagList.at(n).toElement();
			VNS_ASSERT( tag.hasAttribute("k") ,"XML Error","'ref' attribute is missing in <nd>");
			QString k = tag.attribute("k");
			VNS_ASSERT( tag.hasAttribute("v") ,"XML Error","'ref' node does not exist.");
			QString v = tag.attribute("v");
			if(k=="highway"){
				roadType = vns::Road::Undefined;
				isRoad = true;
			}
			if(k=="offset") offset = getDoubleValue(v,"Invalid 'offset' attribute in <tag>.");
			if(k=="leftparking") leftparking = getBoolValue(v,"Invalid 'leftparking' attribute in <tag>.");
			if(k=="rightparking") rightparking = getBoolValue(v,"Invalid 'rightparking' attribute in <tag>.");
			if(k=="startcrosswalk") startcrosswalk = getBoolValue(v,"Invalid 'startcrosswalk' attribute in <tag>.");
			if(k=="endcrosswalk") endcrosswalk = getBoolValue(v,"Invalid 'endcrosswalk' attribute in <tag>.");
			if(k=="queue:forward") backlogforward = getUInt32Value(v,"Invalid 'backlog:forward' attribute in <tag>.");
			if(k=="queue:backward") backlogbackward = getUInt32Value(v,"Invalid 'backlog:backward' attribute in <tag>.");

			if(k=="oneway"){
				if(v=="-1" || v=="reverse"){
					oneway = true;
					isReverse = true;
				}else{
					oneway = getBoolValue(v,"Invalid 'oneway' attribute in <tag>.");
				}
			}
			if(k=="lanes") lanes = getUInt32Value(v,"Invalid 'lanes' attribute in <tag>.");
			if(k=="lanes:forward"){
				lanesforward = getUInt32Value(v,"Invalid 'lanesforward' attribute in <tag>.");
				hasLanesForward = true;
			}
			if(k=="lanes:backward"){
				lanesbackward = getUInt32Value(v,"Invalid 'lanesbackward' attribute in <tag>.");
				hasLanesBackward = true;
			}
			if(k=="maxspeed:lanes") parseDoubleList(v,"Invalid 'maxspeed:lanes' attribute in <tag>.",maxspeed);
			if(k=="minspeed:lanes") parseDoubleList(v,"Invalid 'minspeed:lanes' attribute in <tag>.",minspeed);
			if(k=="width:lanes") parseDoubleList(v,"Invalid 'width:lanes' attribute in <tag>.",width);
			if(k=="start:lanes") parseDoubleList(v,"Invalid 'start:lanes' attribute in <tag>.",start);
			if(k=="end:lanes") parseDoubleList(v,"Invalid 'end:lanes' attribute in <tag>.",end);
			if(k=="bus:lanes") parseBoolList(v,"Invalid 'bus:lanes' attribute in <tag>.",bus);
			if(k=="cars:lanes") parseBoolList(v,"Invalid 'cars:lanes' attribute in <tag>.",cars);
			if(k=="bikes:lanes") parseBoolList(v,"Invalid 'bikes:lanes' attribute in <tag>.",bikes);
			if(k=="trucks:lanes") parseBoolList(v,"Invalid 'trucks:lanes' attribute in <tag>.",trucks);
			if(k=="rails:lanes") parseBoolList(v,"Invalid 'rails:lanes' attribute in <tag>.",rails);
			if(k=="pedestrians:lanes") parseBoolList(v,"Invalid 'pedestrians:lanes' attribute in <tag>.",pedestrians);
			if(k=="yield:lanes") parseBoolList(v,"Invalid 'yield:lanes' attribute in <tag>.",yield);
			if(k=="stop:lanes") parseBoolList(v,"Invalid 'stop:lanes' attribute in <tag>.",stop);
			if(k=="changeleft:lanes") parseBoolList(v,"Invalid 'changeleft:lanes' attribute in <tag>.",changeleft);
			if(k=="changeright:lanes") parseBoolList(v,"Invalid 'changeright:lanes' attribute in <tag>.",changeright);
			if(k=="access:lanes") parseBoolList(v,"Invalid 'access:lanes' attribute in <tag>.",access);
			if(k=="connections:lanes") connections = v;
		}
		if(!isRoad){
			continue;
		}
		if(!hasLanesForward && !hasLanesBackward){
			if(oneway==false){
				lanesforward = lanes/2 + lanes%2;
				lanesbackward = lanes/2;
			}else{
				lanesforward = lanes;
				lanesbackward = 0;
			}
		}else{
			VNS_ASSERT( lanesforward+lanesbackward<=lanes ,"OSM Error","Invalid number of lanes in <tag>.");
			if(hasLanesBackward && !hasLanesBackward){
				lanesbackward = lanes-lanesforward;
			}
			if(!hasLanesBackward && hasLanesBackward){
				lanesforward = lanes-lanesbackward;
			}
		}

		/* split ways */
		QList< vns::PolyLine* > polylines;
		vns::Vector<vns::Vec> np;
		for(int pi=0;pi<points.size();pi++){
			QString ref = points.at(pi);
			vns::Vec pref = nodes.value(ref);
			vns::Vec p = pref-network->getOffset();
			np.append(p);
			if(pi>0 && nodesWays.value(ref)>1){
				// divide
				polylines.append( new vns::PolyLine(np) );
				np.clear();
				if(pi<points.size()-1) np.append(p);
			}
		}
		if(np.size()>1){
			polylines.append( new vns::PolyLine(np) );
		}

		for(int pol=0;pol<polylines.size();pol++){
			vns::PolyLine* polyline = polylines.at(pol);
			if(isReverse) polyline->reverse();
			vns::Road* road = network->createRoad( polyline ,lanesforward, lanesbackward);
			if(pol==0) roads.insert(roadID,road); // primeira rua fica com o roadID
			if(pol==polylines.size()-1) roadConnections.insert(road,connections);

			road->setType(roadType);
			road->setRoadOffset(offset);
			road->setLeftParking(leftparking);
			road->setRightParking(rightparking);
			road->setStartCrossWalk(startcrosswalk);
			road->setEndCrossWalk(endcrosswalk);
			road->setMaximumCarsInQueue(vns::Forward,backlogforward);
			road->setMaximumCarsInQueue(vns::Backward,backlogbackward);
			/*
			 * sink & source
			 */


			for(int l=0;l<road->getNumberOfLanes();l++){
				vns::Lane* lane = road->getLane(l);
				if(l<maxspeed.size()) lane->setMaximumSpeed( maxspeed.at(l)/3.6 );else lane->setMaximumSpeed( 50.0/3.6 );
				if(l<minspeed.size()) lane->setMinimumSpeed( minspeed.at(l)/3.6 );else lane->setMinimumSpeed( 0.0 );
				if(l<width.size()) lane->setWidth( width.at(l) );else lane->setWidth( 3.0 );
				if(l<start.size()) lane->setLaneStart( start.at(l) );
				if(l<end.size()) lane->setLaneEnd( end.at(l) );
				if(l>=access.size() || bus.at(l)==true) lane->addFlags(Lane::Buses); else lane->removeFlags(Lane::Buses);
				if(l>=access.size() || cars.at(l)==true) lane->addFlags(Lane::Cars); else lane->removeFlags(Lane::Cars);
				if(l>=access.size() || bikes.at(l)==true) lane->addFlags(Lane::Bikes); else lane->removeFlags(Lane::Bikes);
				if(l>=access.size() || trucks.at(l)==true) lane->addFlags(Lane::Trucks); else lane->removeFlags(Lane::Trucks);
				if(l>=access.size() || rails.at(l)==true) lane->addFlags(Lane::Rails); else lane->removeFlags(Lane::Rails);
				if(l>=access.size() || pedestrians.at(l)==true) lane->addFlags(Lane::Pedestrians); else lane->removeFlags(Lane::Pedestrians);
				if(l>=access.size() || yield.at(l)==true) lane->addFlags(Lane::YieldSign); else lane->removeFlags(Lane::YieldSign);
				if(l>=access.size() || stop.at(l)==true) lane->addFlags(Lane::StopSign); else lane->removeFlags(Lane::StopSign);
				if(l>=access.size() || changeleft.at(l)==true) lane->addFlags(Lane::ChangeLaneToLeft); else lane->removeFlags(Lane::ChangeLaneToLeft);
				if(l>=access.size() || changeright.at(l)==true) lane->addFlags(Lane::ChangeLaneToRight); else lane->removeFlags(Lane::ChangeLaneToRight);
				if(l>=access.size() || access.at(l)==true) lane->removeFlags(Lane::NoTrafficLane); else lane->addFlags(Lane::NoTrafficLane);
			}
		}
	}

	network->generateRoadsConnectivity();
	network->setAutomaticConnectivityGeneration(true);

	QHash<vns::Road*,QString>::iterator it = roadConnections.begin();
	QHash<vns::Road*,QString>::iterator end = roadConnections.end();
	while( it != end ) {
		vns::Road* road = it.key();
		QList< QList<int> > connections;
		QString text = it.value();
		QStringList lanesList = text.split('|');
		for(int l=0;l<road->getNumberOfLanes();l++){
			vns::Lane* lane = road->getLane(l);
			lane->clearConnections();
			if(l<lanesList.size()){
				QString ss = lanesList.at(l);
				QStringList cons = ss.split(',');
				for(int r=0;r<cons.size();r++){
					QString rid = cons.at(r).trimmed();
					if(rid != ""){
						VNS_ASSERT( roads.contains(rid) ,"XML Error","Invalid roadID in 'connections' - <tag> ");
						vns::Road* nroad = roads.value(rid);
						lane->addConnectionTo( nroad );
					}
				}
			}
		}
	    ++it;
	}

	return true;
}


bool OsmMapFile::save(const vns::RoadNetwork* network, const char* fileName) {
	vns::Vec o = network->getOffset();
	vns::BBox e = network->getExtents();
	QDomDocument doc;

	QDomProcessingInstruction xmlDeclaration = doc.createProcessingInstruction("xml", "version=\"1.0\"");
	doc.appendChild(xmlDeclaration);

	QDomElement osm = doc.createElement("osm");
	osm.setAttribute("version", "1.0" );
	osm.setAttribute("generator", "VNS" );
	doc.appendChild(osm);
	QDomElement bounds = doc.createElement("bound");
	vns::Vec min = vns::Mercartor::toLonLat(e.getMinX()+o.getX(),e.getMinY()+o.getY());
	vns::Vec max = vns::Mercartor::toLonLat(e.getMaxX()+o.getX(),e.getMaxY()+o.getY());
	bounds.setAttribute("box", QString("%1,%2,%3,%4").arg(min.getY())
			.arg(min.getX())
			.arg(max.getY())
			.arg(max.getX()));
	bounds.setAttribute("origin","http://www.openstreetmap.org/api/0.6");
	osm.appendChild(bounds);

	QHash<QString,vns::Vec> nodes;
	QList<QDomElement> ways;

	vns::HashTable<vns::Vec,QString> nodesIDs(8192);

	int idn = network->getNumberOfRoads();
	for (int r = 0; r < network->getNumberOfRoads(); r++) {

		const vns::Road* road = network->getRoad(r);
		const vns::PolyLine* line = road->getPolyLine();
		QDomElement way = doc.createElement("way");
		for(int i=0;i<line->size();i++){
			vns::Vec p = vns::Mercartor::toLonLat(line->getPoint(i)+o);
			QString pointID;
			if(nodesIDs.contains(p)){
				pointID = nodesIDs.value(p);
			}else{
				pointID = QString("%1").arg(idn);
				nodesIDs.insert(p,pointID);
				QDomElement node = doc.createElement("node");
				node.setAttribute("id",pointID);
				node.setAttribute("lon",QString("%1").arg(p.getX(),0,'g',15));
				node.setAttribute("lat",QString("%1").arg(p.getY(),0,'g',15));
				osm.appendChild(node);
				idn++;
			}
			QDomElement nd = doc.createElement("nd");
			nd.setAttribute("ref",pointID);
			way.appendChild(nd);
		}
		way.setAttribute("id",QString("%1").arg(road->getID()));

		QDomElement tag = doc.createElement("tag");
		tag.setAttribute("k","highway");
		tag.setAttribute("v","residential"); // tipo de estrada
		way.appendChild(tag);

		QDomElement offset = doc.createElement("tag");
		offset.setAttribute("k","offset");
		offset.setAttribute("v",QString("%1").arg(road->getRoadOffset(),0,'g',15));
		way.appendChild(offset);

		QDomElement left = doc.createElement("tag");
		left.setAttribute("k","leftparking");
		left.setAttribute("v",(road->hasLeftParking()?"yes":"no"));
		way.appendChild(left);

		QDomElement right = doc.createElement("tag");
		right.setAttribute("k","rightparking");
		right.setAttribute("v",(road->hasRightParking()?"yes":"no"));
		way.appendChild(right);

		QDomElement startwalk = doc.createElement("tag");
		startwalk.setAttribute("k","startcrosswalk");
		startwalk.setAttribute("v",(road->hasStartCrossWalk()?"yes":"no"));
		way.appendChild(startwalk);

		QDomElement endwalk = doc.createElement("tag");
		endwalk.setAttribute("k","endcrosswalk");
		endwalk.setAttribute("v",(road->hasEndCrossWalk()?"yes":"no"));
		way.appendChild(endwalk);

		QDomElement fbacklog = doc.createElement("tag");
		fbacklog.setAttribute("k","queue:forward");
		fbacklog.setAttribute("v",QString("%1").arg(road->getNumberOfCarsInQueue(vns::Forward)));
		way.appendChild(fbacklog);

		QDomElement bbacklog = doc.createElement("tag");
		bbacklog.setAttribute("k","queue:backward");
		bbacklog.setAttribute("v",QString("%1").arg(road->getNumberOfCarsInQueue(vns::Backward)));
		way.appendChild(bbacklog);

		std::stringstream maxsp;
		std::stringstream minsp;
		std::stringstream width;
		std::stringstream startlanes;
		std::stringstream endlanes;
		std::stringstream bus;
		std::stringstream cars;
		std::stringstream bikes;
		std::stringstream trucks;
		std::stringstream rails;
		std::stringstream pedestrians;
		std::stringstream yield;
		std::stringstream stop;
		std::stringstream changeleft;
		std::stringstream changeright;
		std::stringstream access;
		std::stringstream cons;
		int lanesforward = 0;
		int lanesbackward = 0;
		int nl = road->getNumberOfLanes();
		for(int l=0;l<nl;l++){
			const vns::Lane* lane = road->getLane(l);
			if(lane->getWay()==vns::Forward) lanesforward++; else lanesbackward++;
			maxsp << lane->getMaximumSpeed()*3.6;
			minsp << lane->getMinimumSpeed()*3.6;
			width << lane->getWidth();
			startlanes << lane->getLaneStart();
			endlanes << lane->getLaneEnd();
			bus << (lane->hasFlag(Lane::BusLane)?"yes":"no");
			cars << (lane->hasFlag(Lane::Cars)?"yes":"no");
			bikes << (lane->hasFlag(Lane::Bikes)?"yes":"no");
			trucks << (lane->hasFlag(Lane::Trucks)?"yes":"no");
			rails << (lane->hasFlag(Lane::Rails)?"yes":"no");
			pedestrians << (lane->hasFlag(Lane::Pedestrians)?"yes":"no");
			yield << (lane->hasFlag(Lane::YieldSign)?"yes":"no");
			stop << (lane->hasFlag(Lane::StopSign)?"yes":"no");
			changeleft << (lane->hasFlag(Lane::ChangeLaneToLeft)?"yes":"no");
			changeright << (lane->hasFlag(Lane::ChangeLaneToRight)?"yes":"no");
			access << (lane->hasFlag(Lane::NoTrafficLane)?"no":"yes");
			for(int c=0;c<lane->getNumberOfConnections();c++){
				if(c>0){
					cons << ",";
				}
				const vns::Road* nroad = lane->getConnection(c);
				cons << nroad->getID();
			}
			if(l<nl-1){
				maxsp << '|';
				minsp << '|';
				width << '|';
				startlanes << '|';
				endlanes << '|';
				bus << '|';
				cars << '|';
				bikes << '|';
				trucks << '|';
				rails << '|';
				pedestrians << '|';
				yield << '|';
				stop << '|';
				changeleft << '|';
				changeright << '|';
				access << '|';
				cons << '|';
			}
		}

		QDomElement lanes = doc.createElement("tag");
		lanes.setAttribute("k","lanes");
		lanes.setAttribute("v",QString("%1").arg(road->getNumberOfLanes()));
		way.appendChild(lanes);

		QDomElement flanes = doc.createElement("tag");
		flanes.setAttribute("k","lanes:forward");
		flanes.setAttribute("v",QString("%1").arg(lanesforward));
		way.appendChild(flanes);

		QDomElement blanes = doc.createElement("tag");
		blanes.setAttribute("k","lanes:backward");
		blanes.setAttribute("v",QString("%1").arg(lanesbackward));
		way.appendChild(blanes);

		QDomElement maxspeed = doc.createElement("tag");
		maxspeed.setAttribute("k","maxspeed:lanes");
		maxspeed.setAttribute("v",maxsp.str().c_str());
		way.appendChild(maxspeed);

		QDomElement minspeed = doc.createElement("tag");
		minspeed.setAttribute("k","minspeed:lanes");
		minspeed.setAttribute("v",minsp.str().c_str());
		way.appendChild(minspeed);

		QDomElement widthe = doc.createElement("tag");
		widthe.setAttribute("k","width:lanes");
		widthe.setAttribute("v",width.str().c_str());
		way.appendChild(widthe);

		QDomElement startle = doc.createElement("tag");
		startle.setAttribute("k","start:lanes");
		startle.setAttribute("v",startlanes.str().c_str());
		way.appendChild(startle);

		QDomElement endle = doc.createElement("tag");
		endle.setAttribute("k","end:lanes");
		endle.setAttribute("v",endlanes.str().c_str());
		way.appendChild(endle);

		QDomElement busle = doc.createElement("tag");
		busle.setAttribute("k","bus:lanes");
		busle.setAttribute("v",bus.str().c_str());
		way.appendChild(busle);

		QDomElement carsle = doc.createElement("tag");
		carsle.setAttribute("k","cars:lanes");
		carsle.setAttribute("v",cars.str().c_str());
		way.appendChild(carsle);

		QDomElement bikesle = doc.createElement("tag");
		bikesle.setAttribute("k","bikes:lanes");
		bikesle.setAttribute("v",bikes.str().c_str());
		way.appendChild(bikesle);

		QDomElement trucksle = doc.createElement("tag");
		trucksle.setAttribute("k","trucks:lanes");
		trucksle.setAttribute("v",trucks.str().c_str());
		way.appendChild(trucksle);

		QDomElement railsle = doc.createElement("tag");
		railsle.setAttribute("k","rails:lanes");
		railsle.setAttribute("v",rails.str().c_str());
		way.appendChild(railsle);

		QDomElement pedestriansle = doc.createElement("tag");
		pedestriansle.setAttribute("k","pedestrians:lanes");
		pedestriansle.setAttribute("v",pedestrians.str().c_str());
		way.appendChild(pedestriansle);

		QDomElement yieldle = doc.createElement("tag");
		yieldle.setAttribute("k","yield:lanes");
		yieldle.setAttribute("v",yield.str().c_str());
		way.appendChild(yieldle);

		QDomElement stople = doc.createElement("tag");
		stople.setAttribute("k","stop:lanes");
		stople.setAttribute("v",stop.str().c_str());
		way.appendChild(stople);

		QDomElement changeleftle = doc.createElement("tag");
		changeleftle.setAttribute("k","changeleft:lanes");
		changeleftle.setAttribute("v",changeleft.str().c_str());
		way.appendChild(changeleftle);

		QDomElement changerightle = doc.createElement("tag");
		changerightle.setAttribute("k","changeright:lanes");
		changerightle.setAttribute("v",changeright.str().c_str());
		way.appendChild(changerightle);

		QDomElement accessle = doc.createElement("tag");
		accessle.setAttribute("k","access:lanes");
		accessle.setAttribute("v",access.str().c_str());
		way.appendChild(accessle);

		QDomElement conle = doc.createElement("tag");
		conle.setAttribute("k","connections:lanes");
		conle.setAttribute("v",cons.str().c_str());
		way.appendChild(conle);

		ways.append(way);
	}
	for(int w=0;w<ways.size();w++){
		osm.appendChild( ways.at(w) );
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

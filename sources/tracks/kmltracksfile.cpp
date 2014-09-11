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

#include "kmltracksfile.h"
#include "tracker.h"
#include "roadnetwork.h"
#include "mercartor.h"
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>
#include <QtCore/QtCore>

#define LOGOFILE "http://www.dcc.fc.up.pt/~rjf/vns/logo.png"

namespace vns {

KMLTracksFile::KMLTracksFile() : TracksFile() {
	modelFile = "";
	iconFile = "";
	modelHeading = 0.0;
	lineColor = "ffffffff";
	lineWidth = 1;
}

KMLTracksFile::~KMLTracksFile() {

}

bool KMLTracksFile::save(const vns::Tracker* tracker, const RoadNetwork* network, const char* fileName){
	Vec offset = network->getOffset();

	QDomDocument doc;
	doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");

	QDomElement kml = doc.createElement("kml");
	kml.setAttribute("xmlns","http://www.opengis.net/kml/2.2");
	kml.setAttribute("xmlns:gx","http://www.google.com/kml/ext/2.2");
	doc.appendChild(kml);

	QDomElement folder = doc.createElement("Folder");
	kml.appendChild(folder);

	QDomElement logo = doc.createElement("ScreenOverlay");
	QDomElement logoname = doc.createElement("name");
	logoname.appendChild( doc.createTextNode("Vehicular Networks Simulator") );
	logo.appendChild( logoname );
	QDomElement logoorder = doc.createElement("drawOrder");
	logoorder.appendChild( doc.createTextNode("2147483647") );
	logo.appendChild( logoorder );

	QDomElement overlayXY = doc.createElement("overlayXY");
	overlayXY.setAttribute("x","0.0");
	overlayXY.setAttribute("y","0.0");
	overlayXY.setAttribute("xunits","fraction");
	overlayXY.setAttribute("yunits","fraction");
	logo.appendChild( overlayXY );
	QDomElement screenXY = doc.createElement("screenXY");
	screenXY.setAttribute("x","0.01");
	screenXY.setAttribute("y","0.05");
	screenXY.setAttribute("xunits","fraction");
	screenXY.setAttribute("yunits","fraction");
	logo.appendChild( screenXY );

	QDomElement logoicon = doc.createElement("Icon");
	QDomElement href = doc.createElement("href");
	href.appendChild( doc.createTextNode(LOGOFILE));
	logoicon.appendChild( href );
	logo.appendChild( logoicon );
	folder.appendChild( logo );

	QDomElement style = doc.createElement("Style");
	style.setAttribute("id","vehicle");
	QDomElement iconStyle = doc.createElement("IconStyle");
	QDomElement icon = doc.createElement("Icon");
	icon.appendChild( doc.createTextNode(iconFile));
	iconStyle.appendChild( icon );
	style.appendChild( iconStyle );
	QDomElement lineStyle = doc.createElement("LineStyle");
	QDomElement color = doc.createElement("color");
	color.appendChild( doc.createTextNode(lineColor));
	lineStyle.appendChild( color );
	QDomElement width = doc.createElement("width");
	width.appendChild( doc.createTextNode(QString("%1").arg(lineWidth)));
	lineStyle.appendChild( width );
	style.appendChild( lineStyle );
    folder.appendChild( style );

	const List<Tracker::Track*> tracks = tracker->getTracks();
	List<Tracker::Track*>::ConstIterator it = tracks.constBegin();
	while( it != tracks.constEnd() ){
		Tracker::Track* track = *it;
		QDomElement vehicle = doc.createElement("Placemark");

		QDomElement style = doc.createElement("styleUrl");
		style.appendChild( doc.createTextNode("#vehicle"));
		vehicle.appendChild( style );

		QDomElement gps = doc.createElement("gx:Track");

		QDomElement model = doc.createElement("Model");
		QDomElement orientation = doc.createElement("Orientation");
		QDomElement heading = doc.createElement("heading");
		heading.appendChild( doc.createTextNode(QString("%1").arg(modelHeading)));
		orientation.appendChild( heading );
		model.appendChild( orientation );
		QDomElement link = doc.createElement("Link");
		QDomElement href = doc.createElement("href");
		href.appendChild( doc.createTextNode(modelFile));
		link.appendChild( href );
		model.appendChild( link );

		gps.appendChild(model);

		for(int i=0;i<track->size();i++){
			Tracker::Position p = track->at(i);
			QDateTime dt = QDateTime::fromTime_t(p.getTime());
			QDomElement when = doc.createElement("when");
			when.appendChild( doc.createTextNode(dt.toString(Qt::ISODate)) );
			gps.appendChild(when);
			QDomElement coord = doc.createElement("gx:coord");
			Vec pos = Mercartor::toLonLat(p.getPosition()+offset);
			coord.appendChild( doc.createTextNode(dt.toString("%1 %2").arg(pos.x(),0,'g',12).arg(pos.y(),0,'g',12)) );
			gps.appendChild(coord);
			/*QDomElement angles = doc.createElement("gx:angles");
			angles.appendChild( doc.createTextNode(dt.toString("%1 0.0 0.0").arg(p.getDirection().dir_deg())) );
			gps.appendChild(angles);
			*/
		}
		vehicle.appendChild(gps);
		folder.appendChild(vehicle);
		it++;
	}

	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly)) {
		QTextStream out(&file);
		doc.save(out, 2);
		file.close();
	} else {
		return false;
	}
	return true;
}

}

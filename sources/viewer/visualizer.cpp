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

#include "visualizer.h"
#include "viewer.h"
#ifdef OSG
#include "osgmapviewer.h"
#endif
#include "mapviewer.h"
#include <QtGui/QApplication>
#include <cstring>

namespace vns {

Visualizer::Visualizer(Simulator* sim,vns::Visualizer::Type type) {
	const char* str = "vns";
	argc = 1;
	argv = new char*[1];
	int l = strlen(str);
	argv[0] = new char[l];
	strcpy(argv[0],str);
	app = new QApplication(argc,argv);
	switch(type){
	case vns::Visualizer::Viewer2D:
		viewer = new Viewer(sim, new MapViewer(sim) );
		break;
	case vns::Visualizer::ViewerOSG:
#ifdef OSG
		viewer = new Viewer(sim, new OsgMapViewer(sim) );
#else
		viewer = new Viewer(sim, new MapViewer(sim) );
#endif
		break;
	default:
		VNS_ASSERT( false ,"Visualizer::Visualizer","Invalid vns::Visualizer::Type...");
		break;
	}
	width = 800;
	height = 600;
	maximized = false;
}

Visualizer::~Visualizer() {
	delete viewer;
	delete app;
	delete argv[0];
	delete[] argv;
}

int Visualizer::show(){
	if(maximized){
		viewer->showMaximized();
	}else{
		viewer->resize(width,height);
		viewer->show();
	}
	return app->exec();
}

void Visualizer::setPainter(Painter* painter){
	viewer->setPainter(painter);
}

void Visualizer::setTitle(const char* title){
	viewer->setWindowTitle(title);
}

void Visualizer::setInputEventHandler(InputEventHandler* handler){
	viewer->setInputEventHandler(handler);
}

void Visualizer::setMaximized(bool maxim){
	maximized = maxim;
}

void Visualizer::setSize(int w,int h){
	width = w;
	height = h;
}

void Visualizer::start(bool fast){
	viewer->start(fast);
}

}

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

#ifndef VNS_VISUALIZER_H_
#define VNS_VISUALIZER_H_

#include "simulator.h"
#include "inputeventhandler.h"
#include "painter.h"

class QApplication;

namespace vns {

class Viewer;


class Visualizer {

private:
	QApplication* app;
	Viewer* viewer;
	int argc;
	char** argv;
	int width;
	int height;
	bool maximized;

public:

	typedef enum {Viewer2D,ViewerOSG} Type;

	Visualizer(Simulator* simulator, Visualizer::Type type=Visualizer::Viewer2D);
	virtual ~Visualizer();

	int show();
	void setTitle(const char* title);
	void setPainter(Painter* painter);
	void setInputEventHandler(InputEventHandler* handler);
	void setMaximized(bool maximized);
	void setSize(int width,int height);
	void start(bool fast=false);
	//inline Viewer* getViewer(){ return viewer; };
};

}

#endif /* VISUALIZER_H_ */

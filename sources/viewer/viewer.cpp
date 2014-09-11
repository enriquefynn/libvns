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
#include "viewer.h"
#include <QtGui/QStatusBar>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QAction>
#include <QtGui/QCloseEvent>
#include <QtGui/QToolBar>
#include <QtCore/QTimer>
#include <QtCore/QTime>

namespace vns {

Viewer::Viewer(Simulator* sim, SceneViewer* scenev, QWidget* parent) : QMainWindow(parent), Observer() {

	VNS_ASSERT( sim != 0 ,"Viewer::Viewer","Null Simulator!");
	setWindowIcon(QIcon(QString::fromUtf8(":/vns.png")));
    simulator = sim;
    simulator->addObserver(this);
    paused = false;
    realTime = false;
	setWindowTitle(QString::fromUtf8("VNS - Simulation Visualizer"));

	toolbar = this->addToolBar("Simulation");
	toolbar->setMovable(false);
	toolbar->setMaximumHeight(50);

    actionStart = new QAction(QIcon(QString::fromUtf8(":/start_sim.png")),QString::fromUtf8("Start"), this);
    connect(actionStart, SIGNAL(triggered()), this, SLOT(onActionStart()));
    toolbar->addAction( actionStart );

    actionStartFast = new QAction(QIcon(QString::fromUtf8(":/fast_sim.png")),QString::fromUtf8("Fast"), this);
    connect(actionStartFast, SIGNAL(triggered()), this, SLOT(onActionStartFast()));
    toolbar->addAction( actionStartFast );

    actionPause = new QAction(QIcon(QString::fromUtf8(":/pause_sim.png")),QString::fromUtf8("Pause"), this);
    connect(actionPause, SIGNAL(triggered()), this, SLOT(onActionPause()));
    toolbar->addAction( actionPause );

    actionStep = new QAction(QIcon(QString::fromUtf8(":/step_sim.png")),QString::fromUtf8("Step"), this);
    connect(actionStep, SIGNAL(triggered()), this, SLOT(onActionStep()));
    toolbar->addAction( actionStep );

    actionStop = new QAction(QIcon(QString::fromUtf8(":/stop_sim.png")),QString::fromUtf8("Stop"), this);
    connect(actionStop, SIGNAL(triggered()), this, SLOT(onActionStop()));
    toolbar->addAction( actionStop );


    statusbar = new QStatusBar(this);
    statusbar->setSizeGripEnabled(true);

    numberOfVehiclesInBacklogLabel = new QLabel(this);
    numberOfVehiclesInBacklogLabel->setFixedWidth(150);
    numberOfVehiclesInBacklogLabel->setText("");
    numberOfVehiclesInBacklogLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusbar->addPermanentWidget(numberOfVehiclesInBacklogLabel,1);

    numberOfVehiclesLabel = new QLabel(this);
    numberOfVehiclesLabel->setFixedWidth(150);
    numberOfVehiclesLabel->setText("");
    numberOfVehiclesLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusbar->addPermanentWidget(numberOfVehiclesLabel,1);

	dateTimeLabel = new QLabel(this);
	dateTimeLabel->setFixedWidth(190);
	dateTimeLabel->setText("");
	dateTimeLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	statusbar->addPermanentWidget(dateTimeLabel,1);

	timeLabel = new QLabel(this);
	timeLabel->setFixedWidth(190);
	timeLabel->setText("");
	timeLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	statusbar->addPermanentWidget(timeLabel,1);

    stepTimeLabel = new QLabel(this);
    stepTimeLabel->setFixedWidth(150);
    stepTimeLabel->setText("");
    stepTimeLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusbar->addPermanentWidget(stepTimeLabel,1);
    setStatusBar(statusbar);

    sceneviewer = scenev;
    setCentralWidget(sceneviewer);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));

    showNumberOfVehiclesInQueue( simulator->getNumberOfVehiclesInQueue() );
    showNumberOfVehicles( simulator->getNumberOfVehicles() );
    showDateTime( simulator->getDateTime(), simulator->getSimulationTime() );
    showTime( simulator->getSimulationTime() , simulator->getFinishTime() );
    showStepTime( simulator->getStepTime(), 0.0 );
    updateViewer();
}


void Viewer::update(){
	lastStepTick = QTime::currentTime();
    simulator->step();
    sceneviewer->updateGL();
    nowStepTick = QTime::currentTime();
    int32 elapsedTime = lastStepTick.msecsTo( nowStepTick );
    showNumberOfVehiclesInQueue( simulator->getNumberOfVehiclesInQueue() );
    showNumberOfVehicles( simulator->getNumberOfVehicles() );
    showDateTime( simulator->getDateTime(), simulator->getSimulationTime() );
    showTime( simulator->getSimulationTime() , simulator->getFinishTime() );
    showStepTime( simulator->getStepTime(), elapsedTime/1000.0 );
    int32 restTime = simulator->getStepTime()*1000 - elapsedTime;
    if( simulator->getFinishTime() == 0.f || simulator->getSimulationTime() < simulator->getFinishTime() ){
    	if( realTime && restTime > 0){
    		timer->setInterval(restTime);
    	}else{
    		timer->setInterval(0);
    	}
    }else{
		timer->stop();
		simulator->stop();
		updateViewer();
    }
}

void Viewer::setInputEventHandler(InputEventHandler* handler){
	sceneviewer->setInputEventHandler(handler);
};

void Viewer::setCenter(double x, double y) {
	sceneviewer->setCenter(x,y);
}

void Viewer::closeEvent(QCloseEvent *event) {
	stop();
    event->accept();
}

Viewer::~Viewer(){
	delete timer;
}

void Viewer::onActionStart() {
	realTime = true;
	if(!simulator->isRunning()){
		simulator->start();
		timer->start(0);
	}else{
		if(paused){
			paused = false;
			timer->start(0);
		}
	}
	updateViewer();
}

void Viewer::onActionStartFast() {
	realTime = false;
	if(!simulator->isRunning()){
		simulator->start();
		timer->start(0);
	}else{
		if(paused){
			paused = false;
			timer->start(0);
		}
	}
	updateViewer();
}

void Viewer::onActionStop() {
	if(simulator->isRunning()){
		timer->stop();
		simulator->stop();
		paused = false;
		sceneviewer->updateGL();
		showNumberOfVehiclesInQueue( simulator->getNumberOfVehiclesInQueue() );
		showNumberOfVehicles( simulator->getNumberOfVehicles() );
		showDateTime( simulator->getDateTime(), simulator->getSimulationTime() );
		showTime( simulator->getSimulationTime() , simulator->getFinishTime() );
		showStepTime( simulator->getStepTime(), 0.0 );
	}
    updateViewer();
}

void Viewer::onActionStep() {
	if( !simulator->isRunning() ){
		simulator->start();
	}
	timer->stop();
	paused = true;
	lastStepTick = QTime::currentTime();
    simulator->step();
    sceneviewer->updateGL();
    nowStepTick = QTime::currentTime();
    int32 elapsedTime = lastStepTick.msecsTo( nowStepTick );
    showNumberOfVehicles( simulator->getNumberOfVehicles() );
    showDateTime( simulator->getDateTime(), simulator->getSimulationTime() );
    showTime( simulator->getSimulationTime() , simulator->getFinishTime() );
    showStepTime( simulator->getStepTime(), elapsedTime/1000.0 );
    updateViewer();
}

void Viewer::onActionPause() {
	paused = true;
	if(paused){
		timer->stop();
	}
	updateViewer();
}

void Viewer::updateViewer() {
    if (simulator->isRunning()) {
    	if(paused){
    		actionStart->setEnabled( true );
    		actionStartFast->setEnabled( true );
    		actionPause->setEnabled( false );
    	}else{
    		actionStart->setEnabled( !realTime );
    		actionStartFast->setEnabled( realTime );
    		actionPause->setEnabled( true );
    	}
    	actionStop->setEnabled( true );
    }else{
        actionStart->setEnabled(true);
        actionStartFast->setEnabled(true);
        actionStop->setEnabled(false);
        actionPause->setEnabled(false);
    }
}

void Viewer::setPainter(Painter* painter){
	sceneviewer->setPainter(painter);
}

void Viewer::pause(){
	if( actionPause->isEnabled() ){
		onActionPause();
	}
}
void Viewer::stop(){
	if( actionStop->isEnabled() ){
		onActionStop();
	}
}
void Viewer::step(){
	if( actionStep->isEnabled() ){
		onActionStep();
	}
}
void Viewer::start(bool fast){
	if( actionStart->isEnabled() ){
		if(fast){
			onActionStartFast();
		}else{
			onActionStart();
		}
	}
}

void Viewer::onVehicleRemoved(const vns::Simulator* , vns::Vehicle* vehicle){
	sceneviewer->onLaneObjectRemoved(vehicle);
}

void Viewer::onVehicleInjected(const vns::Simulator* , vns::Vehicle* vehicle){
	sceneviewer->onLaneObjectCreated(vehicle);
}

void Viewer::showNumberOfVehicles(unsigned int n) {
	QString buf = QString("#Vehicles: %1").arg(n);
    numberOfVehiclesLabel->setText(buf);
}

void Viewer::showNumberOfVehiclesInQueue(unsigned int n) {
	QString buf = QString("#Queue: %1").arg(n);
    numberOfVehiclesInBacklogLabel->setText(buf);
}

void Viewer::showDateTime(uint32 dt, double t){
	if(dt>0){
		dateTimeLabel->setText( QDateTime::fromTime_t(quint32(dt+t)).toString("dd-MM-yyyy hh:mm:ss") );
	}else{
		dateTimeLabel->setText("");
	}
}

void Viewer::showTime(double t, double end) {
    int ht = (int) (t/3600.0);
    t -= ht*3600.0;
    int mt = (int) (t/60.0);
    if( end == 0){
    	QString buf;
    	buf.sprintf("Time: %02d:%02d:%02.1f / ...", ht, mt, t-mt*60.0);
    	timeLabel->setText(buf);
    }else{
        int he = (int) (end/3600.0);
        end -= he*3600.0;
        int me = (int) (end/60.0);
        QString buf;
    	buf.sprintf("Time: %02d:%02d:%04.1f / %02d:%02d:%02.0f", ht, mt, t-mt*60.0, he,me,end-me*60.0);
        timeLabel->setText(buf);
    }
}

void Viewer::showStepTime(double stepTime, double execTime) {
    QString buf;
	buf.sprintf("Step: %02.3f/%02.3f s", stepTime, execTime);
    stepTimeLabel->setText(buf);
}


}

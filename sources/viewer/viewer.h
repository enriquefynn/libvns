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

#ifndef VNS_VIEWER_H
#define VNS_VIEWER_H

#include <QtGui/QMainWindow>
#include "simulator.h"
#include "sceneviewer.h"
#include "observer.h"

class QWidget;
class QAction;
class QCloseEvent;
class QToolBar;
class QTime;
class QStatusBar;
class QLabel;

namespace vns {

/*! \cond PRIVATE */

class Viewer : public QMainWindow , public Observer {

    Q_OBJECT

public:
    Viewer( Simulator* simulator , SceneViewer* sceneViewer, QWidget* parent=0);
    ~Viewer();
    void setPainter(Painter* painter);
	void setInputEventHandler(InputEventHandler* handler);
    void start(bool fast=false);
    void pause();
    void stop();
    void step();

    inline SceneViewer* getSceneViewer(){ return sceneviewer; };

    /* Observer */
	void onVehicleRemoved(const vns::Simulator* , vns::Vehicle* );
	void onVehicleInjected(const vns::Simulator* , vns::Vehicle* );

protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void onActionStart();
    void onActionStartFast();
    void onActionStep();
    void onActionStop();
    void onActionPause();
    void setCenter(double x, double y);
    void update();

private:

    Simulator *simulator;
    SceneViewer *sceneviewer;

    QAction *actionStart;
    QAction *actionStop;
    QAction *actionStartFast;
    QAction *actionPause;
    QAction *actionStep;

    QToolBar* toolbar;

    /* Status Bar */
    QStatusBar *statusbar;
    QLabel* timeLabel;
    QLabel* dateTimeLabel;
    QLabel* stepTimeLabel;
    QLabel* numberOfVehiclesLabel;
    QLabel* numberOfVehiclesInBacklogLabel;

	void showNumberOfVehicles(unsigned int n);
	void showNumberOfVehiclesInQueue(unsigned int n);
    void showTime(double t, double end);
    void showDateTime(uint32 dt,double t);
    void showFinishTime(double t);
    void showStepTime(double stepTime, double execTime);
    /*----------------*/


    QTimer* timer;
    QTime lastStepTick;
    QTime nowStepTick;
    bool paused;
    bool realTime;

    void updateViewer();

    friend class Simulator;
};
/*! \endcond */

}

#endif

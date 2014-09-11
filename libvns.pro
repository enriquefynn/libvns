# ##################################################################
# #
# Copyright (C) 2011-2012 Ricardo Fernandes - All Rights Reserved #
# Email: rjf@dcc.fc.up.pt or rikardojfernandez@gmail.com          #
# #
# This file is part of VNS - Vehicular Networks Simulator.        #
# #
# For information about the licensing and copyright of this code  #
# please contact the author.                                      #
# #
# ##################################################################
OBJECTS_DIR = tmp/
MOC_DIR = tmp/
DESTDIR = release/
DEFINES -= QT_WEBKIT
CONFIG -= qt
TEMPLATE = lib
VERSION = 1.0
TARGET = vns
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp
CONFIG += all
DEPENDPATH += sources \
    sources/libs \
    sources/core \
    sources/vehicles \
    sources/vehicles/modules \
    sources/drivermodels \
    sources/routing \
    sources/maps \
    sources/trafficlights \
    sources/communication \
    sources/simulation \
    sources/tracks
INCLUDEPATH += sources \
    sources/libs \
    sources/core \
    sources/vehicles \
    sources/vehicles/modules \
    sources/drivermodels \
    sources/routing \
    sources/maps \
    sources/trafficlights \
    sources/communication \
    sources/simulation \
    sources/tracks
HEADERS += sources/simulation/generationfunction.h \
    sources/simulation/trafficgenerationmodel.h \
    sources/simulation/observer.h \
    sources/tracks/kmltracksfile.h \
    sources/tracks/tracksfile.h \
    sources/tracks/tracker.h \
    sources/core/listscheduler.h \
    sources/core/calendarscheduler.h \
    sources/core/eventscheduler.h \
    sources/core/kdtree.h \
    sources/maps/mapfilters.h \
    sources/drivermodels/pipes.h \
    sources/routing/routesgenerator.h \
    sources/drivermodels/traincarmodel.h \
    sources/drivermodels/eidm.h \
    sources/drivermodels/ovm.h \
    sources/drivermodels/fvdm.h \
    sources/drivermodels/nagel.h \
    sources/core/mapgrid.h \
    sources/core/mercartor.h \
    sources/simulation/parallelsimulator.h \
    sources/simulation/simulator.h \
    sources/core/hashtable.h \
    sources/routing/routesfile.h \
    sources/maps/manhattanmap.h \
    sources/core/lock.h \
    sources/core/maths.h \
    sources/core/list.h \
    sources/drivermodels/kkw.h \
    sources/vehicles/modules/travelstatsmodule.h \
    sources/vehicles/modules/vehiclemodule.h \
    sources/vehicles/modules/modulesfactory.h \
    sources/drivermodels/newell.h \
    sources/drivermodels/krauss.h \
    sources/drivermodels/gipps.h \
    sources/drivermodels/idm.h \
    sources/maps/mapfile.h \
    sources/trafficlights/trafficlightsgenerator.h \
    sources/buildings.h \
    sources/building.h \
    sources/core/polygon.h \
    sources/core/datastream.h \
    sources/core/vector.h \
    sources/vehicles/vehiclefactory.h \
    sources/drivermodels/drivers.h \
    sources/drivermodels/driverinfo.h \
    sources/routing/routingtable.h \
    sources/routing/membasedroutingtable.h \
    sources/vehicles/train.h \
    sources/vehicles/taxi.h \
    sources/routing/tablehandler.h \
    sources/routing/dijkstrahandler.h \
    sources/core/core.h \
    sources/routing/scheduledroute.h \
    sources/routing/route.h \
    sources/routing/astarhandler.h \
    sources/routing/path.h \
    sources/routing/pathfinder.h \
    sources/routing/pathfinderhandler.h \
    sources/routing/pathnode.h \
    sources/routing/pathstop.h \
    sources/routing/routes.h \
    sources/core/bbox.h \
    sources/core/bitset.h \
    sources/core/rtree.h \
    sources/core/trajectory.h \
    sources/core/vec.h \
    sources/vns.h \
    sources/vehicles/ghostvehicle.h \
    sources/vehicles/vehicle.h \
    sources/drivermodels/drivermodel.h \
    sources/road.h \
    sources/laneobject.h \
    sources/object.h \
    sources/junction.h \
    sources/lane.h \
    sources/polyline.h \
    sources/roadnetwork.h \
    sources/trigger.h \
    sources/trafficlights/fixedtimetrafficlightcontroller.h \
    sources/trafficlights/pretimedtrafficlightcontroller.h \
    sources/trafficlights/trafficlightcontroller.h \
    sources/communication/networkmodule.h \
    sources/communication/simplenetworkmodule.h
SOURCES += sources/core/lock.cpp \
    sources/simulation/generationfunction.cpp \
    sources/simulation/trafficgenerationmodel.cpp \
    sources/core/mercartor.cpp \
    sources/tracks/kmltracksfile.cpp \
    sources/tracks/tracker.cpp \
    sources/core/listscheduler.cpp \
    sources/core/calendarscheduler.cpp \
    sources/core/kdtree.cpp \
    sources/maps/mapfilters.cpp \
    sources/drivermodels/pipes.cpp \
    sources/core/core.cpp \
    sources/laneobject.cpp \
    sources/routing/routesgenerator.cpp \
    sources/drivermodels/traincarmodel.cpp \
    sources/drivermodels/eidm.cpp \
    sources/drivermodels/ovm.cpp \
    sources/drivermodels/fvdm.cpp \
    sources/drivermodels/nagel.cpp \
    sources/drivermodels/driverinfo.cpp \
    sources/simulation/parallelsimulator.cpp \
    sources/simulation/simulator.cpp \
    sources/core/datastream.cpp \
    sources/routing/routesfile.cpp \
    sources/routing/pathfinderhandler.cpp \
    sources/maps/manhattanmap.cpp \
    sources/core/maths.cpp \
    sources/drivermodels/kkw.cpp \
    sources/vehicles/modules/travelstatsmodule.cpp \
    sources/vehicles/modules/modulesfactory.cpp \
    sources/drivermodels/newell.cpp \
    sources/drivermodels/krauss.cpp \
    sources/drivermodels/gipps.cpp \
    sources/drivermodels/idm.cpp \
    sources/drivermodels/drivers.cpp \
    sources/maps/mapfile.cpp \
    sources/trafficlights/trafficlightsgenerator.cpp \
    sources/buildings.cpp \
    sources/core/polygon.cpp \
    sources/vehicles/vehiclefactory.cpp \
    sources/routing/membasedroutingtable.cpp \
    sources/vehicles/train.cpp \
    sources/vehicles/taxi.cpp \
    sources/routing/pathnode.cpp \
    sources/routing/pathstop.cpp \
    sources/routing/tablehandler.cpp \
    sources/routing/dijkstrahandler.cpp \
    sources/routing/scheduledroute.cpp \
    sources/routing/route.cpp \
    sources/routing/astarhandler.cpp \
    sources/routing/path.cpp \
    sources/routing/pathfinder.cpp \
    sources/routing/routes.cpp \
    sources/core/rtree.cpp \
    sources/vehicles/ghostvehicle.cpp \
    sources/vehicles/vehicle.cpp \
    sources/drivermodels/drivermodel.cpp \
    sources/road.cpp \
    sources/trafficlights/fixedtimetrafficlightcontroller.cpp \
    sources/trafficlights/pretimedtrafficlightcontroller.cpp \
    sources/trafficlights/trafficlightcontroller.cpp \
    sources/junction.cpp \
    sources/lane.cpp \
    sources/polyline.cpp \
    sources/roadnetwork.cpp \
    sources/communication/simplenetworkmodule.cpp
alib:CONFIG += staticlib \
    warn_on \
    openmp \
    shapefile \
    release
all:CONFIG += dll \
    warn_on \ # create_prl \
    openmp \
    viewer \ # splines \
# osg \
    shapefile \
    xml \
    debug
osg { 
    DEFINES += "OSG=1"
    LIBS += -losg \
        -losgDB \
        -losgViewer \
        -losgTerrain
    DEPENDPATH += sources/viewer/osg
    INCLUDEPATH += sources/viewer/osg
    HEADERS += sources/viewer/osg/srtm.h \
        sources/viewer/osg/osgfactory.h \
        sources/viewer/osg/osgglwidget.h \
        sources/viewer/osg/osgmapviewer.h \
        sources/viewer/osg/osgscenario.h \
        sources/viewer/osg/osgvehicle.h
    SOURCES += sources/viewer/osg/srtm.cpp \
        sources/viewer/osg/osgfactory.cpp \
        sources/viewer/osg/osgglwidget.cpp \
        sources/viewer/osg/osgmapviewer.cpp \
        sources/viewer/osg/osgscenario.cpp
}
shapefile { 
    DEFINES += "SHAPEFILE=1"
    DEPENDPATH += sources/libs/shapelib
    INCLUDEPATH += sources/libs/shapelib
    HEADERS += sources/libs/shapelib/shapefil.h \
        sources/maps/shpmapfile.h \
        sources/maps/infoptmapfile.h \
        sources/maps/vnsshpmapfile.h \
        sources/maps/tigermapfile.h \
        sources/maps/buildingsloader.h
    SOURCES += sources/libs/shapelib/dbfopen.c \
        sources/libs/shapelib/safileio.c \
        sources/libs/shapelib/shpopen.c \
        sources/libs/shapelib/shptree.c \
        sources/maps/shpmapfile.cpp \
        sources/maps/infoptmapfile.cpp \
        sources/maps/vnsshpmapfile.cpp \
        sources/maps/tigermapfile.cpp \
        sources/maps/buildingsloader.cpp
}
viewer { 
    DEFINES += "VIEWER=1"
    RESOURCES += resources.qrc
    CONFIG += qt
    QT += core \
        gui \
        opengl
    LIBS += 
    DEPENDPATH += sources/viewer/ \
        sources/viewer/2d
    INCLUDEPATH += sources/viewer \
        sources/viewer/2d
    HEADERS += sources/viewer/viewer.h \
        sources/viewer/visualizer.h \
        sources/viewer/painter.h \
        sources/viewer/2d/mapviewer.h \
        sources/viewer/sceneviewer.h \
        sources/viewer/inputeventhandler.h \
        sources/viewer/panhandler.h
    SOURCES += sources/viewer/viewer.cpp \
        sources/viewer/visualizer.cpp \
        sources/viewer/painter.cpp \
        sources/viewer/panhandler.cpp \
        sources/viewer/sceneviewer.cpp \
        sources/viewer/2d/mapviewer.cpp
}
xml { 
    DEFINES += "XML=1"
    QT += xml
    LIBS += 
    HEADERS += sources/routing/xmlroutesfile.h \
        sources/routing/sumoroutesfile.h \
        sources/maps/xmlmapfile.h \
        sources/maps/osmmapfile.h \
        sources/maps/sumomapfile.h
    SOURCES += sources/routing/sumoroutesfile.cpp \
        sources/routing/xmlroutesfile.cpp \
        sources/maps/osmmapfile.cpp \
        sources/maps/xmlmapfile.cpp \
        sources/maps/sumomapfile.cpp
}
macx:CONFIG -= app-bundle

# INSTALL
VNS_INSTALL_PREFIX = $$[QT_INSTALL_PREFIX]
unix:VNS_INSTALL_PREFIX = /usr
win32:VNS_INSTALL_PREFIX = windows
macx:VNS_INSTALL_PREFIX = /usr
includes.CONFIG = no_check_exist
includes.files = $${HEADERS}
includes.path = $$VNS_INSTALL_PREFIX/include/vns/
INSTALLS += includes
target.path = $$VNS_INSTALL_PREFIX/lib/
INSTALLS += target

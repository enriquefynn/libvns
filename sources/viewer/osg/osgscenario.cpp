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

#include <osgDB/ReadFile>
#include "osgscenario.h"

namespace vns {

OSGScenario::OSGScenario() {
	// TODO Auto-generated constructor stub
	osg::Image* asphaltImage = osgDB::readImageFile("/home/rjf/projects/libvns/images/osg/osgasphalt.jpg");
    osg::ref_ptr<osg::Texture2D> asphaltTexture = new osg::Texture2D;
    asphaltTexture->setDataVariance(osg::Object::STATIC);
    asphaltTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    asphaltTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    asphaltTexture->setImage(asphaltImage);

    asphaltStateSet = new osg::StateSet();
    // assign the material and texture
    //roadStateSet->setAttribute(roadMaterial);
    asphaltStateSet->setTextureAttributeAndModes(0, asphaltTexture, osg::StateAttribute::ON);
    asphaltStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    int alphaDepthState = 1;
    switch (alphaDepthState){
    case 1:
    	asphaltStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
    	asphaltStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    	break;
    case 2:
    	asphaltStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    	break;
    }

	// TODO Auto-generated constructor stub

	osg::ref_ptr<osg::Material> markersMaterial = new osg::Material();
	markersMaterial->setEmission(osg::Material::FRONT, osg::Vec4(0.9, 0.9, 0.9, 0.5));
	markersMaterial->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

	osg::Image* markersImage = osgDB::readImageFile("/home/rjf/projects/libvns/images/osg/osgmarkers.jpg");
    osg::ref_ptr<osg::Texture2D> markersTexture = new osg::Texture2D;
    //markersTexture->setDataVariance(osg::Object::STATIC);
	markersTexture->setDataVariance(osg::Object::DYNAMIC);
	markersTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	markersTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    markersTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    markersTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    markersTexture->setImage(markersImage);



    markersStateSet = new osg::StateSet();
    // assign the material and texture
    //roadStateSet->setAttribute(roadMaterial);
	markersStateSet->setAttribute(markersMaterial);
    markersStateSet->setTextureAttributeAndModes(0, markersTexture, osg::StateAttribute::ON);
    markersStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	markersStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
	markersStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);


	// TODO Auto-generated constructor stub
	osg::Image* grassImage  = osgDB::readImageFile("/home/rjf/projects/libvns/images/osg/osggrass.jpg");
    osg::ref_ptr<osg::Texture2D> grassTexture = new osg::Texture2D;
    grassTexture->setDataVariance(osg::Object::STATIC);
    grassTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    grassTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    grassTexture->setImage(grassImage);

    grassStateSet = new osg::StateSet();
    // assign the material and texture
    //roadStateSet->setAttribute(roadMaterial);
    grassStateSet->setTextureAttributeAndModes(0, grassTexture, osg::StateAttribute::ON);
    grassStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    switch (alphaDepthState){
    case 1:
    	grassStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
    	grassStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    	break;
    case 2:
    	grassStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    	break;
    }


}

OSGScenario::~OSGScenario() {
	// TODO Auto-generated destructor stub
}

osg::Group* OSGScenario::generateRoadNetwork(const vns::RoadNetwork* network){
	osg::Group* root = new osg::Group;

	/*
	osg::Node* terrain = createHeightField(network);
	root->addChild(terrain);
	*/

	int32 nr = network->getNumberOfRoads();
	for(int32 i=0;i<nr;i++){
		const vns::Road* road = network->getRoad(i);
		osg::Node* node = createRoad( road);
		root->addChild(node);
	}

	int32 nj = network->getNumberOfJunctions();
	for(int32 j=0;j<nj;j++){
		const vns::Junction* junction = network->getJunction(j);
		osg::Node* node = createJunction( junction );
		root->addChild(node);
	}

	return root;
}

osg::Node* OSGScenario::createRoad(const vns::Road* road){
	osg::ref_ptr<osg::Geometry> roadGeometry = new osg::Geometry();
	//roadGeometry->setUseDisplayList( false );
	//its arrays
	osg::ref_ptr<osg::Vec3Array> roadVertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> roadNormals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> roadColors = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec2Array> roadTexCoords = new osg::Vec2Array;
	//struct with pointers to pass the above arrays to the method

	//set the geometry arrays and continue
	roadGeometry->setVertexArray( roadVertices.get() );
	roadGeometry->setNormalArray( roadNormals.get() );
	roadGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	roadGeometry->setColorArray(roadColors.get());
	roadGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	roadGeometry->setTexCoordArray( 0, roadTexCoords.get() );

	vns::Vec pt;
	vns::Vec d1, p1, n1;
	vns::Vec dx1, px1, nx1;
	vns::Vec d2, p2, n2;
	vns::Vec dx2, px2, nx2;

	double rwm = road->getWidth()*0.5+ROAD_MARGINSGAP;
	//double rw = road->getWidth()*0.5;
	double start = road->getRoadStart();
	double end = road->getRoadEnd();
	double offset = road->getRoadOffset();
	double texscale = 0.5;

	const vns::PolyLine* line = road->getPolyLine();
	int32 n = line->size();
    int32 i=0;
    while(i<n && line->getT(i) <= start){
    	i++;
    }
    road->getStartPoint(p1,d1,n1);
    road->getEndPoint(p2,d2,n2);

    pt = p1-n1*rwm;
    roadVertices->push_back( osg::Vec3(pt.getX(),pt.getY(), pt.getZ()) );
    roadTexCoords->push_back( osg::Vec2(pt.getX()*texscale,pt.getY()*texscale) );

    pt = p1+n1*rwm;
    roadVertices->push_back( osg::Vec3(pt.getX(),pt.getY(), pt.getZ()) );
    roadTexCoords->push_back( osg::Vec2(pt.getX()*texscale,pt.getY()*texscale) );

    while(i<n && line->getT(i) < end){
        pt = line->getPoint(i)+line->getNormal(i)*(offset-rwm);
        roadVertices->push_back( osg::Vec3(pt.getX(),pt.getY(), pt.getZ()) );
        roadTexCoords->push_back( osg::Vec2(pt.getX()*texscale,pt.getY()*texscale) );
        pt = line->getPoint(i)+line->getNormal(i)*(offset+rwm);
        roadVertices->push_back( osg::Vec3(pt.getX(),pt.getY(), pt.getZ()) );
        roadTexCoords->push_back( osg::Vec2(pt.getX()*texscale,pt.getY()*texscale) );
    	i++;
    }
    pt = p2-n2*rwm;
    roadVertices->push_back( osg::Vec3(pt.getX(),pt.getY(), pt.getZ()) );
    roadTexCoords->push_back( osg::Vec2(pt.getX()*texscale,pt.getY()*texscale) );
    pt = p2+n2*rwm;
    roadVertices->push_back( osg::Vec3(pt.getX(),pt.getY(), pt.getZ()) );
    roadTexCoords->push_back( osg::Vec2(pt.getX()*texscale,pt.getY()*texscale) );

	roadNormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
    roadColors->push_back( osg::Vec4(0.45f, 0.45f, 0.45f, 1.0f ));
	roadGeometry->addPrimitiveSet(new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP ,0,roadVertices->size()));

    osg::Group* roadNode = new osg::Group;

    osg::Geode *geode = new osg::Geode;

    geode->setStateSet( asphaltStateSet );
    geode->addDrawable( roadGeometry );
    roadNode->addChild( geode );

    osg::Node* markers = createRoadMarkers(road);
    roadNode->addChild( markers );

    return roadNode;
}

osg::Node* OSGScenario::createJunction(const vns::Junction* junction){

	osg::ref_ptr<osg::Geometry> junctionGeometry = new osg::Geometry();
	//roadGeometry->setUseDisplayList( false );

	osg::ref_ptr<osg::Vec3Array> junctionVertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> junctionNormals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> junctionColors = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec2Array> junctionTexCoords = new osg::Vec2Array;
	junctionGeometry->setVertexArray( junctionVertices.get() );
	junctionGeometry->setNormalArray( junctionNormals.get() );
	junctionGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	junctionGeometry->setColorArray(junctionColors.get());
	junctionGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	junctionGeometry->setTexCoordArray( 0, junctionTexCoords.get() );


	int32 nr = junction->getNumberOfRoads();
	vns::Vec c = junction->getPosition();
	vns::Vec p,p1,pa,pb,p4,n1,n2,n3,n4,d1,d4;
	vns::Vec pos,dir;
	const vns::Road* road1;
	const vns::Road* road2;
	double texscale = 0.5;

	junctionVertices->push_back( osg::Vec3(c.getX(),c.getY(), c.getZ()) );
	junctionTexCoords->push_back( osg::Vec2(c.getX()*texscale,c.getY()*texscale) );
	for(int32 i=0; i<nr;i++){
		road1 = junction->getRoad(i);
		double w1 = road1->getWidth()*0.5+ROAD_MARGINSGAP;
		if(road1->getEndJunction() == junction){
			road1->getEndPoint(p1,d1,n1,vns::Forward);
		}else{
			road1->getEndPoint(p1,d1,n1,vns::Backward);
		}
		if(i==nr-1){
			road2 = junction->getRoad(0);
		}else{
			road2 = junction->getRoad(i+1);
		}
		double w4 = road2->getWidth()*0.5+ROAD_MARGINSGAP;
		if( road2->getStartJunction() == junction ){
			road2->getStartPoint(p4,d4,n4,vns::Forward);
		}else{
			road2->getStartPoint(p4,d4,n4,vns::Backward);
		}
		pa = p1-n1*w1;
		pb = p4-n4*w4;

		trajectory.create(pa,d1,pb,d4);
		double step = 0.1;
		for(int32 k=0;k<=10;k++){
			trajectory.interpolate(k*step,pos,dir);
		    junctionVertices->push_back( osg::Vec3(pos.getX(),pos.getY(), pos.getZ()) );
		    junctionTexCoords->push_back( osg::Vec2(pos.getX()*texscale,pos.getY()*texscale) );
		}


		if(i==nr-1){
			pos = p4+n4*w4;
			junctionVertices->push_back( osg::Vec3(pos.getX(),pos.getY(), pos.getZ()) );
			junctionTexCoords->push_back( osg::Vec2(pos.getX()*texscale,pos.getY()*texscale) );
		}
	}

	junctionNormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
	junctionColors->push_back( osg::Vec4(0.45f, 0.45f, 0.45f, 1.0f ));
    junctionGeometry->addPrimitiveSet(new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_FAN ,0,junctionVertices->size()));

    osg::Group* junctionNode = new osg::Group;
    osg::Geode *geode = new osg::Geode;

    geode->setStateSet( asphaltStateSet );
    geode->addDrawable( junctionGeometry );

    junctionNode->addChild(geode);

    osg::Node *markers = createJunctionMarkers(junction);
    junctionNode->addChild(markers);

    return junctionNode;
}

osg::Node* OSGScenario::createJunctionMarkers(const vns::Junction* junction){

	osg::LOD* lodNode = new osg::LOD();
	osg::Geode *geode = new osg::Geode;
	geode->setStateSet( markersStateSet );
	Vec p,p1,p2,p3,p4,n1,n2,n3,n4,d1,d4;

	const Road* road1;
	const Road* road2;
	int32 nr = junction->getNumberOfRoads();
	for(int32 i=0; i<nr;i++){
		road1 = junction->getRoad(i);
		double w1 = road1->getWidth()*0.5;
		if(road1->getEndJunction() == junction){
			road1->getEndPoint(p1,d1,n1,Forward);
		}else{
			road1->getEndPoint(p1,d1,n1,Backward);
		}
		if(i==nr-1){
			road2 = junction->getRoad(0);
		}else{
			road2 = junction->getRoad(i+1);
		}
		double w4 = road2->getWidth()*0.5;
		if( road2->getStartJunction() == junction ){
			road2->getStartPoint(p4,d4,n4,Forward);
		}else{
			road2->getStartPoint(p4,d4,n4,Backward);
		}
		p1 = p1-n1*w1;
		p4 = p4-n4*w4;
		osg::Geometry* geometry = createTrajectoryMarkers(p1,d1,n1,p4,d4,n4);
		geode->addDrawable( geometry );
	}
	lodNode->addChild( geode, 1.0f, 300.0f );
	return lodNode;
}

osg::Group* OSGScenario::createHeightField(const vns::RoadNetwork* ){
    vns::SRTM* srtm = new vns::SRTM("/home/rjf/porto_7m.dem");

    osg::Image* high_map = osgDB::readImageFile( "/home/rjf/porto_7m.tif" );

    //osgTerrain::Terrain* terrain = new osgTerrain::Terrain;

	osgTerrain::TerrainTile* tile = new osgTerrain::TerrainTile;

	osg::HeightField* field = new osg::HeightField;
	field->allocate(srtm->height(),srtm->width());
	field->setOrigin(osg::Vec3(srtm->xMin(),srtm->yMin(),0.0));
	field->setXInterval(srtm->dx());
	field->setYInterval(srtm->dy());
	for(int i=0;i<1000;i++){
		for(int j=0;j<srtm->width()-1;j++){
			field->setHeight(i,j, (float)*high_map->data(i,j));
		}
	}
	osgTerrain::HeightFieldLayer* layer = new osgTerrain::HeightFieldLayer;
	layer->setHeightField( field );

	osgTerrain::Locator* locator = new osgTerrain::Locator;
	locator->setCoordinateSystemType( osgTerrain::Locator::PROJECTED );
	double xmin = srtm->xMin();
	double xmax = srtm->xMax();
	double ymin = srtm->yMin();
	double ymax = srtm->yMin()+7.0*1000;
	locator->setTransformAsExtents( xmin, ymin, xmax, ymax );
	layer->setLocator( locator );

	tile->setElevationLayer( layer );

    tile->setStateSet( grassStateSet );
	return tile;

	/*

	osg::ref_ptr<osg::Geometry> terrainGeometry = new osg::Geometry();
	//roadGeometry->setUseDisplayList( false );

	osg::ref_ptr<osg::Vec3Array> terrainVertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> terrainNormals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> terrainColors = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec2Array> terrainTexCoords = new osg::Vec2Array;
	terrainGeometry->setVertexArray( terrainVertices.get() );
	terrainGeometry->setNormalArray( terrainNormals.get() );
	terrainGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	terrainGeometry->setColorArray( terrainColors.get() );
	terrainGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	terrainGeometry->setTexCoordArray( 0, terrainTexCoords.get() );

	vns::BBox bb = network->getExtents();
	vns::Vec o = network->getOffset();
	double texScale = 0.5;


	for(int i=0;i<1000-1;i++){
		if(i%2==0){
			for(int j=0;j<srtm->width()-1;j++){
				double z = srtm->get(i,j);
				vns::Vec p = srtm->getPoint(i,j)-o;
				vns::Vec n = srtm->getNormal(i,j);
			    terrainVertices->push_back( osg::Vec3(p.getX(),p.getY(),z) );
			    terrainTexCoords->push_back( osg::Vec2(p.getX()*0.1,p.getY()*0.1) );
			    terrainNormals->push_back( osg::Vec3(n.getX(),n.getY(),n.getZ()) );
				z = srtm->get(i+1,j);
				p = srtm->getPoint(i+1,j)-o;
				n = srtm->getNormal(i+1,j);
			    terrainVertices->push_back( osg::Vec3(p.getX(),p.getY(),z) );
			    terrainTexCoords->push_back( osg::Vec2(p.getX()*0.9,p.getY()*0.9) );
			    terrainNormals->push_back( osg::Vec3(n.getX(),n.getY(),n.getZ()) );
			}
			double z = srtm->get(i,srtm->width()-1);
			vns::Vec p = srtm->getPoint(i,srtm->width()-1)-o;
			vns::Vec n = srtm->getNormal(i,srtm->width()-1);
		    terrainVertices->push_back( osg::Vec3(p.getX(),p.getY(),z) );
		    terrainTexCoords->push_back( osg::Vec2(p.getX()*0.1,p.getY()*0.1) );
		    terrainNormals->push_back( osg::Vec3(n.getX(),n.getY(),n.getZ()) );
		}else{
			for(int j=srtm->width()-1;j>0;j--){
				double z = srtm->get(i,j);
				vns::Vec p = srtm->getPoint(i,j)-o;
				vns::Vec n = srtm->getNormal(i,j);
			    terrainVertices->push_back( osg::Vec3(p.getX(),p.getY(),z) );
			    terrainTexCoords->push_back( osg::Vec2(p.getX()*0.1,p.getY()*0.1) );
			    terrainNormals->push_back( osg::Vec3(n.getX(),n.getY(),n.getZ()) );
				z = srtm->get(i+1,j);
				p = srtm->getPoint(i+1,j)-o;
				n = srtm->getNormal(i+1,j);
			    terrainVertices->push_back( osg::Vec3(p.getX(),p.getY(),z) );
			    terrainTexCoords->push_back( osg::Vec2(p.getX()*0.9,p.getY()*0.9) );
			    terrainNormals->push_back( osg::Vec3(n.getX(),n.getY(),n.getZ()) );
			}
			double z = srtm->get(i,0);
			vns::Vec p = srtm->getPoint(i,0)-o;
			vns::Vec n = srtm->getNormal(i,0);
		    terrainVertices->push_back( osg::Vec3(p.getX(),p.getY(),z) );
		    terrainTexCoords->push_back( osg::Vec2(p.getX()*0.1,p.getY()*0.1) );
		    terrainNormals->push_back( osg::Vec3(n.getX(),n.getY(),n.getZ()) );
		}

	}

	delete srtm;

    osg::Geode *geode = new osg::Geode;

    terrainNormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
	//terrainColors->push_back( osg::Vec4(0.0f, 0.45f, 0.0f, 1.0f ));
	terrainColors->push_back( osg::Vec4(0.45f, 0.45f, 0.45f, 1.0f ));
	terrainGeometry->addPrimitiveSet(new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP ,0,terrainVertices->size()));

    geode->setStateSet( asphaltStateSet );
    geode->addDrawable( terrainGeometry );

	return geode;
	*/
}

osg::Geometry*  OSGScenario::createTrajectoryMarkers(const vns::Vec& p1,const vns::Vec& d1,const vns::Vec& n1, const vns::Vec& p4,const vns::Vec& d4,const vns::Vec& n4,double lineWidth){
	double lw = lineWidth*0.5;
	double texscale = 0.5;

	osg::Geometry* markersGeometry = new osg::Geometry();
	//roadGeometry->setUseDisplayList( false );
	//its arrays
	osg::ref_ptr<osg::Vec3Array> markersVertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> markersNormals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> markersColors = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec2Array> markersTexCoords = new osg::Vec2Array;
	//struct with pointers to pass the above arrays to the method

	//set the geometry arrays and continue
	markersGeometry->setVertexArray( markersVertices.get() );
	markersGeometry->setNormalArray( markersNormals.get() );
	markersGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	markersGeometry->setColorArray(markersColors.get());
	markersGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	markersGeometry->setTexCoordArray( 0, markersTexCoords.get() );

	vns::Vec p2 = p1+d1*ROAD_MARGINSGAP;
	vns::Vec p3 = p4-d4*ROAD_MARGINSGAP;

	vns::Vec p = p1+n1*lw;
	markersVertices->push_back( osg::Vec3(p.getX(),p.getY(), p.getZ()) );
	markersTexCoords->push_back( osg::Vec2(p.getX()*texscale,p.getY()*texscale) );
	p = p1-n1*lw;
	markersVertices->push_back( osg::Vec3(p.getX(),p.getY(), p.getZ()) );
	markersTexCoords->push_back( osg::Vec2(p.getX()*texscale,p.getY()*texscale) );

	trajectory.create(p2,d1,p3,d4);
	double step = 0.1;
	Vec pos,dir;
	Vec np = n1;
	Vec nc,nn;

	for(int32 k=0;k<=10;k++){
		trajectory.interpolate(k*step,pos,dir);
		nc = dir.perpendicular().normalized();
		nn = (1.0/(1.0+np*nc))*(np+nc);
		p = pos+nn*lw;
		markersVertices->push_back( osg::Vec3(p.getX(),p.getY(), p.getZ()) );
		markersTexCoords->push_back( osg::Vec2(p.getX()*texscale,p.getY()*texscale) );
		p = pos-nn*lw;
		markersVertices->push_back( osg::Vec3(p.getX(),p.getY(), p.getZ()) );
		markersTexCoords->push_back( osg::Vec2(p.getX()*texscale,p.getY()*texscale) );
	}
	p = p4+n4*lw;
	markersVertices->push_back( osg::Vec3(p.getX(),p.getY(), p.getZ()) );
	markersTexCoords->push_back( osg::Vec2(p.getX()*texscale,p.getY()*texscale) );
	p = p4-n4*lw;
	markersVertices->push_back( osg::Vec3(p.getX(),p.getY(), p.getZ()) );
	markersTexCoords->push_back( osg::Vec2(p.getX()*texscale,p.getY()*texscale) );

    markersNormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
	markersColors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 0.8f ));
	markersGeometry->addPrimitiveSet(new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP ,0,markersVertices->size()));

	return markersGeometry;
}


void OSGScenario::addContinuousLine(osg::Geode* geode,const PolyLine* line, double offset, double start, double end, double lineWidth) {

	int32 n = line->size();
	double d = lineWidth*0.5;
	double texscale = 0.5;

	osg::Geometry* markersGeometry = new osg::Geometry();
	//roadGeometry->setUseDisplayList( false );
	osg::ref_ptr<osg::Vec3Array> markersVertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> markersNormals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> markersColors = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec2Array> markersTexCoords = new osg::Vec2Array;
	markersGeometry->setVertexArray( markersVertices.get() );
	markersGeometry->setNormalArray( markersNormals.get() );
	markersGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	markersGeometry->setColorArray(markersColors.get());
	markersGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	markersGeometry->setTexCoordArray( 0, markersTexCoords.get() );

	Vec pt,dir,perp;
    int32 i=0;
    while(i<n && line->getT(i) <= start){ i++; }

    line->pointDirFromFirstPoint(pt,dir,perp,start);
    Vec pt1 = pt+perp*(offset-d);
    Vec pt2 = pt+perp*(offset+d);
    markersVertices->push_back( osg::Vec3(pt1.getX(),pt1.getY(), pt1.getZ()) );
    markersTexCoords->push_back( osg::Vec2(pt1.getX()*texscale,pt1.getY()*texscale) );
    markersVertices->push_back( osg::Vec3(pt2.getX(),pt2.getY(), pt2.getZ()) );
    markersTexCoords->push_back( osg::Vec2(pt2.getX()*texscale,pt2.getY()*texscale) );

    while(i<n && line->getT(i) < end){
        pt1 = line->getPoint(i)+line->getNormal(i)*(offset-d);
        pt2 = line->getPoint(i)+line->getNormal(i)*(offset+d);
        markersVertices->push_back( osg::Vec3(pt1.getX(),pt1.getY(), pt1.getZ()) );
        markersTexCoords->push_back( osg::Vec2(pt1.getX()*texscale,pt1.getY()*texscale) );
        markersVertices->push_back( osg::Vec3(pt2.getX(),pt2.getY(), pt2.getZ()) );
        markersTexCoords->push_back( osg::Vec2(pt2.getX()*texscale,pt2.getY()*texscale) );
    	i++;
    }

    line->pointDirFromFirstPoint(pt,dir,perp,end);
    pt1 = pt+perp*(offset-d);
    pt2 = pt+perp*(offset+d);
    markersVertices->push_back( osg::Vec3(pt1.getX(),pt1.getY(), pt1.getZ()) );
    markersTexCoords->push_back( osg::Vec2(pt1.getX()*texscale,pt1.getY()*texscale) );
    markersVertices->push_back( osg::Vec3(pt2.getX(),pt2.getY(), pt2.getZ()) );
    markersTexCoords->push_back( osg::Vec2(pt2.getX()*texscale,pt2.getY()*texscale) );


    markersNormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
	markersColors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 0.8f ));
	markersGeometry->addPrimitiveSet(new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP ,0,markersVertices->size()));

	geode->addDrawable(markersGeometry);

}

void OSGScenario::addDiscontinuousLine(osg::Geode* geode, const PolyLine* line, double offset, double start, double end, double lineWidth,double spacing) {

	double texscale = 0.5;
	double lw = lineWidth*0.5;
	double lineLength = 1.0;
	int32 n = line->size();

    Vec pt,pt1,pt2,p1,p2,perp,dir;
    int32 i=0;
    while( i < n-1 && line->getT(i+1) < start ){
    	i++;
    }
    double d = start - line->getT(i);
    bool quit = false;
    while( i < n-1){
        p1 = line->getPoint(i)+line->getNormal(i)*offset;
        dir = line->getDirection(i);
        perp = line->getPerpendicular(i);
        if( line->getT(i+1) > end ){
        	p2 = line->getPoint(i)+dir*(end-line->getT(i))+line->getNormal(i+1)*offset;
            quit = true;
        }else{
            p2 = line->getPoint(i+1)+line->getNormal(i+1)*offset;
        }
        double dp = (p2-p1).norm();

        while(d < dp){
            pt1 = (p1+dir*d)-perp*lw;
            pt2 = (p1+dir*d)+perp*lw;
        	osg::Geometry* markersGeometry = new osg::Geometry();
        	//roadGeometry->setUseDisplayList( false );
        	osg::ref_ptr<osg::Vec3Array> markersVertices = new osg::Vec3Array;
        	osg::ref_ptr<osg::Vec3Array> markersNormals = new osg::Vec3Array;
        	osg::ref_ptr<osg::Vec4Array> markersColors = new osg::Vec4Array;
        	osg::ref_ptr<osg::Vec2Array> markersTexCoords = new osg::Vec2Array;
        	markersGeometry->setVertexArray( markersVertices.get() );
        	markersGeometry->setNormalArray( markersNormals.get() );
        	markersGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
        	markersGeometry->setColorArray(markersColors.get());
        	markersGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
        	markersGeometry->setTexCoordArray( 0, markersTexCoords.get() );

            markersVertices->push_back( osg::Vec3(pt1.getX(),pt1.getY(), pt1.getZ()) );
            markersTexCoords->push_back( osg::Vec2(pt1.getX()*texscale,pt1.getY()*texscale) );
            markersVertices->push_back( osg::Vec3(pt2.getX(),pt2.getY(), pt2.getZ()) );
            markersTexCoords->push_back( osg::Vec2(pt2.getX()*texscale,pt2.getY()*texscale) );
            if(d+lineLength > dp){
            	double dif = (d+lineLength)-dp;
                pt1 = pt1+dir*dif;
                pt2 = pt2+dir*dif;
                markersVertices->push_back( osg::Vec3(pt1.getX(),pt1.getY(), pt1.getZ()) );
                markersTexCoords->push_back( osg::Vec2(pt1.getX()*texscale,pt1.getY()*texscale) );
                markersVertices->push_back( osg::Vec3(pt2.getX(),pt2.getY(), pt2.getZ()) );
                markersTexCoords->push_back( osg::Vec2(pt2.getX()*texscale,pt2.getY()*texscale) );
            }else{
                pt1 = pt1+dir*lineLength;
                pt2 = pt2+dir*lineLength;
                markersVertices->push_back( osg::Vec3(pt1.getX(),pt1.getY(), pt1.getZ()) );
                markersTexCoords->push_back( osg::Vec2(pt1.getX()*texscale,pt1.getY()*texscale) );
                markersVertices->push_back( osg::Vec3(pt2.getX(),pt2.getY(), pt2.getZ()) );
                markersTexCoords->push_back( osg::Vec2(pt2.getX()*texscale,pt2.getY()*texscale) );

                markersNormals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
            	markersColors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 0.8f ));
            	markersGeometry->addPrimitiveSet(new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP ,0,markersVertices->size()));

            	geode->addDrawable( markersGeometry );
            }
            d += lineLength+spacing;
        }
        if(quit){
        	break;
        }
        d = d-dp;
        i++;
    }
}

osg::Node* OSGScenario::createRoadMarkers(const vns::Road* road){

	osg::LOD* lodNode = new osg::LOD();
	osg::Geode *geode = new osg::Geode;
	geode->setStateSet( markersStateSet );

	int32 nl = road->getNumberOfLanes();
	for (int32 i = 0; i < nl; ++i) {
		//const Lane* lane = road->getLane(i);
		//drawLane( w, lane , lod );
	}


	/*********************************/
	// draw Cross Walks
	//road->setEndCrossWalk(true);
	if(road->hasEndCrossWalk()){
		//drawCrossWalk(road,Forward);
	}
	if(road->hasStartCrossWalk()){
		//drawCrossWalk(road,Backward);
	}
	/*****************************/

	double start,end,offset,width;
	const Lane* l1;
	const Lane* l2;
	bool left,right;
	nl = road->getNumberOfLanes();
	for(int32 i = 1; i < nl; i++){
		l1 = road->getLane(i);
		l2 = road->getLane(i-1);
		offset = l1->getRoadCenterOffset();
		width = l1->getWidth();
		start = (l1->getWay()==Forward?l1->getLaneStart():road->getPolyLine()->getLength()-l1->getLaneEnd());
		end = (l1->getWay()==Forward?l1->getLaneEnd():road->getPolyLine()->getLength()-l1->getLaneStart());
		right = (l2->getWay()==Forward?l2->hasFlag(Lane::ChangeLaneToRight):l2->hasFlag(Lane::ChangeLaneToLeft));
		left = (l1->getWay()==Forward?l1->hasFlag(Lane::ChangeLaneToLeft):l1->hasFlag(Lane::ChangeLaneToRight));
		if(right && left){
			addDiscontinuousLine(geode,road->getPolyLine(), offset+width*0.5, start, end);
		}else{
			if(!right && !left){
				addContinuousLine(geode,road->getPolyLine(), offset+width*0.5, start, end);
			}else{
				if(right && !left){
					addContinuousLine(geode,road->getPolyLine(), offset+width*0.46, start, end);
					addDiscontinuousLine(geode,road->getPolyLine(), offset+width*0.54, start, end);
				}else{
					addDiscontinuousLine(geode,road->getPolyLine(), offset+width*0.46, start, end);
					addContinuousLine(geode,road->getPolyLine(), offset+width*0.54, start, end);
				}
			}
		}
	}

	double rw = road->getWidth()*0.5;
	offset = road->getRoadOffset();
	addContinuousLine(geode,road->getPolyLine(), rw+offset, road->getRoadStart(), road->getRoadEnd());
	addContinuousLine(geode,road->getPolyLine(), -rw+offset, road->getRoadStart(), road->getRoadEnd());

	lodNode->addChild( geode, 1.0f, 1000.0f );
	return lodNode;
}

}

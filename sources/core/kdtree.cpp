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

#include "kdtree.h"
#include <algorithm>

namespace vns {

KdTree::KdNode::KdNode(VecItem* item,KdNode* parent,KdNode* l,KdNode* r):item(item),parent(parent),left(l),right(r){

}

KdTree::KdNode::~KdNode(){
	if(left) delete left;
	if(right) delete right;
}

KdTree::KdTree(int k):m_root(0),m_size(0),m_k(k){

}

KdTree::KdTree(const Vector<VecItem*>& v, int k):m_root(0),m_size(0),m_k(k){
	for(int i=0;i<v.size();i++){
		VecItem* item = v.at(i);
		addItem(item);
	}
}

KdTree::KdTree(const List<VecItem*>& v, int k):m_root(0),m_size(0),m_k(k){
	for(List<VecItem*>::ConstIterator it=v.constBegin();it!=v.constEnd();it++){
		VecItem* item = *it;
		addItem(item);
	}
}

KdTree::~KdTree(){
	clear();
}

void KdTree::addItem(VecItem* item){
	if( m_root == 0){
		m_root = new KdNode(item,m_root);
		m_size++;
	}else{
		addInternal(m_root,0,item);
	}
}

void KdTree::addInternal(KdNode* node,int cd,VecItem* item){
	if( item->getPosition()[cd] < node->item->getPosition()[cd] ){
		if( node->left == 0){
			node->left = new KdNode(item,node->left);
			m_size++;
		}else{
			addInternal(node->left,(cd+1)%m_k,item);
		}
	}else{
		if( node->right == 0){
			node->right = new KdNode(item,node->right);
			m_size++;
		}else{
			addInternal(node->right,(cd+1)%m_k,item);
		}
	}
}

void KdTree::searchInternal(KdNode* node,const Vec& p,double range,int d,List<VecItem*>& items){
	if( node ){
		Vec pn = node->item->getPosition();

		if( (d==0 && (p.getX()-range)<pn.getX()) || (p.getY()-range)<pn.getY() ){
			searchInternal(node->left,p,range,(d+1)%m_k,items);
		}
		if( p.distanceToLessThan(pn,range) ){
			items.append( node->item );
		}
		if( (d==0 && pn.getX()<=(p.getX()+range)) || pn.getY()<=(p.getY()+range) ){
			searchInternal(node->right,p,range,(d+1)%m_k,items);
		}
	}
}

void KdTree::searchInternal(KdNode* node,const BBox& bb,int d,List<VecItem*>& items){
	if( node ){
		Vec pn = node->item->getPosition();
		if( (d==0 && bb.getMinX()<pn.getX()) || bb.getMinY()<pn.getY() ){
			searchInternal(node->left,bb,(d+1)%m_k,items);
		}
		if( bb.contains(pn) ){
			items.append( node->item );
		}
		if( (d==0 && pn.getX()<=bb.getMaxX()) || pn.getY()<=bb.getMaxY() ){
			searchInternal(node->right,bb,(d+1)%m_k,items);
		}
	}
}

void KdTree::searchInternal(KdNode* node,List<VecItem*>& items){
	if( node ){
		searchInternal(node->left,items);
		items.append( node->item );
		searchInternal(node->right,items);
	}
}

List<VecItem*> KdTree::getItems(){
	List<VecItem*> items;
	searchInternal(m_root,items);
	return items;
}

List<VecItem*> KdTree::getItems(const Vec& p, double range){
	List<VecItem*> items;
	searchInternal(m_root,p,range,0,items);
	return items;
}


List<VecItem*> KdTree::getItems(const BBox& bb){
	List<VecItem*> items;
	searchInternal(m_root,bb,0,items);
	return items;
}

VecItem* KdTree::findMin(KdNode* node,int dim,int d){
	if(node==0) return 0;
	int next_d = (d+1)%m_k;
	if( dim==d ){
		if(node->left==0){
			return node->item;
		}else{
			return findMin(node->left,dim,next_d);
		}
	}
	VecItem* best = node->item;
	VecItem* item = findMin(node->left,dim,next_d);
	if(item && item->getPosition()[dim]<=best->getPosition()[dim]) best = item;
	item = findMin(node->right,dim,next_d);
	if(item && item->getPosition()[dim]<=best->getPosition()[dim]) best = item;
	return best;
}

KdTree::KdNode* KdTree::removeInternal(KdNode* node,VecItem* item,int cd){
	if(node==0) return node;
	int next_d = (cd+1)%m_k;
	if( item==node->item){
		if(node->right){
			node->item = findMin(node->right,cd,next_d);
			node->right = removeInternal(node->right,node->item,next_d);
		}else{
			if(node->left){
				node->item = findMin(node->left,cd,next_d);
				node->left = removeInternal(node->left,node->item,next_d);
			}else{
				delete node;
				m_size--;
				return 0;
			}
		}
	}else{
		if( item->getPosition()[cd] < node->item->getPosition()[cd] ){
			node->left = removeInternal(node->left,item,next_d);
		}else{
			node->right = removeInternal(node->right,item,next_d);
		}
	}
	return node;
}

void KdTree::removeItem(VecItem* item){
	m_root = removeInternal(m_root,item,0);
}

/*
void KdTree::getNearestItemInternal(KdNode* node,const Vec& p,int cd,VecItem*& best,double bestD){
	if( node==0 ) return;

	double d = p.distanceTo(node->item->getPosition());
	if( d < bestD ){
		best = node->item;
		bestD = d;
	}

	if(p[cd]<node->item->getPosition()[cd]){
		getNearestItemInternal(node->left,p,next_cd,
	}else{

	}
}*/

VecItem* KdTree::getNearestItem(VecItem* item){
	return getNearestItem(item->getPosition());
}

VecItem* KdTree::getNearestItem(const Vec& p){
	if(m_root==0) return 0;
	VecItem* item = m_root->item;
	//getNearestItemInternal(m_root,p,0,item,vns::MAX_DOUBLE);
	return item;
}


void KdTree::clear(){
	delete m_root;
}


}

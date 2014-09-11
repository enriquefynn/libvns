/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2011-2012 Ricardo Fernandes - All Rights Reserved       *
 *   Email: rjf@dcc.fc.up.pt or rikardojfernandez@gmail.com                *
 *   Based on the RTree from Dustin Spicuzza <dustin@virtualroadside.com>  *
 *                                                                         *
 *   This file is part of VNS - Vehicular Networks Simulator.              *
 *                                                                         *
 *   For information about the licensing and copyright of this code        *
 *   please contact the author.                                            *
 *                                                                         *
 ***************************************************************************/

#include "rtree.h"
#include <algorithm>

namespace vns {

class RemoveItemVisitor : public vns::RTree::RemoveVisitor {
private:
	BBoxItem* itemToRemove;
public:
	RemoveItemVisitor(BBoxItem* item){ itemToRemove = item; };
	bool accept(BBoxItem* item){
		if( itemToRemove->getBBox().intersects( item->getBBox() ) ){
			return true;
		}
		return false;
	};
	bool remove(BBoxItem* item){
		if(itemToRemove == item){
			return true;
		}
		return false;
	};
};

class RemoveWithinBBoxVisitor : public vns::RTree::RemoveVisitor {
private:
	const BBox bb;
public:
	RemoveWithinBBoxVisitor(const BBox& bb):bb(bb){};
	bool accept(BBoxItem* item){
		if( bb.intersects( item->getBBox() ) ){
			return true;
		}
		return false;
	};
	bool remove(BBoxItem* item){
		if( bb.intersects( item->getBBox() ) ){
			return true;
		}
		return false;
	};
};

class ClearVisitor : public vns::RTree::RemoveVisitor {
public:
	bool accept(BBoxItem*){ return true; };
	bool remove(BBoxItem*){ return true; };
};

class GetItemsVisitor : public vns::RTree::Visitor {
private:
	vns::List<BBoxItem*> items;
	BBox bb;
public:
	GetItemsVisitor(const BBox& bb,vns::List<BBoxItem*>& items){
		this->bb = bb;
		this->items = items;
	};
	bool accept(BBoxItem* item) {
		if(bb.intersects( item->getBBox() ) ) return true;
		return false;
	};
	void visit(BBoxItem* item){
		items.append(item);
	};
};

/*
 * Compare Functions
 */

class CompareByAreaEnlargement {
private:
	const double area;
public:
	CompareByAreaEnlargement(double area) : area(area) {};
	bool operator()(BBoxItem* bi1, BBoxItem*bi2) const {
		return area - bi1->getBBox().getArea() < area - bi2->getBBox().getArea();
	};
};
class CompareByOverlapEnlargement {
private:
	const BBox box;
public:
	CompareByOverlapEnlargement(const BBox& bb) : box(bb) {};
	bool operator()(BBoxItem* bi1, BBoxItem* bi2) const {
		return bi1->getBBox().getIntersectionArea(box) < bi2->getBBox().getIntersectionArea(box);
	};
};
class CompareByDistanceFromCenter {
private:
	const BBox box;
public:
	CompareByDistanceFromCenter(const BBox& bb) : box(bb){};
	bool operator()(BBoxItem* bi1, BBoxItem* bi2) const {
		return bi1->getBBox().getDistanceFromCenter(box) < bi2->getBBox().getDistanceFromCenter(box);
	};
};
class CompareXByFirstEdge {
public:
	bool operator()(BBoxItem* bi1, BBoxItem* bi2) const {
		return bi1->getBBox().getMinX() < bi2->getBBox().getMinX();
	};
};
class CompareXBySecondEdge {
public:
	bool operator()(BBoxItem* bi1, BBoxItem* bi2) const {
		return bi1->getBBox().getMaxX() < bi2->getBBox().getMaxX();
	};
};
class CompareYByFirstEdge {
public:
	bool operator()(BBoxItem* bi1, BBoxItem* bi2) const {
		return bi1->getBBox().getMinY() < bi2->getBBox().getMinY();
	};
};
class CompareYBySecondEdge {
public:
	bool operator()(BBoxItem* bi1, BBoxItem* bi2) const {
		return bi1->getBBox().getMaxY() < bi2->getBBox().getMaxY();
	};
};



RTree::RTree(int mi,int ma) {
	root = 0;
	m_size = 0;
	minChilds = mi;
	maxChilds = ma;
}

RTree::~RTree() {
	clear();
}

RTree::RTreeNode::RTreeNode(){

}

RTree::RTreeNode::~RTreeNode(){
	items.clear();
}


void RTree::removeItem(BBoxItem* item){
	RemoveItemVisitor visitor(item);
	applyRemoveVisitor( visitor );
}

void RTree::applyVisitor(RTree::Visitor& visitor){
	if ( root && visitor.accept(root) ){
		applyVisitor(root,visitor);
	}
}

void RTree::applyVisitor(RTreeNode* node, RTree::Visitor& visitor){
	int n = node->items.size();
	if (node->hasLeaves){
		for(int i=0;i<n;i++){
			BBoxItem* item = node->items[i];
			if( visitor.accept( item ) ){
				visitor.visit( item );
			}
		}
	}else{
		for(int i=0;i<n;i++){
			BBoxItem* item = node->items[i];
			if( visitor.accept( item ) ){
				applyVisitor(static_cast<RTreeNode*>(item),visitor);
			}
		}
	}
}


void RTree::applyRemoveVisitor(RTree::RemoveVisitor& visitor){
	if (root == 0){
		return;
	}
	List<BBoxItem*> itemsToReinsert;
	applyRemoveVisitor(root,visitor,itemsToReinsert);
	while( !itemsToReinsert.isEmpty() ){
		BBoxItem* item = itemsToReinsert.front();
		insertInternal( item , root, false);
		itemsToReinsert.pop_front();
	}
}

bool RTree::applyRemoveVisitor(BBoxItem* item, RTree::RemoveVisitor& visitor, List<BBoxItem*>& itemsToReinsert){
	RTreeNode * node = static_cast<RTreeNode*> (item);
	if( visitor.accept(node) ) {
		if (node->hasLeaves){
			bool changed = false;
			for(int i=node->items.size()-1;i>=0;i--){
				if( visitor.remove(node->items[i]) ){
					node->items.remove( i );
					m_size--;
					changed = true;
				}
			}
			// UPDATE BBOX
			if(changed){
				node->bb.clear();
				int n = node->items.size();
				for(int i=0;i<n;i++){
					node->bb.addBBox( node->items[i]->getBBox() );
				}
			}
		}else{
			bool changed = false;
			for(int i=node->items.size()-1;i>=0;i--){
				if( applyRemoveVisitor(node->items[i], visitor, itemsToReinsert)){
					node->items.remove( i );
					changed = true;
				}
			}
			if(changed){
				node->bb.clear();
				int n = node->items.size();
				for(int i=0;i<n;i++){
					node->bb.addBBox( node->items[i]->getBBox() );
				}
			}
		}
		if(node != root) {
			if(node->items.isEmpty()) {
				delete node;
				node = 0;
				return true;
			} else {
				if(node->items.size() < minChilds) {
					queueItemsToReinsert(node,itemsToReinsert);
					return true;
				}
			}
		}else{
			if (root->items.isEmpty()) {
				root->hasLeaves = true;
				root->bb.clear();
			}
		}
	}
	return false;
}

void RTree::queueItemsToReinsert(RTreeNode * node, List<BBoxItem*>& itemsToReinsert){
	int32 n = node->items.size();
	if (node->hasLeaves){
		for(int32 i=n-1;i>=0;i--){
			itemsToReinsert.push_back( node->items[i] );
		}
	}else{
		for(int i=n-1;i>=0;i--){
			queueItemsToReinsert( static_cast<RTreeNode*>(node->items[i]), itemsToReinsert);
		}
	}
	delete node;
}


List<BBoxItem*> RTree::getItems(const BBox& bb){
	List<BBoxItem*> items;
	GetItemsVisitor visitor(bb,items);
	applyVisitor( visitor );
	return items;
}

void RTree::removeItems(const BBox& bb){
	RemoveWithinBBoxVisitor visitor(bb);
	applyRemoveVisitor( visitor );
}

void RTree::clear(){
	ClearVisitor visitor;
	applyRemoveVisitor( visitor );
}

void RTree::addItems(Vector<BBoxItem*> items){
	for(Vector<BBoxItem*>::iterator i = items.begin(); i != items.end(); ++i){
		addItem( *i );
	}
}

void RTree::addItem(BBoxItem* newItem){
	if ( root == 0 ){
		root = new RTreeNode();
		root->hasLeaves = true;
		root->items.reserve(minChilds);
		root->items.push_back( newItem );
		root->bb.clear();
		root->bb.addBBox( newItem->getBBox() );
		m_size++;
	}else{
		insertInternal( newItem, root );
	}

}

RTree::RTreeNode* RTree::insertInternal(BBoxItem* newItem, RTreeNode* node, bool firstInsert){
	node->bb.addBBox( newItem->getBBox() );
	if( node->hasLeaves ){
		for(Vector<BBoxItem*>::iterator i = node->items.begin(); i != node->items.end(); ++i){
			if( (*i) == newItem ){
				return 0;
			}
		}
		node->items.push_back( newItem );
		if(firstInsert){
			m_size++;
		}
	} else {
		RTreeNode* tmpNode = insertInternal( newItem, chooseSubtree(node, newItem->getBBox()), firstInsert );
		if( tmpNode == 0 ){
			return 0;
		}
		node->items.push_back( tmpNode );
	}
	if(node->items.size() > maxChilds ) {
		return overflowTreatment(node, firstInsert);
	}
	return 0;
}

RTree::RTreeNode* RTree::chooseSubtree(RTreeNode* node, const BBox& bound){
	if (static_cast<RTreeNode*>(node->items[0])->hasLeaves){
		if (maxChilds > (chooseSubtreeP*2)/3  && node->items.size() > chooseSubtreeP){
			std::sort( node->items.begin(), node->items.begin() + chooseSubtreeP, CompareByAreaEnlargement(bound.getArea()) );
			return static_cast<RTreeNode*>(*std::min_element(node->items.begin(), node->items.begin() + chooseSubtreeP, CompareByOverlapEnlargement(bound) ));
		}
		return static_cast<RTreeNode*>(* std::min_element(node->items.begin(), node->items.end(), CompareByOverlapEnlargement(bound) ));
	}
	return static_cast<RTreeNode*>(*std::min_element( node->items.begin(), node->items.end(), CompareByAreaEnlargement(bound.getArea()) ));
}


RTree::RTreeNode* RTree::overflowTreatment(RTreeNode * level, bool firstInsert){
	if (level != root && firstInsert){
		reinsert(level);
		return 0;
	}

	RTreeNode* splitItem = split(level);

	if (level == root) {
		RTreeNode* newRoot = new RTreeNode();
		newRoot->hasLeaves = false;
		newRoot->items.append( root );
		newRoot->items.append( splitItem );
		// Update the newRoot item bounding box
		newRoot->bb.clear();
		newRoot->bb.addBBox( root->getBBox() );
		newRoot->bb.addBBox( splitItem->getBBox() );

		root = newRoot;
		return 0;
	}
	return splitItem;
}


RTree::RTreeNode* RTree::split(RTreeNode * node){
	RTreeNode* newNode = new RTreeNode();
	newNode->hasLeaves = node->hasLeaves;

	int n_items = node->items.size();
	int distribution_count = n_items - 2*minChilds + 1;
	int dimensions = 2;
	int split_axis = dimensions+1;
	int split_edge = 0;
	int split_index = 0;
	int split_margin = 0;

	BBox R1,R2;

	for (int axis = 0; axis < dimensions; axis++)
	{
		int margin = 0;
		double overlap = 0, dist_area, dist_overlap;
		int dist_edge = 0, dist_index = 0;

		dist_area = dist_overlap = 1e300;

		for( int edge = 0; edge < 2; edge++){
			// std::sort the items by the correct key (upper edge, lower edge)
			if( axis == 0){
				if( edge == 0){
					std::sort(node->items.begin(), node->items.end(), CompareXByFirstEdge());
				}else{
					std::sort(node->items.begin(), node->items.end(), CompareXBySecondEdge());
				}
			}else{
				if( edge == 0){
					std::sort(node->items.begin(), node->items.end(), CompareYByFirstEdge());
				}else{
					std::sort(node->items.begin(), node->items.end(), CompareYBySecondEdge());
				}
			}

			for(int k = 0; k < distribution_count; k++){
				double area = 0;

				// calculate bounding box of R1
				R1.clear();
				for(Vector<BBoxItem*>::iterator i = node->items.begin(); i != node->items.begin()+(minChilds+k); ++i){
					R1.addBBox( (*i)->getBBox() );
				}
				R2.clear();
				for(Vector<BBoxItem*>::iterator i = node->items.begin()+(minChilds+k+1); i != node->items.end(); ++i){
					R2.addBBox( (*i)->getBBox() );
				}

				// calculate the three values
				double edR1 = (R1.getMaxX() - R1.getMinX()) + (R1.getMaxY() - R1.getMinY());
				double edR2 = (R2.getMaxX() - R2.getMinX()) + (R2.getMaxY() - R2.getMinY());
				margin 	+=  edR1 + edR2;
				area 	+= R1.getArea() + R2.getArea();
				overlap =  R1.getIntersectionArea(R2);

				if (overlap < dist_overlap || (overlap == dist_overlap && area < dist_area))
				{
					// if so, store the parameters that allow us to recreate it at the end
					dist_edge = 	edge;
					dist_index = 	minChilds+k;
					dist_overlap = 	overlap;
					dist_area = 	area;
				}
			}
		}

		// CSA2: Choose the axis with the minimum S as split axis
		if (split_axis == dimensions+1 || split_margin > margin )
		{
			split_axis 		= axis;
			split_margin 	= margin;
			split_edge 		= dist_edge;
			split_index 	= dist_index;
		}
	}

	if (split_edge == 0){
		if(split_axis == 0){
			std::sort(node->items.begin(), node->items.end(), CompareXByFirstEdge() );
		}else{
			std::sort(node->items.begin(), node->items.end(), CompareYByFirstEdge() );
		}
	}else{
		if (split_axis == 0){
			std::sort(node->items.begin(), node->items.end(), CompareXBySecondEdge() );
		}
	}
	// distribute the end of the array to the new node, then erase them from the original node
	newNode->items = node->items.mid(split_index);
	node->items.resize(split_index);

	// adjust the bounding box for each 'new' node
	node->bb.clear();
	for(Vector<BBoxItem*>::iterator i = node->items.begin(); i != node->items.end(); ++i){
		node->bb.addBBox( (*i)->getBBox() );
	}
	newNode->bb.clear();
	for(Vector<BBoxItem*>::iterator i = newNode->items.begin(); i != newNode->items.end(); ++i){
		newNode->bb.addBBox( (*i)->getBBox() );
	}

	return newNode;
}

// This routine is used to do the opportunistic reinsertion that the
// R* algorithm calls for
void RTree::reinsert(RTreeNode * node){

	int n_items = node->items.size();
	int p = (int)((double)n_items*reinsertP)>0 ? (int)((double)n_items*reinsertP) : 1;


	// RI2: std::sort the items in increasing order of their distances
	// computed in RI1
	std::sort(node->items.begin(), node->items.end()-p, CompareByDistanceFromCenter(node->getBBox()) );

	// RI3.A: Remove the last p items from N
	List< BBoxItem* > removedItems;
	int n = node->items.size();
	for(int i=n-1;i>=(n-p-1);i--){
		removedItems.push_back( node->items[i] );
	}
	node->items.resize( n-p-1 );

	// RI3.B: adjust the bounding rectangle of N
	node->bb.clear();
	for(Vector<BBoxItem*>::iterator i = node->items.begin(); i != node->items.end(); ++i){
		node->bb.addBBox( (*i)->getBBox() );
	}
	while( !removedItems.isEmpty() ){
		BBoxItem* item = removedItems.front();
		insertInternal( item , root, false);
		removedItems.pop_front();
	}
}

}

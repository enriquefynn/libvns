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

#ifndef VNS_KDTREE_H_
#define VNS_KDTREE_H_

#include "vec.h"
#include "bbox.h"
#include "vector.h"
#include "list.h"

/*! \cond PRIVATE */

namespace vns {

class KdTree {
private:
	class KdNode {
	public:
		VecItem* item;
		KdNode *parent;
		KdNode *left;
		KdNode *right;

		KdNode(VecItem* item,KdNode* parent=0,KdNode* l=0,KdNode* r=0);
		~KdNode();
	};

	KdNode* m_root;
	int m_size;
	int m_k;

	void addInternal(KdNode* node,int dim,VecItem* item);
	VecItem* findMin(KdNode* node,int dim,int d);
	void searchInternal(KdNode* node,List<VecItem*>& items);
	void searchInternal(KdNode* node,const BBox& bb,int d,List<VecItem*>&items);
	void searchInternal(KdNode* node,const Vec& pt,double range,int d,List<VecItem*>&items);
	KdNode* removeInternal(KdNode* node,VecItem* item,int cd);

public:
	KdTree(int k=2);
	KdTree(const Vector<VecItem*>& v, int k=2);
	KdTree(const List<VecItem*>& v, int k=2);
	~KdTree();

	void addItem(VecItem* item);
	//void balance();
	List<VecItem*> getItems();
	List<VecItem*> getItems(const BBox& bb);
	List<VecItem*> getItems(const Vec& p, double range);
	VecItem* getNearestItem(VecItem* item);
	VecItem* getNearestItem(const Vec& p);
	void removeItem(VecItem* item);
	void clear();
	inline int size() const { return m_size; };
};

}

/*! \endcond */

#endif

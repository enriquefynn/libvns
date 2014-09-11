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

#ifndef VNS_RTREE_H_
#define VNS_RTREE_H_

#include "bbox.h"
#include "vector.h"
#include "list.h"

namespace vns {

/*! \cond PRIVATE */

class RTree {
public:

	class Visitor {
	public:
		virtual ~Visitor(){};
		virtual bool accept(BBoxItem* item) = 0;
		virtual void visit(BBoxItem* item) = 0;
	};

	/*! \cond PRIVATE */
	class RemoveVisitor {
	public:
		virtual bool accept(BBoxItem* item) = 0;
		virtual bool remove(BBoxItem* item) = 0;
	};
	/*! \endcond */

private:
	/*! \cond PRIVATE */
	class RTreeNode : public BBoxItem {
	private:
		BBox bb;

	public:
		RTreeNode();
		~RTreeNode();
		vns::Vector< BBoxItem* > items;
		bool hasLeaves;
		inline const BBox& getBBox() const { return bb; };
		friend class RTree;
	};



	/*! \endcond */

	RTreeNode* root;
	int m_size;
	int minChilds;
	int maxChilds;
	static const float reinsertP = 0.30;
	static const int chooseSubtreeP = 32;

	RTreeNode* insertInternal(BBoxItem* newItem, RTreeNode* node, bool firstInsert = true);
	RTreeNode* chooseSubtree(RTreeNode* node, const BBox& bound);
	RTreeNode* overflowTreatment(RTreeNode * level, bool firstInsert);
	RTreeNode* split(RTreeNode* node);
	void applyVisitor(RTreeNode* node,RTree::Visitor& visitor);
	void applyRemoveVisitor(RTree::RemoveVisitor& visitor);
	bool applyRemoveVisitor(BBoxItem* item, RTree::RemoveVisitor& visitor, List<BBoxItem*>& itemsToReinsert);
	void reinsert(RTreeNode * node);
	void queueItemsToReinsert(RTreeNode * node, List<BBoxItem*>& itemsToReinsert);
	void getItems(const BBox& bb, RTreeNode* node, List<BBoxItem*>& items);

public:

	RTree(int minChilds=4,int maxChilds=8);
	~RTree();

	void addItem(BBoxItem* item);
	void addItems(Vector<BBoxItem*> items);
	List<BBoxItem*> getItems(const BBox& bb);
	void applyVisitor(RTree::Visitor& visitor);
	void removeItem(BBoxItem* item);
	void removeItems(const BBox& box);
	void clear();
	inline int size() const { return m_size; };

};

/*! \endcond */

}

#endif /* VNS_RTREE_H_ */

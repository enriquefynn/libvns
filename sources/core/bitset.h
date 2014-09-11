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

#ifndef VNS_BITSET_H
#define VNS_BITSET_H

namespace vns {

/*! \cond PRIVATE */

class BitSet {
public:
	BitSet(){ mask=0; };
	BitSet(int32 m){ mask=m; };

    inline void add(int32 k) { mask |= 1<<k; count++;};
    inline void remove(int32 k) { mask &= ~(1<<k); count--;};
    inline bool contains(int32 k) const { return (mask&(1<<k)) != 0; };
    inline bool isEmpty() const { return mask==0; };
    inline void clear() { mask = 0; count = 0;};
    inline int32 value() const { return mask; };
    inline int8 size() const { return count; };
    
    class Iterator {
    private:
        int8 i;
        int32 mask;
    public:
        inline Iterator(int32 v):i(-1),mask(v){};
        inline int8 next(){
        	do{
        		i++;
        	}while((mask&(1<<i)) == 0 && i<32);
        	if(i>=32) return -1;
        	return i;
        };
    };

    inline Iterator getIterator(){ return Iterator(mask); };

private:
    int32 mask;
    int8 count;
};

/*! \endcond */

}

#endif

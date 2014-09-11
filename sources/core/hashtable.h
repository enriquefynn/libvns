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

#ifndef VNS_HASHTABLE_H_
#define VNS_HASHTABLE_H_

#include "core.h"
#include <iostream>
#include "datastream.h"

namespace vns {


inline uint hash(char key) { return uint(key); }
inline uint hash(int8 key) { return uint(key); }
inline uint hash(uint8 key) { return uint(key); }
inline uint hash(int16 key) { return uint(key); }
inline uint hash(uint16 key) { return uint(key); }
inline uint hash(int32 key) { return uint(key); }
inline uint hash(uint32 key) { return uint(key); }
inline uint hash(uint64 key){ return sizeof(uint64) > sizeof(uint)?uint((key >> (8 * sizeof(uint) - 1)) ^ key):uint(key); }
inline uint hash(int64 key) { return hash(uint64(key)); }
template <class T> inline uint hash(const T* key){ return hash(reinterpret_cast<uintptr>(key));}

inline uint hash(const char *p){
	uint h = 0;
	while(*p) {
		h = (h << 4) + *p++;
		h ^= (h & 0xf0000000) >> 23;
		h &= 0x0fffffff;
	}
	return h;
}



template <class Key,class T>
class HashTable {
private:
	/*! \cond PRIVATE */
	template <class BKey,class BT>
	struct Bucket {
	    BKey key;
	    BT value;
	    Bucket* next;
	    inline Bucket(const BKey& k,const BT& v) : key(k), value(v), next(0) {};
	};
	/*! \endcond */

    unsigned int mtableSize;
    Bucket<Key,T>** mtable;
    unsigned int msize;

public:
    class Iterator {

    	HashTable<Key,T>* table;
    	unsigned int i;
    	Bucket<Key,T>* bucket;

        public:
            inline Iterator(HashTable<Key,T>* t){
            	table = t; i = 0;
            	bucket = table->mtable[i];
            	if(bucket==0) next();
            }
            inline Iterator(const Iterator& it){
            	i = it.i;
            	bucket = it.bucket;
            	table = it.table;
            }

            inline bool isValid(){ return bucket!=0; }
            inline void next(){
            	do{
            		if(bucket && bucket->next){
            			bucket = bucket->next;
            		}else{
            			i++;
            			if(i>=table->mtableSize){
            				bucket = 0;return;
            			}
            			bucket = table->mtable[i];
            		}
            	}while(bucket==0);
            }
            inline const Key& key() const { return bucket->key; }
            inline T& value() const { return bucket->value; }
            inline T& operator*() const { return bucket->value; }
            inline T* operator->() const { return &bucket->value; }
            inline bool operator==(const Iterator &o) const { return bucket == o.bucket; }
            inline bool operator!=(const Iterator &o) const { return bucket != o.bucket; }
            inline Iterator& operator++(){ next(); return *this; }
            inline Iterator operator++(int){ Iterator r = *this; r.next(); return r;}
    };

    class ConstIterator {
    	friend class Iterator;
    	const HashTable<Key,T>* table;
    	unsigned int i;
    	const Bucket<Key,T>* bucket;

        public:
            inline ConstIterator(const HashTable<Key,T>* t){
            	table = t; i = 0;
            	bucket = table->mtable[i];
            	if(bucket==0) next();
            }
            inline ConstIterator(const Iterator& it){
            	i = it.i;
            	bucket = it.bucket;
            	table = it.table;
            }

            inline bool isValid() const { return bucket!=0; }
            inline void next(){
            	do{
            		if(bucket && bucket->next){
            			bucket = bucket->next;
            		}else{
            			i++;
            			if(i>=table->mtableSize){
            				bucket = 0;return;
            			}
            			bucket = table->mtable[i];
            		}
            	}while(bucket==0);
            }
            inline const Key& key() const { return bucket->key; }
            inline const T& value() const { return bucket->value; }
            inline const T& operator*() const { return bucket->value; }
            inline const T* operator->() const { return &bucket->value; }
            inline bool operator==(const Iterator &o) const { return bucket == o.bucket; }
            inline bool operator!=(const Iterator &o) const { return bucket != o.bucket; }
            inline Iterator& operator++(){ next(); return *this; }
            inline Iterator operator++(int){ Iterator r = *this; r.next(); return r;}
    };

    HashTable(unsigned int tableSize=1024);
    HashTable(const HashTable<Key, T>& other);
    virtual ~HashTable();

    void clear();
    void print();
    bool remove(const Key& key);
    bool insert(const Key& key, const T& value);
    bool contains(const Key &key) const;
    const T value(const Key& key) const;
    const T value(const Key& key, const T& defaultValue) const;
    inline int size() const { return msize; }
    inline bool isEmpty() const { return msize==0; }

    T& operator[](const Key& key);
    const T operator[](const Key& key) const { return value(key); };

    HashTable<Key, T>& operator=(const HashTable<Key, T>& other);

    template <class KK, class TT>
    friend DataStream& operator>>(DataStream& ds, HashTable<KK,TT>& v);
    template <class KK, class TT>
    friend DataStream& operator<<(DataStream& ds, const HashTable<KK,TT>& v);

};

template <class KK, class TT>
DataStream& operator>>(DataStream& ds, HashTable<KK,TT>& v){
    v.clear();
    int n;
    ds >> n;
    for(int i = 0; i < n; i++) {
    	KK k;
        TT t;
        ds >> k;
        ds >> t;
        v.insert(k,t);
    }
    return ds;
}

template <class KK, class TT>
DataStream& operator<<(DataStream& ds, const HashTable<KK,TT>& v){
	int n = v.size();
    ds << n;
	typename vns::HashTable<KK,TT>::ConstIterator it(&v);
	while(it.isValid()){
		ds << it.key() << it.value();
		it.next();
	}
	return ds;
}


template <typename Key,typename T>
inline void HashTable<Key,T>::print(){
	for(unsigned int i=0;i<mtableSize;i++){
		if(mtable[i]){
			std::cout << i << " - ";
			Bucket<Key,T>* b = mtable[i];
			while(b){
				Bucket<Key,T>* n = b;
				std::cout << b->key << " " << b->value << " , ";
				b = b->next;
			}
		}else{
			std::cout << i << " - 0";
		}
		std::cout << "\n";
	}
}

template <typename Key,typename T>
HashTable<Key,T>::HashTable(unsigned int tableSize) {
    mtable = new Bucket<Key,T>*[tableSize];
    for(unsigned int i=0;i<tableSize;i++){
    	mtable[i] = 0;
    }
    mtableSize = tableSize;
    msize = 0;
}

template <typename Key,typename T>
HashTable<Key,T>::HashTable(const HashTable<Key, T>& other){
	mtableSize = other.mtableSize;
	msize = other.msize;
    mtable = new Bucket<Key,T>*[other.mtableSize];
    for(int i=0;i<other.mtableSize;i++){
    	if(other.mtable[i]){
    		Bucket<Key,T>* ob = other.mtable[i];
    		mtable[i] = new Bucket<Key,T>(ob->key,ob->value);
    		Bucket<Key,T>* b = mtable[i];
    		while(ob->next){
    			ob = ob->next;
    			b->next = new Bucket<Key,T>(ob->key,ob->value);
    			b = b->next;
    		}
    	}else{
    		mtable[i] = 0;
    	}
    }
}

template <typename Key,typename T>
HashTable<Key,T>::~HashTable(){
	clear();
    delete[] mtable;
}

template <typename Key,typename T>
void HashTable<Key,T>::clear(){
	for(unsigned int i=0;i<mtableSize;i++){
		if(mtable[i]){
			Bucket<Key,T>* b = mtable[i];
			while(b){
				Bucket<Key,T>* n = b;
				b = b->next;
				delete n;
			}
		}
		mtable[i] = 0;
	}
}


template <typename Key,typename T>
bool HashTable<Key,T>::contains(const Key& key) const {
	uint h = hash(key) % mtableSize;
	Bucket<Key,T>* b = mtable[h];
	while(b){
		if (b->key == key) return true;
		b = b->next;
	};
	return false;
}

template <typename Key,typename T>
const T HashTable<Key,T>::value(const Key& key) const {
	uint h = hash(key) % mtableSize;
	Bucket<Key,T>* b = mtable[h];
	while(b){
		if (b->key == key) return b->value;
		b = b->next;
	};
	return T();
}

template <typename Key,typename T>
const T HashTable<Key,T>::value(const Key& key,const T& defaultValue) const {
	uint h = hash(key) % mtableSize;
	Bucket<Key,T>* b = mtable[h];
	while(b){
		if (b->key == key) return b->value;
		b = b->next;
	};
	return defaultValue;
}

template <typename Key,typename T>
bool HashTable<Key,T>::remove(const Key& key){
	uint h = hash(key) % mtableSize;
	Bucket<Key,T>* b = mtable[h];
	Bucket<Key,T>* prev = b;
	while(b){
		if (b->key == key){
			Bucket<Key,T>* n = b;
			if(b==prev){
				mtable[h] = b->next;
			}else{
				prev->next = b->next;
			}
			delete n;
			msize--;
			return true;
		}
		prev = b;
		b = b->next;
	}
	return false;
}

template <typename Key,typename T>
bool HashTable<Key,T>::insert(const Key& key, const T& value){
	uint h = hash(key) % mtableSize;
	if(mtable[h]==0){
		mtable[h] = new Bucket<Key,T>(key,value);
		msize++;
		return true;
	}
	Bucket<Key,T>* bucket = mtable[h];
	while(bucket){
		if(bucket->key == key){
			bucket->value = value;
			return true;
		}
		if(bucket->next==0){
			bucket->next = new Bucket<Key,T>(key,value);
			msize++;
			return true;
		}
		bucket = bucket->next;
	}
	return false;
}

template <typename Key,typename T>
T& HashTable<Key,T>::operator[](const Key& key){
	uint h = hash(key) % mtableSize;
	if(mtable[h]){
		Bucket<Key,T>* bucket = mtable[h];
		while(bucket){
			if(bucket->key == key) return bucket->value;
			if(bucket->next==0){
				bucket->next = new Bucket<Key,T>(key,T());
				msize++;
				return bucket->next->value;
			}
			bucket = bucket->next;
		}
	}
	mtable[h] = new Bucket<Key,T>(key,T());
	msize++;
	return mtable[h]->value;
}

template <typename Key,typename T>
HashTable<Key, T>& HashTable<Key,T>::operator=(const HashTable<Key, T>& other){
	clear();
	delete[] mtable;
	mtableSize = other.mtableSize;
	msize = other.msize;
    mtable = new Bucket<Key,T>*[other.mtableSize];
    for(int i=0;i<other.mtableSize;i++){
    	if(other.mtable[i]){
    		Bucket<Key,T>* ob = other.mtable[i];
    		mtable[i] = new Bucket<Key,T>(ob->key,ob->value);
    		Bucket<Key,T>* b = mtable[i];
    		while(ob->next){
    			ob = ob->next;
    			b->next = new Bucket<Key,T>(ob->key,ob->value);
    			b = b->next;
    		}
    	}else{
    		mtable[i] = 0;
    	}
    }
	return *this;
}

}

#endif

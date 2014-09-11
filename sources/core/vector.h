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

#ifndef VNS_VECTOR_H_
#define VNS_VECTOR_H_

#include "datastream.h"
#include <iostream>

namespace vns {

template <typename T>
class Vector {

private:
   int msize;
   int mcapacity;
   T * mbuffer;

   static int compareSort(const void* a, const void* b){
	   return (*(T*)a<=*(T*)b?-1:1);
   }

public:

   class Iterator {
   private:
       T *i;
   public:
       typedef std::bidirectional_iterator_tag  iterator_category;
       typedef ptrdiff difference_type;
       typedef T value_type;
       typedef const T *pointer;
       typedef const T &reference;

       inline Iterator() : i(0) {}
       inline Iterator(T* n) : i(n) {}
       inline Iterator(const Iterator& other): i(other.i){}
       inline T& operator*() const { return *i; }
       inline T* operator->() const { return i; }
       inline T& operator[](int j) const { return *(i + j); }
       inline bool operator==(const Iterator& other) const { return i == other.i; }
       inline bool operator!=(const Iterator& other) const { return i != other.i; }
       inline bool operator<(const Iterator& other) const { return i < other.i; }
       inline bool operator<=(const Iterator& other) const { return i <= other.i; }
       inline bool operator>(const Iterator& other) const { return i > other.i; }
       inline bool operator>=(const Iterator& other) const { return i >= other.i; }
       inline Iterator& operator++() { ++i; return *this; }
       inline Iterator operator++(int) { T *n = i; ++i; return n; }
       inline Iterator& operator--() { i--; return *this; }
       inline Iterator operator--(int) { T *n = i; i--; return n; }
       inline Iterator& operator+=(int j) { i+=j; return *this; }
       inline Iterator& operator-=(int j) { i-=j; return *this; }
       inline Iterator operator+(int j) const { return Iterator(i+j); }
       inline Iterator operator-(int j) const { return Iterator(i-j); }
       inline int operator-(Iterator j) const { return i - j.i; }
       friend class Vector;
   };
   friend class Iterator;

   class ConstIterator {
   private:
       T *i;
   public:
       typedef std::bidirectional_iterator_tag  iterator_category;
       typedef ptrdiff difference_type;
       typedef T value_type;
       typedef const T *pointer;
       typedef const T &reference;

       inline ConstIterator() : i(0) {}
       inline ConstIterator(T* n) : i(n) {}
       inline ConstIterator(const ConstIterator& other): i(other.i) {};
       inline explicit ConstIterator(const Iterator& other): i(other.i) {};
       inline const T& operator*() const { return *i; }
       inline const T* operator->() const { return i; }
       inline const T& operator[](int j) const { return *(i + j); }
       inline bool operator==(const ConstIterator& other) const { return i == other.i; }
       inline bool operator!=(const ConstIterator& other) const { return i != other.i; }
       inline bool operator<(const ConstIterator& other) const { return i < other.i; }
       inline bool operator<=(const ConstIterator& other) const { return i <= other.i; }
       inline bool operator>(const ConstIterator& other) const { return i > other.i; }
       inline bool operator>=(const ConstIterator& other) const { return i >= other.i; }
       inline ConstIterator& operator++() { ++i; return *this; }
       inline ConstIterator operator++(int) { T *n = i; ++i; return n; }
       inline ConstIterator& operator--() { i--; return *this; }
       inline ConstIterator operator--(int) { T *n = i; i--; return n; }
       inline ConstIterator& operator+=(int j) { i+=j; return *this; }
       inline ConstIterator& operator-=(int j) { i+=j; return *this; }
       inline ConstIterator operator+(int j) const { return ConstIterator(i+j); }
       inline ConstIterator operator-(int j) const { return ConstIterator(i-j); }
       inline int operator-(ConstIterator j) const { return i - j.i; }
       friend class Vector;
   };
   friend class ConstIterator;

   typedef Iterator iterator;
   typedef ConstIterator const_iterator;
   typedef int size_type;
   typedef T value_type;
   typedef value_type *pointer;
   typedef const value_type *const_pointer;
   typedef value_type &reference;
   typedef const value_type &const_reference;

   inline Vector(){mcapacity=0;msize=0;mbuffer=0;};
   Vector(int size);
   Vector(const Vector<T>& v);
   ~Vector();

   inline void sort(){ qsort(mbuffer, msize, sizeof(T), Vector::compareSort); };
   inline void sort(int (*compare)(const void*,const void*)){ qsort(mbuffer, msize, sizeof(T), compare); };
   inline int capacity() const { return mcapacity; };
   inline int size() const { return msize; };
   inline bool isEmpty() const { return msize==0; };
   inline T *data() { return mbuffer; };
   inline const T *data() const { return mbuffer; };
   inline const T *constData() const { return mbuffer; };
   void reserve(int capacity);

   inline Iterator begin() { return mbuffer; }
   inline ConstIterator begin() const { return mbuffer; }
   inline ConstIterator constBegin() const { return mbuffer; }
   inline Iterator end() { return mbuffer + msize; }
   inline ConstIterator end() const { return mbuffer + msize; }
   inline ConstIterator constEnd() const { return mbuffer + msize; }

   inline T& front() { return mbuffer[0]; };
   inline T& back() { return mbuffer[msize-1]; };
   inline void pop_back() { msize--; };
   inline void append(const T &t){ push_back(t); };
   inline void prepend(const T &t){ insert(0,t); };
   inline const T& at(int i) const { return mbuffer[i]; };
   inline T& operator[](int i){ return mbuffer[i]; };
   inline const T& operator[](int i) const { return mbuffer[i]; };
   void push_back(const T & value);
   void insert(int i,const T & v);
   void remove(int i);
   bool removeItem(const T & v);
   void resize(int size);
   bool contains(const T& t) const;
   int count(const T& t) const;
   Vector<T>& fill(const T& from, int asize);
   Vector<T> mid(int pos, int length = -1) const;
   Vector<T>& operator=(const Vector<T> &);
   bool operator==(const Vector<T> &v) const;
   inline bool operator!=(const Vector<T> &v) const { return !(*this == v); }
   Vector<T> &operator+=(const Vector<T> &l);
   inline Vector<T> operator+(const Vector<T> &l) const { Vector n = *this; n += l; return n; }
   inline Vector<T> &operator+=(const T &t){ append(t); return *this; }
   inline Vector<T> &operator<<(const T &t){ append(t); return *this; }
   inline Vector<T> &operator<<(const Vector<T> &l){ *this += l; return *this; }
   void clear();

   template <typename U>
   friend DataStream& operator>>(DataStream& s, Vector<U>& v);
   template <typename U>
   friend DataStream& operator<<(DataStream& s, const Vector<U>& v);
   template <typename U>
   friend std::ostream& operator<<(std::ostream& out, const Vector<U>& v);
};

template<typename U>
std::ostream& operator<<(std::ostream& out, const Vector<U>& v ){
    for(int i=0;i<v.size();i++){
    	out << v[i] << " ";
    }
    out << "\n";
    return out;
}

template<typename U>
DataStream& operator>>(DataStream& s, Vector<U>& v){
    v.clear();
    int32 n;
    s >> n;
    v.resize(n);
    for(int32 i = 0; i < n; i++) {
        U t;
        s >> t;
        v[i] = t;
    }
    return s;
}

template<typename U>
DataStream& operator<<(DataStream& s, const Vector<U>& v){
	int32 n = v.size();
    s << n;
    for(int32 i=0;i<v.size();i++){
    	s << v[i];
    }
    return s;
}

template<typename T>
Vector<T>::Vector(const Vector<T> & v){
    msize = v.msize;
    mcapacity = v.mcapacity;
    mbuffer = new T[mcapacity];
    for(int i=0;i<msize;i++){
    	mbuffer[i] = v.mbuffer[i];
    }
}

template<typename T>
Vector<T>::Vector(int size){
    mcapacity = size;
    msize = size;
    mbuffer = new T[msize];
}

template<typename T>
Vector<T>::~Vector(){
	if(mbuffer){
		delete[] mbuffer;
	}
}

template<typename T>
Vector<T> & Vector<T>::operator = (const Vector<T> & v){
    delete[] mbuffer;
    msize = v.msize;
    mcapacity = v.mcapacity;
    mbuffer = new T[mcapacity];
    for(int i=0;i<msize;i++){
    	mbuffer[i] = v.mbuffer[i];
    }
    return *this;
}

template<typename T>
bool Vector<T>::operator==(const Vector<T> &v) const {
    if (msize != v.msize) return false;
    if (mbuffer == v.mbuffer) return true;
    T* b = mbuffer;
    T* i = b + msize;
    T* j = v.mbuffer + msize;
    while (i != b){
        if (!(*--i == *--j)){
            return false;
        }
    }
    return true;
}


template <typename T>
Vector<T> &Vector<T>::fill(const T &from, int asize){
	if(asize>msize) asize = msize;
	if(asize<0) asize = 0;
    const T copy(from);
    if(asize){
        T *i = mbuffer + asize;
        T *b = mbuffer;
        while (i != b){
            *--i = copy;
        }
    }
    return *this;
}

template<typename T>
void Vector<T>::push_back(const T & v){
    if(msize+1 > mcapacity){
    	reserve(mcapacity+5);
    }
    T copy(v);
    mbuffer[msize] = copy;
    msize++;
}

template<typename T>
void Vector<T>::reserve(int capacity){
	if(capacity<msize){
		return;
	}
    T* buffer = new T[capacity];
    for(int i=0;i<msize;i++){
    	buffer[i] = mbuffer[i];
    }
    mcapacity = capacity;
    if(mbuffer){
    	delete[] mbuffer;
    }
    mbuffer = buffer;
}

template<typename T>
void Vector<T>::remove(int i){
	if(i<0 && i>=msize) return;
	while(i<msize-1){
		mbuffer[i] = mbuffer[i+1];
		i++;
	}
	msize--;
}

template<typename T>
bool Vector<T>::removeItem(const T& v){
	int i=0;
	int j=0;
	while(i<msize){
		if(mbuffer[j]==v){
			j++;
			msize--;
			continue;
		}
		mbuffer[i] = mbuffer[j];
		i++;
		j++;
	}
	return i!=j;
}

template<typename T>
void Vector<T>::insert(int i,const T & v){
	if(i>msize) i = msize;
	if(i<0) i = 0;
	if(msize+1 > mcapacity){
		reserve(mcapacity+5);
	}
	int j=msize-1;
	while(j>=i){
		mbuffer[j+1] = mbuffer[j];
		j--;
	}
	mbuffer[i] = v;
	msize++;
}

template<typename T>
void Vector<T>::resize(int size){
    reserve(size);
    msize = size;
}

template <typename T>
inline void Vector<T>::clear(){
	*this = Vector<T>();
}

template <typename T>
bool Vector<T>::contains(const T& t) const {
    T* b = mbuffer;
    T* i = mbuffer + msize;
    while (i != b){
        if (*--i == t){
            return true;
        }
    }
    return false;
}

template <typename T>
int Vector<T>::count(const T& t) const {
    int c = 0;
    T* b = mbuffer;
    T* i = mbuffer + msize;
    while (i != b){
        if (*--i == t){
            ++c;
        }
    }
    return c;
}

template <typename T>
Vector<T> Vector<T>::mid(int pos, int length) const {
	if (length < 0){
		length = msize-pos;
	}
    if (pos == 0 && length == msize){
        return *this;
    }
    if (pos + length > msize){
        length = msize - pos;
    }

    Vector<T> copy;
    copy.reserve(length);
    for (int i = pos; i < pos + length; ++i){
        copy += mbuffer[i];
    }
    return copy;
}

template <typename T>
Vector<T> &Vector<T>::operator+=(const Vector &l){

    int newSize = msize + l.msize;

    reserve(newSize);

    T* w = mbuffer + newSize;
    T* i = l.mbuffer + l.msize;
    T* b = l.mbuffer;

    while (i != b) {
    	*--w = *--i;
    }
    msize = newSize;
    return *this;
}

}
#endif /* VNS_VECTOR_H_ */

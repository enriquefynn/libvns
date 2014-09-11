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

#ifndef VNS_LIST_H_
#define VNS_LIST_H_

#include "datastream.h"
#include <iostream>

namespace vns {

template <class T>
class List {
private:
	/*! \cond PRIVATE */
	template<typename TT>
	struct ListNode {
		inline ListNode(const TT& data) : data(data), n(0), p(0){};
	    TT data;
	    ListNode* n;
	    ListNode* p;
	};
	/*! \endcond */


	ListNode<T>* head;
	ListNode<T>* tail;
	int s;

public:

	class ConstIterator;
	class Iterator {
	private:
		ListNode<T>* i;
	public:
      inline Iterator() : i(0) {};
      inline Iterator(ListNode<T>* n) : i(n) {};
      inline Iterator(const Iterator& it) : i(it.i) {};
      inline Iterator& operator=(const Iterator& it) { i = it.i; return *this; };
      inline T& operator*() const { return i->data; };
      inline T* operator->() const { return &i->data; };
      inline bool operator==(const Iterator& it) const { return i == it.i; };
      inline bool operator!=(const Iterator& it) const { return i != it.i; };
      inline bool operator==(const ConstIterator &it) const { return i == it.i; };
      inline bool operator!=(const ConstIterator &it) const { return i != it.i; };
      inline Iterator& operator++() { i = i->n; return *this; };
      inline Iterator operator++(int) { ListNode<T>* n = i; i = i->n; return n; }
      inline Iterator& operator--() { i = i->p; return *this; };
      inline Iterator operator--(int) { ListNode<T>* n = i; i = i->p; return n; };
      inline Iterator operator+(int j) const { ListNode<T>* n = i; if (j > 0) while (j--) n = n->n; else while (j++) n = n->p; return n; };
      inline Iterator operator-(int j) const { return operator+(-j); };
      inline Iterator& operator+=(int j) { return *this = *this + j; };
      inline Iterator& operator-=(int j) { return *this = *this - j; };
      friend class List;
  };
  friend class Iterator;
  class ConstIterator {
  private:
	  ListNode<T>* i;
  public:

      inline ConstIterator() : i(0) {};
      inline ConstIterator(ListNode<T> *n) : i(n) {};
      inline ConstIterator(const ConstIterator& it) : i(it.i){};
      inline explicit ConstIterator(const Iterator it) : i(it.i){};
      inline ConstIterator &operator=(const ConstIterator& it) { i = it.i; return *this; };
      inline const T& operator*() const { return i->data; };
      inline const T* operator->() const { return &i->data; };
      inline bool operator==(const ConstIterator& it) const { return i == it.i; };
      inline bool operator!=(const ConstIterator& it) const { return i != it.i; };
      inline ConstIterator& operator++() { i = i->n; return *this; };
      inline ConstIterator operator++(int) { ListNode<T>* n = i; i = i->n; return n; };
      inline ConstIterator& operator--() { i = i->p; return *this; };
      inline ConstIterator operator--(int) { ListNode<T>* n = i; i = i->p; return n; };
      inline ConstIterator operator+(int j) const { ListNode<T>* n = i; if (j > 0) while (j--) n = n->n; else while (j++) n = n->p; return n; };
      inline ConstIterator operator-(int j) const { return operator+(-j); };
      inline ConstIterator& operator+=(int j) { return *this = *this + j; };
      inline ConstIterator& operator-=(int j) { return *this = *this - j; };
      friend class List;
  };
  friend class ConstIterator;

  typedef Iterator iterator;
  typedef ConstIterator const_iterator;


	inline List():head(0),tail(0),s(0) {};
	List(const List<T>& l);
	~List();

	void append(const T& t);
	void prepend(const T& t);
	void push_back(const T& t) { append(t); };
	void push_front(const T& t) { prepend(t); };

	inline bool isEmpty() const { return s == 0; };
    inline int size() const { return s; };
    inline int count() const { return s; };
	inline T& front(){ return first(); };
	inline const T& front() const { return first(); };
	inline T& back(){ return last(); };
	inline const T& back() const { return last(); };
	inline void pop_front() { removeFirst(); };
	inline void pop_back() { removeLast(); };
    inline T& first() { return head->data; };
    inline const T& first() const { return head->data; };
    inline T& last() { return tail->data; };
    inline const T& last() const { return tail->data; };
    inline bool startsWith(const T& t) const { return s>0 && head->data == t; };
    inline bool endsWith(const T& t) const { return s>0 && tail->data == t; };

    void removeFirst();
    void removeLast();
    void clear();


    T takeFirst();
    T takeLast();
    int removeAll(const T& t);
    bool removeOne(const T& t);
    bool contains(const T& t) const;
    int count(const T &t) const;
    void print();

    bool operator==(const List<T>& l) const;
    inline bool operator!=(const List<T>& l) const { return !(*this == l); };
    List<T>& operator=(const List<T>& l);
	List<T>& operator+=(const List<T>& l);
	List<T> operator+(const List<T>& l) const;
    inline List<T>& operator+=(const T& t) { append(t); return *this; };
    inline List<T>& operator<<(const T& t) { append(t); return *this; };
    inline List<T>& operator<<(const List<T>& l) { *this += l; return *this; };

    inline Iterator begin() { return head; };
    inline ConstIterator begin() const { return head; };
    inline ConstIterator constBegin() const { return head; };
    inline Iterator end() { return 0; };
    inline ConstIterator end() const { return 0; };
    inline ConstIterator constEnd() const { return 0; };
    Iterator insert(Iterator before, const T& t);
    Iterator erase(Iterator pos);
    Iterator erase(Iterator first, Iterator last);

    template <typename U>
    friend DataStream& operator>>(DataStream& s, List<U>& v);
    template <typename U>
    friend DataStream& operator<<(DataStream& s, const List<U>& v);
    template <typename U>
    friend std::ostream& operator<<(std::ostream& out, const List<U>& v);
};

template<typename U>
std::ostream& operator<<(std::ostream& out, const List<U>& v ){
    for(typename List<U>::ConstIterator it=v.begin();it!=v.end();it++){
    	out << *it << " ";
    }
    out << "\n";
    return out;
}

template<typename U>
DataStream& operator>>(DataStream& s, List<U>& v){
    v.clear();
    int n;
    s >> n;
    for(int i = 0; i < n; i++) {
        U t;
        s >> t;
        v.append(t);
    }
    return s;
}

template<typename U>
DataStream& operator<<(DataStream& s, const List<U>& v){
	int n = v.size();
    s << n;
    for(typename List<U>::ConstIterator it=v.begin();it!=v.end();it++){
    	s << *it;
    }
    return s;
}


template<typename T>
List<T>::List(const List<T>& l){
	head = tail = 0; s = 0;
    ListNode<T>* li = l.head;
    while( li != 0 ) {
    	ListNode<T>* copy = new ListNode<T>(li->data);
    	if(tail==0){
    		head = tail = copy;
    	}else{
    		copy->p = tail;
    		tail->n = copy;
    		tail = copy;
    	}
    	li = li->n;
    }
    s = l.s;
}

template <typename T>
inline List<T>::~List(){
	clear();
}

template <typename T>
void List<T>::clear(){
    ListNode<T>* i = head;
    while( i != 0 ){
    	ListNode<T>* n = i;
        i = i->n;
        delete n;
    }
    head = tail = 0;
    s = 0;
}

template <typename T>
List<T>& List<T>::operator=(const List<T>& l){
	clear();
    ListNode<T>* li = l.head;
    while( li != 0 ) {
    	ListNode<T>* copy = new ListNode<T>(li->data);
    	if(tail==0){
    		head = tail = copy;
    	}else{
    		copy->p = tail;
    		tail->n = copy;
    		tail = copy;
    	}
    	li = li->n;
    }
    s = l.s;

    return *this;
}

template <typename T>
bool List<T>::operator==(const List<T>& l) const {
    if(s != l.s) return false;
    ListNode<T>* i = head;
    ListNode<T>* il = l.head;
    while( i != 0 ){
    	if (i->data != il->data){
    		return false;
    	}
    	i = i->n;
    	il = il->n;
    }
    return true;
}


template <typename T>
void List<T>::append(const T& t){
    ListNode<T>* i = new ListNode<T>(t);
    if(s==0){
    	head = tail = i;
    }else{
    	i->p = tail;
    	tail->n = i;
    	tail = i;
    }
    s++;
}

template <typename T>
void List<T>::prepend(const T& t){
    ListNode<T>* i = new ListNode<T>(t);
    if(s==0){
    	head = tail = i;
    }else{
    	i->n = head;
    	head->p = i;
    	head = i;
    }
    s++;
}


template <typename T>
int List<T>::removeAll(const T& t) {
    ListNode<T>* i = head;
    int c = 0;
    while( i != 0 ){
    	if (i->data == t) {
    		ListNode<T>* n = i;
            if(i->p) i->p->n = i->n;
            if(i->n) i->n->p = i->p;
            if(i==head) head = i->n;
            if(i==tail) tail = i->p;
            i = i->n;
            delete n;
            s--;
            c++;
        } else {
            i = i->n;
        }
    }
    return c;
}

template <typename T>
bool List<T>::removeOne(const T& t) {
    ListNode<T>* i = head;
    while( i != 0 ){
    	if (i->data == t) {
    		ListNode<T>* n = i;
            if(i->p) i->p->n = i->n;
            if(i->n) i->n->p = i->p;
            if(i==head) head = i->n;
            if(i==tail) tail = i->p;
            i = i->n;
            delete n;
            s--;
            return true;
        } else {
            i = i->n;
        }
    }
    return false;
}

template <typename T>
inline void List<T>::removeFirst(){
	if(s==0) return;
	ListNode<T>* n = head;
	if(s==1){
		head = tail = 0;
	}else{
	    head = head->n;
	    head->p = 0;
	}
	delete n;
    s--;
}

template <typename T>
inline void List<T>::removeLast(){
	if(s==0) return;
	ListNode<T>* n = tail;
	if(s==1){
		head = tail = 0;
	}else{
	    tail = tail->p;
	    tail->n = 0;
	}
	delete n;
    s--;
}


template <typename T>
inline T List<T>::takeFirst(){
    T t = head->data;
    removeFirst();
    return t;
}

template <typename T>
inline T List<T>::takeLast() {
	T t = tail->data;
    removeLast();
    return t;
}


template <typename T>
bool List<T>::contains(const T& t) const {
	ListNode<T>* i = head;
    while( i != 0 ){
        if (i->data == t){
            return true;
        }
        i = i->n;
    }
    return false;
}

template <typename T>
int List<T>::count(const T& t) const {
	ListNode<T>* i = head;
    int c = 0;
    while( i != 0 ){
        if (i->data == t){
            c++;
        }
        i = i->n;
    }
    return c;
}


template <typename T>
List<T>& List<T>::operator+=(const List<T>& l){
    ListNode<T>* li = l.head;
    while( li != 0 ) {
    	ListNode<T>* copy = new ListNode<T>(li->data);
    	if(tail==0){
    		head = tail = copy;
    	}else{
    		copy->p = tail;
    		tail->n = copy;
    		tail = copy;
    	}
    	li = li->n;
    }
    s += l.s;
    return *this;
}

template <typename T>
List<T> List<T>::operator+(const List<T>& l) const {
	List<T> n = *this;
    n += l;
    return n;
}

template <typename T>
typename List<T>::Iterator List<T>::insert(Iterator before, const T &t){
	if(before.i==0){
		this->append(t);
		return 0;
	}
    ListNode<T>* i = before.i;
    ListNode<T>* m = new ListNode<T>(t);
    m->n = i;
    if(i){
    	m->p = i->p;
    	i->p = m;
    }
    if(m->p) m->p->n = m;
    if(head==0) head = tail = m;
    if(i==head) head = m;
    s++;
    return m;
}

template <typename T>
typename List<T>::Iterator List<T>::erase(Iterator pos) {
	ListNode<T>* i = pos.i;
    if(i != 0){
		ListNode<T>* n = i;
        if(i==head) head = i->n;
        if(i==tail) tail = i->p;
        if(i->p) i->p->n = i->n;
        if(i->n) i->n->p = i->p;
        i = i->n;
        delete n;
        s--;
        if(i){
        	if(i->p==0) head = i;
        	if(i->n==0) tail = i;
        }
    }
    return i;
}

template <typename T>
typename List<T>::Iterator List<T>::erase(typename List<T>::Iterator afirst,typename List<T>::Iterator alast){
    while (afirst != alast)
        erase(afirst++);
    return alast;
}

}

#endif

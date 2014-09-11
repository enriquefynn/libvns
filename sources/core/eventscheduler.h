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

#ifndef VNS_EVENTSCHEDULER_H_
#define VNS_EVENTSCHEDULER_H_

namespace vns {

class Event {
protected:
	double time;
public:
	Event(double t):time(t){};
	virtual ~Event(){};
	inline double getTime() const { return time; };
	virtual void invoke() = 0;
};


class EventScheduler {
private:
	/*! \cond PRIVATE */
    template <typename FUNC, typename OBJ>
    class ObjEvent : public Event {
    private:
    	OBJ obj;FUNC function;
    public:
    	void invoke(){ (obj->function)(); };
    	ObjEvent(double time,FUNC function,OBJ obj):Event(time),obj(obj),function(function){};
    };
    template <typename FUNC, typename OBJ,typename T1>
    class ObjEventT1 : public Event {
    public:
    	ObjEventT1(double t,FUNC function,OBJ obj,T1 t1):Event(t),obj(obj),function(function),t1(t1){};
    	void invoke(){ ((*obj).*function)(t1); };
    private:
    	OBJ obj;FUNC function;T1 t1;
    };
    template <typename FUNC, typename OBJ,typename T1,typename T2>
    class ObjEventT2 : public Event {
    public:
    	ObjEventT2(double t,FUNC function,OBJ obj,T1 t1,T2 t2):Event(t),obj(obj),function(function),t1(t1),t2(t2){};
    	void invoke(){ ((*obj).*function)(t1,t2); };
    private:
    	OBJ obj;FUNC function;T1 t1;T2 t2;
    };
    template <typename FUNC, typename OBJ,typename T1,typename T2,typename T3>
    class ObjEventT3 : public Event {
    public:
    	ObjEventT3(double t,FUNC function,OBJ obj,T1 t1,T2 t2,T3 t3):Event(t),obj(obj),function(function),t1(t1),t2(t2),t3(t3){};
    	void invoke(){ ((*obj).*function)(t1,t2,t3); };
    private:
    	OBJ obj;FUNC function;T1 t1;T2 t2;T3 t3;
    };
    class FunctionEvent : public Event {
    private:
    	void (*function)(void);
    public:
    	void invoke(){ function(); };
    	FunctionEvent(double t,void (*function)(void)):Event(t),function(function){};
    };
    /*! \endcond */
public:
    EventScheduler(){};
    virtual ~EventScheduler(){};
	virtual void schedule(Event* event) = 0;
	virtual void invokeEvents(double start,double dt) = 0;
	virtual int getNumberOfEvents() const = 0;
	virtual bool isEmpty() const = 0;
	virtual void clearEvents() = 0;

    inline void schedule(double time, void (*function)(void) ){
    	schedule(new FunctionEvent(time,function));
    };
	template <typename FUNC, typename OBJ>
	void schedule(double time, FUNC function, OBJ obj){
		schedule(new ObjEvent<FUNC,OBJ>(time,function,obj) );
	};
	template <typename FUNC, typename OBJ,typename T1>
	void schedule(double time, FUNC function, OBJ obj,T1 t1){
		schedule(new ObjEventT1<FUNC,OBJ,T1>(time,function,obj,t1) );
	};
	template <typename FUNC, typename OBJ,typename T1,typename T2>
	void schedule(double time, FUNC function, OBJ obj,T1 t1,T2 t2){
		schedule(new ObjEventT2<FUNC,OBJ,T1,T2>(time,function,obj,t1,t2) );
	};
	template <typename FUNC, typename OBJ,typename T1,typename T2,typename T3>
	void schedule(double time, FUNC function, OBJ obj,T1 t1,T2 t2,T3 t3){
		schedule(new ObjEventT3<FUNC,OBJ,T1,T2,T3>(time,function,obj,t1,t2,t3) );
	};
};

}

#endif

#ifndef EVENT_QUEUE_H_INCLUDED
#define EVENT_QUEUE_H_INCLUDED

#include <iostream>
#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "primeID.h"

// TODO: re-factor all the inline functions to event_queue.inl

namespace evq
{
    class EventQueue;
    class Listener;

    typedef uintmax_t EventType;

    class Event
    {
        private:
            EventType _event_type;
        public:
            Event( EventType _et ) : _event_type(_et) {  }
            Event( const Event& e ) = delete;
            inline EventType eventType() const { return _event_type; }
            template < typename T > static inline EventType Type() { return primeid::type<T>(); }
            template < typename T > inline EventType DeriveEventType() { return _event_type = primeid::getID<T>( _event_type ); } // the template parameter T should be of the derived class in the derived classes constructor. ex:   class A : public Event { A() : Event(Event::Type<A>()) {} }; class B : public A { B(){ DeriveEventType<B>(); } };
            template < typename T > static inline bool isType( const Event * e ) { static_assert(std::is_base_of<Event,T>::value,""); return e->eventType() % Type<T>() == 0; }
    };

    class EventQueue
    {
        friend class Listener;

        private:
            std::mutex _eventQueueLock;
            std::condition_variable _eventQueueCondition;
            std::queue<Event*> events;

            std::mutex _listenerVectorLock;
            std::vector<Listener*> listeners;

            std::size_t numThreads;

            std::thread _processingThread;

        public:
            EventQueue( std::size_t num_threads = 1 );
            virtual ~EventQueue();

            std::size_t queueEvent( Event * e );
            std::size_t hookListener( Listener * l );

            std::size_t numEventsQueued();

            void waitForEmpty();

            Event * pullNextEvent();

        private:
            static void _eventProcessingFunction( void * _data );

            void _passEventToListeners( Event * e, std::size_t starting_index = 0, std::size_t increment = 1 );

            void _deallocateEvent( Event * e );
    };

    class Listener
    {
        friend class EventQueue;
        private:
            EventQueue * _parent_queue;
        public:

            Listener() : _parent_queue(nullptr) {  }
            Listener( const Listener & _l ) : _parent_queue( _l._parent_queue ) {  }
            virtual ~Listener() { _parent_queue = nullptr; }

            inline EventQueue * parentQueue() const { return _parent_queue; }
            inline void broadcastEvent( Event * e ) { parentQueue()->queueEvent(e);}

            virtual void processEvent( const Event * e ) = 0;
            virtual bool isRelevant( const Event * e ) = 0;

        private:
            Listener & operator= ( const Listener & _l ) = delete;
    };
}

namespace EventQueue { using namespace evq; }

#endif // EVENT_QUEUE_H_INCLUDED

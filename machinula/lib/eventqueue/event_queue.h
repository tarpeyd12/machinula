#ifndef EVENT_QUEUE_H_INCLUDED
#define EVENT_QUEUE_H_INCLUDED

#include <iostream>
#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "primeID.h"

#include "../alloc.h"

// TODO(dean): re-factor all the inline functions to event_queue.inl

namespace evq
{
    class EventQueue;
    class Listener;

    typedef uintmax_t EventType;

    // TODO(dean): rewrite this so that it is much cleaner ( ie. using bool(dynemic_cast<T>()) )
    struct Event
    {
        private:
            EventType _event_type;
        public:
            Event( EventType _et ) : _event_type( _et ) { }
            Event( const Event& e ) = delete;
            virtual ~Event() = default;
            inline EventType eventType() const;
            template < typename T > static inline EventType Type();
            template < typename T > inline EventType DeriveEventType(); // the template parameter T should be of the derived class in the derived classes constructor. ex:   class A : public Event { A() : Event(Event::Type<A>()) {} }; class B : public A { B(){ DeriveEventType<B>(); } };
            template < typename T > static inline bool isType( const Event * e );
            template < typename T > static inline bool isType( const ptr::shared_ptr<Event> e );
    };

    class EventQueue
    {
        friend class Listener;

        private:
            std::mutex _eventQueueLock;
            std::condition_variable _eventQueueCondition;
            std::queue< ptr::shared_ptr<Event> > events;

            std::mutex _listenerVectorLock;
            std::vector< ptr::shared_ptr<Listener> > listeners;

            std::size_t numThreads;

            std::thread _processingThread;

        public:
            EventQueue( std::size_t num_threads = 1 );
            virtual ~EventQueue();

            std::size_t queueEvent( ptr::shared_ptr<Event> );
            std::size_t queueEvents( const std::vector< ptr::shared_ptr<Event> >& ve );
            std::size_t hookListener( ptr::shared_ptr<Listener> l );

            std::size_t numEventsQueued();

            void waitForEmpty();

            ptr::shared_ptr<Event> pullNextEvent();

        private:
            EventQueue( const EventQueue & ) = delete;
            EventQueue & operator= ( const EventQueue & ) = delete;

            static void _eventProcessingFunction( EventQueue * eventQueue );

            void _passEventToListeners( ptr::shared_ptr<Event> e, std::size_t starting_index = 0, std::size_t increment = 1 );

            void _deallocateEvent( ptr::shared_ptr<Event> e );
    };

    class Listener
    {
        friend class EventQueue;

        private:
            EventQueue * _parent_queue;
        public:

            Listener() : _parent_queue(nullptr) { }
            Listener( const Listener & _l ) : _parent_queue( _l._parent_queue ) { }
            virtual ~Listener() { _parent_queue = nullptr; }

            inline EventQueue * parentQueue() const;
            inline void broadcastEvent( ptr::shared_ptr<Event>& e );

            virtual void processEvent( ptr::shared_ptr<Event>& e ) = 0;
            virtual bool isRelevant( const ptr::shared_ptr<Event>& e ) = 0;

        private:
            // non assignable
            Listener & operator= ( const Listener & _l ) = delete;
    };
}

namespace EventQueue { using namespace evq; }

#include "event_queue.inl"

#endif // EVENT_QUEUE_H_INCLUDED

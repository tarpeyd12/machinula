#ifndef EVENT_QUEUE_H_INCLUDED
#define EVENT_QUEUE_H_INCLUDED

#include <iostream>
#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "primeID.h"

// TODO(dean): re-factor all the inline functions to event_queue.inl

namespace evq
{
    class EventQueue;
    class Listener;

    typedef uintmax_t EventType;

    struct Event
    {
        private:
            EventType _event_type;
        public:
            Event( EventType _et ) : _event_type(_et) {  }
            Event( const Event& e ) = delete;
            virtual ~Event() = default;
            inline EventType eventType() const;
            template < typename T > static inline EventType Type();
            template < typename T > inline EventType DeriveEventType(); // the template parameter T should be of the derived class in the derived classes constructor. ex:   class A : public Event { A() : Event(Event::Type<A>()) {} }; class B : public A { B(){ DeriveEventType<B>(); } };
            template < typename T > static inline bool isType( const Event * e );
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
            std::size_t queueEvents( const std::vector<Event *>& ve );
            std::size_t hookListener( Listener * l );

            std::size_t numEventsQueued();

            void waitForEmpty();

            Event * pullNextEvent();

        private:
            static void _eventProcessingFunction( EventQueue * eventQueue );

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

            inline EventQueue * parentQueue() const;
            inline void broadcastEvent( Event * e );

            virtual void processEvent( Event * e ) = 0;
            virtual bool isRelevant( const Event * e ) = 0;

        private:
            Listener & operator= ( const Listener & _l ) = delete;
    };
}

namespace EventQueue { using namespace evq; }

#include "event_queue.inl"

#endif // EVENT_QUEUE_H_INCLUDED

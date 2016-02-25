#ifndef EVENT_QUEUE_H_INCLUDED
#define EVENT_QUEUE_H_INCLUDED

#include <iostream>
#include <vector>
#include <queue>

#include "lib/lthread/lthread.h"

namespace EventQueue
{
    class EventQueue;

    class Event
    {
        private:
            uintmax_t _event_type;
        public:
            Event( uintmax_t _et ) : _event_type(_et) {  }
            inline uintmax_t eventType() const { return _event_type; }
            template < typename T > static inline uintmax_t Type() { return typeid(T).hash_code(); }
    };

    class Listener
    {
        friend class EventQueue;
        private:
            EventQueue * _parent_queue;
        public:

            Listener() : _parent_queue(nullptr) {  }
            virtual ~Listener() { _parent_queue = nullptr; }

            EventQueue * parentQueue() const { return _parent_queue; }

            virtual void processEvent( const Event * e ) = 0;
            virtual bool isRelevant( const Event * e ) = 0;
    };

    class EventQueue
    {
        friend class Listener;

        private:
            lthread::mutex _eventQueueLock;
            lthread::condition_variable _eventQueueCondition;
            std::queue<Event*> events;

            lthread::mutex _listenerVectorLock;
            std::vector<Listener*> listeners;

            std::size_t numThreads;

            lthread::thread _processingThread;

        public:
            EventQueue( std::size_t num_threads = 1 );
            virtual ~EventQueue();

            int queueEvent( Event * e );
            int hookListener( Listener * l );

            std::size_t numEventsQueued();

            void waitForEmpty();

            Event * pullNextEvent();

        private:
            static void _eventProcessingFunction( void * _data );

            void _passEventToListeners( Event * e, std::size_t starting_index = 0, std::size_t increment = 1 );

            void _deallocateEvent( Event * e );
    };
}

#endif // EVENT_QUEUE_H_INCLUDED

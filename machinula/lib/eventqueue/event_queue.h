#ifndef EVENT_QUEUE_H_INCLUDED
#define EVENT_QUEUE_H_INCLUDED

#include <iostream>
#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

namespace EventQueue
{
    class EventQueue;
    class Listener;

    class Event
    {
        private:
            uintmax_t _event_type;
        public:
            Event( uintmax_t _et ) : _event_type(_et) {  }
            inline uintmax_t eventType() const { return _event_type; }
            template < typename T > static inline uintmax_t Type() { return typeid(T).hash_code(); }
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

namespace evq { using namespace EventQueue; }

#endif // EVENT_QUEUE_H_INCLUDED

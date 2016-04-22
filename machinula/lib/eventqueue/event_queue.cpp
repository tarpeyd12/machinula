#include "event_queue.h"

#include <cassert>

namespace evq
{
    EventQueue::EventQueue( std::size_t num_threads )
    : _eventQueueLock(), _eventQueueCondition(), events(), _listenerVectorLock(), listeners(), numThreads( num_threads ? num_threads : 1 ), _processingThread( EventQueue::_eventProcessingFunction, this )
    {  }

    EventQueue::~EventQueue()
    {
        // queue a null pointer to signal the thread to terminate
        {
            // we push manually to bypass queueEvent()'s nullptr check
            std::lock_guard<std::mutex> _lock( _eventQueueLock );
            events.push( nullptr );
            _eventQueueCondition.notify_all();
        }

        // join the thread
        _processingThread.join();

        // clear all remaining events.
        while( events.size() )
        {
            assert( events.front() != nullptr );
            _deallocateEvent( events.front() );
            events.pop();
        }
    }

    std::size_t
    EventQueue::queueEvent( Event * e )
    {
        assert( e != nullptr );

        std::size_t size = 0;
        {
            // lock
            std::lock_guard<std::mutex> _lock(_eventQueueLock);

            // wait for a slot in the queue
            /*while( events.size() > 8192 )
            {
                _eventQueueCondition.wait( _eventQueueLock );
            }*/

            // enque
            events.push( e );
            size = events.size();

            _eventQueueCondition.notify_all();
        }
        return size;
    }

    std::size_t
    EventQueue::queueEvents( const std::vector< Event * > & ve )
    {
        std::size_t size = 0;
        {
            // lock
            std::lock_guard<std::mutex> _lock(_eventQueueLock);

            // enque
            //for( auto e : ve )
            for( std::size_t i = 0; i < ve.size(); ++i  )
            {

                assert( ve[ i ] != nullptr );
                events.push( ve[ i ] );
                //ve[ i ] = nullptr;
            }

            size = events.size();

            _eventQueueCondition.notify_all();
        }
        return size;
    }

    std::size_t
    EventQueue::hookListener( Listener * l )
    {
        assert( l != nullptr );
        assert( l->_parent_queue == nullptr && this != l->_parent_queue );

        std::size_t size = 0;
        {
            // lock
            std::lock_guard<std::mutex> _lock(_listenerVectorLock);

            //attach
            listeners.push_back( l );
            size = listeners.size();
            l->_parent_queue = this;
        }
        return size;
    }

    std::size_t
    EventQueue::numEventsQueued()
    {
        std::size_t size = 0;
        {
             // lock
            std::lock_guard<std::mutex> _lock(_eventQueueLock);

            // get size
            size = events.size();
        }
        return size;
    }

    void
    EventQueue::waitForEmpty()
    {
        // TODO: check if _eventQueueCondition.wait( _lock ); is equal to _eventQueueCondition.wait( _eventQueueLock );
        // get the lock
        //std::lock_guard<std::mutex> _lock(_eventQueueLock);
        std::unique_lock<std::mutex> _lock(_eventQueueLock);

        // wait for an empty queue
        while( events.size() > 0 )
        {
            //_eventQueueCondition.wait( _eventQueueLock );
            _eventQueueCondition.wait( _lock );
        }
    }

    Event *
    EventQueue::pullNextEvent()
    {
        // TODO: check if _eventQueueCondition.wait( _lock ); is equal to _eventQueueCondition.wait( _eventQueueLock );
        // lock
        //std::lock_guard<std::mutex> _lock(_eventQueueLock);
        std::unique_lock<std::mutex> _lock(_eventQueueLock);

        // wait until we have events to process
        while( events.size() < 1 )
        {
            //_eventQueueCondition.wait( _eventQueueLock );
            _eventQueueCondition.wait( _lock );
        }

        Event * e = events.front();
        events.pop();

        _eventQueueCondition.notify_all();

        return e;
    }

    // this function is the main function of the thread.
    void
    EventQueue::_eventProcessingFunction( EventQueue * eventQueue )
    {
        assert( eventQueue != nullptr );

        // make sure we dont have more threads than we have listeners.
        std::size_t num_threads = std::min( eventQueue->numThreads, eventQueue->listeners.size() );
        // make sure we have at least one thread.
        num_threads = std::max<std::size_t>( num_threads, 1 );

        /*std::thread ** listenerThreads = nullptr;
        if( num_threads > 1 )
        {
            new std::thread*[num_threads-1];
        }*/

        std::thread * listenerThreads = nullptr;
        if( num_threads > 1 )
        {
            listenerThreads = new std::thread[num_threads-1];
        }

        while( true )
        {
            Event * e = eventQueue->pullNextEvent();

            // if the event is a null pointer we exit the infinite loop
            if( e == nullptr )
                break;

            // pass the event to the listeners
            {
                // lock
                std::lock_guard<std::mutex> _lock(eventQueue->_listenerVectorLock);

                // TODO: hoist this if-else
                // threaded?
                if( num_threads <= 1 )
                {
                    // single threaded
                    eventQueue->_passEventToListeners( e, 0, 1 );
                }
                else
                {
                    for( std::size_t i = 0; i < num_threads-1; ++i )
                    {
                        listenerThreads[i] = std::thread( [=]{ eventQueue->_passEventToListeners( e, i, num_threads ); } );
                    }

                    eventQueue->_passEventToListeners( e, num_threads-1, num_threads );

                    for( std::size_t i = 0; i < num_threads-1; ++i )
                    {
                        listenerThreads[i].join();
                    }
                }

            }

            // deallocate event
            eventQueue->_deallocateEvent( e );
        }

        if( listenerThreads )
        {
            delete [] listenerThreads;
        }
    }

    void
    EventQueue::_passEventToListeners( Event * e, std::size_t starting_index, std::size_t increment )
    {
        // this function assumes that it is being executed when _listenerVectorLock is locked

        assert( increment > 0 && starting_index < increment );

        // if universal event
        if( e->eventType() == 0 ) // a hoisted if statement
        {
            // iterate over the listeners
            for( std::size_t i = starting_index; i < listeners.size(); i += increment )
            {
                // pass the listener the universal event
                listeners.at( i )->processEvent( e );
            }
            // exit the function
            return;
        }

        // iterate over the listeners
        for( std::size_t i = starting_index; i < listeners.size(); i += increment )
        {
            Listener * l = listeners.at( i );

            // give listeners relevant events
            if( l->isRelevant( e ) )
            {
                l->processEvent( e );
            }
        }
    }

    void
    EventQueue::_deallocateEvent( Event * e )
    {
        // TODO: make this function thread-safe, since we might be using allocators!
        assert( nullptr != e );
        delete e;
    }

}

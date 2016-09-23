

#include <cstdlib>

#include <iostream>
#include <sstream>

#include "lib/alloc.h"
#include "lib/event_queue.h"
#include "lib/ecs.h"

#include "timer_dispatch.h"

#include "random_functor.h"

// NOTE(dean): this is a fix for Code::Blocks MinGW in windows not having std::to_string working correctly
template < typename T >
inline
std::string
to_string( const T& n )
{
    std::ostringstream stm;
    stm << n ;
    return stm.str();
}

void eventQueueStuff( evq::EventQueue * eventQueue );
void memoryStuff( evq::EventQueue * eventQueue );

// an event listener that prints out message events to a given std::ostream & ( default = std::cout )
class DebugListener : public evq::Listener
{
    private:

        // the destination output stream
        std::ostream & out;

    public:

        // the message event
        struct MessageEvent : public evq::Event
        {
            std::string message;

            MessageEvent( const std::string& m )
            : evq::Event( evq::Event::Type<MessageEvent>() ), message( m )
            {  }
        };

        DebugListener( std::ostream & o = std::cout ) : out(o) {  }
        ~DebugListener() {  }

        void
        processEvent( const evq::Event * e )
        {
            // pull message from given event pointer
            std::string message = static_cast<const MessageEvent*>(e)->message;

            // print message
            out << message;

            // put newline on end of message if one is not given
            if( !message.length() || ( message.length() && '\n' != message.at( message.length() - 1 ) ) )
            {
                out << std::endl;
            }
        }

        // check for relevant events
        bool
        isRelevant( const evq::Event * e )
        {
            return evq::Event::isType<MessageEvent>( e );// && static_cast<const MessageEvent*>(e)->message.size();
        }
};

int
main( int /*argc*/, char* /*argv*/[] )
{
    std::ios_base::sync_with_stdio( false );

    // create our event queue
    evq::EventQueue * eventQueue = new evq::EventQueue();

    // register our debug listener to the event queue
    eventQueue->hookListener( new DebugListener( std::cout ) );

    // execute some fun stuff with timers and the event queue.
    eventQueueStuff( eventQueue );

    // execute some fun stuff with memory allocators, print debug with event queue
    memoryStuff( eventQueue );

    // make sure there are no more events to execute
    eventQueue->waitForEmpty();

    // delete the event queue to be clean
    delete eventQueue;

    // good by program.
    return EXIT_SUCCESS;
}

void
eventQueueStuff( evq::EventQueue * eventQueue )
{
    eventQueue->queueEvent( new DebugListener::MessageEvent("Hooking TimerSignalDispatchListener.") );

    // hook a listener to the event queue so we can see the timers ticking
    eventQueue->hookListener( new timer_dispatch::TimerSignalDispatchListener() );

    double timers_runtime = 3.0; // seconds

    // list of timers
    std::vector< timer_dispatch::Timer * > timers;

    // we make 4 timers to keep things simple
    for( std::size_t i = 1; i <= 4; ++i )
    {
        timer_dispatch::Timer * timer = new timer_dispatch::Timer(
                                                                i,                          // timer ID number
                                                                "TestTimer"+to_string(i-1), // timer name
                                                                i*timers_runtime,           // number of ticks to dispatch
                                                                timers_runtime,             // number of seconds to run
                                                                eventQueue                  // the event queue to dispatch ticks to
                                                                );

        timers.push_back( timer );

        //std::this_thread::sleep_for( std::chrono::microseconds(50) ); // stager when the timers start to give some
    }

    // wait for timers to finish.
    // timer threads are joined in their destructor

    for( auto t : timers )
    {
        delete t;
    }

    eventQueue->queueEvent( new DebugListener::MessageEvent("Timers Done.") );
}

/*
 * a few things to note about this function; the debug messages are not operating in the limited memory space
 * which would be nice, but we would have to override the allocation tendencies of not only std::string
 * but also std::ostringstream and its like, which would be rather a tedious and error prone process.
 *
 * any who.  In this function we allocate a 1GB chunk of memory which we have exclusive control over and
 * we use the Free List Allocation method to divvy it up. we then allocate a multimap of pair<int,double>'s
 * multimap is good for testing for two reasons, it has horrible times in deallocation with the FreeLitAllocator
 * and it deallocates ALL of its memory when multimap<>::clear() is called. which the unordered_map variants
 * do not since they keep their hash tables until their destructors are called.
 */
void
memoryStuff( evq::EventQueue * eventQueue )
{
    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent("Creating DefaultAllocator.") );

    // wrapper for ::operator new() and ::operator delete()
    alloc::DefaultAllocator defaultAllocator( 0, nullptr );

    // pointer to our memory pool
    void * _mem_pool;

    // size of our memory pool: 1GB
    //std::size_t _mem_size = 1024*1024*1024;

    // size of our memory pool: 0.5MB
    std::size_t _mem_size = 1024*1024*0.5f;

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent("Allocating Memory Pool of size " + to_string(_mem_size) + "bytes.") );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent("Allocating FreeListAllocator, for Memory Pool.") );

    // allocate a free list allocator and allocate the memory pool and pass it to the free list allocator.
    alloc::FreeListAllocator * fla = new( (alloc::FreeListAllocator*)defaultAllocator ) alloc::FreeListAllocator( _mem_size, _mem_pool = defaultAllocator.allocateBlock(_mem_size,0) );

    {
        alloc::ptr::weak_ptr<int> test_weak_ptr_outer;
        alloc::ptr::shared_ptr<int> test_shared_ptr_outer;
        {
            alloc::ptr::shared_ptr<int> test_shared_ptr( new( (int*)*fla ) int(1), fla );

            std::cout << *test_shared_ptr << std::endl;
            test_weak_ptr_outer = test_shared_ptr;
            test_shared_ptr_outer = test_shared_ptr;
        }
        if( auto t = test_weak_ptr_outer.lock() )
        {
            std::cout << *t << std::endl;
        }


        alloc::ptr::unique_ptr<int> test_unique_ptr_outer;
        {
            alloc::ptr::unique_ptr<int> test_unique_ptr( new( (int*)*fla ) int(1000), fla );

            test_unique_ptr_outer.swap( test_unique_ptr );
        }
        std::cout << *test_unique_ptr_outer << std::endl;
    }

    std::ostringstream stream; // we don't have control over this memory

    {

        // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueEvent( new DebugListener::MessageEvent("Allocating IntMap from FreeListAllocator.") );

        // allocate a std::multimap<int,double> with our custom allocator adapters
        alloc::stl::multimap< int, double > * intmap = new( (alloc::stl::multimap< int, double >*)*fla ) alloc::stl::multimap< int, double >( fla );

        // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueEvent( new DebugListener::MessageEvent("") );

        {
            // make 10000 pairs!
            for( std::size_t i = 0; i < 10000; ++i )
            {
                // insert a pair

                int key = Rand::Int();
                double value = sqrt( double(key) );

                // here multimap will allocate space for a node that contains a pair<int,double> in it.
                intmap->insert( std::pair<int,double>(key,value) );

                // print progress
                if( (i) % 200 == 0 )
                {
                    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
                    eventQueue->queueEvent( new DebugListener::MessageEvent("Inserted " + to_string(i) + " pairs into IntMap on FreeListAllocator.\n") );
                }

            }
        }

        // clear the stringstream buffer
        stream.str( std::string() );
        stream.clear();

        // put allocator debug info into stringstream buffer
        fla->printDebugInfo( stream );

        // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueEvent( new DebugListener::MessageEvent( stream.str() ) );

        // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueEvent( new DebugListener::MessageEvent("Deallocating all memory associated with IntMap in FreeListAllocator.") );

        // deallocate and destruct the multimap<int,double>
        fla->deallocate( intmap );
    }

    // clear the stringstream buffer
    stream.str( std::string() );
    stream.clear();

    // put allocator debug info into stringstream buffer
    fla->printDebugInfo( stream );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( stream.str() ) );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( "cleaning up ... fla" ) );

    // destruct the FreeListAllocator
    defaultAllocator.deallocate( fla );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( "cleaning up ... _mem_pool" ) );

    // deallocate the memory block
    defaultAllocator.deallocateBlock( _mem_pool );

    // clear the stringstream buffer
    stream.str( std::string() );
    stream.clear();

    // put allocator debug info into stringstream buffer
    defaultAllocator.printDebugInfo( stream );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( stream.str() ) );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( "complete." ) );
}

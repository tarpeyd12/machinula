

#include <cstdlib>

#include <iostream>
#include <sstream>

#include "lib/alloc.h"
#include "lib/event_queue.h"
#include "lib/ecs.h"

#include "timer_dispach.h"

#include "random_functor.h"

// TODO: NOTE: this is a fix for Code::Blocks MinGW not having std::to_string working correctly
template < typename T >
std::string
to_string( const T& n )
{
    std::ostringstream stm;
    stm << n ;
    return stm.str();
}

void eventQueueStuff( evq::EventQueue * eventQueue );
void memoryStuff( evq::EventQueue * eventQueue );

class DebugListener : public evq::Listener
{
    private:

        std::ostream & out;

    public:

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
            std::string message = static_cast<const MessageEvent*>(e)->message;

            out << message;

            if( message.length() && '\n' != message.at( message.length() - 1 ) )
            {
                out << std::endl;
            }
        }

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

    evq::EventQueue * eventQueue = new evq::EventQueue();
    eventQueue->hookListener( new DebugListener() );

    eventQueueStuff( eventQueue );

    memoryStuff( eventQueue );

    eventQueue->waitForEmpty();

    delete eventQueue;

    return 0;
}

void
eventQueueStuff( evq::EventQueue * eventQueue )
{
    eventQueue->hookListener( new timer_dispach::TimerSignalDispachListener() );

    double timers_runtime = 3.0; // seconds

    std::vector<timer_dispach::Timer *> timers;

    for( std::size_t i = 1; i <= 4; ++i )
    {
        double dt = 1.0/double(i);
        timers.push_back( new timer_dispach::Timer( i, "TestTimer"+to_string(i-1), i*timers_runtime, timers_runtime, eventQueue ) );
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }

    // wait for timers to finish.

    for( auto t : timers )
    {
        delete t;
    }

    eventQueue->queueEvent( new DebugListener::MessageEvent("Timers Done.") );
}


void
memoryStuff( evq::EventQueue * eventQueue )
{
    // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent("Creating DefaultAllocator.") );

    alloc::DefaultAllocator defaultAllocator( 0, nullptr );

    void * _mem_pool;

    std::size_t _mem_size = 1024*1024*1024;

    // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent("Allocating Memory Pool of size " + to_string(_mem_size) + "bytes.") );

    // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent("Allocating FreeListAllocator, for Memory Pool.") );

    alloc::FreeListAllocator * fla = new(defaultAllocator.allocate<alloc::FreeListAllocator>()) alloc::FreeListAllocator( _mem_size, _mem_pool = defaultAllocator.allocateBlock(_mem_size,0) );

    std::ostringstream stream; // we don't have control over this memory

    {

        // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueEvent( new DebugListener::MessageEvent("Allocating IntMap from FreeListAllocator.") );

        alloc::stl::multimap< int, double > * intmap = new(fla->allocate<alloc::stl::multimap< int, double >>()) alloc::stl::multimap< int, double >( fla );

        // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueEvent( new DebugListener::MessageEvent("") );

        {
            for( std::size_t i = 0; i < 10000; ++i )
            {
                int key = Rand::Int();
                double value = sqrt( double(key) );
                intmap->insert( std::pair<int,double>(key,value) );

                if( (i) % 200 == 0 )
                {
                    // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
                    eventQueue->queueEvent( new DebugListener::MessageEvent("Inserted " + to_string(i) + " pairs into IntMap on FreeListAllocator.") );
                }

            }
        }

        // clear the stringstream buffer
        stream.str( std::string() );
        stream.clear();

        // put allocator debug info into stringstream buffer
        fla->printDebugInfo( stream );

        // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueEvent( new DebugListener::MessageEvent( stream.str() ) );

        // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueEvent( new DebugListener::MessageEvent("Deallocating all memory associated with IntMap in FreeListAllocator.") );

        fla->deallocate<alloc::stl::multimap< int, double >>( intmap );
    }

    // clear the stringstream buffer
    stream.str( std::string() );
    stream.clear();

    // put allocator debug info into stringstream buffer
    fla->printDebugInfo( stream );

    // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( stream.str() ) );

    // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( "cleaning up ... fla" ) );

    defaultAllocator.deallocate<alloc::FreeListAllocator>( fla );

    // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( "cleaning up ... _mem_pool" ) );

    defaultAllocator.deallocateBlock( _mem_pool );

    // clear the stringstream buffer
    stream.str( std::string() );
    stream.clear();

    // put allocator debug info into stringstream buffer
    defaultAllocator.printDebugInfo( stream );

    // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( stream.str() ) );

    // we dont use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueEvent( new DebugListener::MessageEvent( "complete." ) );
}



#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <ctime>
#include <chrono>

#include "lib/alloc.h"
#include "lib/event_queue.h"
#include "lib/ecs.h"

#include "timer_dispatch.h"

#include "random_functor.h"

ptr::Allocator * globalAllocator;

// NOTE(dean): this is a fix for Code::Blocks MinGW in windows not having std::to_string working correctly
template < typename T >
inline
std::string
to_string( const T& n )
{
    std::ostringstream stm;
    stm << n;
    return stm.str();
}

void eventQueueStuff( evq::EventQueue * eventQueue );
void memoryStuff( evq::EventQueue * eventQueue );
void entityStuff( evq::EventQueue * eventQueue );

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
            std::chrono::system_clock::time_point broadcast_timestamp;
            std::string message;

            MessageEvent( const std::string& m )
            : evq::Event( evq::Event::Type<MessageEvent>() ), broadcast_timestamp( std::chrono::system_clock::now() ), message( std::move(m) )
            { }
        };

        DebugListener( std::ostream & o = std::cout ) : out(o) { }
        ~DebugListener() { }

        void
        processEvent( ptr::shared_ptr<evq::Event>& e )
        {
            if( auto m = std::dynamic_pointer_cast<MessageEvent>( e ) )
            {
                // pull message from given event pointer
                std::string message( std::move(m->message) );
                auto t = std::chrono::system_clock::to_time_t( m->broadcast_timestamp );
                std::string timestamp( std::move(std::ctime( &t )) ); // uses non-managed memory

                while( isspace( timestamp.back() ) ) { timestamp.pop_back(); }

                // print message
                out << timestamp << ( message.find( "\n" ) != std::string::npos ? ":\n" : ": " ) << message;

                // put newline on end of message if one is not given
                if( !message.length() || ( message.length() && '\n' != message.back() ) )
                {
                    out << std::endl;
                }
            }
        }

        // check for relevant events
        bool
        isRelevant( const ptr::shared_ptr<evq::Event>& e )
        {
            return evq::Event::isType<MessageEvent>( e );// && static_cast<const MessageEvent*>(e)->message.size();
            // TODO(dean): check if this test is actually faster
            //const MessageEvent * m;
            //return ( m = dynamic_cast<const MessageEvent*>( e ) );
        }
};

int
main( int /*argc*/, char* /*argv*/[] )
{
    try
    {
        std::ios_base::sync_with_stdio( false );

        // SafeGlobalAllocator() is a bare-bones wrapper for ::operator new() and ::operator delete()
        std::size_t _size = 1024*10; // 3K
        void * _mem = ptr::SafeGlobalAllocator().allocateBlock( _size );

        globalAllocator = new ptr::SafeFreeListAllocator( _size, _mem );
        //globalAllocator = new ptr::GlobalAllocator();// _size, _mem );

        // create our event queue
        evq::EventQueue * eventQueue = new( (evq::EventQueue*)*globalAllocator ) evq::EventQueue();

        // register our debug listener to the event queue
        eventQueue->hookListener( ptr::allocate_shared<DebugListener>( globalAllocator, std::cout ) );

        // execute some fun stuff with timers and the event queue.
        eventQueueStuff( eventQueue );

        // execute some fun stuff with memory allocators, print debug with event queue
        memoryStuff( eventQueue );

        // execute some entity component test code
        entityStuff( eventQueue );

        // make sure there are no more events to execute
        eventQueue->waitForEmpty();

        // delete the event queue to be clean
        globalAllocator->deallocate( eventQueue );

        globalAllocator->printDebugInfo();

        delete globalAllocator;

        ptr::SafeGlobalAllocator().deallocateBlock( _mem );
    }
    catch( std::exception e )
    {
        std::cerr << std::flush;
        std::cout << std::flush;
        std::cerr << e.what() << std::endl;
        std::cerr << std::flush;
        std::cout << std::flush;
    }

    // good by program.
    return EXIT_SUCCESS;
}

void
eventQueueStuff( evq::EventQueue * eventQueue )
{
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Hooking TimerSignalDispatchListener." );

    // hook a listener to the event queue so we can see the timers ticking
    eventQueue->hookListener( ptr::allocate_shared<timer_dispatch::TimerSignalDispatchListener>( globalAllocator ) );

    double timers_runtime = 10; // seconds

    // list of timers
    std::vector< timer_dispatch::Timer * > timers;

    // we make 4 timers to keep things simple
    for( std::size_t i = 1; i <= 10; ++i )
    {
        timer_dispatch::Timer * timer = new timer_dispatch::Timer( globalAllocator,
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

    //std::string s = std::string( ptr::stl_adapter<char>( globalAllocator ) );

    if( false )
    {
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

        std::basic_ostringstream< char > stream( std::string( ptr::stl_adapter<char>( globalAllocator ) ), std::ios_base::out );
        stream.str( std::string( ptr::stl_adapter<char>( globalAllocator ) ) );
        stream.clear();

        globalAllocator->printDebugInfo( stream );

        //s = std::basic_string< char, std::char_traits<char>, ptr::stl_adapter<char> >( stream.str().c_str(), ptr::stl_adapter<char>( globalAllocator ) );
        //s.assign( (const char*)stream.str().c_str() );
        //s = stream.str();

        stream.str( std::string( ptr::stl_adapter<char>( globalAllocator ) ) );
        stream.clear();
    }

    //for( auto t : timers )
    for( std::size_t i = 0; i < timers.size(); ++i )
    {
        auto t = timers.at(i);

        eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Deleting Timer_" + to_string(i) );

        delete t;
    }

    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Timers Done." );

    //eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, s );
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
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Creating DefaultAllocator." );

    // wrapper for ::operator new() and ::operator delete() that also keeps track of all allocations unlike GlobalAllocator
    ptr::SafeDefaultAllocator defaultAllocator( 0, nullptr );

    // pointer to our memory pool
    void * _mem_pool;

    // size of our memory pool: 1GB
    std::size_t _mem_size = 1024*1024*512;

    // size of our memory pool: 0.5MB
    //std::size_t _mem_size = 1024*1024*0.5f;

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Allocating Memory Pool of size " + to_string(_mem_size) + "bytes." );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Allocating FreeListAllocator, for Memory Pool." );

    // allocate a free list allocator and allocate the memory pool and pass it to the free list allocator.
    ptr::SafeFreeListAllocator * fla = new( (ptr::SafeFreeListAllocator*)defaultAllocator ) ptr::SafeFreeListAllocator( _mem_size, _mem_pool = defaultAllocator.allocateBlock(_mem_size,0) );

    {
        ptr::weak_ptr<int> test_weak_ptr_outer;
        ptr::shared_ptr<int> test_shared_ptr_outer;
        {
            //ptr::shared_ptr<int> test_shared_ptr( new( (int*)*fla ) int(1), fla );
            ptr::shared_ptr<int> test_shared_ptr( ptr::allocate_shared<int>( fla, 1 ) );

            std::cout << *test_shared_ptr << std::endl;
            test_weak_ptr_outer = test_shared_ptr;
            test_shared_ptr_outer = test_shared_ptr;
        }
        if( auto t = test_weak_ptr_outer.lock() )
        {
            std::cout << *t << std::endl;
        }

        ptr::unique_ptr<int> test_unique_ptr_outer;
        {
            ptr::unique_ptr<int> test_unique_ptr( new( (int*)*fla ) int(1000), fla );

            test_unique_ptr_outer.swap( test_unique_ptr );

            {
                ptr::unique_ptr<int[]> test_array_unique_ptr( new( fla->allocateArray<int>( 100 ) ) int[100], fla );
                for( std::size_t i = 0; i < 100; ++i )
                {
                    test_array_unique_ptr[i] = i;
                }
            }
        }
        std::cout << *test_unique_ptr_outer << std::endl;
    }

    {
        ptr::LockAllocator< ptr::LockAllocator< int > > llalt(); // FIXME(dean): this line should not compile
        ptr::LockAllocator< int > llalt2(); // FIXME(dean): this line should not compile
        //llalt.printDebugInfo();
    }

    //std::ostringstream stream; // we don't have control over this memory
    std::ostringstream stream( std::string( ptr::stl_adapter<char>( fla ) ), std::ios_base::out );

    {
        // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Allocating IntMap from FreeListAllocator." );

        // allocate a std::multimap<int,double> with our custom allocator adapters
        ptr::map< int, double > * intmap = new( (ptr::map< int, double >*)*fla ) ptr::map< int, double >( fla );

        // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "" );

        {
            // make 10000 pairs!
            for( std::size_t i = 0; i < 100000; ++i )
            {
                // insert a pair
                uint32_t key = Rand::Int( 0u, ~0u );
                double value = sqrt( double(key) );

                // here multimap will allocate space for a node that contains a pair<int,double> in it.
                //intmap->insert( std::pair<int,double>(key,value) );
                (*intmap)[key] = value;

                // print progress
                if( (i) % 1000 == 0 )
                {
                    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
                    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Inserted " + to_string(i) + " pairs into IntMap on FreeListAllocator. Last pair: " + to_string(key) + "," + to_string(value) + "." );
                }
            }
        }

        // clear the stringstream buffer
        stream.str( std::string( ptr::stl_adapter<char>(fla) ) );
        stream.clear();

        // put allocator debug info into stringstream buffer
        fla->printDebugInfo( stream );

        // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator,  std::string( stream.str().c_str() ) );

        // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
        eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Deallocating all memory associated with IntMap in FreeListAllocator." );

        // deallocate and destruct the multimap<int,double>
        fla->deallocate( intmap );
    }

    // clear the stringstream buffer
    stream.str( std::basic_string< char >( ptr::stl_adapter<char>(fla) ) );
    stream.clear();

    // put allocator debug info into stringstream buffer
    fla->printDebugInfo( stream );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator,  std::string( stream.str().c_str() ) );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator,  "cleaning up ... fla" );

    // destruct the FreeListAllocator
    defaultAllocator.deallocate( fla );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator,  "cleaning up ... _mem_pool" );

    // deallocate the memory block
    defaultAllocator.deallocateBlock( _mem_pool );

    // clear the stringstream buffer
    //stream.str( std::basic_string< char >( ptr::stl_adapter<char>(fla) ) );
    //stream.clear();

    // put allocator debug info into stringstream buffer
    defaultAllocator.printDebugInfo( stream );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator,  std::string( stream.str().c_str() ) );

    // we don't use the memory allocator with the events because we have yet to properly setup custom allocator deletion for events
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator,  "complete." );
}


class StringTestComponent : public ecs::Component
{
    public:
        ecs_ComponentID;

        std::string string;

        StringTestComponent( const std::string& str ) : string( str )
        { }
};
ecs_initComponentID( StringTestComponent );

class StringTestSystem : public ecs::System
{
    private:
        evq::EventQueue * eventQueue;
    public:
        StringTestSystem( evq::EventQueue * eq ) : ecs::System( StringTestComponent::_component_id ), eventQueue(eq) {}

        void
        processEntity( ecs::Entity * e ) override
        {
            eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, e->get<StringTestComponent>()->string );
            //std::cout <<  << std::cout;
        }
};

void
entityStuff( evq::EventQueue * eventQueue )
{
    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "Begin ECS Tests." );

    //ecs::
    StringTestSystem s( eventQueue );

    ecs::Manager manager;

    manager.addComponent( new ecs::Entity(), new StringTestComponent("StringTestComponent") );

    s.processEntities();

    eventQueue->queueNewEvent<DebugListener::MessageEvent>( globalAllocator, "End ECS Tests." );
}

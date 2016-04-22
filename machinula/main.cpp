

#include <cstdlib>

#include <iostream>
#include <sstream>

#include "lib/alloc.h"
#include "lib/event_queue.h"
#include "lib/ecs.h"

#include "timer_dispach.h"

// TODO: NOTE: this is a fix for Code::Blocks MinGW not having std::to_string working correctly
template < typename T >
std::string
to_string( const T& n )
{
    std::ostringstream stm;
    stm << n ;
    return stm.str();
}

class DebugListener : public evq::Listener
{
    public:

        struct MessageEvent : public evq::Event
        {
            std::string message;

            MessageEvent( const std::string& m )
            : evq::Event( evq::Event::Type<MessageEvent>() ), message( m )
            {  }
        };

        void
        processEvent( const evq::Event * e )
        {
            std::cout << static_cast<const MessageEvent*>(e)->message << std::endl;
        }

        bool
        isRelevant( const evq::Event * e )
        {
            return evq::Event::isType<MessageEvent>( e ) && static_cast<const MessageEvent*>(e)->message.size();
        }
};

int
main( int /*argc*/, char* /*argv*/[] )
{
    std::ios_base::sync_with_stdio( false );

    evq::EventQueue * eventQueue = new evq::EventQueue();

    eventQueue->hookListener( new DebugListener() );
    eventQueue->hookListener( new TimerSignalDispachListener() );

    for(uintmax_t i = 0; i < 1000000; ++i )
    {
        //eventQueue->queueEvent( new TimerSignalDispachListener::TimerEvent(0) );
        /*eventQueue->queueEvent( new TimerSignalDispachListener::TimerTick(0) );
        eventQueue->queueEvent( new TimerSignalDispachListener::TimerStart(0) );
        eventQueue->queueEvent( new TimerSignalDispachListener::TimerStop(0) );*/

        if(i%(1)==0) eventQueue->queueEvent( new DebugListener::MessageEvent(to_string(i)) );

    }

    std::cout << "Waiting for empty Queue ..." << std::endl;

    eventQueue->waitForEmpty();

    std::cout << "Deleting eventQueue ..." << std::endl;

    delete eventQueue;

    std::cout << "Done." << std::endl;

    return 0;
}

/*
void
memoryStuff()
{
    alloc::DefaultAllocator defaultAllocator( 0, nullptr );

    void * _mem_pool;

    std::size_t _mem_size = 1024*1024*1024;

    alloc::FreeListAllocator * fla = new(defaultAllocator.allocate<alloc::FreeListAllocator>()) alloc::FreeListAllocator( _mem_size, _mem_pool = defaultAllocator.allocateBlock(_mem_size,0) );

    {

    }


    fla->printDebugInfo( std::cout );
    std::cout << std::flush;

    std::cout << "cleaning up ... fla" << std::endl;
    defaultAllocator.deallocate<alloc::FreeListAllocator>( fla );

    std::cout << "cleaning up ... _mem_pool" << std::endl;
    defaultAllocator.deallocateBlock( _mem_pool );

    defaultAllocator.printDebugInfo( std::cout );

    std::cout << "complete." << std::endl;
}
*/



#include <cstdlib>

#include <iostream>

#include "lib/alloc.h"
#include "lib/event_queue.h"
#include "lib/ecs.h"


class TimerSignalDispachListener : public evq::Listener
{
    public:

        typedef uintmax_t TimerID;

        struct TimerEvent : public evq::Event
        {
            TimerID uniqueTimerID;

            TimerEvent( TimerID tid )
            : evq::Event( evq::Event::Type<TimerEvent>() ), uniqueTimerID( tid )
            {  }
        };

        struct TimerTick : public TimerEvent
        {
            uintmax_t tick_count;
            uintmax_t max_tick_count;
            float dt;
            float total_dt;

            TimerTick( TimerID tid )
            : TimerEvent( tid )
            {  }
        };

        struct TimerStart : public TimerEvent
        {
            float start_time;
            float length;

            TimerStart( TimerID tid )
            : TimerEvent( tid )
            {  }
        };

        struct TimerStop : public TimerEvent
        {
            float start_time;
            float length;

            TimerStop( TimerID tid )
            : TimerEvent( tid )
            {  }
        };

        void processEvent( const evq::Event * /*e*/ ) { return; }
        bool isRelevant( const evq::Event * /*e*/ ) { return false; }
};

int
main( int /*argc*/, char* /*argv*/[] )
{
    std::ios_base::sync_with_stdio( false );

    evq::EventQueue * eventQueue = new evq::EventQueue();

    eventQueue->hookListener( new TimerSignalDispachListener() );

    delete eventQueue;

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

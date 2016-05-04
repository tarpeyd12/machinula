

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
            if( evq::Event::isType<MessageEvent>( e ) && static_cast<const MessageEvent*>(e)->message.size() )
            {
                std::cout << static_cast<const MessageEvent*>(e)->message << std::endl;
            }
            else if( evq::Event::isType<timer_dispach::TimerEvent>( e ) )
            {
                if( evq::Event::isType<timer_dispach::TimerStart>( e ) )
                {
                    const timer_dispach::TimerStart * tse = static_cast<const timer_dispach::TimerStart*>(e);
                    std::cout << "TimerStart Event: " << tse->uniqueTimerID << " start: " << (unsigned long long int)(tse->start_time*1000000000.0) << " length: " << tse->length << std::endl;
                }
                else if( evq::Event::isType<timer_dispach::TimerStop>( e ) )
                {
                    const timer_dispach::TimerStop * tse = static_cast<const timer_dispach::TimerStop*>(e);
                    std::cout << "TimerStop Event: " << tse->uniqueTimerID << " start: " << (unsigned long long int)(tse->start_time*1000000000.0) << " length: " << tse->length << std::endl;
                }
                else if( evq::Event::isType<timer_dispach::TimerTick>( e ) )
                {
                    const timer_dispach::TimerTick * tte = static_cast<const timer_dispach::TimerTick*>(e);
                    std::cout << "TimerTick Event: " << tte->uniqueTimerID << " start: " << (unsigned long long int)(tte->start_time*1000000000.0) << " length: " << tte->length;
                    std::cout << " tick:" << tte->tick_count << " max_tick_count:" << tte->max_tick_count << " dt:" << tte->dt << " total_dt:" << tte->total_dt;
                    std::cout << std::endl;
                }
                else
                {
                    const timer_dispach::TimerEvent * tse = static_cast<const timer_dispach::TimerEvent*>(e);
                    std::cout << "TimerStart Event: " << tse->uniqueTimerID << std::endl;
                }
            }
        }

        bool
        isRelevant( const evq::Event * e )
        {
            return true;//evq::Event::isType<MessageEvent>( e ) && static_cast<const MessageEvent*>(e)->message.size();
        }
};

int
main( int /*argc*/, char* /*argv*/[] )
{
    std::ios_base::sync_with_stdio( false );

    evq::EventQueue * eventQueue = new evq::EventQueue();

    eventQueue->hookListener( new DebugListener() );
    eventQueue->hookListener( new timer_dispach::TimerSignalDispachListener() );

    std::vector<timer_dispach::Timer *> timers;
    for( std::size_t i = 1; i <= 4; ++i )
    {
        double dt = 1.0/double(i);
        timers.push_back( new timer_dispach::Timer( i, "TestTimer"+to_string(i-1), dt, eventQueue ) );
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }

    /*std::cout << "Waiting for empty Queue ..." << std::endl;

    eventQueue->waitForEmpty();

    std::cout << "Deleting eventQueue ..." << std::endl;*/

    for( auto t : timers )
    {
        delete t;
    }

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

#include "timer_dispatch.h"

namespace timer_dispatch
{

    Timer::Timer( TimerID utid, const std::string& n, std::size_t numticks, double timerduration, evq::EventQueue * eq  )
    : uniqueTimerID(utid), name(n), timerThread(), targetEventQueue(eq), start_time(), run_time(), time_per_tick(), total_ticks(numticks), current_tick( 0 ), infinite(false)
    {
        start_time = std::chrono::system_clock::now();

        run_time = std::chrono::duration<double>(timerduration);
        time_per_tick = run_time/total_ticks;

        timerThread = std::thread( [=]{ this->runTimer(); } );
    }

    Timer::Timer( TimerID utid, const std::string& n, double timepertick, evq::EventQueue * eq )
    : uniqueTimerID(utid), name(n), timerThread(), targetEventQueue(eq), start_time(), run_time(), time_per_tick(), total_ticks(0), current_tick( 0 ), infinite(true)
    {
        start_time = std::chrono::system_clock::now();

        run_time = std::chrono::duration<double>(0.0);
        time_per_tick = std::chrono::duration<double>(timepertick);

        timerThread = std::thread( [=]{ this->runTimer(); } );
    }

    Timer::~Timer()
    {
        timerThread.join();
    }

    void
    Timer::runTimer()
    {
        TimerStart * timer_start_event = new TimerStart( uniqueTimerID );

        timer_start_event->length     = std::chrono::duration_cast< std::chrono::duration<double> >(run_time).count();
        timer_start_event->start_time = std::chrono::duration_cast< std::chrono::duration<double> >(start_time.time_since_epoch()).count();

        targetEventQueue->queueEvent( timer_start_event );
        timer_start_event = nullptr;


        std::chrono::system_clock::time_point lastTime = std::chrono::system_clock::now();

        current_tick = 0;
        for(;;)
        {
            if( lastTime > start_time + time_per_tick*current_tick )
            {
                //std::this_thread::sleep_for( time_per_tick );
            }
            else
            {
                std::this_thread::sleep_until( start_time + time_per_tick*current_tick );
            }
            //

            std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
            std::chrono::duration<double> deltaTime = ( currentTime - lastTime );

            TimerTick * timer_tick_event = new TimerTick( uniqueTimerID );

            timer_tick_event->tick_count     = current_tick;
            timer_tick_event->max_tick_count = total_ticks;
            timer_tick_event->dt             = deltaTime.count();
            timer_tick_event->total_dt       = std::chrono::duration_cast< std::chrono::duration<double> >( std::chrono::system_clock::now() - start_time ).count();
            timer_tick_event->length         = std::chrono::duration_cast< std::chrono::duration<double> >( run_time ).count();
            timer_tick_event->start_time     = std::chrono::duration_cast< std::chrono::duration<double> >( start_time.time_since_epoch() ).count();

            targetEventQueue->queueEvent( timer_tick_event );
            timer_tick_event = nullptr;

            lastTime = currentTime;

            if( !infinite )
            {
                if( current_tick >= total_ticks && currentTime - start_time > run_time )
                {
                    break;
                }
            }
            ++current_tick;
        }

        TimerStop * timer_stop_event = new TimerStop( uniqueTimerID );

        timer_stop_event->length     = std::chrono::duration_cast< std::chrono::duration<double> >(run_time).count();
        timer_stop_event->start_time = std::chrono::duration_cast< std::chrono::duration<double> >(start_time.time_since_epoch()).count();

        targetEventQueue->queueEvent( timer_stop_event );
        timer_stop_event = nullptr;

    }

    TimerSignalDispatchListener::TimerSignalDispatchListener()
    {

    }

    TimerSignalDispatchListener::~TimerSignalDispatchListener()
    {

    }

    void
    TimerSignalDispatchListener::processEvent( const evq::Event * e )
    {

        const TimerEvent * tev = static_cast<const TimerEvent*>( e );

        if( evq::Event::isType<TimerStart>( e ) )
        {
            const TimerStart * tse = static_cast<const TimerStart*>( e );
            std::cout << "TimerStart Event: " << tse->uniqueTimerID << " start: " << (unsigned long long int)(tse->start_time*1000000000.0) << " length: " << tse->length << std::endl;
        }
        else if( evq::Event::isType<TimerStop>( e ) )
        {
            const TimerStop * tse = static_cast<const TimerStop*>( e );
            std::cout << "TimerStop Event: " << tse->uniqueTimerID << " start: " << (unsigned long long int)(tse->start_time*1000000000.0) << " length: " << tse->length << std::endl;
        }
        else if( evq::Event::isType<TimerTick>( e ) )
        {
            const TimerTick * tte = static_cast<const TimerTick*>( e );
            std::cout << "TimerTick Event: " << tte->uniqueTimerID << " start: " << (unsigned long long int)(tte->start_time*1000000000.0) << " length: " << tte->length;
            std::cout << " tick:" << tte->tick_count << " max_tick_count:" << tte->max_tick_count << " dt:" << tte->dt << " total_dt:" << tte->total_dt;
            std::cout << std::endl;
        }
        else
        {
            std::cout << "TimerEvent: " << tev->uniqueTimerID << std::endl;
        }
    }

    bool
    TimerSignalDispatchListener::isRelevant( const evq::Event * e )
    {
        return evq::Event::isType<TimerEvent>( e );
    }

}

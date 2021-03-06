#ifndef TIMER_DISPATCH_H_INCLUDED
#define TIMER_DISPATCH_H_INCLUDED

#include <string>
#include <thread>
#include <chrono>

#include "lib/event_queue.h"

namespace timer_dispatch
{
    typedef uintmax_t TimerID;

    struct Timer
    {
        TimerID uniqueTimerID;
        std::string name;
        std::thread timerThread;

        evq::EventQueue * targetEventQueue;

        std::chrono::system_clock::time_point start_time;
        std::chrono::duration<double> run_time;
        std::chrono::duration<double> time_per_tick;
        std::size_t total_ticks;
        std::size_t current_tick;
        bool infinite;

        Timer( TimerID utid, const std::string& n, std::size_t numticks, double duration, evq::EventQueue * eq );
        Timer( TimerID utid, const std::string& n, double timepertick, evq::EventQueue * eq );
        Timer( const Timer& other ) = delete;
        ~Timer();

        Timer& operator=( const Timer& other ) = default;

        void runTimer();
    };

    struct TimerEvent : public evq::Event
    {
        TimerID uniqueTimerID;

        TimerEvent( TimerID tid )
        : evq::Event( evq::Event::Type<TimerEvent>() ), uniqueTimerID( tid )
        {  }
    };

    struct TimerTick : public TimerEvent
    {
        std::size_t tick_count;
        std::size_t max_tick_count;
        double dt;
        double total_dt;
        double start_time;
        double length;

        TimerTick( TimerID tid )
        : TimerEvent( tid ),
        tick_count(0), max_tick_count(0), dt(0.0), total_dt(0.0), start_time(-1.0), length(0.0)
        { DeriveEventType<TimerTick>(); }
    };

    struct TimerStart : public TimerEvent
    {
        double start_time;
        double length;

        TimerStart( TimerID tid )
        : TimerEvent( tid ),
        start_time(-1.0), length(0.0)
        { DeriveEventType<TimerStart>(); }
    };

    struct TimerStop : public TimerEvent
    {
        double start_time;
        double length;

        TimerStop( TimerID tid )
        : TimerEvent( tid ),
        start_time(-1.0), length(0.0)
        { DeriveEventType<TimerStop>(); }
    };

    class TimerSignalDispatchListener : public evq::Listener
    {
        private:

            //TimerID nextTimerID;

        public:

            TimerSignalDispatchListener() = default;
            virtual ~TimerSignalDispatchListener() = default;

            void processEvent( evq::Event * e );
            bool isRelevant( const evq::Event * e );
    };
}

#endif // TIMER_DISPATCH_H_INCLUDED

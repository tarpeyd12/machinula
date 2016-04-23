#ifndef TIMER_DISPACH_H_INCLUDED
#define TIMER_DISPACH_H_INCLUDED

#include <string>
#include <thread>
#include <chrono>

#include "lib/event_queue.h"

namespace timer_dispach
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
        ~Timer();
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
        : TimerEvent( tid )
        { DeriveEventType<TimerTick>(); }
    };

    struct TimerStart : public TimerEvent
    {
        double start_time;
        double length;

        TimerStart( TimerID tid )
        : TimerEvent( tid )
        { DeriveEventType<TimerStart>(); }
    };

    struct TimerStop : public TimerEvent
    {
        double start_time;
        double length;

        TimerStop( TimerID tid )
        : TimerEvent( tid )
        { DeriveEventType<TimerStop>(); }
    };

    class TimerSignalDispachListener : public evq::Listener
    {
        private:

            TimerID nextTimerID;

        public:

            void processEvent( const evq::Event * /*e*/ ) { return; }
            bool isRelevant( const evq::Event * /*e*/ ) { return false; }
    };
}

#endif // TIMER_DISPACH_H_INCLUDED

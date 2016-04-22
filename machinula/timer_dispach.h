#ifndef TIMER_DISPACH_H_INCLUDED
#define TIMER_DISPACH_H_INCLUDED

class TimerSignalDispachListener : public evq::Listener
{
    public:

        typedef uintmax_t TimerID;

        //evq::EventQueue *

    private:

        TimerID nextTimerID;

    public:

        struct Timer
        {
            TimerID uniqueTimerID;
            std::string timerName;
            std::thread timerThread;

            double dt;
            double total_dt;
            double start_time;
            double length;

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

        void processEvent( const evq::Event * /*e*/ ) { return; }
        bool isRelevant( const evq::Event * /*e*/ ) { return false; }
};

#endif // TIMER_DISPACH_H_INCLUDED

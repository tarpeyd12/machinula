
#ifndef EVENT_QUEUE_INL_INCLUDED
#define EVENT_QUEUE_INL_INCLUDED

namespace evq
{
    inline
    EventType
    Event::eventType() const
    {
        return _event_type;
    }

    template < typename T >
    inline
    EventType
    Event::Type()
    {
        static_assert( std::is_base_of< Event, T >::value, "evq::Event::Type<T>: T is not derived from evq::Event.");
        return primeid::type<T>();
    }

    template < typename T >
    inline
    EventType
    Event::DeriveEventType()
    {
        // the template parameter T should be of the derived class in the derived classes constructor. ex:
        // class A : public Event { A() : Event(Event::Type<A>()) {} }; class B : public A { B(){ DeriveEventType<B>(); } };
        static_assert( std::is_base_of< Event, T >::value, "evq::Event::DeriveEventType<T>: T is not derived from evq::Event." );
        return _event_type = primeid::getID< T >( _event_type );
    }

    template < typename T >
    inline
    bool
    Event::isType( const Event * e )
    {
        static_assert( std::is_base_of< Event, T >::value, "evq::Event::isType<T>: T is not derived from evq::Event." );
        return e->eventType() % Type< T >() == 0;
        // TODO(dean): check if this test is actually faster
        /*const T * ev;
        return ( ev = dynamic_cast< const T * >(e) );*/

    }


    inline
    EventQueue *
    Listener::parentQueue() const
    {
        return _parent_queue;
    }

    inline
    void
    Listener::broadcastEvent( Event * e )
    {
        parentQueue()->queueEvent(e);
    }
}

#endif // EVENT_QUEUE_INL_INCLUDED

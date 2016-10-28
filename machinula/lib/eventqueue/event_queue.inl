
#ifndef EVENT_QUEUE_INL_INCLUDED
#define EVENT_QUEUE_INL_INCLUDED

namespace evq
{
    EventType
    Event::eventType() const
    {
        return _event_type;
    }

    template < typename T >
    EventType
    Event::Type()
    {
        static_assert( std::is_base_of< Event, T >::value, "evq::Event::Type<T>: T is not derived from evq::Event.");
        return primeid::type<T>();
    }

    template < typename T >
    EventType
    Event::DeriveEventType()
    {
        // the template parameter T should be of the derived class in the derived classes constructor. ex:
        // class A : public Event { A() : Event(Event::Type<A>()) {} }; class B : public A { B(){ DeriveEventType<B>(); } };
        static_assert( std::is_base_of< Event, T >::value, "evq::Event::DeriveEventType<T>: T is not derived from evq::Event." );
        return _event_type = primeid::getID< T >( _event_type );
    }

    template < typename T >
    bool
    Event::isType( const Event * e )
    {
        static_assert( std::is_base_of< Event, T >::value, "evq::Event::isType<T>: T is not derived from evq::Event." );
        return e->eventType() % Type< T >() == 0;
        // TODO(dean): check if this test is actually faster
        /*const T * ev;
        return ( ev = dynamic_cast< const T * >(e) );*/
    }

    template < typename T >
    bool
    Event::isType( ptr::shared_ptr<Event> e )
    {
        static_assert( std::is_base_of< Event, T >::value, "evq::Event::isType<T>: T is not derived from evq::Event." );
        return e->eventType() % Type< T >() == 0;
        // TODO(dean): check if this test is actually faster
        /*const T * ev;
        return ( ev = dynamic_cast< const T * >(e) );*/
    }

    template < typename T, typename ... ArgTypes >
    std::size_t
    EventQueue::queueNewEvent( ptr::Allocator * alloc, ArgTypes && ... args )
    {
        static_assert( std::is_base_of< Event, T >::value, "evq::Event::isType<T>: T is not derived from evq::Event." );
        assert( alloc != nullptr );
        return queueEvent( ptr::allocate_shared< T >( alloc, args... ) );
        //return queueEvent( ptr::shared_ptr< T >( new( alloc->allocate<T>() ) T( args... ), alloc ) );
    }

    inline
    EventQueue *
    Listener::parentQueue() const
    {
        return _parent_queue;
    }

    inline
    void
    Listener::broadcastEvent( ptr::shared_ptr<Event>& e )
    {
        parentQueue()->queueEvent(e);
    }
}

#endif // EVENT_QUEUE_INL_INCLUDED

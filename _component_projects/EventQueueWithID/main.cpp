#include <iostream>

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <fstream>
#include <sstream>

#include "event_queue.h"

#include "primeID.h"

class characterEvent : public EventQueue::Event
{
    public:
        uint32_t character;
        characterEvent( uint32_t c ) : EventQueue::Event( EventQueue::Event::Type<characterEvent>() ), character( c ) {  }
};

class printEvent : public EventQueue::Event
{
    public:
        uint32_t character;
        printEvent( uint32_t c ) : EventQueue::Event( EventQueue::Event::Type<printEvent>() ), character( c ) {  }
};

class waitEvent : public EventQueue::Event
{
    public:
        uint32_t useconds;
        waitEvent( uint32_t us ) : EventQueue::Event( EventQueue::Event::Type<waitEvent>() ), useconds( us ) {  }
};

class myListener : public EventQueue::Listener
{
        uint32_t lastCharacter;
        uint32_t _state;
    public:
        myListener()
        : lastCharacter('\0'), _state(0)
        {  }

        void
        processEvent( const EventQueue::Event * e ) override
        {
            assert( e != nullptr );

            if( e->eventType() == EventQueue::Event::Type<characterEvent>() )
            {
                const characterEvent * event = static_cast<const characterEvent*>( e );

                float usec = (float(rand())/float(RAND_MAX));
                usec *= (float(rand())/float(RAND_MAX));
                usec *= (float(rand())/float(RAND_MAX));
                usec *= 5;

                uint32_t character = event->character;

                if( character == '#' )
                {
                    _state = 2;
                }
                if( character == '\n' || character == '\r' )
                {
                    if( !_state )
                        character = ' ';
                    else
                        --_state;
                }

                bool isSpace( character == ' ' );
                if( !isSpace || lastCharacter != ' ' )
                    parentQueue()->queueEvent( new printEvent( character ) );
                if( !isSpace )
                    parentQueue()->queueEvent( new waitEvent( int(usec) ) );

                lastCharacter = character;
            }
            else
            if( e->eventType() == EventQueue::Event::Type<printEvent>() )
            {
                const printEvent * event = static_cast<const printEvent*>( e );
                std::cout << char( event->character ) << std::flush;
            }
            else
            if( e->eventType() == EventQueue::Event::Type<waitEvent>() )
            {
                const waitEvent * event = static_cast<const waitEvent*>( e );
                //Sleep( event->useconds );
            }
            else
            {

            }
        }

        bool
        isRelevant( const EventQueue::Event * e ) override
        {
            if( e->eventType() == EventQueue::Event::Type<characterEvent>() ) return true;
            if( e->eventType() == EventQueue::Event::Type<printEvent>() ) return true;
            if( e->eventType() == EventQueue::Event::Type<waitEvent>() ) return true;
            return false;
        }
};

class countListener : public EventQueue::Listener
{
    public:

        unsigned characterEventCount;
        unsigned printEventCount;
        unsigned waitEventCount;

        countListener()
        : characterEventCount(0), printEventCount(0), waitEventCount(0)
        {

        }

        ~countListener()
        {
            std::cout << "characterEventCount = " << characterEventCount << std::endl;
            std::cout << "    printEventCount = " << printEventCount << std::endl;
            std::cout << "     waitEventCount = " << waitEventCount << std::endl;
        }

        void
        processEvent( const EventQueue::Event * e ) override
        {
            assert( e != nullptr );
            if( e->eventType() == EventQueue::Event::Type<characterEvent>() ) ++characterEventCount;
            if( e->eventType() == EventQueue::Event::Type<printEvent>() ) ++printEventCount;
            if( e->eventType() == EventQueue::Event::Type<waitEvent>() ) ++waitEventCount;
        }

        bool
        isRelevant( const EventQueue::Event * e ) override
        {
            return true;
        }
};

class heavyListener : public EventQueue::Listener
{
    uint64_t n;
    public:
        heavyListener()
        { n = 3; }

        ~heavyListener()
        {  }

        uint64_t
        a( uint64_t p )
        {
            return sqrt(p);
        }

        void
        processEvent( const EventQueue::Event * e ) override
        {
            assert( e != nullptr );

            for( uint32_t i = 0; i < 10; ++i )
            {
                n = a(n*n*n);
            }
            //std::cout << n << std::endl;
        }

        bool
        isRelevant( const EventQueue::Event * e ) override
        {
            return true;
        }
};

int main( int argc, char ** argv )
{
    std::ios_base::sync_with_stdio( false );

    if( argc < 2 )
    {
        std::cout << "Please add a second argument as the input file path." << std::endl;
        return 0;
    }

    EventQueue::EventQueue * event_queue = new EventQueue::EventQueue( 1 );
    myListener * l = new myListener();
    event_queue->hookListener( l );
    countListener * l2 = new countListener();
    event_queue->hookListener( l2 );
    //heavyListener * l3 = new heavyListener();
    //event_queue->hookListener( l3 );

/*
    if( 0 )
    {
        std::string outstring = "namespace primeid\n{\n\tint __primeNumberData[] =\n\t{\n\t\t";
        {
            std::fstream file( "primes.num", std::fstream::in | std::fstream::binary );

            file.seekg( 0, file.end );
            int length = file.tellg();
            file.seekg( 0, file.beg );

            std::vector<char> dataout;
            dataout.resize( length );

            file.read( &dataout[0], length );

            file.close();

            int * pdataout = reinterpret_cast<int*>( dataout.data() );

            std::vector<int> primes( pdataout, pdataout + dataout.size()/sizeof(int) );

            //int l = primes.size();
            while( primes.size() > 1024 ) primes.pop_back();

            for( uintmax_t i = 0; i < primes.size(); ++i )
            {
                std::stringstream stream;

                stream << "0x" << std::hex << primes[i] << ", ";
                if( (i+1) % 8 == 0 )
                {
                    stream << "\n\t\t";
                }

                outstring += stream.str();
            }
        }
        outstring += "0x0\n\t};\n}\n";

        std::fstream ofile( "primenumbers.h", std::fstream::out | std::fstream::trunc );

        ofile << outstring;

        ofile.close();
    }

*/
    std::fstream file( argv[1], std::fstream::in | std::fstream::binary );

    event_queue->queueEvent( new waitEvent( 0 ) );

    while( file.good() )
    {
        event_queue->queueEvent( new characterEvent( file.get() ) );
    }
    file.close();

    event_queue->queueEvent( new characterEvent( '\a' ) );

    //std::cout << "Completed Character Queueing." << std::endl;

    event_queue->waitForEmpty();

    std::cout << std::endl;

    delete event_queue;

    delete l;
    delete l2;
    //delete l3;

    return 0;
}

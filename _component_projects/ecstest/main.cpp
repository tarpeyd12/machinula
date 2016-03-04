#include <iostream>
#include <typeinfo>
#include "ecs.h"

template< int I = 0 >
struct testcomponent : public ecs::Component
{
    ecs_ComponentID;
    static int p;
    int a;
    testcomponent()
    : a(++p)
    { }
};
template< int I > ecs_initComponentID(testcomponent<I>);
template< int I > int testcomponent<I>::p = 0;

class TestSystem : public ecs::System
{
    public:
        TestSystem( const std::vector<ecs::ComponentID> &requiredComponentTypes )
        : System( requiredComponentTypes )
        { }

        inline
        void
        processEntity( ecs::Entity *e )
        {
            e->get<testcomponent<1>>()->a++;
            e->get<testcomponent<2>>()->a++;
            e->get<testcomponent<3>>()->a++;
            e->get<testcomponent<4>>()->a++;
            e->get<testcomponent<5>>()->a++;
        }

};

int main()
{
    std::cout << "Hello world!" << std::endl;

    ecs::Manager entityManager;

    for(int i = 0; i < 750000; ++i)
    {
        ecs::Entity * e = new ecs::Entity();

        if(i%1==0) entityManager.addComponent( e, new testcomponent<1>() );
        if(i%2==0) entityManager.addComponent( e, new testcomponent<2>() );
        if(i%3==0) entityManager.addComponent( e, new testcomponent<3>() );
        if(i%4==0) entityManager.addComponent( e, new testcomponent<4>() );
        if(i%5==0) entityManager.addComponent( e, new testcomponent<5>() );
    }
    std::cout << "Init done." << std::endl;

    std::vector< ecs::Entity * > entities;

    std::vector< ecs::ComponentID > desiredComponents;
    desiredComponents.push_back( testcomponent<1>::_component_id );
    desiredComponents.push_back( testcomponent<2>::_component_id );
    desiredComponents.push_back( testcomponent<3>::_component_id );
    desiredComponents.push_back( testcomponent<4>::_component_id );
    desiredComponents.push_back( testcomponent<5>::_component_id );

    TestSystem testSystem(desiredComponents);

    for( int i = 0; i < 1000; ++i )
    //for( int i = 0; i < 10; ++i )
    {
        std::cout << i << std::endl;
        //entitySystem.getEntities<testcomponent7>( entities );

        testSystem.processEntities();

        /*
        std::cout << "ResultEntityCount=" << entities.size() << "\n";

        for( auto e : entities )
        {
            e->get<testcomponent<1>>()->a++;
            e->get<testcomponent<2>>()->a++;
            e->get<testcomponent<3>>()->a++;
            e->get<testcomponent<4>>()->a++;
            e->get<testcomponent<5>>()->a++;
        }

        /*entities.clear();

        entitySystem.getEntities<testcomponent2>( entities );

        for( auto e : entities )
        {
            e->getAs<testcomponent2>()->a++;
        }*/
    }

    return 0;
}

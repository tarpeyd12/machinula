#ifndef ECS_H_INCLUDED
#define ECS_H_INCLUDED

#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cassert>

/* pulled from :
 *   http://entity-systems.wikidot.com/test-for-parallel-processing-of-components#cpp
 */

#define ecs_ComponentID static ecs::ComponentID _component_id;
#define ecs_initComponentID(a) ecs::ComponentID a::_component_id = typeid(a).hash_code();

namespace ecs
{
    typedef std::size_t ComponentID;
    struct Manager;

    struct Component
    {

    };

    struct Entity
    {
        Entity();

        template< typename ComponentType > ComponentType *get();
        std::unordered_map< ComponentID, Component* > mComponents; // is actually a hashmap, so it uses some extra memory :/
    };

    struct Manager
    {
        protected:
            static std::multimap< ComponentID, Entity* > componentStore;
        public:

            Manager();

            template < typename ComponentType >
            inline void addComponent( Entity * e, ComponentType * comp );

            template < typename ComponentType >
            inline ComponentType * getComponent( Entity * e ) const;

            template < typename ComponentType >
            inline void getEntities( std::vector< Entity* > &result ) const;

            inline void getEntities( const std::vector<ComponentID> &componentTypes, std::vector< Entity* > &result ) const;

        private:
            inline void _process_get_entities_horizontal( const std::vector< std::pair<std::size_t,ComponentID> > & componentLikelyhood, std::vector< Entity* > &result ) const;
            inline void _process_get_entities_verticle( const std::vector< std::pair<std::size_t,ComponentID> > & componentLikelyhood, std::vector< Entity* > &result ) const;
    };

    std::multimap< ComponentID, Entity* > Manager::componentStore;

    static Manager * manager;// There should only be one entity manager in a game, and this is it

    class System
    {
        protected:
            std::vector<ComponentID> requiredComponentTypes;

        public:
            System( const std::vector<ComponentID> &requiredComponentTypes );

            inline void getReleventEntities( std::vector< Entity* > &result );

            inline void processEntities();

        protected:
            inline virtual void processEntity( Entity *e ) = 0;
    };
}


#include "ecs.inl"

#endif // ECS_H_INCLUDED

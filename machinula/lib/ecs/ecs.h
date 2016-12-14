#ifndef ECS_H_INCLUDED
#define ECS_H_INCLUDED

#include <algorithm>
#include <cassert>

// TODO(dean): Rewrite to use the allocator system i developed

/* pulled and adapted from :
 *   http://entity-systems.wikidot.com/test-for-parallel-processing-of-components#cpp
 */

// TODO(dean): try std::type_index which hopefully can be made const. http://en.cppreference.com/w/cpp/types/type_index
#define ecs_ComponentID      static ecs::ComponentID    _component_id;
#define ecs_initComponentID(a)      ecs::ComponentID a::_component_id = typeid(a).hash_code();

namespace ecs
{
    typedef std::size_t ComponentID; // TODO(dean): try std::type_index which hopefully can be made const. http://en.cppreference.com/w/cpp/types/type_index
    struct Manager;

    struct Component
    {

    };

    struct Entity
    {
        std::unordered_map< ComponentID, Component* > mComponents; // is actually a hashmap, so it uses some extra memory :/

        Entity();

        template< typename ComponentType > ComponentType *get();
        // TODO(dean): Add addComponent here
        // TODO(dean): Add removeComponent here
    };

    struct Manager
    {
        protected:
            static std::multimap< ComponentID, Entity* > componentStore; // TODO(dean): make non-static, so each manager can exist on its own.
        public:

            Manager();
            ~Manager();

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

    // TODO(dean): remove the global-ness of the manager
    //static Manager * manager;// There should only be one entity manager in a game, and this is it

    class System
    {
        private:
            Manager * manager;
        protected:
            std::vector<ComponentID> requiredComponentTypes;

        public:
            System( Manager * _manager, const std::vector<ComponentID> &_requiredComponentTypes );
            System( Manager * _manager, ComponentID _requiredComponentType );
            virtual ~System();

            inline void getReleventEntities( std::vector< Entity* > &result );

            inline void processEntities();

        protected:
            virtual void processEntity( Entity * e ) = 0;
    };
}


#include "ecs.inl"

#endif // ECS_H_INCLUDED

#ifndef ECS_INL_INCLUDED
#define ECS_INL_INCLUDED

namespace ecs
{
    Entity::Entity() //: mComponents(32)
    {

    }

    template< typename ComponentType >
    inline
    ComponentType *
    Entity::getAs()
    {
       return (ComponentType*)mComponents[ ComponentType::_component_id ];
    }


    EntityManager::EntityManager()
    {

    }

    template < typename ComponentType >
    inline
    void
    EntityManager::addComponent( Entity * e, ComponentType * comp )
    {
        componentStore.insert( std::pair< ComponentID, Entity* >( ComponentType::_component_id, e ) );
        e->mComponents.insert( std::pair< ComponentID, Component* >( ComponentType::_component_id, comp ) );
    }

    template < typename ComponentType >
    inline
    ComponentType *
    EntityManager::getComponent( Entity * e ) const
    {
        return (ComponentType*)e->mComponents[ ComponentType::_component_id ];
    }

    template < typename ComponentType >
    inline
    void
    EntityManager::getEntities( std::vector< Entity* > &result ) const
    {
        auto iterPair = componentStore.equal_range( ComponentType::_component_id );
        for( auto iter = iterPair.first; iter != iterPair.second; ++iter )
        {
            result.push_back( iter->second );
        }
    }

    inline
    void
    EntityManager::getEntities( const std::vector<ComponentID> &componentTypes, std::vector< Entity* > &result ) const
    {
        if( !componentTypes.size() )
            return;

        // don't waste time with the multiple component code if you only have one
        if( componentTypes.size() == 1 )
        {
            auto iterPair = componentStore.equal_range( componentTypes.back() );
            for( auto iter = iterPair.first; iter != iterPair.second; ++iter )
            {
                result.push_back( iter->second );
            }
            return;
        }

        std::vector< std::pair<std::size_t,ComponentID> > componentLikelyhood;

        for( ComponentID id : componentTypes )
        {
            // allow zero terminated stuff
            //if( !id ) break;

            auto iterPair = componentStore.equal_range( id );
            std::size_t len = std::distance( iterPair.first, iterPair.second );

            // if there are no entities with at least one of the given components, then there will be no entities returned.
            if( !len ) return;

            componentLikelyhood.push_back( std::pair<std::size_t,ComponentID>( len, id ) );
        }

        std::sort( componentLikelyhood.begin(), componentLikelyhood.end(), std::less< std::pair<std::size_t,ComponentID> >() );

        _process_get_entities_horizontal( componentLikelyhood, result );
        //_process_get_entities_verticle( componentLikelyhood, result );
    }

    inline
    void
    EntityManager::_process_get_entities_horizontal( const std::vector< std::pair<std::size_t,ComponentID> > & componentLikelyhood, std::vector< Entity* > &result ) const
    {
        // this section is faster for small sets of components on a single thread
        std::size_t num_components = componentLikelyhood.size();

        auto iterPair = componentStore.equal_range( componentLikelyhood.front().second );

        for( auto iter = iterPair.first; iter != iterPair.second; ++iter )
        {
            Entity * e = iter->second;
            auto & components = e->mComponents;
            auto eend = components.end();
            for( std::size_t i = 1; i < num_components; ++i )
            {
                if( components.find( componentLikelyhood[i].second ) == eend )
                {
                    e = nullptr;
                    break;
                }
            }

            if( e )
                result.push_back( e );
        }
    }

    inline
    void
    EntityManager::_process_get_entities_verticle( const std::vector< std::pair<std::size_t,ComponentID> > & componentLikelyhood, std::vector< Entity* > &result ) const
    {
        // this section is faster for large sets of components on MULTIPLE THREADS
        auto iterPair = componentStore.equal_range( componentLikelyhood.front().second );

        std::size_t numents = std::distance( iterPair.first, iterPair.second );

        std::vector< Entity * > temp_entities( 0 );
        std::vector< bool > doadd_entities( 0 );

        for( auto iter = iterPair.first; iter != iterPair.second; ++iter )
        {
            temp_entities.push_back( iter->second );
            doadd_entities.push_back( true );
        }

        assert( temp_entities.size() == doadd_entities.size() && temp_entities.size() == numents );

        // this for loop can be multi-threaded
        for( std::size_t i = 1; i < componentLikelyhood.size()-1; ++i )
        {
            ComponentID id = componentLikelyhood[i].second;
            for( std::size_t e = 0; e < numents; ++e )
            {
                if( !doadd_entities[e] ) continue;

                if( temp_entities[e]->mComponents.find( id ) == temp_entities[e]->mComponents.end() )
                    doadd_entities[e] = false;
            }
        }

        ComponentID id = componentLikelyhood.back().second;
        for( std::size_t e = 0; e < numents; ++e )
        {
            if( !doadd_entities[e] ) continue;

            if( temp_entities[e]->mComponents.find( id ) != temp_entities[e]->mComponents.end() )
                result.push_back( temp_entities[e] );
        }
    }
}

#endif // ECS_INL_INCLUDED

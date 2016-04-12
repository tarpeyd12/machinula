#ifndef LIB_ECS_H_INCLUDED
#define LIB_ECS_H_INCLUDED

#include "ecs/ecs.h"

#endif // LIB_ECS_H_INCLUDED

#ifndef LIB_TRY_LOCK_H_INCLUDED

class IDMutexStore
{
    private:
        std::unordered_map< ecs::ComponentID, std::mutex > id_mutexes;

    public:

        bool lockAllOrNone( const std::vector<ecs::ComponentID>& );
};

#define LIB_TRY_LOCK_H_INCLUDED

#endif // LIB_TRY_LOCK_H_INCLUDED

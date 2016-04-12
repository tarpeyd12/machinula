#include "try_lock.h"

class IDMutexStore
{
    private:
        std::unordered_map< ecs::ComponentID, std::mutex > id_mutexes;

    public:

        bool lockAllOrNone( const std::vector<ecs::ComponentID>& input_vector) //Can change this name later
        {
            std::vector<ecs::ComponentID> id_vector = input_vector;
            std::sort(id_vector.begin(), id_vector.end());
            std::vector<std::mutex*> mutex_location;
            for ( auto a : id_vector)
            {
                mutex_location.push_back(&id_mutexes[a]);
            }

            int fail_iter = 0;
            bool fail = false;
            for ( auto a : mutex_location)
            {
                if(std::try_lock(a*)!=-1)
                {
                    fail = true;
                    break;
                }
                fail_iter++;
            }

            if(fail)
            {
                for ( auto a : mutex_location)
                {
                    if(fail_iter-- = 0)
                        break;
                    std::unlock(a*);
                }
                return false;
            }
            return true;
        }
};

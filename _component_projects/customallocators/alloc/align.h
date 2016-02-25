#ifndef ALIGN_H_INCLUDED
#define ALIGN_H_INCLUDED

namespace alloc
{
    namespace align
    {
        inline
        uint8_t
        forwardAdj( const void * address, uint8_t alignment )
        {
            uint8_t adjust = alignment - ( reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment-1) );
            if( adjust == alignment )
                return 0;
            return adjust;
        }
    }
}

#endif // ALIGN_H_INCLUDED

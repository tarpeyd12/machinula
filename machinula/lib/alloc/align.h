#ifndef ALIGN_H_INCLUDED
#define ALIGN_H_INCLUDED

namespace alloc
{
    namespace align
    {
        inline
        void *
        _add( void * address, std::size_t n )
        {
            return (void*)( reinterpret_cast<uintptr_t>(address) + n );
        }

        inline
        const void *
        _add( const void * address, std::size_t n )
        {
            return (const void*)( reinterpret_cast<uintptr_t>(address) + n );
        }

        inline
        void *
        _sub( void * address, std::size_t n )
        {
            return (void*)( reinterpret_cast<uintptr_t>(address) - n );
        }

        inline
        const void *
        _sub( const void * address, std::size_t n )
        {
            return (const void*)( reinterpret_cast<uintptr_t>(address) - n );
        }

        inline
        uint8_t
        forwardAdj( const void * address, uint8_t alignment )
        {
            uint8_t adjust = alignment - ( reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment-1) );
            if( adjust == alignment )
                return 0;
            return adjust;
        }

        inline
        uint8_t
        forwardAdj_withHeader( const void * address, uint8_t align, uint8_t header_size )
        {
            // TODO: make sure without a doubt that this line is comparable to the ones commented out
            //return forwardAdj( address + header_size, align );

            uint8_t adjustment = forwardAdj( address, align );

            uint8_t bytes_needed = header_size;

            if( adjustment < bytes_needed )
            {
                bytes_needed -= adjustment;

                // make the adjustment include the header size with weird math
                adjustment += align * ( bytes_needed / align );

                if( bytes_needed % align )
                    adjustment += align;
            }

            return adjustment;
        }
    }
}

#endif // ALIGN_H_INCLUDED

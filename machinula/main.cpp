
#include <cstdlib>

#include <iostream>

#include "simple_test.h"

int
main( int /*argc*/, char* /*argv*/[] )
{
    std::ios_base::sync_with_stdio( false );

    Simple_Test::test_all();

    return EXIT_SUCCESS;
}

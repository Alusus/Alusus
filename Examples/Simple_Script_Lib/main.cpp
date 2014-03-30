/**
 * @file main.cpp
 * Contains the program's entry point.
 *
 * @author Sarmad Khalid Abdulla
 *
 * @date 2013-01-03 : Created.
 */
////////////////////////////////////////////////////////////////////////////////

#include "main.h"

using namespace Simple_Script;

__attribute__((constructor)) void constructor()
{
}


__attribute__((destructor)) void destructor()
{
}


DL_EXPORTED LIBRARY_GATEWAY_GETTER_DEF
{
    static Library_Gateway library_gateway;
    return &library_gateway;
}

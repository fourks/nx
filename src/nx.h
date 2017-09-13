//----------------------------------------------------------------------------------------------------------------------
// NX system
// Launches other subsystems based command line arguments, and manages memory.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "machine.h"

typedef struct
{
    Machine     machine;
}
Nx;

//----------------------------------------------------------------------------------------------------------------------
// API to platform layer
//----------------------------------------------------------------------------------------------------------------------

// Initialise the NX system by providing callback functions for platform-specific memory and IO operations.
bool nxOpen(Nx* N);

// Close down the NX system
void nxClose(Nx* N);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#ifdef NX_IMPL

//----------------------------------------------------------------------------------------------------------------------
// Public API
//----------------------------------------------------------------------------------------------------------------------

bool nxOpen(Nx* N)
{
    K_ASSERT(N);

    return machineOpen(&N->machine);
}

void nxClose(Nx* N)
{
    K_ASSERT(N);
    machineClose(&N->machine);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

#endif // NX_IMPL

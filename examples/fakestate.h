#pragma once
#include <penumbra/penumbra.h>
#include "fakestate_export.h"

struct FAKESTATE_EXPORT fakestate {
    std::unique_ptr<Pumbra::Penumbra> pumbra = nullptr;

    fakestate();
};


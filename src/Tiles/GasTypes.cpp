#pragma once

#include "Other/Types.cpp"

enum GasType {
    GAS_EMPTY,
    GAS_SOLID,
    GAS_OXYGEN,
    GAS_CARBON_DIOXIDE
};

const float defaultGasMass[] = {
    0,
    0,
    3000,
    3000
};

cstr gasNames[] = {
    "Void",
    "Void",
    "Oxygen",
    "Carbon Dioxide"
};
#pragma once

#include <vector>
#include "raylib.h"
#include "CubismFramework.hpp"
#include "Math/CubismMatrix44.hpp"
#include "Model/CubismModel.hpp"

struct CubismClippingContext
{
    CubismClippingContext(const int* clippingDrawableIndices, int clipCount);

    bool IsSameClipping(const int* drawableMasks, int drawableMaskCounts) const;

    void UpdateBounds(Live2D::Cubism::Framework::CubismModel* model);

    bool active = false;
    int layoutChannel = 0;
    std::vector<int> clippings;
    std::vector<int> drawables;
    Rectangle clipBound = { 0 };       // region contains all clipped drawables
    Rectangle layoutRegion = { 0 };    // region in channel
    Matrix maskMatrix;
    Matrix drawMatrix;
};

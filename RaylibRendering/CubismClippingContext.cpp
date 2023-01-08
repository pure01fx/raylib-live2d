#include "CubismClippingContext.hpp"
#include "CubismFramework.hpp"
#include <utility>

using namespace Live2D::Cubism::Framework;

CubismClippingContext::CubismClippingContext(const int* clippingDrawableIndices, int clipCount)
{
	clippings.assign(clippingDrawableIndices, clippingDrawableIndices + clipCount);
}

bool CubismClippingContext::IsSameClipping(const int* drawableMasks, int drawableMaskCounts) const
{
	if (clippings.size() != drawableMaskCounts) return false;

	for (int i = 0; i < drawableMaskCounts; i++)
	{
		const auto clipId = drawableMasks[i];

		for (const auto i : clippings)
		{
			if (i == clipId)
			{
				goto NEXT;
			}
		}
		return false;
	NEXT:
		(void)0;
	}

	return true;
}

void CubismClippingContext::UpdateBounds(CubismModel* model)
{
	float left = FLT_MAX, right = -FLT_MAX, top = FLT_MAX, bottom = -FLT_MAX;
	active = false;
	for (auto drawable : drawables)
	{
		float left1 = FLT_MAX, right1 = -FLT_MAX, top1 = FLT_MAX, bottom1 = -FLT_MAX;
		auto vertexCount = model->GetDrawableVertexCount(drawable);
		auto vertices = model->GetDrawableVertices(drawable);

		if (vertexCount <= 0)
		{
			continue;
		}

		for (int i = 0; i < vertexCount; ++i)
		{
			auto x = vertices[(i << 1) | 0];
			auto y = vertices[(i << 1) | 1];
			left1 = std::min(left1, x), right1 = std::max(right1, x), top1 = std::min(top1, y), bottom1 = std::max(bottom1, y);
		}

		left = std::min(left, left1), right = std::max(right, right1), top = std::min(top, top1), bottom = std::max(bottom, bottom1);
		active = true;
	}

	if (active)
	{
		clipBound.x = left;
		clipBound.y = top;
		clipBound.width = right - left;
		clipBound.height = bottom - top;
	}
}


#ifndef _RG_GLOBAL_DEF_H_
#define _RG_GLOBAL_DEF_H_

#include <memory>
#include <vector>
#include <unordered_map>

#include "common/common.h"

#include "stl/stl_shared_ptr.h"
#include "stl/stl_unique_ptr.h"
#include "stl/stl_weak_ptr.h"

#include "memory/allocator.h"
namespace rg
{
	using IAllocatorPtr = ykl::STLSharedPtr<ykl::IAllocator>;
}
namespace ykl::test
{
	using IAllocatorPtr = ykl::STLSharedPtr<ykl::IAllocator>;
}

#include "math/math.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"
#include "math/color.h"
#include "math/line_segment.h"
#include "math/rect.h"

namespace rg
{
	// 共通で使用する小数型
	using Float = float;

	// ベクトル
	using Vector2 = ykl::TVector2<Float>;
	using Vector3 = ykl::TVector3<Float>;
	using Vector4 = ykl::TVector4<Float>;

	// 行列
	using Matrix22 = ykl::TMatrix22<Float>;
	using Matrix33 = ykl::TMatrix33<Float>;
	using Matrix44 = ykl::TMatrix44<Float>;

	// クオータニオン
	using Quaternion = ykl::TQuaternion<Float>;

	// 色
	using Color3 = ykl::TColor<Float, 3>;
	using Color4 = ykl::TColor<Float, 4>;
	using Color3u = ykl::TColor<uint8_t, 3>;
	using Color4u = ykl::TColor<uint8_t, 4>;

	// 線分
	using LineSegment = ykl::TLineSegment<Float>;
	// 矩形
	using Rect = ykl::TRect<Float>;
}

#endif
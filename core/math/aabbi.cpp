/**************************************************************************/
/*  aabbi.cpp                                                             */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "aabbi.h"

#include "core/math/aabb.h"
#include "core/string/ustring.h"
#include "core/variant/variant.h"

int64_t AABBi::get_volume() const {
	return static_cast<int64_t>(size.x) * size.y * size.z;
}

void AABBi::merge_with(const AABBi &p_aabbi) {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabbi.size.x < 0 || p_aabbi.size.y < 0 || p_aabbi.size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	Vector3i beg_1, beg_2;
	Vector3i end_1, end_2;
	Vector3i min, max;

	beg_1 = position;
	beg_2 = p_aabbi.position;
	end_1 = size + beg_1;
	end_2 = p_aabbi.size + beg_2;

	min.x = (beg_1.x < beg_2.x) ? beg_1.x : beg_2.x;
	min.y = (beg_1.y < beg_2.y) ? beg_1.y : beg_2.y;
	min.z = (beg_1.z < beg_2.z) ? beg_1.z : beg_2.z;

	max.x = (end_1.x > end_2.x) ? end_1.x : end_2.x;
	max.y = (end_1.y > end_2.y) ? end_1.y : end_2.y;
	max.z = (end_1.z > end_2.z) ? end_1.z : end_2.z;

	position = min;
	size = max - min;
}

bool AABBi::is_same(const AABBi &p_aabbi) const {
	return position == p_aabbi.position && size == p_aabbi.size;
}

AABBi AABBi::intersection(const AABBi &p_aabbi) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabbi.size.x < 0 || p_aabbi.size.y < 0 || p_aabbi.size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	Vector3i src_min = position;
	Vector3i src_max = position + size;
	Vector3i dst_min = p_aabbi.position;
	Vector3i dst_max = p_aabbi.position + p_aabbi.size;

	Vector3i min, max;

	if (src_min.x > dst_max.x || src_max.x < dst_min.x) {
		return AABBi();
	} else {
		min.x = (src_min.x > dst_min.x) ? src_min.x : dst_min.x;
		max.x = (src_max.x < dst_max.x) ? src_max.x : dst_max.x;
	}

	if (src_min.y > dst_max.y || src_max.y < dst_min.y) {
		return AABBi();
	} else {
		min.y = (src_min.y > dst_min.y) ? src_min.y : dst_min.y;
		max.y = (src_max.y < dst_max.y) ? src_max.y : dst_max.y;
	}

	if (src_min.z > dst_max.z || src_max.z < dst_min.z) {
		return AABBi();
	} else {
		min.z = (src_min.z > dst_min.z) ? src_min.z : dst_min.z;
		max.z = (src_max.z < dst_max.z) ? src_max.z : dst_max.z;
	}

	return AABBi(min, max - min);
}

Vector3i AABBi::get_longest_axis() const {
	Vector3i axis(1, 0, 0);
	int32_t max_size = size.x;

	if (size.y > max_size) {
		axis = Vector3i(0, 1, 0);
		max_size = size.y;
	}

	if (size.z > max_size) {
		axis = Vector3i(0, 0, 1);
	}

	return axis;
}

int AABBi::get_longest_axis_index() const {
	int axis = 0;
	int32_t max_size = size.x;

	if (size.y > max_size) {
		axis = 1;
		max_size = size.y;
	}

	if (size.z > max_size) {
		axis = 2;
	}

	return axis;
}

Vector3i AABBi::get_shortest_axis() const {
	Vector3i axis(1, 0, 0);
	int32_t min_size = size.x;

	if (size.y < min_size) {
		axis = Vector3i(0, 1, 0);
		min_size = size.y;
	}

	if (size.z < min_size) {
		axis = Vector3i(0, 0, 1);
	}

	return axis;
}

int AABBi::get_shortest_axis_index() const {
	int axis = 0;
	int32_t min_size = size.x;

	if (size.y < min_size) {
		axis = 1;
		min_size = size.y;
	}

	if (size.z < min_size) {
		axis = 2;
	}

	return axis;
}

AABBi AABBi::merge(const AABBi &p_with) const {
	AABBi aabb = *this;
	aabb.merge_with(p_with);
	return aabb;
}

AABBi AABBi::expand(const Vector3i &p_vector) const {
	AABBi aabb = *this;
	aabb.expand_to(p_vector);
	return aabb;
}

AABBi AABBi::grow(int32_t p_by) const {
	AABBi aabb = *this;
	aabb.grow_by(p_by);
	return aabb;
}

AABBi::operator String() const {
	return "[P: " + String(position) + ", S: " + String(size) + "]";
}

AABBi::operator AABB() const {
	return AABB(position, size);
}

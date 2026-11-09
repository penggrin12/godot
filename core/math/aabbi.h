/**************************************************************************/
/*  aabbi.h                                                               */
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

#pragma once

#include "core/math/vector3.h"
#include "core/math/vector3i.h"
#include "core/templates/hashfuncs.h"

/**
 * AABB (Axis Aligned Bounding Box) using integer (Vector3i) coordinates.
 */

class Variant;
struct AABB;

struct [[nodiscard]] AABBi {
	Vector3i position;
	Vector3i size;

	int64_t get_volume() const;
	_FORCE_INLINE_ bool has_volume() const {
		return size.x > 0 && size.y > 0 && size.z > 0;
	}

	_FORCE_INLINE_ bool has_surface() const {
		return size.x > 0 || size.y > 0 || size.z > 0;
	}

	const Vector3i &get_position() const { return position; }
	void set_position(const Vector3i &p_pos) { position = p_pos; }
	const Vector3i &get_size() const { return size; }
	void set_size(const Vector3i &p_size) { size = p_size; }

	constexpr bool operator==(const AABBi &p_rval) const {
		return position == p_rval.position && size == p_rval.size;
	}
	constexpr bool operator!=(const AABBi &p_rval) const {
		return position != p_rval.position || size != p_rval.size;
	}

	bool is_same(const AABBi &p_aabbi) const;
	_FORCE_INLINE_ bool intersects(const AABBi &p_aabbi) const; /// Both AABBs overlap
	_FORCE_INLINE_ bool encloses(const AABBi &p_aabbi) const; /// p_aabbi is completely inside this

	AABBi merge(const AABBi &p_with) const;
	void merge_with(const AABBi &p_aabbi);
	AABBi intersection(const AABBi &p_aabbi) const; ///get box where two intersect, empty if no intersection occurs

	_FORCE_INLINE_ bool has_point(const Vector3i &p_point) const;
	_FORCE_INLINE_ Vector3i get_support(const Vector3i &p_direction) const;

	Vector3i get_longest_axis() const;
	int get_longest_axis_index() const;
	_FORCE_INLINE_ int32_t get_longest_axis_size() const;

	Vector3i get_shortest_axis() const;
	int get_shortest_axis_index() const;
	_FORCE_INLINE_ int32_t get_shortest_axis_size() const;

	AABBi grow(int32_t p_by) const;
	_FORCE_INLINE_ void grow_by(int32_t p_amount);

	_FORCE_INLINE_ Vector3i get_endpoint(int p_point) const;

	AABBi expand(const Vector3i &p_vector) const;
	_FORCE_INLINE_ void expand_to(const Vector3i &p_vector); /** expand to contain a point if necessary */

	_FORCE_INLINE_ AABBi abs() const {
		return AABBi(position + size.mini(0), size.abs());
	}

	_FORCE_INLINE_ void set_end(const Vector3i &p_end) {
		size = p_end - position;
	}

	_FORCE_INLINE_ Vector3i get_end() const {
		return position + size;
	}

	_FORCE_INLINE_ Vector3 get_center() const {
		return Vector3(position) + (Vector3(size) * 0.5f);
	}

	uint32_t hash() const {
		uint32_t h = hash_murmur3_one_32(position.x);
		h = hash_murmur3_one_32(position.y, h);
		h = hash_murmur3_one_32(position.z, h);
		h = hash_murmur3_one_32(size.x, h);
		h = hash_murmur3_one_32(size.y, h);
		h = hash_murmur3_one_32(size.z, h);
		return hash_fmix32(h);
	}

	explicit operator String() const;
	operator AABB() const;

	AABBi() = default;
	constexpr AABBi(const Vector3i &p_pos, const Vector3i &p_size) :
			position(p_pos),
			size(p_size) {
	}
	constexpr AABBi(int32_t p_pos_x, int32_t p_pos_y, int32_t p_pos_z, int32_t p_size_x, int32_t p_size_y, int32_t p_size_z) :
			position(p_pos_x, p_pos_y, p_pos_z),
			size(p_size_x, p_size_y, p_size_z) {
	}
};

inline bool AABBi::intersects(const AABBi &p_aabbi) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabbi.size.x < 0 || p_aabbi.size.y < 0 || p_aabbi.size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	if (position.x >= (p_aabbi.position.x + p_aabbi.size.x)) {
		return false;
	}
	if ((position.x + size.x) <= p_aabbi.position.x) {
		return false;
	}
	if (position.y >= (p_aabbi.position.y + p_aabbi.size.y)) {
		return false;
	}
	if ((position.y + size.y) <= p_aabbi.position.y) {
		return false;
	}
	if (position.z >= (p_aabbi.position.z + p_aabbi.size.z)) {
		return false;
	}
	if ((position.z + size.z) <= p_aabbi.position.z) {
		return false;
	}

	return true;
}

inline bool AABBi::encloses(const AABBi &p_aabbi) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabbi.size.x < 0 || p_aabbi.size.y < 0 || p_aabbi.size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	Vector3i src_min = position;
	Vector3i src_max = position + size;
	Vector3i dst_min = p_aabbi.position;
	Vector3i dst_max = p_aabbi.position + p_aabbi.size;

	return (
			(src_min.x <= dst_min.x) &&
			(src_max.x >= dst_max.x) &&
			(src_min.y <= dst_min.y) &&
			(src_max.y >= dst_max.y) &&
			(src_min.z <= dst_min.z) &&
			(src_max.z >= dst_max.z));
}

Vector3i AABBi::get_support(const Vector3i &p_direction) const {
	Vector3i support = position;
	if (p_direction.x > 0) {
		support.x += size.x;
	}
	if (p_direction.y > 0) {
		support.y += size.y;
	}
	if (p_direction.z > 0) {
		support.z += size.z;
	}
	return support;
}

Vector3i AABBi::get_endpoint(int p_point) const {
	switch (p_point) {
		case 0:
			return Vector3i(position.x, position.y, position.z);
		case 1:
			return Vector3i(position.x, position.y, position.z + size.z);
		case 2:
			return Vector3i(position.x, position.y + size.y, position.z);
		case 3:
			return Vector3i(position.x, position.y + size.y, position.z + size.z);
		case 4:
			return Vector3i(position.x + size.x, position.y, position.z);
		case 5:
			return Vector3i(position.x + size.x, position.y, position.z + size.z);
		case 6:
			return Vector3i(position.x + size.x, position.y + size.y, position.z);
		case 7:
			return Vector3i(position.x + size.x, position.y + size.y, position.z + size.z);
	}

	ERR_FAIL_V(Vector3i());
}

bool AABBi::has_point(const Vector3i &p_point) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	if (p_point.x < position.x) {
		return false;
	}
	if (p_point.y < position.y) {
		return false;
	}
	if (p_point.z < position.z) {
		return false;
	}
	if (p_point.x >= position.x + size.x) {
		return false;
	}
	if (p_point.y >= position.y + size.y) {
		return false;
	}
	if (p_point.z >= position.z + size.z) {
		return false;
	}

	return true;
}

inline void AABBi::expand_to(const Vector3i &p_vector) {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	Vector3i begin = position;
	Vector3i end = position + size;

	if (p_vector.x < begin.x) {
		begin.x = p_vector.x;
	}
	if (p_vector.y < begin.y) {
		begin.y = p_vector.y;
	}
	if (p_vector.z < begin.z) {
		begin.z = p_vector.z;
	}

	if (p_vector.x > end.x) {
		end.x = p_vector.x;
	}
	if (p_vector.y > end.y) {
		end.y = p_vector.y;
	}
	if (p_vector.z > end.z) {
		end.z = p_vector.z;
	}

	position = begin;
	size = end - begin;
}

inline int32_t AABBi::get_longest_axis_size() const {
	int32_t max_size = size.x;

	if (size.y > max_size) {
		max_size = size.y;
	}

	if (size.z > max_size) {
		max_size = size.z;
	}

	return max_size;
}

inline int32_t AABBi::get_shortest_axis_size() const {
	int32_t max_size = size.x;

	if (size.y < max_size) {
		max_size = size.y;
	}

	if (size.z < max_size) {
		max_size = size.z;
	}

	return max_size;
}

void AABBi::grow_by(int32_t p_amount) {
	position.x -= p_amount;
	position.y -= p_amount;
	position.z -= p_amount;
	size.x += 2 * p_amount;
	size.y += 2 * p_amount;
	size.z += 2 * p_amount;
}

template <>
struct is_zero_constructible<AABBi> : std::true_type {};

/**************************************************************************/
/*  test_aabbi.cpp                                                        */
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

#include "core/math/vector3i.h"
#include "tests/test_macros.h"

TEST_FORCE_LINK(test_aabbi)

#include "core/math/aabbi.h"

namespace TestAABBi {

TEST_CASE("[AABBi] Constructor methods") {
	constexpr AABBi aabbi_default;

	CHECK_MESSAGE(
			(aabbi_default.position == Vector3i(0, 0, 0)),
			"Default constructor should set position to Vector3i(0, 0, 0).");
	CHECK_MESSAGE(
			(aabbi_default.size == Vector3i(0, 0, 0)),
			"Default constructor should set size to Vector3i(0, 0, 0).");

	constexpr AABBi aabbi_vec = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	constexpr AABBi aabbi_scalar = AABBi(-1, 2, -2, 4, 5, 6);
	CHECK_MESSAGE(
			(aabbi_vec == aabbi_scalar),
			"AABBi constructed with 6 scalar values should match AABBi constructed with Vector3i");

	constexpr AABBi aabbi_copy = AABBi(aabbi_vec);
	CHECK_MESSAGE(
			(aabbi_vec == aabbi_copy),
			"AABBi constructed with a copy should match AABBi constructed with Vector3i");
}

TEST_CASE("[AABBi] String conversion") {
	CHECK_MESSAGE(
			(String(AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6))) == "[P: (-1, 2, -2), S: (4, 5, 6)]"),
			"The string representation should match the expected value.");
}

TEST_CASE("[AABBi] Basic getters") {
	constexpr AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	CHECK_MESSAGE(
			(aabbi.get_position() == Vector3i(-1, 2, -2)),
			"get_position() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_size() == Vector3i(4, 5, 6)),
			"get_size() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_end() == Vector3i(3, 7, 4)),
			"get_end() should return the expected value.");
	CHECK_MESSAGE(
			aabbi.get_center().is_equal_approx(Vector3(1.0f, 4.5f, 1.0f)),
			"get_center() should return the expected value.");
}

TEST_CASE("[AABBi] Basic setters") {
	AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	aabbi.set_end(Vector3i(100, 10, 100));
	CHECK_MESSAGE(
			(aabbi == AABBi(Vector3i(-1, 2, -2), Vector3i(101, 8, 102))),
			"set_end() should result in the expected AABBi.");

	aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	aabbi.set_position(Vector3i(-1000, -2000, -3000));
	CHECK_MESSAGE(
			(aabbi == AABBi(Vector3i(-1000, -2000, -3000), Vector3i(4, 5, 6))),
			"set_position() should result in the expected AABBi.");

	aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	aabbi.set_size(Vector3i(0, 0, 50));
	CHECK_MESSAGE(
			(aabbi == AABBi(Vector3i(-1, 2, -2), Vector3i(0, 0, 50))),
			"set_size() should result in the expected AABBi.");
}

TEST_CASE("[AABBi] Absolute") {
	constexpr AABBi positive_aabbi = AABBi(Vector3i(5, 0, 5), Vector3i(20, 10, 5));
	CHECK_MESSAGE(
			(positive_aabbi.abs() == positive_aabbi),
			"abs() on an AABBi with positive size should return an identical AABBi.");

	constexpr AABBi negative_aabbi = AABBi(Vector3i(5, 0, 5), Vector3i(-20, -10, -5));
	const AABBi abs_negative = negative_aabbi.abs();
	CHECK_MESSAGE(
			(abs_negative.position == Vector3i(-15, -10, 0)),
			"abs() should offset position by negative size.");
	CHECK_MESSAGE(
			(abs_negative.size == Vector3i(20, 10, 5)),
			"abs() should invert negative sizes to positive.");

	constexpr AABBi zero_aabbi = AABBi(Vector3i(2, -3, 4), Vector3i(0, 0, 0));
	CHECK_MESSAGE(
			(zero_aabbi.abs() == zero_aabbi),
			"abs() on zero-sized AABBi should leave position and size unchanged.");
}

TEST_CASE("[AABBi] Volume getters") {
	AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	CHECK_MESSAGE(
			(aabbi.get_volume() == 120),
			"get_volume() should return the expected value with positive size.");
	CHECK_MESSAGE(
			aabbi.has_volume(),
			"Non-empty volumetric AABBi should have a volume.");

	// Equals to 8000000000. Well above the 32-bit integer limit.
	constexpr int64_t large_aabbi_expected_volume = 2000ull * 2000 * 2000;
	aabbi = AABBi(Vector3i(0, 0, 0), Vector3i(2000, 2000, 2000));
	CHECK_MESSAGE(
			(aabbi.get_volume() == large_aabbi_expected_volume),
			"get_volume() should correctly calculate 64-bit volume without 32-bit overflow.");

	aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 0, 6));
	CHECK_MESSAGE(
			!aabbi.has_volume(),
			"Non-empty flat AABBi should not have a volume.");

	CHECK_MESSAGE(
			!AABBi().has_volume(),
			"Empty AABBi should not have a volume.");
}

TEST_CASE("[AABBi] Equality") {
	constexpr AABBi a = AABBi(Vector3i(1, 2, 3), Vector3i(4, 5, 6));
	constexpr AABBi b = AABBi(Vector3i(1, 2, 3), Vector3i(4, 5, 6));
	constexpr AABBi diff_pos = AABBi(Vector3i(0, 2, 3), Vector3i(4, 5, 6));
	constexpr AABBi diff_size = AABBi(Vector3i(1, 2, 3), Vector3i(4, 0, 6));

	CHECK_MESSAGE(
			(a == b),
			"Boxes with identical position and size should evaluate as equal.");
	CHECK_MESSAGE(
			!(a != b),
			"Boxes with identical position and size should not evaluate as not-equal.");
	CHECK_MESSAGE(
			(a.is_same(b)),
			"is_same() on identical boxes should return true.");

	CHECK_MESSAGE(
			(a != diff_pos),
			"Boxes with different positions should not evaluate as equal.");
	CHECK_MESSAGE(
			(a != diff_size),
			"Boxes with different sizes should not evaluate as equal.");
	CHECK_MESSAGE(
			(!a.is_same(diff_pos)),
			"is_same() with different positions should return false.");
	CHECK_MESSAGE(
			(!a.is_same(diff_size)),
			"is_same() with different sizes should return false.");
}

TEST_CASE("[AABBi] Surface getters") {
	AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	CHECK_MESSAGE(
			aabbi.has_surface(),
			"Non-empty volumetric AABBi should have an surface.");

	aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 0, 6));
	CHECK_MESSAGE(
			aabbi.has_surface(),
			"Non-empty flat AABBi should have a surface.");

	aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 0, 0));
	CHECK_MESSAGE(
			aabbi.has_surface(),
			"Non-empty linear AABBi should have a surface.");

	CHECK_MESSAGE(
			!AABBi().has_surface(),
			"Empty AABBi should not have an surface.");
}

TEST_CASE("[AABBi] Intersection") {
	constexpr AABBi aabbi_big = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));

	AABBi aabbi_small = AABBi(Vector3i(-1, 2, -2), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			aabbi_big.intersects(aabbi_small),
			"intersects() with fully contained AABBi (touching the edge) should return the expected result.");

	aabbi_small = AABBi(Vector3i(0, 1, -2), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			!aabbi_big.intersects(aabbi_small),
			"intersects() with partially contained AABBi (overflowing on Y axis) should return the expected result.");

	aabbi_small = AABBi(Vector3i(10, -10, -10), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			!aabbi_big.intersects(aabbi_small),
			"intersects() with non-contained AABBi should return the expected result.");

	aabbi_small = AABBi(Vector3i(-1, 2, -2), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			(aabbi_big.intersection(aabbi_small) == aabbi_small),
			"intersection() with fully contained AABBi (touching the edge) should return the expected result.");

	aabbi_small = AABBi(Vector3i(0, 1, -2), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			(aabbi_big.intersection(aabbi_small) == AABBi(Vector3i(0, 2, -2), Vector3i(1, 0, 1))),
			"intersection() with partially contained AABBi (overflowing on Y axis) should return the expected result.");

	aabbi_small = AABBi(Vector3i(10, -10, -10), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			(aabbi_big.intersection(aabbi_small) == AABBi()),
			"intersection() with non-contained AABBi should return the expected result.");
}

TEST_CASE("[AABBi] Merging") {
	constexpr AABBi aabbi_big = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));

	AABBi aabbi_small = AABBi(Vector3i(-1, 2, -2), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			(aabbi_big.merge(aabbi_small) == aabbi_big),
			"merge() with fully contained AABBi (touching the edge) should return the expected result.");

	aabbi_small = AABBi(Vector3i(0, 1, -2), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			(aabbi_big.merge(aabbi_small) == AABBi(Vector3i(-1, 1, -2), Vector3i(4, 6, 6))),
			"merge() with partially contained AABBi (overflowing on Y axis) should return the expected result.");

	aabbi_small = AABBi(Vector3i(10, -10, -10), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			(aabbi_big.merge(aabbi_small) == AABBi(Vector3i(-1, -10, -10), Vector3i(12, 17, 14))),
			"merge() with non-contained AABBi should return the expected result.");
}

TEST_CASE("[AABBi] Encloses") {
	constexpr AABBi aabbi_big = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));

	CHECK_MESSAGE(
			aabbi_big.encloses(aabbi_big),
			"encloses() with itself should return the expected result.");

	AABBi aabbi_small = AABBi(Vector3i(-1, 2, -2), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			aabbi_big.encloses(aabbi_small),
			"encloses() with fully contained AABBi (touching the edge) should return the expected result.");

	aabbi_small = AABBi(Vector3i(1, 6, 2), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			aabbi_big.encloses(aabbi_small),
			"encloses() with fully contained AABBi (touching the edge) should return the expected result.");

	aabbi_small = AABBi(Vector3i(0, 1, -2), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			!aabbi_big.encloses(aabbi_small),
			"encloses() with partially contained AABBi (overflowing on Y axis) should return the expected result.");

	aabbi_small = AABBi(Vector3i(10, -10, -10), Vector3i(1, 1, 1));
	CHECK_MESSAGE(
			!aabbi_big.encloses(aabbi_small),
			"encloses() with non-contained AABBi should return the expected result.");
}

TEST_CASE("[AABBi] Get endpoints") {
	constexpr AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	CHECK_MESSAGE(
			(aabbi.get_endpoint(0) == Vector3i(-1, 2, -2)),
			"The endpoint at index 0 should match the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_endpoint(1) == Vector3i(-1, 2, 4)),
			"The endpoint at index 1 should match the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_endpoint(2) == Vector3i(-1, 7, -2)),
			"The endpoint at index 2 should match the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_endpoint(3) == Vector3i(-1, 7, 4)),
			"The endpoint at index 3 should match the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_endpoint(4) == Vector3i(3, 2, -2)),
			"The endpoint at index 4 should match the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_endpoint(5) == Vector3i(3, 2, 4)),
			"The endpoint at index 5 should match the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_endpoint(6) == Vector3i(3, 7, -2)),
			"The endpoint at index 6 should match the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_endpoint(7) == Vector3i(3, 7, 4)),
			"The endpoint at index 7 should match the expected value.");

	ERR_PRINT_OFF;
	CHECK_MESSAGE(
			(aabbi.get_endpoint(8) == Vector3i()),
			"The endpoint at invalid index 8 should match the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_endpoint(-1) == Vector3i()),
			"The endpoint at invalid index -1 should match the expected value.");
	ERR_PRINT_ON;
}

TEST_CASE("[AABBi] Get longest/shortest axis") {
	constexpr AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	CHECK_MESSAGE(
			(aabbi.get_longest_axis() == Vector3i(0, 0, 1)),
			"get_longest_axis() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_longest_axis_index() == Vector3i::AXIS_Z),
			"get_longest_axis_index() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_longest_axis_size() == 6),
			"get_longest_axis_size() should return the expected value.");

	CHECK_MESSAGE(
			(aabbi.get_shortest_axis() == Vector3i(1, 0, 0)),
			"get_shortest_axis() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_shortest_axis_index() == Vector3i::AXIS_X),
			"get_shortest_axis_index() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_shortest_axis_size() == 4),
			"get_shortest_axis_size() should return the expected value.");
}

TEST_CASE("[AABBi] Get support") {
	constexpr AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	CHECK_MESSAGE(
			(aabbi.get_support(Vector3i(1, 0, 0)) == Vector3i(3, 2, -2)),
			"get_support() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_support(Vector3i(0, 1, 1)) == Vector3i(-1, 7, 4)),
			"get_support() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_support(Vector3i(0, 1, -400)) == Vector3i(-1, 7, -2)),
			"get_support() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_support(Vector3i(0, -1, 0)) == Vector3i(-1, 2, -2)),
			"get_support() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_support(Vector3i(0, -0, 0)) == Vector3i(-1, 2, -2)),
			"get_support() should return the expected value.");
	CHECK_MESSAGE(
			(aabbi.get_support(Vector3i()) == Vector3i(-1, 2, -2)),
			"get_support() should return the AABBi position when given a zero vector.");
}

TEST_CASE("[AABBi] Grow") {
	constexpr AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	CHECK_MESSAGE(
			(aabbi.grow(5) == AABBi(Vector3i(-6, -3, -7), Vector3i(14, 15, 16))),
			"grow() with positive value should return the expected AABBi.");
	CHECK_MESSAGE(
			(aabbi.grow(-1) == AABBi(Vector3i(0, 3, -1), Vector3i(2, 3, 4))),
			"grow() with negative value should return the expected AABBi.");
}

TEST_CASE("[AABBi] Has point") {
	constexpr AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	CHECK_MESSAGE(
			aabbi.has_point(Vector3i(-1, 3, 0)),
			"has_point() with contained point should return the expected value.");
	CHECK_MESSAGE(
			aabbi.has_point(Vector3i(2, 3, 0)),
			"has_point() with contained point should return the expected value.");
	CHECK_MESSAGE(
			!aabbi.has_point(Vector3i(-20, 0, 0)),
			"has_point() with non-contained point should return the expected value.");

	CHECK_MESSAGE(
			aabbi.has_point(Vector3i(-1, 3, 0)),
			"has_point() with positive size should include point on near face (X axis).");
	CHECK_MESSAGE(
			!aabbi.has_point(Vector3i(3, 3, 0)),
			"has_point() with positive size shouldn't include point on far face (X axis).");
	CHECK_MESSAGE(
			aabbi.has_point(Vector3i(0, 2, 0)),
			"has_point() with positive size should include point on near face (Y axis).");
	CHECK_MESSAGE(
			!aabbi.has_point(Vector3i(0, 7, 0)),
			"has_point() with positive size shouldn't include point on far face (Y axis).");
	CHECK_MESSAGE(
			aabbi.has_point(Vector3i(0, 3, -2)),
			"has_point() with positive size should include point on near face (Z axis).");
	CHECK_MESSAGE(
			!aabbi.has_point(Vector3i(0, 3, 4)),
			"has_point() with positive size shouldn't include point on far face (Z axis).");
}

TEST_CASE("[AABBi] Expanding") {
	constexpr AABBi aabbi = AABBi(Vector3i(-1, 2, -2), Vector3i(4, 5, 6));
	CHECK_MESSAGE(
			(aabbi.expand(Vector3i(-1, 3, 0)) == aabbi),
			"expand() with contained point should return the expected AABBi.");
	CHECK_MESSAGE(
			(aabbi.expand(Vector3i(2, 3, 0)) == aabbi),
			"expand() with contained point should return the expected AABBi.");
	CHECK_MESSAGE(
			(aabbi.expand(Vector3i(-1, 3, 0)) == aabbi),
			"expand() with contained point on negative edge should return the expected AABBi.");
	CHECK_MESSAGE(
			(aabbi.expand(Vector3i(3, 3, 0)) == aabbi),
			"expand() with contained point on positive edge should return the expected AABBi.");
	CHECK_MESSAGE(
			(aabbi.expand(Vector3i(-20, 0, 0)) == AABBi(Vector3i(-20, 0, -2), Vector3i(23, 7, 6))),
			"expand() with non-contained point should return the expected AABBi.");
}

} // namespace TestAABBi

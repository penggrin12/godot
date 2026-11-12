using System;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;

#nullable enable

namespace Godot
{
    /// <summary>
    /// A 3D axis-aligned bounding box using integer coordinates.
    /// </summary>
    [Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct AabbI : IEquatable<AabbI>
    {
        private Vector3I _position;
        private Vector3I _size;

        /// <summary>
        /// The origin point. This is usually the corner on the bottom-left and forward of the bounding box.
        /// </summary>
        /// <value>Directly uses a private field.</value>
        public Vector3I Position
        {
            readonly get { return _position; }
            set { _position = value; }
        }

        /// <summary>
        /// The bounding box's width, height, and depth starting from <see cref="Position"/>.
        /// Setting this value also affects the <see cref="End"/> point.
        /// It's recommended setting the width, height, and depth to non-negative values.
        /// This is because most methods in Godot assume that the <see cref="Position"/> is the bottom-left-forward corner,
        /// and the <see cref="End"/> is the top-right-back corner.
        /// To get an equivalent bounding box with non-negative size, use <see cref="Abs"/>.
        /// </summary>
        /// <value>Directly uses a private field.</value>
        public Vector3I Size
        {
            readonly get { return _size; }
            set { _size = value; }
        }

        /// <summary>
        /// The ending point. This is usually the corner on the top-right and back of the bounding box,
        /// and is equivalent to <see cref="Position"/> plus <see cref="Size"/>.
        /// Setting this point affects the <see cref="Size"/>.
        /// </summary>
        /// <value>
        /// Getting is equivalent to <paramref name="value"/> = <see cref="Position"/> + <see cref="Size"/>,
        /// setting is equivalent to <see cref="Size"/> = <paramref name="value"/> - <see cref="Position"/>
        /// </value>
        public Vector3I End
        {
            readonly get { return _position + _size; }
            set { _size = value - _position; }
        }

        /// <summary>
        /// The bounding box's volume.
        /// See also <see cref="HasVolume"/>.
        /// </summary>
        public readonly long Volume
        {
            get { return (long)_size.X * (long)_size.Y * (long)_size.Z; }
        }

        /// <summary>
        /// Returns an <see cref="AabbI"/> equivalent to this bounding box, with its width, height, and depth modified
        /// to be non-negative values.
        /// </summary>
        /// <returns>The modified <see cref="AabbI"/>.</returns>
        public readonly AabbI Abs()
        {
            Vector3I topLeft = _position + _size.Min(Vector3I.Zero);
            return new AabbI(topLeft, _size.Abs());
        }

        /// <summary>
        /// Returns the center point of the bounding box.
        /// </summary>
        /// <returns>The center.</returns>
        public readonly Vector3 GetCenter()
        {
            return (Vector3)_position + ((Vector3)_size * 0.5f);
        }

        /// <summary>
        /// Returns <see langword="true"/> if this bounding box <i>completely</i> encloses the <paramref name="with"/> box.
        /// The edges of both boxes are included.
        /// <code>
        ///     var a = new AabbI(new Vector3I(0, 0, 0), new Vector3I(4, 4, 4));
        ///     var b = new AabbI(new Vector3I(1, 1, 1), new Vector3I(3, 3, 3));
        ///     var c = new AabbI(new Vector3I(2, 2, 2), new Vector3I(8, 8, 8));
        ///
        ///     GD.Print(a.Encloses(a)); // Prints true
        ///     GD.Print(a.Encloses(b)); // Prints true
        ///     GD.Print(a.Encloses(c)); // Prints false
        /// </code>
        /// </summary>
        /// <param name="with">The other <see cref="AabbI"/> that may be enclosed.</param>
        /// <returns>
        /// A <see langword="bool"/> for whether or not this <see cref="AabbI"/> encloses <paramref name="with"/>.
        /// </returns>
        public readonly bool Encloses(AabbI with)
        {
            Vector3I srcMin = _position;
            Vector3I srcMax = _position + _size;
            Vector3I dstMin = with._position;
            Vector3I dstMax = with._position + with._size;

            return srcMin.X <= dstMin.X &&
                   srcMax.X >= dstMax.X &&
                   srcMin.Y <= dstMin.Y &&
                   srcMax.Y >= dstMax.Y &&
                   srcMin.Z <= dstMin.Z &&
                   srcMax.Z >= dstMax.Z;
        }

        /// <summary>
        /// Returns a copy of this bounding box expanded to align the edges with the given <paramref name="toPoint"/>, if necessary.
        /// <code>
        ///     var box = new AabbI(new Vector3I(0, 0, 0), new Vector3I(5, 2, 5));
        ///
        ///     box = box.Expand(new Vector3I(10, 0, 0));
        ///     GD.Print(box.Position); // Prints (0, 0, 0)
        ///     GD.Print(box.Size);     // Prints (10, 2, 5)
        ///
        ///     box = box.Expand(new Vector3I(-5, 0, 5));
        ///     GD.Print(box.Position); // Prints (-5, 0, 0)
        ///     GD.Print(box.Size);     // Prints (15, 2, 5)
        /// </code>
        /// </summary>
        /// <param name="toPoint">The point to include.</param>
        /// <returns>The expanded <see cref="AabbI"/>.</returns>
        public readonly AabbI Expand(Vector3I toPoint)
        {
            Vector3I begin = _position;
            Vector3I end = _position + _size;

            if (toPoint.X < begin.X)
            {
                begin.X = toPoint.X;
            }
            if (toPoint.Y < begin.Y)
            {
                begin.Y = toPoint.Y;
            }
            if (toPoint.Z < begin.Z)
            {
                begin.Z = toPoint.Z;
            }

            if (toPoint.X > end.X)
            {
                end.X = toPoint.X;
            }
            if (toPoint.Y > end.Y)
            {
                end.Y = toPoint.Y;
            }
            if (toPoint.Z > end.Z)
            {
                end.Z = toPoint.Z;
            }

            return new AabbI(begin, end - begin);
        }

        /// <summary>
        /// Returns the position of one of the 8 vertices that compose this bounding box.
        /// With an <paramref name="idx"/> of 0 this is the same as <see cref="Position"/>,
        /// and an <paramref name="idx"/> of 7 is the same as <see cref="End"/>.
        /// Gets the position of one of the 8 endpoints of the <see cref="AabbI"/>.
        /// </summary>
        /// <param name="idx">Which endpoint to get.</param>
        /// <exception cref="ArgumentOutOfRangeException">
        /// <paramref name="idx"/> is less than 0 or greater than 7.
        /// </exception>
        /// <returns>An endpoint of the <see cref="AabbI"/>.</returns>
        public readonly Vector3I GetEndpoint(int idx)
        {
            switch (idx)
            {
                case 0:
                    return new Vector3I(_position.X, _position.Y, _position.Z);
                case 1:
                    return new Vector3I(_position.X, _position.Y, _position.Z + _size.Z);
                case 2:
                    return new Vector3I(_position.X, _position.Y + _size.Y, _position.Z);
                case 3:
                    return new Vector3I(_position.X, _position.Y + _size.Y, _position.Z + _size.Z);
                case 4:
                    return new Vector3I(_position.X + _size.X, _position.Y, _position.Z);
                case 5:
                    return new Vector3I(_position.X + _size.X, _position.Y, _position.Z + _size.Z);
                case 6:
                    return new Vector3I(_position.X + _size.X, _position.Y + _size.Y, _position.Z);
                case 7:
                    return new Vector3I(_position.X + _size.X, _position.Y + _size.Y, _position.Z + _size.Z);
                default:
                {
                    throw new ArgumentOutOfRangeException(nameof(idx),
                        $"Index is {idx}, but a value from 0 to 7 is expected.");
                }
            }
        }

        /// <summary>
        /// Returns the normalized longest axis of the <see cref="AabbI"/>.
        /// </summary>
        /// <returns>A vector representing the normalized longest axis of the <see cref="AabbI"/>.</returns>
        public readonly Vector3I GetLongestAxis()
        {
            var axis = new Vector3I(1, 0, 0);
            int maxSize = _size.X;

            if (_size.Y > maxSize)
            {
                axis = new Vector3I(0, 1, 0);
                maxSize = _size.Y;
            }

            if (_size.Z > maxSize)
            {
                axis = new Vector3I(0, 0, 1);
            }

            return axis;
        }

        /// <summary>
        /// Returns the <see cref="Vector3I.Axis"/> index of the longest axis of the <see cref="AabbI"/>.
        /// </summary>
        /// <returns>A <see cref="Vector3I.Axis"/> index for which axis is longest.</returns>
        public readonly Vector3I.Axis GetLongestAxisIndex()
        {
            var axis = Vector3I.Axis.X;
            int maxSize = _size.X;

            if (_size.Y > maxSize)
            {
                axis = Vector3I.Axis.Y;
                maxSize = _size.Y;
            }

            if (_size.Z > maxSize)
            {
                axis = Vector3I.Axis.Z;
            }

            return axis;
        }

        /// <summary>
        /// Returns the longest dimension of this bounding box's <see cref="Size"/>.
        /// </summary>
        /// <returns>The length of the longest axis of the <see cref="AabbI"/>.</returns>
        public readonly int GetLongestAxisSize()
        {
            int maxSize = _size.X;

            if (_size.Y > maxSize)
                maxSize = _size.Y;

            if (_size.Z > maxSize)
                maxSize = _size.Z;

            return maxSize;
        }

        /// <summary>
        /// Returns the shortest normalized axis of this bounding box's <see cref="Size"/>, as a <see cref="Vector3I"/>.
        /// <code>
        ///     var box = new AabbI(new Vector3I(0, 0, 0), new Vector3I(2, 4, 8));
        ///
        ///     GD.Print(box.GetShortestAxis());      // Prints (1, 0, 0)
        ///     GD.Print(box.GetShortestAxisIndex()); // Prints X
        ///     GD.Print(box.GetShortestAxisSize());  // Prints 2
        /// </code>
        /// See also <see cref="GetShortestAxisIndex"/> and <see cref="GetShortestAxisSize"/>.
        /// </summary>
        /// <returns>A vector representing the normalized shortest axis of the <see cref="AabbI"/>.</returns>
        public readonly Vector3I GetShortestAxis()
        {
            var axis = new Vector3I(1, 0, 0);
            int maxSize = _size.X;

            if (_size.Y < maxSize)
            {
                axis = new Vector3I(0, 1, 0);
                maxSize = _size.Y;
            }

            if (_size.Z < maxSize)
            {
                axis = new Vector3I(0, 0, 1);
            }

            return axis;
        }

        /// <summary>
        /// Returns the index to the shortest axis of this bounding box's <see cref="Size"/> (see <see cref="Vector3I.Axis"/>).
        /// For an example, see <see cref="GetLongestAxis"/>.
        /// </summary>
        /// <returns>A <see cref="Vector3I.Axis"/> index for which axis is shortest.</returns>
        public readonly Vector3I.Axis GetShortestAxisIndex()
        {
            var axis = Vector3I.Axis.X;
            int maxSize = _size.X;

            if (_size.Y < maxSize)
            {
                axis = Vector3I.Axis.Y;
                maxSize = _size.Y;
            }

            if (_size.Z < maxSize)
            {
                axis = Vector3I.Axis.Z;
            }

            return axis;
        }

        /// <summary>
        /// Returns the shortest normalized axis of this bounding box's <see cref="Size"/>.
        /// For an example, see <see cref="GetShortestAxis"/>.
        /// </summary>
        /// <returns>The scalar length of the shortest axis of the <see cref="AabbI"/>.</returns>
        public readonly int GetShortestAxisSize()
        {
            int maxSize = _size.X;

            if (_size.Y < maxSize)
                maxSize = _size.Y;

            if (_size.Z < maxSize)
                maxSize = _size.Z;

            return maxSize;
        }

        /// <summary>
        /// Returns the vertex's position of this bounding box that's the farthest in the given direction.
        /// This point is commonly known as the support point in collision detection algorithms.
        /// </summary>
        /// <param name="direction">The direction to find support for.</param>
        /// <returns>A vector representing the support.</returns>
        public readonly Vector3I GetSupport(Vector3I direction)
        {
            Vector3I support = _position;
            if (direction.X > 0)
            {
                support.X += _size.X;
            }
            if (direction.Y > 0)
            {
                support.Y += _size.Y;
            }
            if (direction.Z > 0)
            {
                support.Z += _size.Z;
            }
            return support;
        }

        /// <summary>
        /// Returns a copy of this bounding box extended on all sides by the given amount <paramref name="by"/>.
        /// A negative amount shrinks the box instead.
        /// <code>
        ///     var a = new AabbI(new Vector3I(4, 4, 4), new Vector3I(8, 8, 8)).Grow(4);
        ///     GD.Print(a.Position); // Prints (0, 0, 0)
        ///     GD.Print(a.Size);     // Prints (16, 16, 16)
        ///
        ///     var b = new AabbI(new Vector3I(0, 0, 0), new Vector3I(8, 4, 2)).Grow(2);
        ///     GD.Print(b.Position); // Prints (-2, -2, -2)
        ///     GD.Print(b.Size);     // Prints (12, 8, 6)
        /// </code>
        /// </summary>
        /// <param name="by">The amount to grow by.</param>
        /// <returns>The grown <see cref="AabbI"/>.</returns>
        public readonly AabbI Grow(int by)
        {
            AabbI res = this;

            res._position.X -= by;
            res._position.Y -= by;
            res._position.Z -= by;
            res._size.X += 2 * by;
            res._size.Y += 2 * by;
            res._size.Z += 2 * by;

            return res;
        }

        /// <summary>
        /// Returns <see langword="true"/> if the bounding box contains the given <paramref name="point"/>.
        /// By convention, points exactly on the right, top, and back sides are <b>not</b> included.
        /// This method is not reliable for <see cref="AabbI"/> with a <i>negative</i> <see cref="Size"/>.
        /// Use <see cref="Abs"/> first to get a valid bounding box.
        /// </summary>
        /// <param name="point">The point to check.</param>
        /// <returns>
        /// A <see langword="bool"/> for whether or not the <see cref="AabbI"/> contains <paramref name="point"/>.
        /// </returns>
        public readonly bool HasPoint(Vector3I point)
        {
            if (point.X < _position.X)
                return false;
            if (point.Y < _position.Y)
                return false;
            if (point.Z < _position.Z)
                return false;
            if (point.X >= _position.X + _size.X)
                return false;
            if (point.Y >= _position.Y + _size.Y)
                return false;
            if (point.Z >= _position.Z + _size.Z)
                return false;

            return true;
        }

        /// <summary>
        /// Returns <see langword="true"/> if this bounding box has a surface or a length, that is,
        /// at least one component of <see cref="Size"/> is positive. Otherwise, returns <see langword="false"/>.
        /// </summary>
        /// <returns>
        /// A <see langword="bool"/> for whether or not the <see cref="AabbI"/> has surface.
        /// </returns>
        public readonly bool HasSurface()
        {
            return _size.X > 0 || _size.Y > 0 || _size.Z > 0;
        }

        /// <summary>
        /// Returns <see langword="true"/> if this bounding box's width, height, and depth are all positive.
        /// See also <see cref="Volume"/>.
        /// </summary>
        /// <returns>
        /// A <see langword="bool"/> for whether or not the <see cref="AabbI"/> has volume.
        /// </returns>
        public readonly bool HasVolume()
        {
            return _size.X > 0 && _size.Y > 0 && _size.Z > 0;
        }

        /// <summary>
        /// Returns the intersection of this <see cref="AabbI"/> and <paramref name="with"/>.
        /// </summary>
        /// <param name="with">The other <see cref="AabbI"/>.</param>
        /// <returns>The clipped <see cref="AabbI"/>.</returns>
        public readonly AabbI Intersection(AabbI with)
        {
            Vector3I srcMin = _position;
            Vector3I srcMax = _position + _size;
            Vector3I dstMin = with._position;
            Vector3I dstMax = with._position + with._size;

            Vector3I min, max;

            if (srcMin.X > dstMax.X || srcMax.X < dstMin.X)
            {
                return new AabbI();
            }
            else
            {
                min.X = srcMin.X > dstMin.X ? srcMin.X : dstMin.X;
                max.X = srcMax.X < dstMax.X ? srcMax.X : dstMax.X;
            }

            if (srcMin.Y > dstMax.Y || srcMax.Y < dstMin.Y)
            {
                return new AabbI();
            }
            else
            {
                min.Y = srcMin.Y > dstMin.Y ? srcMin.Y : dstMin.Y;
                max.Y = srcMax.Y < dstMax.Y ? srcMax.Y : dstMax.Y;
            }

            if (srcMin.Z > dstMax.Z || srcMax.Z < dstMin.Z)
            {
                return new AabbI();
            }
            else
            {
                min.Z = srcMin.Z > dstMin.Z ? srcMin.Z : dstMin.Z;
                max.Z = srcMax.Z < dstMax.Z ? srcMax.Z : dstMax.Z;
            }

            return new AabbI(min, max - min);
        }

        /// <summary>
        /// Returns <see langword="true"/> if the <see cref="AabbI"/> overlaps with <paramref name="with"/>
        /// (i.e. they have at least one point in common).
        /// </summary>
        /// <param name="with">The other <see cref="AabbI"/> to check for intersections with.</param>
        /// <returns>
        /// A <see langword="bool"/> for whether or not they are intersecting.
        /// </returns>
        public readonly bool Intersects(AabbI with)
        {
            if (_position.X >= (with._position.X + with._size.X))
                return false;
            if ((_position.X + _size.X) <= with._position.X)
                return false;
            if (_position.Y >= (with._position.Y + with._size.Y))
                return false;
            if ((_position.Y + _size.Y) <= with._position.Y)
                return false;
            if (_position.Z >= (with._position.Z + with._size.Z))
                return false;
            if ((_position.Z + _size.Z) <= with._position.Z)
                return false;

            return true;
        }

        /// <summary>
        /// Returns a larger <see cref="AabbI"/> that contains this <see cref="AabbI"/> and <paramref name="with"/>.
        /// </summary>
        /// <param name="with">The other <see cref="AabbI"/>.</param>
        /// <returns>The merged <see cref="AabbI"/>.</returns>
        public readonly AabbI Merge(AabbI with)
        {
            Vector3I beg1 = _position;
            Vector3I beg2 = with._position;
            var end1 = new Vector3I(_size.X, _size.Y, _size.Z) + beg1;
            var end2 = new Vector3I(with._size.X, with._size.Y, with._size.Z) + beg2;

            Vector3I min = new(
                beg1.X < beg2.X ? beg1.X : beg2.X,
                beg1.Y < beg2.Y ? beg1.Y : beg2.Y,
                beg1.Z < beg2.Z ? beg1.Z : beg2.Z
            );

            Vector3I max = new(
                end1.X > end2.X ? end1.X : end2.X,
                end1.Y > end2.Y ? end1.Y : end2.Y,
                end1.Z > end2.Z ? end1.Z : end2.Z
            );

            return new AabbI(min, max - min);
        }

        /// <summary>
        /// Constructs an <see cref="AabbI"/> from a position and size.
        /// </summary>
        /// <param name="position">The position.</param>
        /// <param name="size">The size, typically positive.</param>
        public AabbI(Vector3I position, Vector3I size)
        {
            _position = position;
            _size = size;
        }

        /// <summary>
        /// Constructs an <see cref="AabbI"/> from a <paramref name="position"/>,
        /// <paramref name="width"/>, <paramref name="height"/>, and <paramref name="depth"/>.
        /// </summary>
        /// <param name="position">The position.</param>
        /// <param name="width">The width, typically positive.</param>
        /// <param name="height">The height, typically positive.</param>
        /// <param name="depth">The depth, typically positive.</param>
        public AabbI(Vector3I position, int width, int height, int depth)
        {
            _position = position;
            _size = new Vector3I(width, height, depth);
        }

        /// <summary>
        /// Constructs an <see cref="AabbI"/> from <paramref name="x"/>,
        /// <paramref name="y"/>, <paramref name="z"/>, and <paramref name="size"/>.
        /// </summary>
        /// <param name="x">The position's X coordinate.</param>
        /// <param name="y">The position's Y coordinate.</param>
        /// <param name="z">The position's Z coordinate.</param>
        /// <param name="size">The size, typically positive.</param>
        public AabbI(int x, int y, int z, Vector3I size)
        {
            _position = new Vector3I(x, y, z);
            _size = size;
        }

        /// <summary>
        /// Constructs an <see cref="AabbI"/> from <paramref name="x"/>,
        /// <paramref name="y"/>, <paramref name="z"/>, <paramref name="width"/>,
        /// <paramref name="height"/>, and <paramref name="depth"/>.
        /// </summary>
        /// <param name="x">The position's X coordinate.</param>
        /// <param name="y">The position's Y coordinate.</param>
        /// <param name="z">The position's Z coordinate.</param>
        /// <param name="width">The width, typically positive.</param>
        /// <param name="height">The height, typically positive.</param>
        /// <param name="depth">The depth, typically positive.</param>
        public AabbI(int x, int y, int z, int width, int height, int depth)
        {
            _position = new Vector3I(x, y, z);
            _size = new Vector3I(width, height, depth);
        }

        /// <summary>
        /// Returns <see langword="true"/> if the <see cref="AabbI"/>s are exactly equal.
        /// </summary>
        /// <param name="left">The left <see cref="AabbI"/>.</param>
        /// <param name="right">The right <see cref="AabbI"/>.</param>
        /// <returns>Whether or not the <see cref="AabbI"/>s are exactly equal.</returns>
        public static bool operator ==(AabbI left, AabbI right)
        {
            return left.Equals(right);
        }

        /// <summary>
        /// Returns <see langword="true"/> if the <see cref="AabbI"/>s are not equal.
        /// </summary>
        /// <param name="left">The left <see cref="AabbI"/>.</param>
        /// <param name="right">The right <see cref="AabbI"/>.</param>
        /// <returns>Whether or not the <see cref="AabbI"/>s are not equal.</returns>
        public static bool operator !=(AabbI left, AabbI right)
        {
            return !left.Equals(right);
        }

        /// <summary>
        /// Converts this <see cref="AabbI"/> to a <see cref="Aabb"/>.
        /// </summary>
        /// <param name="value">The <see cref="AabbI"/> to convert.</param>
        public static implicit operator Aabb(AabbI value)
        {
            return new Aabb(value._position, value._size);
        }

        /// <summary>
        /// Converts a <see cref="Aabb"/> to a <see cref="AabbI"/>.
        /// </summary>
        /// <param name="value">The <see cref="Aabb"/> to convert.</param>
        public static explicit operator AabbI(Aabb value)
        {
            return new AabbI((Vector3I)value.Position, (Vector3I)value.Size);
        }

        /// <summary>
        /// Returns <see langword="true"/> if the <see cref="AabbI"/> is exactly equal
        /// to the given <paramref name="obj"/>.
        /// </summary>
        /// <param name="obj">The object to compare with.</param>
        /// <returns>Whether or not the <see cref="AabbI"/> and the object are equal.</returns>
        public override readonly bool Equals([NotNullWhen(true)] object? obj)
        {
            return obj is AabbI other && Equals(other);
        }

        /// <summary>
        /// Returns <see langword="true"/> if the <see cref="AabbI"/>s are exactly equal.
        /// </summary>
        /// <param name="other">The other <see cref="AabbI"/>.</param>
        /// <returns>Whether or not the <see cref="AabbI"/>s are exactly equal.</returns>
        public readonly bool Equals(AabbI other)
        {
            return _position == other._position && _size == other._size;
        }

        /// <summary>
        /// Serves as the hash function for <see cref="AabbI"/>.
        /// </summary>
        /// <returns>A hash code for this <see cref="AabbI"/>.</returns>
        public override readonly int GetHashCode()
        {
            return HashCode.Combine(_position, _size);
        }

        /// <summary>
        /// Converts this <see cref="AabbI"/> to a string.
        /// </summary>
        /// <returns>A string representation of this <see cref="AabbI"/>.</returns>
        public override readonly string ToString() => ToString(null);

        /// <summary>
        /// Converts this <see cref="AabbI"/> to a string with the given <paramref name="format"/>.
        /// </summary>
        /// <returns>A string representation of this <see cref="AabbI"/>.</returns>
        public readonly string ToString(string? format)
        {
            return $"{_position.ToString(format)}, {_size.ToString(format)}";
        }
    }
}

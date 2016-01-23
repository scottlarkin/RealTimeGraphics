/*
  Copyright (c) 2011 Tyrone Davison

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once
#ifndef __TSL_MATH__
#define __TSL_MATH__

namespace tsl
{
  /**
   * A utility class to describe 2d vectors.
   */
  class Vector2
  {
  public:
    float x, y;
  public:
    Vector2() : x(0), y(0) {}
    Vector2( float X, float Y ) : x(X), y(Y) {}
  };

  /**
   * A utility class to describe 3d vectors.
   */
  class Vector3
  {
  public:
    float x, y, z;
  public:
    Vector3() : x(0), y(0), z(0) {}
    Vector3( float X, float Y, float Z ) : x(X), y(Y), z(Z) {}
    Vector3( const Vector2& v, float Z ) : x(v.x), y(v.y), z(Z) {}
  };

  /**
   * A utility class to describe 4d vectors.
   */
  class Vector4
  {
  public:
    float x, y, z, w;
  public:
    Vector4() : x(0), y(0), z(0), w(1) {}
    Vector4( float X, float Y, float Z, float W ) : x(X), y(Y), z(Z), w(W) {}
    Vector4( const Vector3& v, float W ) : x(v.x), y(v.y), z(v.z), w(W) {}
  };

  /**
   * A utility class to describe 4x4 matrices.
   */
  class Matrix
  {
  public:
    float m00, m10, m20, m30;
    float m01, m11, m21, m31;
    float m02, m12, m22, m32;
    float m03, m13, m23, m33;
  public:
    Matrix() : m00(1), m10(0), m20(0), m30(0), m01(0), m11(1), m21(0), m31(0), m02(0), m12(0), m22(1), m32(0), m03(0), m13(0), m23(0), m33(1) {}
    Matrix( float M00, float M01, float M02, float M03, float M10, float M11, float M12, float M13, float M20, float M21, float M22, float M23, float M30, float M31, float M32, float M33 ) : m00(M00), m10(M10), m20(M20), m30(M30), m01(M01), m11(M11), m21(M21), m31(M31), m02(M02), m12(M12), m22(M22), m32(M32), m03(M03), m13(M13), m23(M23), m33(M33) {}
  };
}

#endif

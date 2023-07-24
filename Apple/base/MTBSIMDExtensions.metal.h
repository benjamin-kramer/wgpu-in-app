// Copyright (c) 2019 Lightricks. All rights reserved.
// Created by Barak Weiss.

#import <simd/simd.h>

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
NS_ASSUME_NONNULL_BEGIN
#endif

#if __METAL_VERSION__
#define MTB_FUNC METAL_FUNC
#else
#define MTB_FUNC inline SIMD_CFUNC
#endif

#if __cplusplus
#define MTB_EXTERN extern "C"
#else
#define MTB_EXTERN extern
#endif

#if __METAL_VERSION__
#ifndef __HAVE_SIMD_TYPES_PREFIX__
typedef float2 simd_float2;
typedef float3 simd_float3;
typedef float4 simd_float4;

using simd_ushort2 = metal::ushort2;

using simd_float2x2 = metal::float2x2;
using simd_float3x2 = metal::float3x2;
using simd_float4x2 = metal::float4x2;

using simd_float2x3 = metal::float2x3;
using simd_float3x3 = metal::float3x3;
using simd_float4x3 = metal::float4x3;

using simd_float2x4 = metal::float2x4;
using simd_float3x4 = metal::float3x4;
using simd_float4x4 = metal::float4x4;
#endif

static MTB_FUNC simd_float2x2 simd_mul(simd_float2x2 x, simd_float2x2 y) {
  return x * y;
}

static MTB_FUNC simd_float3x3 simd_mul(simd_float3x3 x, simd_float3x3 y) {
  return x * y;
}

static MTB_FUNC simd_float4x4 simd_mul(simd_float4x4 x, simd_float4x4 y) {
  return x * y;
}

static MTB_FUNC float simd_sin(float x) {
  return simd::sin(x);
}

static MTB_FUNC float simd_cos(float x) {
  return simd::cos(x);
}

static MTB_FUNC float simd_tan(float x) {
  return simd::tan(x);
}

static MTB_FUNC vector_float3 simd_normalize(vector_float3 x) {
  return simd::normalize(x);
}
#else

static MTB_FUNC float simd_sin(float x) {
  return sinf(x);
}

static MTB_FUNC float simd_cos(float x) {
  return cosf(x);
}

static MTB_FUNC float simd_tan(float x) {
  return tanf(x);
}
#endif

/// Returns a \c 2x2 matrix created from individual (column major) component values.
static MTB_FUNC simd_float2x2 simd_matrix(float m00, float m01, float m10, float m11) {
  return (simd_float2x2){{
    {m00, m01},
    {m10, m11},
  }};
}

/// Returns a \c 3x3 matrix created from individual (column major) component values.
static MTB_FUNC simd_float3x3 simd_matrix(float m00, float m01, float m02,
                                          float m10, float m11, float m12,
                                          float m20, float m21, float m22) {
  return (simd_float3x3){{
    {m00, m01, m02},
    {m10, m11, m12},
    {m20, m21, m22}
  }};
}

/// Returns a \c 4x4 matrix created from individual (column major) component values.
static MTB_FUNC simd_float4x4 simd_matrix(float m00, float m01, float m02, float m03,
                                          float m10, float m11, float m12, float m13,
                                          float m20, float m21, float m22, float m23,
                                          float m30, float m31, float m32, float m33) {
  return (simd_float4x4){{
    {m00, m01, m02, m03},
    {m10, m11, m12, m13},
    {m20, m21, m22, m23},
    {m30, m31, m32, m33}
  }};
}

/// Returns a 2x2 matrix that performs a scaling transformation.
/// @param sx scale factor in the \c X axis.
/// @param sy scale factor in the \c Y axis.
static MTB_FUNC simd_float2x2 simd_matrix_scale(float sx, float sy) {
  return (simd_float2x2){{
    {sx, 0},
    {0, sy}
  }};
}

/// Returns a 3x3 matrix that performs a scaling transformation.
/// @param sx scale factor in the \c X axis.
/// @param sy scale factor in the \c Y axis.
/// @param sz scale factor in the \c Z axis.
static MTB_FUNC simd_float3x3 simd_matrix_scale(float sx, float sy, float sz) {
  return (simd_float3x3){{
    {sx, 0, 0},
    {0, sy, 0},
    {0, 0, sz}
  }};
}

/// Returns a 4x4 matrix that performs a scaling transformation.
/// @param sx scale factor in the \c X axis.
/// @param sy scale factor in the \c Y axis.
/// @param sz scale factor in the \c Z axis.
/// @param sw scale factor in the \c W axis.
static MTB_FUNC simd_float4x4 simd_matrix_scale(float sx, float sy, float sz, float sw) {
  return (simd_float4x4){{
    {sx, 0, 0, 0},
    {0, sy, 0, 0},
    {0, 0, sz, 0},
    {0, 0, 0, sw}
  }};
}

/// Performs a scaling operation on the given \c matrix.
/// @param sx scale factor in the \c X axis.
/// @param sy scale factor in the \c Y axis.
static MTB_FUNC simd_float2x2 simd_scale(simd_float2x2 matrix, float sx, float sy) {
  return simd_mul(matrix, simd_matrix_scale(sx, sy));
}

/// Performs a scaling operation on the given \c matrix.
/// @param sx scale factor in the \c X axis.
/// @param sy scale factor in the \c Y axis.
/// @param sz scale factor in the \c Z axis.
static MTB_FUNC simd_float3x3 simd_scale(simd_float3x3 matrix, float sx, float sy, float sz) {
  return simd_mul(matrix, simd_matrix_scale(sx, sy, sz));
}

/// Performs a scaling operation on the given \c matrix.
/// @param sx scale factor in the \c X axis.
/// @param sy scale factor in the \c Y axis.
/// @param sz scale factor in the \c Z axis.
/// @param sw scale factor in the \c W axis.
static MTB_FUNC simd_float4x4 simd_scale(simd_float4x4 matrix, float sx, float sy, float sz,
                                           float sw) {
  return simd_mul(matrix, simd_matrix_scale(sx, sy, sz, sw));
}

/// Returns a 2x2 matrix that performs counterclockwise rotation by the given angle (in radians).
static MTB_FUNC simd_float2x2 simd_matrix_rotation(float radians) {
  return (simd_float2x2){{
    {simd_cos(radians), simd_sin(radians)},
    {-simd_sin(radians), simd_cos(radians)}
  }};
}

/// Returns a 3x3 matrix that performs counterclockwise rotation by the given angle (in radians)
/// around the given \c vector.
static MTB_FUNC simd_float3x3 simd_matrix_rotation(float radians, simd_float3 vector) {
  simd_float3 v = simd_normalize(vector);
  float cos = simd_cos(radians);
  float cosp = 1.0f - cos;
  float sin = simd_sin(radians);

  return (simd_float3x3){{
    {cos + cosp * v[0] * v[0],
      cosp * v[0] * v[1] + v[2] * sin,
      cosp * v[0] * v[2] - v[1] * sin},
    {cosp * v[0] * v[1] - v[2] * sin,
      cos + cosp * v[1] * v[1],
      cosp * v[1] * v[2] + v[0] * sin},
    {cosp * v[0] * v[2] + v[1] * sin,
      cosp * v[1] * v[2] - v[0] * sin,
      cos + cosp * v[2] * v[2]}
  }};
}

/// Returns a 4x4 matrix that performs counterclockwise rotation by the given angle (in radians)
/// around the given \c vector.
static MTB_FUNC simd_float4x4 simd_matrix4_rotation(float radians, simd_float3 vector) {
  simd_float3 v = simd_normalize(vector);
  float cos = simd_cos(radians);
  float cosp = 1.0f - cos;
  float sin = simd_sin(radians);

  return (simd_float4x4){{
    {cos + cosp * v[0] * v[0],
      cosp * v[0] * v[1] + v[2] * sin,
      cosp * v[0] * v[2] - v[1] * sin,
      0.0f},
    {cosp * v[0] * v[1] - v[2] * sin,
      cos + cosp * v[1] * v[1],
      cosp * v[1] * v[2] + v[0] * sin,
      0.0f},
    {cosp * v[0] * v[2] + v[1] * sin,
      cosp * v[1] * v[2] - v[0] * sin,
      cos + cosp * v[2] * v[2],
      0.0f},
    {0.0f,
      0.0f,
      0.0f,
      1.0f,
    }
  }};
}

/// Returns a 3x3 matrix that performs counterclockwise rotation by the given angle (in radians)
/// around the vector defined by <tt>(x, y, z)</tt>.
static MTB_FUNC simd_float3x3 simd_matrix_rotation(float radians, float x, float y, float z) {
  return simd_matrix_rotation(radians, (simd_float3){x, y, z});
}

/// Returns a 4x4 matrix that performs counterclockwise rotation by the given angle (in radians)
/// around the vector defined by <tt>(x, y, z)</tt>.
static MTB_FUNC simd_float4x4 simd_matrix4_rotation(float radians, float x, float y, float z) {
  return simd_matrix4_rotation(radians, (simd_float3){x, y, z});
}

/// Performs a counterclockwise rotation operation by \c radians on the given \c matrix.
static MTB_FUNC simd_float2x2 simd_rotate(simd_float2x2 matrix, float radians) {
  return simd_mul(matrix, simd_matrix_rotation(radians));
}

/// Performs a counterclockwise rotation operation by \c radians around the vector defined by
/// <tt>(x, y, z)</tt> on the given \c matrix.
static MTB_FUNC simd_float3x3 simd_rotate(simd_float3x3 matrix, float radians, float x, float y,
                                            float z) {
  return simd_mul(matrix, simd_matrix_rotation(radians, x, y, z));
}

/// Performs a counterclockwise rotation operation by \c radians around the vector defined by
/// <tt>(x, y, z)</tt> on the given \c matrix.
static MTB_FUNC simd_float4x4 simd_rotate(simd_float4x4 matrix, float radians, float x, float y,
                                            float z) {
  return simd_mul(matrix, simd_matrix4_rotation(radians, x, y, z));
}

/// Returns a 3x3 matrix that performs a translation transformation.
/// @param tx translation in the \c X axis.
/// @param ty translation in the \c Y axis.
static MTB_FUNC simd_float3x3 simd_matrix_translation(float tx, float ty) {
  return (simd_float3x3){{
    {1, 0, 0},
    {0, 1, 0},
    {tx, ty, 1}
  }};
}

/// Returns a 4x4 matrix that performs a translation transformation.
/// @param tx translation in the \c X axis.
/// @param ty translation in the \c Y axis.
/// @param tz translation in the \c Z axis.
static MTB_FUNC simd_float4x4 simd_matrix_translation(float tx, float ty, float tz) {
  return (simd_float4x4){{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {tx, ty, tz, 1}
  }};
}

/// Returns a 4x4 matrix that performs a translation transformation by given vector. Same as calling
/// @code
/// simd_matrix_translation(v.x, v.y, v.z);
/// @endcode
/// @param v translation component is 3 axes.
static MTB_FUNC simd_float4x4 simd_matrix_translation(simd_float3 v) {
  return simd_matrix_translation(v.x, v.y, v.z);
}

/// Performs a translation operation on the given \c matrix.
/// @param matrix matrix to translate.
/// @param tx translation in the \c X axis.
/// @param ty translation in the \c Y axis.
static MTB_FUNC simd_float3x3 simd_translate(simd_float3x3 matrix, float tx, float ty) {
  return simd_mul(matrix, simd_matrix_translation(tx, ty));
}

/// Performs a translation operation on the given \c matrix.
/// @param matrix matrix to translate.
/// @param tx translation in the \c X axis.
/// @param ty translation in the \c Y axis.
/// @param tz translation in the \c Z axis.
static MTB_FUNC simd_float4x4 simd_translate(simd_float4x4 matrix, float tx, float ty, float tz) {
  return simd_mul(matrix, simd_matrix_translation(tx, ty, tz));
}

/// Performs a translation operation on the given \c matrix. Same as calling
/// @code
/// simd_translate(v.x, v.y, v.z);
/// @endcode
/// @param matrix matrix to translate.
/// @param v translation component is 3 axes.
static MTB_FUNC simd_float4x4 simd_translate(simd_float4x4 matrix, simd_float3 v) {
  return simd_translate(matrix, v.x, v.y, v.z);
}

#ifndef __METAL_VERSION__

/// Converts \c transform to a 3x3 matrix.
static MTB_FUNC simd_float3x3 simd_matrix_transform(CGAffineTransform transform) {
  return (simd_float3x3){{
    {(float)transform.a, (float)transform.b, 0},
    {(float)transform.c, (float)transform.d, 0},
    {(float)transform.tx, (float)transform.ty, 1}
  }};
}

/// Converts \c CGSize to float2 vector.
static inline SIMD_CFUNC simd_float2 simd_make_float2(CGSize size) {
  return simd_make_float2(size.width, size.height);
}

/// Converts \c CGPoint to float2 vector.
static inline SIMD_CFUNC simd_float2 simd_make_float2(CGPoint point) {
  return simd_make_float2(point.x, point.y);
}

/// A float2 vector with all elements set to zero.
MTB_EXTERN const simd_float2 simd_float2_zero;

/// A float3 vector with all elements set to zero.
MTB_EXTERN const simd_float3 simd_float3_zero;

/// A float2 vector with all elements set to zero.
MTB_EXTERN const simd_float4 simd_float4_zero;

/// Returns a float3 vector that is the coefficients of the standard line equation (Ax + By + C = 0)
/// between the points \c source and \c target. Returns a vector with zero values if \c source and
/// \c target are equal.
static MTB_FUNC simd_float3 simd_line_equation(simd_float2 source, simd_float2 target) {
  if (simd_equal(source, target)) {
    return simd_float3_zero;
  }

  simd_float2 normal = simd_make_float2(target.y - source.y, source.x - target.x);
  simd_float3 line;
  if (simd_equal(source, simd_float2_zero)) {
    line = simd_make_float3(normal.x, normal.y, -simd_dot(normal, target));
  } else {
    line = simd_make_float3(normal.x, normal.y, -simd_dot(normal, source));
  }
  return line / (line.z ? ABS(line.z) : 1.0);
}

#endif

/// Returns a \c 4x4 matrix that performs a perspective projection.
/// @param fovyRadians Field of view angle in the \c Y direction, in radians.
/// @param aspectRatio The aspect ratio between \c X and \c Y that determines the field of view in
///        the \c X direction.
/// @param nearZ Coordinate of the near \c Z clipping plane of the viewing frustum.
/// @param farZ Coordinate of the far \c Z clipping plane of the viewing frustum.
/// @see gluPerspective
/// @note This projection follows Metal's convention where the NDC cube is [-1,1]x[-1,1]x[0,1], i.e.
/// the Z coordinate goes from 0 to 1.
static MTB_FUNC simd_float4x4 simd_matrix_perspective(float fovyRadians, float aspectRatio,
                                                      float nearZ, float farZ) {
  float cotan = 1.0f / simd_tan(fovyRadians / 2.0f);

  return (simd_float4x4){{
    {cotan / aspectRatio, 0.0f, 0.0f, 0.0f},
    {0.0f, cotan, 0.0f, 0.0f},
    {0.0f, 0.0f, farZ / (nearZ - farZ), -1.0f},
    {0.0f, 0.0f, farZ * nearZ / (nearZ - farZ), 0.0f}
  }};
}

/// Returns \c 4x4 matrix that performs a perspective projection.
/// @param left The left clipping plane of the viewing frustum. The left-X coordinate of the viewing
///        frustum on the near \c Z clipping plane.
/// @param right The right clipping plane of the viewing frustum. The right-X coordinate of the
///        viewing frustum on the near \c Z clipping plane.
/// @param bottom The bottom clipping plane of the viewing frustum. The bottom-Y coordinate of the
///        viewing frustum on the near \c Z clipping plane.
/// @param top The top clipping plane of the viewing frustum. The top-Y coordinate of the
///        viewing frustum on the near \c Z clipping plane.
/// @param nearZ Near \c Z clipping plane of the viewing frustum.
/// @param farZ Far \c Z clipping plane of the viewing frustum.
/// @note The matrix is calculated with the assumption that the eye is at (0, 0, 0).
/// @see glFrustum
/// @note This projection follows Metal's convention where the NDC cube is [-1,1]x[-1,1]x[0,1], i.e.
/// the Z coordinate goes from 0 to 1.
static MTB_FUNC simd_float4x4 simd_matrix_frustum(float left, float right, float bottom, float top,
                                                  float nearZ, float farZ) {
  float ral = right + left;
  float rsl = right - left;
  float tab = top + bottom;
  float tsb = top - bottom;
  float fsn = farZ - nearZ;

  return (simd_float4x4){{
    {2.0f * nearZ / rsl, 0.0f, 0.0f, 0.0f},
    {0.0f, 2.0f * nearZ / tsb, 0.0f, 0.0f},
    {ral / rsl, tab / tsb, -farZ / fsn, -1.0f},
    {0.0f, 0.0f, (-1.0f * farZ * nearZ) / fsn, 0.0f}
  }};
}

/// Returns a 4x4 matrix that performs an orthographic (parallel) projection. The parameters define
/// the clipping plane for the projection.
/// @param left The left vertical clipping plane.
/// @param right The right vertical clipping plane..
/// @param bottom The bottom horizontal clipping plane.
/// @param top The top horizontal clipping plane.
/// @param nearZ Near \c Z (depth) clipping plane.
/// @param farZ Far \c Z (depth) clipping plane.
/// @see glOrtho
/// @note This projection follows Metal's convention where the NDC cube is [-1,1]x[-1,1]x[0,1], i.e.
/// the Z coordinate goes from 0 to 1.
static MTB_FUNC simd_float4x4 simd_matrix_ortho(float left, float right, float bottom, float top,
                                                float nearZ, float farZ) {
  float ral = right + left;
  float rsl = right - left;
  float tab = top + bottom;
  float tsb = top - bottom;
  float fsn = farZ - nearZ;

  return (simd_float4x4){{
    {2.0f / rsl, 0.0f, 0.0f, 0.0f},
    {0.0f, 2.0f / tsb, 0.0f, 0.0f},
    {0.0f, 0.0f, -1.0f / fsn, 0.0f},
    {-ral / rsl, -tab / tsb, -nearZ / fsn, 1.0f}
  }};
}

#ifndef __METAL_VERSION__

/// Returns \c true if every element in \c x is within a specified tolerance \c tol to the
/// corresponding element in \c y. Returns \c false otherwise
static MTB_FUNC simd_bool simd_almost_equal_elements(simd_float2 x, simd_float2 y, float tol) {
  return simd_all(simd_abs(x - y) <= tol);
}

/// Returns \c true if every element in \c x is within a specified tolerance \c tol to the
/// corresponding element in \c y. Returns \c false otherwise
static MTB_FUNC simd_bool simd_almost_equal_elements(simd_float3 x, simd_float3 y, float tol) {
  return simd_all(simd_abs(x - y) <= tol);
}

/// Returns \c true if every element in \c x is within a specified tolerance \c tol to the
/// corresponding element in \c y. Returns \c false otherwise
static MTB_FUNC simd_bool simd_almost_equal_elements(simd_float4 x, simd_float4 y, float tol) {
  return simd_all(simd_abs(x - y) <= tol);
}

#endif

#ifdef __METAL_VERSION__
constant simd_float4x4 matrix_identity_float4x4 = metal::float4x4(1);

constant simd_float3x3 matrix_identity_float3x3 = metal::float3x3(1);

constant simd_float2x2 matrix_identity_float2x2 = metal::float2x2(1);
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __OBJC__

/// Returns a string representation of the given vector.
SIMD_OVERLOAD NSString *NSStringFromSimdVector(simd_float2 v);

/// Returns a string representation of the given vector.
SIMD_OVERLOAD NSString *NSStringFromSimdVector(simd_float3 v);

/// Returns a string representation of the given vector.
SIMD_OVERLOAD NSString *NSStringFromSimdVector(simd_float4 v);

/// Returns a string representation of the given matrix.
SIMD_OVERLOAD NSString *NSStringFromSimdMatrix(simd_float2x2 m);

/// Returns a string representation of the given matrix.
SIMD_OVERLOAD NSString *NSStringFromSimdMatrix(simd_float3x3 m);

/// Returns a string representation of the given matrix.
SIMD_OVERLOAD NSString *NSStringFromSimdMatrix(simd_float4x4 m);

#endif

#ifdef __cplusplus
}
#endif

#ifdef __OBJC__
NS_ASSUME_NONNULL_END
#endif

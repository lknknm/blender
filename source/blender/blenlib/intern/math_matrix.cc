/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright 2022 Blender Foundation. */

/** \file
 * \ingroup bli
 */

#include "BLI_math_matrix.hh"
#include "BLI_math_rotation.hh"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

using Eigen::Map;
using Eigen::Matrix;
using Eigen::Stride;

/* -------------------------------------------------------------------- */
/** \name Matrix multiplication
 * \{ */

namespace blender {

template<> float4x4 float4x4::operator*(const float4x4 &b) const
{
  using namespace math;
  const float4x4 &a = *this;
  float4x4 result;

#ifdef BLI_HAVE_SSE2
  __m128 A0 = _mm_load_ps(a[0]);
  __m128 A1 = _mm_load_ps(a[1]);
  __m128 A2 = _mm_load_ps(a[2]);
  __m128 A3 = _mm_load_ps(a[3]);

  for (int i = 0; i < 4; i++) {
    __m128 B0 = _mm_set1_ps(b[i][0]);
    __m128 B1 = _mm_set1_ps(b[i][1]);
    __m128 B2 = _mm_set1_ps(b[i][2]);
    __m128 B3 = _mm_set1_ps(b[i][3]);

    __m128 sum = _mm_add_ps(_mm_add_ps(_mm_mul_ps(B0, A0), _mm_mul_ps(B1, A1)),
                            _mm_add_ps(_mm_mul_ps(B2, A2), _mm_mul_ps(B3, A3)));

    _mm_store_ps(result[i], sum);
  }
#else
  const float4x4 T = transpose(b);

  result.x = float4(dot(a.x, T.x), dot(a.x, T.y), dot(a.x, T.z), dot(a.x, T.w));
  result.y = float4(dot(a.y, T.x), dot(a.y, T.y), dot(a.y, T.z), dot(a.y, T.w));
  result.z = float4(dot(a.z, T.x), dot(a.z, T.y), dot(a.z, T.z), dot(a.z, T.w));
  result.w = float4(dot(a.w, T.x), dot(a.w, T.y), dot(a.w, T.z), dot(a.w, T.w));
#endif
  return result;
}

template float2x2 float2x2::operator*(const float2x2 &b) const;
template float3x3 float3x3::operator*(const float3x3 &b) const;
template double2x2 double2x2::operator*(const double2x2 &b) const;
template double3x3 double3x3::operator*(const double3x3 &b) const;
template double4x4 double4x4::operator*(const double4x4 &b) const;

}  // namespace blender

/** \} */

namespace blender::math {

/* -------------------------------------------------------------------- */
/** \name Determinant
 * \{ */

template<typename T, int Size> T determinant(const MatBase<T, Size, Size> &mat)
{
  return Map<const Matrix<T, Size, Size>>(mat.base_ptr()).determinant();
}

template float determinant(const float2x2 &mat);
template float determinant(const float3x3 &mat);
template float determinant(const float4x4 &mat);
template double determinant(const double2x2 &mat);
template double determinant(const double3x3 &mat);
template double determinant(const double4x4 &mat);

template<typename T> bool is_negative(const MatBase<T, 4, 4> &mat)
{
  return Map<const Matrix<T, 3, 3>, 0, Stride<4, 1>>(mat.base_ptr()).determinant() < T(0);
}

template bool is_negative(const float4x4 &mat);
template bool is_negative(const double4x4 &mat);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Adjoint
 * \{ */

template<typename T, int Size> MatBase<T, Size, Size> adjoint(const MatBase<T, Size, Size> &mat)
{
  MatBase<T, Size, Size> adj;
  unroll<Size>([&](auto c) {
    unroll<Size>([&](auto r) {
      /* Copy other cells except the "cross" to compute the determinant. */
      MatBase<T, Size - 1, Size - 1> tmp;
      unroll<Size>([&](auto m_c) {
        unroll<Size>([&](auto m_r) {
          if (m_c != c && m_r != r) {
            int d_c = (m_c < c) ? m_c : (m_c - 1);
            int d_r = (m_r < r) ? m_r : (m_r - 1);
            tmp[d_c][d_r] = mat[m_c][m_r];
          }
        });
      });
      T minor = determinant(tmp);
      /* Transpose directly to get the adjugate. Swap destination row and col. */
      adj[r][c] = ((c + r) & 1) ? -minor : minor;
    });
  });
  return adj;
}

template float2x2 adjoint(const float2x2 &mat);
template float3x3 adjoint(const float3x3 &mat);
template float4x4 adjoint(const float4x4 &mat);
template double2x2 adjoint(const double2x2 &mat);
template double3x3 adjoint(const double3x3 &mat);
template double4x4 adjoint(const double4x4 &mat);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Inverse
 * \{ */

template<typename T, int Size> MatBase<T, Size, Size> invert(const MatBase<T, Size, Size> &mat)
{
  MatBase<T, Size, Size> result;
  Map<const Matrix<T, Size, Size>> M(mat.base_ptr());
  Map<Matrix<T, Size, Size>> R(result.base_ptr());
  bool is_invertible = true;
  M.computeInverseWithCheck(R, is_invertible, 0.0f);
  if (!is_invertible) {
    R = R.Zero();
  }
  return result;
}

template float2x2 invert(const float2x2 &mat);
template float3x3 invert(const float3x3 &mat);
template float4x4 invert(const float4x4 &mat);
template double2x2 invert(const double2x2 &mat);
template double3x3 invert(const double3x3 &mat);
template double4x4 invert(const double4x4 &mat);

/** \} */

/* -------------------------------------------------------------------- */
/** \name Polar Decomposition
 * \{ */

/**
 * Right polar decomposition:
 *     M = UP
 *
 * U is the 'rotation'-like component, the closest orthogonal matrix to M.
 * P is the 'scaling'-like component, defined in U space.
 *
 * See https://en.wikipedia.org/wiki/Polar_decomposition for more.
 */
template<typename T>
static void polar_decompose(const MatBase<T, 3, 3> &mat3,
                            MatBase<T, 3, 3> &r_U,
                            MatBase<T, 3, 3> &r_P)
{
  /* From svd decomposition (M = WSV*), we have:
   *     U = WV*
   *     P = VSV*
   */
  MatBase<T, 3, 3> W, V;
  vec_base<T, 3> S_val;

  {
    using namespace Eigen;
    using MatrixT = Matrix<T, 3, 3>;
    using VectorT = Matrix<T, 3, 1>;
    /* Blender and Eigen matrices are both column-major.
     * Since our matrix is squared, we can use thinU/V. */
    /** WORKAROUND:
     * (ComputeThinU | ComputeThinV) must be set as runtime parameters in Eigen < 3.4.0.
     * But this requires the matrix type to be dynamic to avoid an assert.
     */
    using MatrixDynamicT = Matrix<T, Dynamic, Dynamic>;
    JacobiSVD<MatrixDynamicT, NoQRPreconditioner> svd(
        Map<const MatrixDynamicT>(mat3.base_ptr(), 3, 3), ComputeThinU | ComputeThinV);

    (Map<MatrixT>(W.base_ptr())) = svd.matrixU();
    (Map<VectorT>(S_val)) = svd.singularValues();
    (Map<MatrixT>(V.base_ptr())) = svd.matrixV();
  }

  MatBase<T, 3, 3> S = from_scale<MatBase<T, 3, 3>>(S_val);
  MatBase<T, 3, 3> Vt = transpose(V);

  r_U = W * Vt;
  r_P = (V * S) * Vt;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name Interpolate
 * \{ */

template<typename T>
MatBase<T, 3, 3> interpolate(const MatBase<T, 3, 3> &A, const MatBase<T, 3, 3> &B, T t)
{
  using Mat3T = MatBase<T, 3, 3>;
  /* 'Rotation' component ('U' part of polar decomposition,
   * the closest orthogonal matrix to M3 rot/scale
   * transformation matrix), spherically interpolated. */
  Mat3T U_A, U_B;
  /* 'Scaling' component ('P' part of polar decomposition, i.e. scaling in U-defined space),
   * linearly interpolated. */
  Mat3T P_A, P_B;

  polar_decompose(A, U_A, P_A);
  polar_decompose(B, U_B, P_B);

  /* Quaternions cannot represent an axis flip. If such a singularity is detected, choose a
   * different decomposition of the matrix that still satisfies A = U_A * P_A but which has a
   * positive determinant and thus no axis flips. This resolves T77154.
   *
   * Note that a flip of two axes is just a rotation of 180 degrees around the third axis, and
   * three flipped axes are just an 180 degree rotation + a single axis flip. It is thus sufficient
   * to solve this problem for single axis flips. */
  if (is_negative(U_A)) {
    U_A = -U_A;
    P_A = -P_A;
  }
  if (is_negative(U_B)) {
    U_B = -U_B;
    P_B = -P_B;
  }

  detail::Quaternion<T> quat_A = math::to_quaternion(U_A);
  detail::Quaternion<T> quat_B = math::to_quaternion(U_B);
  detail::Quaternion<T> quat = math::interpolate(quat_A, quat_B, t);
  Mat3T U = from_rotation<Mat3T>(quat);

  Mat3T P = interpolate_linear(P_A, P_B, t);
  /* And we reconstruct rot/scale matrix from interpolated polar components */
  return U * P;
}

template float3x3 interpolate(const float3x3 &a, const float3x3 &b, float t);
template double3x3 interpolate(const double3x3 &a, const double3x3 &b, double t);

template<typename T>
MatBase<T, 4, 4> interpolate(const MatBase<T, 4, 4> &A, const MatBase<T, 4, 4> &B, T t)
{
  MatBase<T, 4, 4> result = MatBase<T, 4, 4>(
      interpolate(MatBase<T, 3, 3>(A), MatBase<T, 3, 3>(B), t));

  /* Location component, linearly interpolated. */
  const auto &loc_a = static_cast<const MatBase<T, 4, 4> &>(A).location();
  const auto &loc_b = static_cast<const MatBase<T, 4, 4> &>(B).location();
  result.location() = interpolate(loc_a, loc_b, t);

  return result;
}

template float4x4 interpolate(const float4x4 &a, const float4x4 &b, float t);
template double4x4 interpolate(const double4x4 &a, const double4x4 &b, double t);

template<typename T>
MatBase<T, 3, 3> interpolate_fast(const MatBase<T, 3, 3> &a, const MatBase<T, 3, 3> &b, T t)
{
  using QuaternionT = detail::Quaternion<T>;
  using Vec3T = typename MatBase<T, 3, 3>::vec3_type;

  Vec3T a_scale, b_scale;
  QuaternionT a_quat, b_quat;
  to_rot_scale<true>(a, a_quat, a_scale);
  to_rot_scale<true>(b, b_quat, b_scale);

  const Vec3T scale = interpolate(a_scale, b_scale, t);
  const QuaternionT rotation = interpolate(a_quat, b_quat, t);
  return from_rot_scale<MatBase<T, 3, 3>>(rotation, scale);
}

template float3x3 interpolate_fast(const float3x3 &a, const float3x3 &b, float t);
template double3x3 interpolate_fast(const double3x3 &a, const double3x3 &b, double t);

template<typename T>
MatBase<T, 4, 4> interpolate_fast(const MatBase<T, 4, 4> &a, const MatBase<T, 4, 4> &b, T t)
{
  using QuaternionT = detail::Quaternion<T>;
  using Vec3T = typename MatBase<T, 3, 3>::vec3_type;

  Vec3T a_loc, b_loc;
  Vec3T a_scale, b_scale;
  QuaternionT a_quat, b_quat;
  to_loc_rot_scale<true>(a, a_loc, a_quat, a_scale);
  to_loc_rot_scale<true>(b, b_loc, b_quat, b_scale);

  const Vec3T location = interpolate(a_loc, b_loc, t);
  const Vec3T scale = interpolate(a_scale, b_scale, t);
  const QuaternionT rotation = interpolate(a_quat, b_quat, t);
  return from_loc_rot_scale<MatBase<T, 4, 4>>(location, rotation, scale);
}

template float4x4 interpolate_fast(const float4x4 &a, const float4x4 &b, float t);
template double4x4 interpolate_fast(const double4x4 &a, const double4x4 &b, double t);

/** \} */

}  // namespace blender::math

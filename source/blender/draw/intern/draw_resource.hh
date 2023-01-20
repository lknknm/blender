/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright 2022 Blender Foundation. */

#pragma once

/** \file
 * \ingroup draw
 *
 * Component / Object level resources like object attributes, matrices, visibility etc...
 * Each of them are reference by resource index (#ResourceHandle).
 */

#include "BKE_curve.h"
#include "BKE_duplilist.h"
#include "BKE_mesh.h"
#include "BKE_mesh_wrapper.h"
#include "BKE_object.h"
#include "BKE_volume.h"
#include "BLI_hash.h"
#include "DNA_curve_types.h"
#include "DNA_layer_types.h"
#include "DNA_meta_types.h"
#include "DNA_object_types.h"

#include "draw_handle.hh"
#include "draw_manager.hh"
#include "draw_shader_shared.h"

/* -------------------------------------------------------------------- */
/** \name ObjectMatrices
 * \{ */

inline void ObjectMatrices::sync(const Object &object)
{
  model = object.object_to_world;
  model_inverse = object.world_to_object;
}

inline void ObjectMatrices::sync(const float4x4 &model_matrix)
{
  model = model_matrix;
  model_inverse = model_matrix.inverted();
}

inline std::ostream &operator<<(std::ostream &stream, const ObjectMatrices &matrices)
{
  stream << "ObjectMatrices(" << std::endl;
  stream << "model=" << matrices.model << ", " << std::endl;
  stream << "model_inverse=" << matrices.model_inverse << ")" << std::endl;
  return stream;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name ObjectInfos
 * \{ */

ENUM_OPERATORS(eObjectInfoFlag, OBJECT_NEGATIVE_SCALE)

inline void ObjectInfos::sync()
{
  object_attrs_len = 0;
  object_attrs_offset = 0;

  flag = eObjectInfoFlag::OBJECT_NO_INFO;
}

inline void ObjectInfos::sync(const blender::draw::ObjectRef ref, bool is_active_object)
{
  object_attrs_len = 0;
  object_attrs_offset = 0;

  ob_color = ref.object->color;
  index = ref.object->index;
  SET_FLAG_FROM_TEST(flag, is_active_object, eObjectInfoFlag::OBJECT_ACTIVE);
  SET_FLAG_FROM_TEST(
      flag, ref.object->base_flag & BASE_SELECTED, eObjectInfoFlag::OBJECT_SELECTED);
  SET_FLAG_FROM_TEST(
      flag, ref.object->base_flag & BASE_FROM_DUPLI, eObjectInfoFlag::OBJECT_FROM_DUPLI);
  SET_FLAG_FROM_TEST(
      flag, ref.object->base_flag & BASE_FROM_SET, eObjectInfoFlag::OBJECT_FROM_SET);
  SET_FLAG_FROM_TEST(
      flag, ref.object->transflag & OB_NEG_SCALE, eObjectInfoFlag::OBJECT_NEGATIVE_SCALE);

  /* Default values. Set if needed. */
  random = 0.0f;

  if (ref.object->data == nullptr) {
    orco_add = float3(0.0f);
    orco_mul = float3(1.0f);
    return;
  }

  switch (GS(reinterpret_cast<ID *>(ref.object->data)->name)) {
    case ID_VO: {
      BoundBox &bbox = *BKE_volume_boundbox_get(ref.object);
      orco_add = (float3(bbox.vec[6]) + float3(bbox.vec[0])) * 0.5f; /* Center. */
      orco_mul = float3(bbox.vec[6]) - float3(bbox.vec[0]);          /* Size. */
      break;
    }
    case ID_ME: {
      BKE_mesh_texspace_get(static_cast<Mesh *>(ref.object->data), orco_add, orco_mul);
      break;
    }
    case ID_CU_LEGACY: {
      Curve &cu = *static_cast<Curve *>(ref.object->data);
      BKE_curve_texspace_ensure(&cu);
      orco_add = cu.loc;
      orco_mul = cu.size;
      break;
    }
    case ID_MB: {
      MetaBall &mb = *static_cast<MetaBall *>(ref.object->data);
      orco_add = mb.loc;
      orco_mul = mb.size;
      break;
    }
    default:
      orco_add = float3(0.0f);
      orco_mul = float3(1.0f);
      break;
  }
}

inline std::ostream &operator<<(std::ostream &stream, const ObjectInfos &infos)
{
  stream << "ObjectInfos(";
  if (infos.flag == eObjectInfoFlag::OBJECT_NO_INFO) {
    stream << "skipped)" << std::endl;
    return stream;
  }
  stream << "orco_add=" << infos.orco_add << ", ";
  stream << "orco_mul=" << infos.orco_mul << ", ";
  stream << "ob_color=" << infos.ob_color << ", ";
  stream << "index=" << infos.index << ", ";
  stream << "random=" << infos.random << ", ";
  stream << "flag=" << infos.flag << ")" << std::endl;
  return stream;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name ObjectBounds
 * \{ */

inline void ObjectBounds::sync()
{
  test_enabled = false;
}

inline void ObjectBounds::sync(Object &ob)
{
  float3 min, max;
  INIT_MINMAX(min, max);

  if (ob.type == OB_MESH) {
    /* Optimization: Retrieve the mesh cached min max directly.
     * Avoids allocating a BoundBox on every sample for each DupliObject instance.
     * TODO(Miguel Pozo): Remove once T92963 or T96968 are done */
    BKE_mesh_wrapper_minmax(static_cast<Mesh *>(ob.data), min, max);
  }
  else {
    const BoundBox *bbox = BKE_object_boundbox_get(&ob);
    if (bbox == nullptr) {
      test_enabled = false;
      return;
    }

    for (const float3 &corner : bbox->vec) {
      minmax_v3v3_v3(min, max, corner);
    }
  }

  size = (max - min) / 2.0f;
  center = min + size;
  test_enabled = true;
}

inline void ObjectBounds::sync(const float3 &center, const float3 &size)
{
  this->center = center;
  this->size = size;
  test_enabled = true;
}

inline std::ostream &operator<<(std::ostream &stream, const ObjectBounds &bounds)
{
  stream << "ObjectBounds(";
  if (!bounds.test_enabled) {
    stream << "skipped)" << std::endl;
    return stream;
  }
  stream << std::endl;
  stream << ".center" << bounds.center << std::endl;
  stream << ".size" << bounds.size << std::endl;
  return stream;
}

/** \} */

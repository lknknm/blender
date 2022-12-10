/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cstring>

#include "MEM_guardedalloc.h"

#include "NOD_geometry.h"

#include "BKE_context.h"
#include "BKE_layer.h"
#include "BKE_node.h"
#include "BKE_object.h"

#include "BLT_translation.h"

#include "DNA_modifier_types.h"
#include "DNA_node_types.h"
#include "DNA_space_types.h"

#include "RNA_access.h"
#include "RNA_prototypes.h"

#include "UI_resources.h"

#include "node_common.h"

#include "node_geometry_register.hh"

bNodeTreeType *ntreeType_Geometry;

static void geometry_node_tree_get_from_context(
    const bContext *C, bNodeTreeType * /*treetype*/, bNodeTree **r_ntree, ID **r_id, ID **r_from)
{
  const Scene *scene = CTX_data_scene(C);
  ViewLayer *view_layer = CTX_data_view_layer(C);
  BKE_view_layer_synced_ensure(scene, view_layer);
  Object *ob = BKE_view_layer_active_object_get(view_layer);

  if (ob == nullptr) {
    return;
  }

  const ModifierData *md = BKE_object_active_modifier(ob);

  if (md == nullptr) {
    return;
  }

  if (md->type == eModifierType_Nodes) {
    const NodesModifierData *nmd = reinterpret_cast<const NodesModifierData *>(md);
    if (nmd->node_group != nullptr) {
      *r_from = &ob->id;
      *r_id = &ob->id;
      *r_ntree = nmd->node_group;
    }
  }
}

static void geometry_node_tree_update(bNodeTree *ntree)
{
  ntreeSetOutput(ntree);

  /* Needed to give correct types to reroutes. */
  ntree_update_reroute_nodes(ntree);
}

static void foreach_nodeclass(Scene * /*scene*/, void *calldata, bNodeClassCallback func)
{
  func(calldata, NODE_CLASS_INPUT, N_("Input"));
  func(calldata, NODE_CLASS_GEOMETRY, N_("Geometry"));
  func(calldata, NODE_CLASS_ATTRIBUTE, N_("Attribute"));
  func(calldata, NODE_CLASS_OP_COLOR, N_("Color"));
  func(calldata, NODE_CLASS_OP_VECTOR, N_("Vector"));
  func(calldata, NODE_CLASS_CONVERTER, N_("Converter"));
  func(calldata, NODE_CLASS_LAYOUT, N_("Layout"));
}

static bool geometry_node_tree_validate_link(eNodeSocketDatatype type_a,
                                             eNodeSocketDatatype type_b)
{
  /* Geometry, string, object, material, texture and collection sockets can only be connected to
   * themselves. The other types can be converted between each other. */
  if (ELEM(type_a, SOCK_FLOAT, SOCK_VECTOR, SOCK_RGBA, SOCK_BOOLEAN, SOCK_INT) &&
      ELEM(type_b, SOCK_FLOAT, SOCK_VECTOR, SOCK_RGBA, SOCK_BOOLEAN, SOCK_INT)) {
    return true;
  }
  return type_a == type_b;
}

static bool geometry_node_tree_socket_type_valid(bNodeTreeType * /*treetype*/,
                                                 bNodeSocketType *socket_type)
{
  return nodeIsStaticSocketType(socket_type) && ELEM(socket_type->type,
                                                     SOCK_FLOAT,
                                                     SOCK_VECTOR,
                                                     SOCK_RGBA,
                                                     SOCK_BOOLEAN,
                                                     SOCK_INT,
                                                     SOCK_STRING,
                                                     SOCK_OBJECT,
                                                     SOCK_GEOMETRY,
                                                     SOCK_COLLECTION,
                                                     SOCK_TEXTURE,
                                                     SOCK_IMAGE,
                                                     SOCK_MATERIAL);
}

void register_node_tree_type_geo()
{
  bNodeTreeType *tt = ntreeType_Geometry = static_cast<bNodeTreeType *>(
      MEM_callocN(sizeof(bNodeTreeType), "geometry node tree type"));
  tt->type = NTREE_GEOMETRY;
  strcpy(tt->idname, "GeometryNodeTree");
  strcpy(tt->group_idname, "GeometryNodeGroup");
  strcpy(tt->ui_name, N_("Geometry Node Editor"));
  tt->ui_icon = ICON_GEOMETRY_NODES;
  strcpy(tt->ui_description, N_("Geometry nodes"));
  tt->rna_ext.srna = &RNA_GeometryNodeTree;
  tt->update = geometry_node_tree_update;
  tt->get_from_context = geometry_node_tree_get_from_context;
  tt->foreach_nodeclass = foreach_nodeclass;
  tt->valid_socket_type = geometry_node_tree_socket_type_valid;
  tt->validate_link = geometry_node_tree_validate_link;

  ntreeTypeAdd(tt);
}

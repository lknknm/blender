
#include "eevee_defines.hh"

#include "gpu_shader_create_info.hh"

/* -------------------------------------------------------------------- */
/** \name Shadow pipeline
 * \{ */

GPU_SHADER_CREATE_INFO(eevee_shadow_tilemap_init)
    .do_static_compilation(true)
    .local_group_size(SHADOW_TILEMAP_RES, SHADOW_TILEMAP_RES)
    .storage_buf(0, Qualifier::READ_WRITE, "ShadowTileMapData", "tilemaps_buf[]")
    .storage_buf(1, Qualifier::READ_WRITE, "ShadowTileDataPacked", "tiles_buf[]")
    .additional_info("eevee_shared")
    .compute_source("eevee_shadow_tilemap_init_comp.glsl");

GPU_SHADER_CREATE_INFO(eevee_shadow_tag_update)
    .do_static_compilation(true)
    .local_group_size(1, 1, 1)
    .storage_buf(0, Qualifier::READ_WRITE, "ShadowTileMapData", "tilemaps_buf[]")
    .storage_buf(1, Qualifier::READ_WRITE, "ShadowTileDataPacked", "tiles_buf[]")
    .storage_buf(5, Qualifier::READ, "ObjectBounds", "bounds_buf[]")
    .storage_buf(6, Qualifier::READ, "uint", "resource_ids_buf[]")
    .additional_info("eevee_shared", "draw_view")
    .compute_source("eevee_shadow_tag_update_comp.glsl");

GPU_SHADER_CREATE_INFO(eevee_shadow_tag_usage_opaque)
    .do_static_compilation(true)
    .local_group_size(SHADOW_DEPTH_SCAN_GROUP_SIZE, SHADOW_DEPTH_SCAN_GROUP_SIZE)
    .sampler(0, ImageType::DEPTH_2D, "depth_tx")
    .storage_buf(5, Qualifier::READ_WRITE, "ShadowTileMapData", "tilemaps_buf[]")
    .storage_buf(6, Qualifier::READ_WRITE, "ShadowTileDataPacked", "tiles_buf[]")
    .push_constant(Type::FLOAT, "tilemap_pixel_radius")
    .push_constant(Type::FLOAT, "screen_pixel_radius_inv")
    .additional_info("eevee_shared", "draw_view", "eevee_light_data")
    .compute_source("eevee_shadow_tag_usage_comp.glsl");

GPU_SHADER_INTERFACE_INFO(eevee_shadow_tag_transparent_iface, "interp")
    .smooth(Type::VEC3, "P")
    .smooth(Type::VEC3, "vP");

GPU_SHADER_CREATE_INFO(eevee_shadow_tag_usage_transparent)
    .do_static_compilation(true)
    .vertex_in(0, Type::VEC3, "pos")
    .storage_buf(4, Qualifier::READ, "ObjectBounds", "bounds_buf[]")
    .storage_buf(5, Qualifier::READ_WRITE, "ShadowTileMapData", "tilemaps_buf[]")
    .storage_buf(6, Qualifier::READ_WRITE, "ShadowTileDataPacked", "tiles_buf[]")
    .push_constant(Type::FLOAT, "tilemap_pixel_radius")
    .push_constant(Type::FLOAT, "screen_pixel_radius_inv")
    .vertex_out(eevee_shadow_tag_transparent_iface)
    .additional_info("eevee_shared", "draw_view", "draw_modelmat_new", "eevee_light_data")
    .vertex_source("eevee_shadow_tag_usage_vert.glsl")
    .fragment_source("eevee_shadow_tag_usage_frag.glsl");

GPU_SHADER_CREATE_INFO(eevee_shadow_page_free)
    .do_static_compilation(true)
    .local_group_size(SHADOW_TILEDATA_PER_TILEMAP)
    .storage_buf(0, Qualifier::READ_WRITE, "ShadowTileMapData", "tilemaps_buf[]")
    .storage_buf(1, Qualifier::READ_WRITE, "ShadowTileDataPacked", "tiles_buf[]")
    .storage_buf(2, Qualifier::READ_WRITE, "ShadowPagesInfoData", "pages_infos_buf")
    .storage_buf(3, Qualifier::READ_WRITE, "uint", "pages_free_buf[]")
    .storage_buf(4, Qualifier::READ_WRITE, "uvec2", "pages_cached_buf[]")
    .additional_info("eevee_shared")
    .compute_source("eevee_shadow_page_free_comp.glsl");

GPU_SHADER_CREATE_INFO(eevee_shadow_page_defrag)
    .do_static_compilation(true)
    .local_group_size(1)
    .storage_buf(1, Qualifier::READ_WRITE, "ShadowTileDataPacked", "tiles_buf[]")
    .storage_buf(2, Qualifier::READ_WRITE, "ShadowPagesInfoData", "pages_infos_buf")
    .storage_buf(3, Qualifier::READ_WRITE, "uint", "pages_free_buf[]")
    .storage_buf(4, Qualifier::READ_WRITE, "uvec2", "pages_cached_buf[]")
    .additional_info("eevee_shared")
    .compute_source("eevee_shadow_page_defrag_comp.glsl");

GPU_SHADER_CREATE_INFO(eevee_shadow_page_allocate)
    .do_static_compilation(true)
    .local_group_size(SHADOW_TILEDATA_PER_TILEMAP)
    .storage_buf(0, Qualifier::READ_WRITE, "ShadowTileMapData", "tilemaps_buf[]")
    .storage_buf(1, Qualifier::READ_WRITE, "ShadowTileDataPacked", "tiles_buf[]")
    .storage_buf(2, Qualifier::READ_WRITE, "ShadowPagesInfoData", "pages_infos_buf")
    .storage_buf(3, Qualifier::READ_WRITE, "uint", "pages_free_buf[]")
    .storage_buf(4, Qualifier::READ_WRITE, "uvec2", "pages_cached_buf[]")
    .additional_info("eevee_shared")
    .compute_source("eevee_shadow_page_allocate_comp.glsl");

GPU_SHADER_CREATE_INFO(eevee_shadow_tilemap_finalize)
    .do_static_compilation(true)
    .local_group_size(SHADOW_TILEMAP_RES, SHADOW_TILEMAP_RES)
    .storage_buf(0, Qualifier::READ_WRITE, "ShadowTileMapData", "tilemaps_buf[]")
    .storage_buf(1, Qualifier::READ_WRITE, "ShadowTileDataPacked", "tiles_buf[]")
    .storage_buf(2, Qualifier::READ_WRITE, "ShadowPagesInfoData", "pages_infos_buf")
    .image(0, GPU_R32UI, Qualifier::WRITE, ImageType::UINT_2D, "tilemaps_img")
    .additional_info("eevee_shared")
    .compute_source("eevee_shadow_tilemap_finalize_comp.glsl");

GPU_SHADER_CREATE_INFO(eevee_shadow_render_culling)
    .local_group_size(64)
    .sampler(0, ImageType::DEPTH_2D, "depth_tx")
    .storage_buf(4, Qualifier::WRITE, "uint", "view_id_buf[]")
    .additional_info("eevee_shared", "draw_view", "eevee_light_data")
    .compute_source("eevee_shadow_tag_usage_comp.glsl");

/** \} */

/* -------------------------------------------------------------------- */
/** \name Shadow resources
 * \{ */

GPU_SHADER_CREATE_INFO(eevee_shadow_data)
    .sampler(SHADOW_ATLAS_TEX_SLOT, ImageType::FLOAT_2D, "shadow_atlas_tx")
    .sampler(SHADOW_TILEMAPS_TEX_SLOT, ImageType::UINT_2D, "shadow_tilemaps_tx");

/** \} */

/* -------------------------------------------------------------------- */
/** \name Debug
 * \{ */

GPU_SHADER_CREATE_INFO(eevee_shadow_debug)
    .do_static_compilation(true)
    .additional_info("eevee_shared")
    .fragment_out(0, Type::VEC4, "out_color_add", DualBlend::SRC_0)
    .fragment_out(0, Type::VEC4, "out_color_mul", DualBlend::SRC_1)
    .push_constant(Type::INT, "debug_mode")
    .push_constant(Type::INT, "debug_tilemap_index")
    .fragment_source("eevee_shadow_debug_frag.glsl")
    .additional_info(
        "draw_fullscreen", "draw_view", "eevee_hiz_data", "eevee_light_data", "eevee_shadow_data");

/** \} */

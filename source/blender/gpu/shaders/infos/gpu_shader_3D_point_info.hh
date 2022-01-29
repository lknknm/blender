/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2022 Blender Foundation.
 * All rights reserved.
 */

/** \file
 * \ingroup gpu
 */

#include "gpu_shader_create_info.hh"

GPU_SHADER_CREATE_INFO(gpu_shader_3D_point_fixed_size_varying_color)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::VEC4, "color")
    .vertex_out(smooth_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .push_constant(Type::MAT4, "ModelViewProjectionMatrix")
    .vertex_source("gpu_shader_3D_point_fixed_size_varying_color_vert.glsl")
    .fragment_source("gpu_shader_point_varying_color_frag.glsl")
    .do_static_compilation(true);

GPU_SHADER_CREATE_INFO(gpu_shader_3D_point_varying_size_varying_color)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_in(1, Type::VEC4, "color")
    .vertex_in(2, Type::FLOAT, "size")
    .vertex_out(smooth_color_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .push_constant(Type::MAT4, "ModelViewProjectionMatrix")
    .vertex_source("gpu_shader_3D_point_varying_size_varying_color_vert.glsl")
    .fragment_source("gpu_shader_point_varying_color_frag.glsl")
    .do_static_compilation(true);

GPU_SHADER_CREATE_INFO(gpu_shader_3D_point_uniform_size_uniform_color_aa)
    .vertex_in(0, Type::VEC3, "pos")
    .vertex_out(smooth_radii_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .push_constant(Type::MAT4, "ModelViewProjectionMatrix")
    .push_constant(Type::VEC4, "color")
    .push_constant(Type::FLOAT, "size")
    .vertex_source("gpu_shader_3D_point_uniform_size_aa_vert.glsl")
    .fragment_source("gpu_shader_point_uniform_color_aa_frag.glsl")
    .do_static_compilation(true);

GPU_SHADER_CREATE_INFO(gpu_shader_3D_point_uniform_size_uniform_color_aa_clipped)
    .additional_info("gpu_shader_3D_point_uniform_size_uniform_color_aa")
    .additional_info("gpu_clip_planes");

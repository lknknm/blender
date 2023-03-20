/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

void register_node_tree_type_geo();

void register_node_type_geo_group();

void register_node_type_geo_accumulate_field();
void register_node_type_geo_attribute_capture();
void register_node_type_geo_attribute_domain_size();
void register_node_type_geo_attribute_separate_xyz();
void register_node_type_geo_attribute_statistic();
void register_node_type_geo_blur_attribute();
void register_node_type_geo_boolean();
void register_node_type_geo_bounding_box();
void register_node_type_geo_collection_info();
void register_node_type_geo_convex_hull();
void register_node_type_geo_curve_endpoint_selection();
void register_node_type_geo_curve_fill();
void register_node_type_geo_curve_fillet();
void register_node_type_geo_curve_handle_type_selection();
void register_node_type_geo_curve_length();
void register_node_type_geo_curve_primitive_arc();
void register_node_type_geo_curve_primitive_bezier_segment();
void register_node_type_geo_curve_primitive_circle();
void register_node_type_geo_curve_primitive_line();
void register_node_type_geo_curve_primitive_quadratic_bezier();
void register_node_type_geo_curve_primitive_quadrilateral();
void register_node_type_geo_curve_primitive_spiral();
void register_node_type_geo_curve_primitive_star();
void register_node_type_geo_curve_resample();
void register_node_type_geo_curve_reverse();
void register_node_type_geo_curve_sample();
void register_node_type_geo_curve_set_handle_type();
void register_node_type_geo_curve_spline_parameter();
void register_node_type_geo_curve_spline_type();
void register_node_type_geo_curve_subdivide();
void register_node_type_geo_curve_to_mesh();
void register_node_type_geo_curve_to_points();
void register_node_type_geo_curve_topology_curve_of_point();
void register_node_type_geo_curve_topology_points_of_curve();
void register_node_type_geo_curve_trim();
void register_node_type_geo_deform_curves_on_surface();
void register_node_type_geo_delete_geometry();
void register_node_type_geo_distribute_points_in_volume();
void register_node_type_geo_distribute_points_on_faces();
void register_node_type_geo_dual_mesh();
void register_node_type_geo_duplicate_elements();
void register_node_type_geo_edge_paths_to_curves();
void register_node_type_geo_edge_paths_to_selection();
void register_node_type_geo_edge_split();
void register_node_type_geo_edges_to_face_groups();
void register_node_type_geo_evaluate_at_index();
void register_node_type_geo_evaluate_on_domain();
void register_node_type_geo_extrude_mesh();
void register_node_type_geo_flip_faces();
void register_node_type_geo_geometry_to_instance();
void register_node_type_geo_image_info();
void register_node_type_geo_image_texture();
void register_node_type_geo_image();
void register_node_type_geo_input_curve_handles();
void register_node_type_geo_input_curve_tilt();
void register_node_type_geo_input_id();
void register_node_type_geo_input_index();
void register_node_type_geo_input_instance_rotation();
void register_node_type_geo_input_instance_scale();
void register_node_type_geo_input_material_index();
void register_node_type_geo_input_material();
void register_node_type_geo_input_mesh_edge_angle();
void register_node_type_geo_input_mesh_edge_neighbors();
void register_node_type_geo_input_mesh_edge_vertices();
void register_node_type_geo_input_mesh_face_area();
void register_node_type_geo_input_mesh_face_is_planar();
void register_node_type_geo_input_mesh_face_neighbors();
void register_node_type_geo_input_mesh_island();
void register_node_type_geo_input_mesh_vertex_neighbors();
void register_node_type_geo_input_named_attribute();
void register_node_type_geo_input_normal();
void register_node_type_geo_input_position();
void register_node_type_geo_input_radius();
void register_node_type_geo_input_scene_time();
void register_node_type_geo_input_shade_smooth();
void register_node_type_geo_input_shortest_edge_paths();
void register_node_type_geo_input_spline_cyclic();
void register_node_type_geo_input_spline_length();
void register_node_type_geo_input_spline_resolution();
void register_node_type_geo_input_tangent();
void register_node_type_geo_instance_on_points();
void register_node_type_geo_instances_to_points();
void register_node_type_geo_interpolate_curves();
void register_node_type_geo_is_viewport();
void register_node_type_geo_join_geometry();
void register_node_type_geo_material_replace();
void register_node_type_geo_material_selection();
void register_node_type_geo_merge_by_distance();
void register_node_type_geo_mean_filter_sdf_volume();
void register_node_type_geo_mesh_face_group_boundaries();
void register_node_type_geo_mesh_primitive_circle();
void register_node_type_geo_mesh_primitive_cone();
void register_node_type_geo_mesh_primitive_cube();
void register_node_type_geo_mesh_primitive_cylinder();
void register_node_type_geo_mesh_primitive_grid();
void register_node_type_geo_mesh_primitive_ico_sphere();
void register_node_type_geo_mesh_primitive_line();
void register_node_type_geo_mesh_primitive_uv_sphere();
void register_node_type_geo_mesh_subdivide();
void register_node_type_geo_mesh_to_curve();
void register_node_type_geo_mesh_to_points();
void register_node_type_geo_mesh_to_sdf_volume();
void register_node_type_geo_mesh_to_volume();
void register_node_type_geo_mesh_topology_corners_of_face();
void register_node_type_geo_mesh_topology_corners_of_vertex();
void register_node_type_geo_mesh_topology_edges_of_corner();
void register_node_type_geo_mesh_topology_edges_of_vertex();
void register_node_type_geo_mesh_topology_face_of_corner();
void register_node_type_geo_mesh_topology_offset_corner_in_face();
void register_node_type_geo_mesh_topology_vertex_of_corner();
void register_node_type_geo_object_info();
void register_node_type_geo_offset_point_in_curve();
void register_node_type_geo_offset_sdf_volume();
void register_node_type_geo_points_to_vertices();
void register_node_type_geo_points_to_sdf_volume();
void register_node_type_geo_points_to_volume();
void register_node_type_geo_points();
void register_node_type_geo_proximity();
void register_node_type_geo_raycast();
void register_node_type_geo_realize_instances();
void register_node_type_geo_remove_attribute();
void register_node_type_geo_rotate_instances();
void register_node_type_geo_sample_index();
void register_node_type_geo_sample_nearest_surface();
void register_node_type_geo_sample_nearest();
void register_node_type_geo_sample_uv_surface();
void register_node_type_geo_scale_elements();
void register_node_type_geo_scale_instances();
void register_node_type_geo_sdf_volume_sphere();
void register_node_type_geo_select_by_handle_type();
void register_node_type_geo_self_object();
void register_node_type_geo_separate_components();
void register_node_type_geo_separate_geometry();
void register_node_type_geo_set_curve_handles();
void register_node_type_geo_set_curve_normal();
void register_node_type_geo_set_curve_radius();
void register_node_type_geo_set_curve_tilt();
void register_node_type_geo_set_id();
void register_node_type_geo_set_material_index();
void register_node_type_geo_set_material();
void register_node_type_geo_set_point_radius();
void register_node_type_geo_set_position();
void register_node_type_geo_set_shade_smooth();
void register_node_type_geo_set_spline_cyclic();
void register_node_type_geo_set_spline_resolution();
void register_node_type_geo_simulation_input();
void register_node_type_geo_simulation_output();
void register_node_type_geo_store_named_attribute();
void register_node_type_geo_string_join();
void register_node_type_geo_string_to_curves();
void register_node_type_geo_subdivision_surface();
void register_node_type_geo_switch();
void register_node_type_geo_transform_geometry();
void register_node_type_geo_translate_instances();
void register_node_type_geo_triangulate();
void register_node_type_geo_uv_pack_islands();
void register_node_type_geo_uv_unwrap();
void register_node_type_geo_viewer();
void register_node_type_geo_volume_cube();
void register_node_type_geo_volume_to_mesh();


/**
 * Scatter pass: Use sprites to scatter the color of very bright pixel to have higher quality blur.
 *
 * We only scatter one triangle per sprite and one sprite per 4 pixels to reduce vertex shader
 * invocations and overdraw.
 **/

#pragma BLENDER_REQUIRE(eevee_depth_of_field_scatter_lib.glsl)

layout(std140) uniform dof_block
{
  DepthOfFieldData dof;
};

uniform sampler2D color_tx;
uniform sampler2D coc_tx;

/* Load 4 Circle of confusion values. texel_co is centered around the 4 taps. */
vec4 fetch_cocs(vec2 texel_co)
{
  /* TODO(@fclem): The `textureGather(sampler, co, comp)` variant isn't here on some
   * implementations.
   */
#if 0  // GPU_ARB_texture_gather
  vec2 uvs = texel_co / vec2(textureSize(coc_tx, 0));
  /* Reminder: Samples order is CW starting from top left. */
  cocs = textureGather(coc_tx, uvs, isForegroundPass ? 0 : 1);
#else
  ivec2 texel = ivec2(texel_co - 0.5);
  vec4 cocs;
  cocs.x = texelFetchOffset(coc_tx, texel, 0, ivec2(0, 1)).r;
  cocs.y = texelFetchOffset(coc_tx, texel, 0, ivec2(1, 1)).r;
  cocs.z = texelFetchOffset(coc_tx, texel, 0, ivec2(1, 0)).r;
  cocs.w = texelFetchOffset(coc_tx, texel, 0, ivec2(0, 0)).r;
#endif

  if (is_foreground) {
    cocs *= -1.0;
  }

  cocs = max(vec4(0.0), cocs);
  /* We are scattering at half resolution, so divide CoC by 2. */
  return cocs * 0.5;
}

void vertex_discard()
{
  /* Don't produce any fragments */
  gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}

void main()
{
  ivec2 tex_size = textureSize(coc_tx, 0);

  int t_id = gl_VertexID / 3; /* Triangle Id */

  /* Some math to get the target pixel. */
  ivec2 texelco = ivec2(t_id % dof.scatter_sprite_per_row, t_id / dof.scatter_sprite_per_row) * 2;

  /* Center sprite around the 4 texture taps. */
  spritepos = vec2(texelco) + 1.0;

  cocs = fetch_cocs(spritepos);

  /* Early out from local CoC radius. */
  if (all(lessThan(cocs, vec4(0.5)))) {
    vertex_discard();
    return;
  }

  vec2 input_texel_size = 1.0 / vec2(tex_size);
  vec2 quad_center = spritepos * input_texel_size;
  vec4 colors[4];
  bool no_color = true;
  for (int i = 0; i < 4; i++) {
    vec2 sample_uv = quad_center + quad_offsets[i] * input_texel_size;

    colors[i] = textureLod(color_tx, sample_uv, 0.0);
    no_color = no_color && all(equal(colors[i].rgb, vec3(0.0)));
  }

  /* Early out from no color to scatter. */
  if (no_color) {
    vertex_discard();
    return;
  }

  weights = dof_layer_weight(cocs) * dof_sample_weight(cocs);
  /* Filter NaNs. */
  weights = mix(weights, vec4(0.0), equal(cocs, vec4(0.0)));

  color1 = colors[0] * weights[0];
  color2 = colors[1] * weights[1];
  color3 = colors[2] * weights[2];
  color4 = colors[3] * weights[3];

  /* Extend to cover at least the unit circle */
  const float extend = (cos(M_PI / 4.0) + 1.0) * 2.0;
  /* Crappy diagram
   * ex 1
   *    | \
   *    |   \
   *  1 |     \
   *    |       \
   *    |         \
   *  0 |     x     \
   *    |   Circle    \
   *    |   Origin      \
   * -1 0 --------------- 2
   *   -1     0     1     ex
   */

  /* Generate Triangle : less memory fetches from a VBO */
  int v_id = gl_VertexID % 3;                     /* Vertex Id */
  gl_Position.x = float(v_id / 2) * extend - 1.0; /* int divisor round down */
  gl_Position.y = float(v_id % 2) * extend - 1.0;
  gl_Position.z = 0.0;
  gl_Position.w = 1.0;

  spritesize = max_v4(cocs);

  /* Add 2.5 to max_coc because the max_coc may not be centered on the sprite origin
   * and because we smooth the bokeh shape a bit in the pixel shader. */
  gl_Position.xy *= spritesize * dof.bokeh_anisotropic_scale + 2.5;
  /* Position the sprite. */
  gl_Position.xy += spritepos;
  /* NDC range [-1..1]. */
  gl_Position.xy = gl_Position.xy * dof.texel_size * 2.0 - 1.0;

  /* Add 2.5 for the same reason but without the ratio. */
  spritesize += 2.5;
}

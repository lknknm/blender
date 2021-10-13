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
 * Copyright 2011, Blender Foundation.
 */

#include "COM_ColorSpillOperation.h"
#define AVG(a, b) ((a + b) / 2)

namespace blender::compositor {

ColorSpillOperation::ColorSpillOperation()
{
  addInputSocket(DataType::Color);
  addInputSocket(DataType::Value);
  addOutputSocket(DataType::Color);

  inputImageReader_ = nullptr;
  inputFacReader_ = nullptr;
  spillChannel_ = 1; /* GREEN */
  spillMethod_ = 0;
  flags.can_be_constant = true;
}

void ColorSpillOperation::initExecution()
{
  inputImageReader_ = this->getInputSocketReader(0);
  inputFacReader_ = this->getInputSocketReader(1);
  if (spillChannel_ == 0) {
    rmut_ = -1.0f;
    gmut_ = 1.0f;
    bmut_ = 1.0f;
    channel2_ = 1;
    channel3_ = 2;
    if (settings_->unspill == 0) {
      settings_->uspillr = 1.0f;
      settings_->uspillg = 0.0f;
      settings_->uspillb = 0.0f;
    }
  }
  else if (spillChannel_ == 1) {
    rmut_ = 1.0f;
    gmut_ = -1.0f;
    bmut_ = 1.0f;
    channel2_ = 0;
    channel3_ = 2;
    if (settings_->unspill == 0) {
      settings_->uspillr = 0.0f;
      settings_->uspillg = 1.0f;
      settings_->uspillb = 0.0f;
    }
  }
  else {
    rmut_ = 1.0f;
    gmut_ = 1.0f;
    bmut_ = -1.0f;

    channel2_ = 0;
    channel3_ = 1;
    if (settings_->unspill == 0) {
      settings_->uspillr = 0.0f;
      settings_->uspillg = 0.0f;
      settings_->uspillb = 1.0f;
    }
  }
}

void ColorSpillOperation::deinitExecution()
{
  inputImageReader_ = nullptr;
  inputFacReader_ = nullptr;
}

void ColorSpillOperation::executePixelSampled(float output[4],
                                              float x,
                                              float y,
                                              PixelSampler sampler)
{
  float fac[4];
  float input[4];
  inputFacReader_->readSampled(fac, x, y, sampler);
  inputImageReader_->readSampled(input, x, y, sampler);
  float rfac = MIN2(1.0f, fac[0]);
  float map;

  switch (spillMethod_) {
    case 0: /* simple */
      map = rfac * (input[spillChannel_] - (settings_->limscale * input[settings_->limchan]));
      break;
    default: /* average */
      map = rfac * (input[spillChannel_] -
                    (settings_->limscale * AVG(input[channel2_], input[channel3_])));
      break;
  }

  if (map > 0.0f) {
    output[0] = input[0] + rmut_ * (settings_->uspillr * map);
    output[1] = input[1] + gmut_ * (settings_->uspillg * map);
    output[2] = input[2] + bmut_ * (settings_->uspillb * map);
    output[3] = input[3];
  }
  else {
    copy_v4_v4(output, input);
  }
}

void ColorSpillOperation::update_memory_buffer_partial(MemoryBuffer *output,
                                                       const rcti &area,
                                                       Span<MemoryBuffer *> inputs)
{
  for (BuffersIterator<float> it = output->iterate_with(inputs, area); !it.is_end(); ++it) {
    const float *color = it.in(0);
    const float factor = MIN2(1.0f, *it.in(1));

    float map;
    switch (spillMethod_) {
      case 0: /* simple */
        map = factor * (color[spillChannel_] - (settings_->limscale * color[settings_->limchan]));
        break;
      default: /* average */
        map = factor * (color[spillChannel_] -
                        (settings_->limscale * AVG(color[channel2_], color[channel3_])));
        break;
    }

    if (map > 0.0f) {
      it.out[0] = color[0] + rmut_ * (settings_->uspillr * map);
      it.out[1] = color[1] + gmut_ * (settings_->uspillg * map);
      it.out[2] = color[2] + bmut_ * (settings_->uspillb * map);
      it.out[3] = color[3];
    }
    else {
      copy_v4_v4(it.out, color);
    }
  }
}

}  // namespace blender::compositor

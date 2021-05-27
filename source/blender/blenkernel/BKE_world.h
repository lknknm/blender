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
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 */
#pragma once

/** \file
 * \ingroup bke
 */

#ifdef __cplusplus
extern "C" {
#endif

struct Depsgraph;
struct Main;
struct World;

struct World *BKE_world_add(struct Main *bmain, const char *name);
void BKE_world_eval(struct Depsgraph *depsgraph, struct World *world);

struct World *BKE_world_default(void);

void BKE_world_defaults_free_gpu(void);

/* Module */

void BKE_worlds_init(void);
void BKE_worlds_exit(void);

#ifdef __cplusplus
}
#endif

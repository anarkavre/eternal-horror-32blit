// Eternal Horror
// Copyright(C) 2020 John D. Corrado
// Planes based on Doom visplane code
// Copyright(C) 1993-1996 Id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#ifndef __FIXED_H__
#define __FIXED_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define FRACBITS 16
#define FRACUNIT (1 << FRACBITS)

#define ANGLES 256
#define ANGLESMASK (ANGLES - 1)

typedef int32_t fixed_t;
typedef uint32_t angle_t;

fixed_t fixedSin(angle_t a);
fixed_t fixedCos(angle_t a);
fixed_t fixedTan(angle_t a);
fixed_t fixedCot(angle_t a);
fixed_t fixedMul(fixed_t a, fixed_t b);
fixed_t fixedDiv(fixed_t a, fixed_t b);

#ifdef __cplusplus
}
#endif

#endif
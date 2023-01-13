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

#include <limits.h>
#include <stdint.h>

#include "fixed.h"

const fixed_t sinTable[ANGLES >> 2] =
{
	804, 2412, 4018, 5622, 7223, 8819, 10410, 11995, 13573, 15142, 16702, 18253, 19792, 21319, 22833, 24334,
	25820, 27291, 28745, 30181, 31600, 32999, 34379, 35738, 37075, 38390, 39682, 40950, 42194, 43412, 44603, 45768,
	46906, 48015, 49095, 50146, 51166, 52155, 53114, 54040, 54933, 55794, 56621, 57414, 58172, 58895, 59583, 60235,
	60850, 61429, 61971, 62475, 62942, 63371, 63762, 64115, 64428, 64703, 64939, 65136, 65294, 65412, 65491, 65531
};

const fixed_t tanTable[ANGLES >> 2] =
{
	804, 2413, 4026, 5643, 7267, 8900, 10544, 12201, 13874, 15563, 17273, 19005, 20761, 22545, 24360, 26208,
	28092, 30017, 31986, 34002, 36070, 38195, 40382, 42635, 44962, 47369, 49862, 52451, 55143, 57949, 60879, 63946,
	67164, 70548, 74115, 77886, 81884, 86135, 90669, 95522, 100736, 106358, 112446, 119071, 126313, 134275, 143081, 152883,
	163877, 176309, 190499, 206869, 225990, 248647, 275959, 309567, 351993, 407305, 482534, 590957, 761030, 1066729, 1779313, 5340085,
};

fixed_t fixedSin(angle_t a)
{
	const uint32_t quadrant = (((a & ANGLESMASK) & 0xC0) >> 6);
	const uint32_t index = (((a & ANGLESMASK) & 0x3F) >> 0);
	switch (quadrant)
	{
	case 0: return sinTable[index];
	case 1: return sinTable[63 - index];
	case 2: return -sinTable[index];
	case 3: return -sinTable[63 - index];
	default: return 0;
	}
}

fixed_t fixedCos(angle_t a)
{
	const uint32_t quadrant = (((a & ANGLESMASK) & 0xC0) >> 6);
	const uint32_t index = (((a & ANGLESMASK) & 0x3F) >> 0);
	switch (quadrant)
	{
	case 0: return sinTable[63 - index];
	case 1: return -sinTable[index];
	case 2: return -sinTable[63 - index];
	case 3: return sinTable[index];
	default: return 0;
	}
}

fixed_t fixedTan(angle_t a)
{
	const uint32_t quadrant = (((a & ANGLESMASK) & 0xC0) >> 6);
	const uint32_t index = (((a & ANGLESMASK) & 0x3F) >> 0);
	switch (quadrant)
	{
	case 0: case 2: return tanTable[index];
	case 1: case 3: return -tanTable[63 - index];
	default: return 0;
	}
}

fixed_t fixedCot(angle_t a)
{
	const uint8_t quadrant = (((a & ANGLESMASK) & 0xC0) >> 6);
	const uint8_t index = (((a & ANGLESMASK) & 0x3F) >> 0);
	switch (quadrant)
	{
	case 0: case 2: return tanTable[63 - index];
	case 1: case 3: return -tanTable[index];
	default: return 0;
	}
}

fixed_t fixedMul(fixed_t a, fixed_t b)
{
	int64_t result = ((int64_t) a * (int64_t) b) >> FRACBITS;
	return (result < INT_MIN ? INT_MIN : result > INT_MAX ? INT_MAX : (fixed_t) result);
}

fixed_t fixedDiv(fixed_t a, fixed_t b)
{
	if (b == 0)
		return (a < 0 ? INT_MIN : INT_MAX);
	int64_t result = ((int64_t) a << FRACBITS) / b;
	return (result < INT_MIN ? INT_MIN : result > INT_MAX ? INT_MAX : (fixed_t) result);
}
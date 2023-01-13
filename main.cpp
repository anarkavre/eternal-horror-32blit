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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <memory>

#include "32blit.hpp"
#include "audio/mp3-stream.hpp"

#include "assets.hpp"

using namespace blit;

#include "fixed.h"

typedef struct
{
	int32_t mapIndex;
	uint32_t state;
	fixed_t offset;
	uint32_t tics;
} door_t;

typedef struct
{
	int32_t mapIndex;
	uint32_t type;
	uint32_t state;
	int32_t health;
	int32_t gridX;
	int32_t gridY;
	uint32_t damageTics;
	uint32_t attackTics;
	uint32_t render;
	uint32_t damage;
} enemy_t;

typedef struct
{
	uint32_t type;
	int32_t gridX;
	int32_t gridY;
	uint32_t render;
} health_t;

typedef struct
{
	int32_t minX;
	int32_t maxX;
	uint32_t pad1;
	uint32_t top[64];
	uint32_t pad2;
} plane_t;

int32_t mapWidth;
int32_t mapHeight;

uint32_t *mapData;
std::unique_ptr<uint32_t> mapDataPtr;

uint32_t level = 1;
const uint32_t numLevels = 4;
const char *levels[] =
{
	"level1.map",
	"level2.map",
	"level3.map",
	"level4.map"
};
const char *music[] =
{
	"music1.mp3",
	"music2.mp3"
};

fixed_t cameraX;
fixed_t cameraY;
angle_t cameraAngle;
fixed_t oldCameraX;
fixed_t oldCameraY;
uint32_t state = 2;
int32_t health;
uint32_t attackTics = 0;

bool moveForwardPressed = false;
bool moveBackwardPressed = false;
bool strafeLeftPressed = false;
bool strafeRightPressed = false;
bool turnLeftPressed = false;
bool turnRightPressed = false;
bool fireWeaponPressed = false;
bool restartLevelPressed = false;

door_t doors[64] =
{
	{ -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 },
	{ -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 },
	{ -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 },
	{ -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 },
	{ -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 },
	{ -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 },
	{ -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 },
	{ -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }, { -1, 0, 64 << FRACBITS, 0 }
};

enemy_t enemies[64] =
{
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

health_t healths[64] =
{
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
};

uint8_t *yTable[64];
uint32_t xTable[64];

plane_t plane;

uint32_t start[32];
uint32_t stop[32];
fixed_t currentX[32];
fixed_t currentY[32];
fixed_t stepX[32];
fixed_t stepY[32];

fixed_t zBuffer[64];

std::unique_ptr<uint8_t> pixels;
std::unique_ptr<Pen> palettes[5];
std::unique_ptr<Surface> surface;
std::unique_ptr<Surface> textures;
std::unique_ptr<Surface> sprites;
std::unique_ptr<Surface> hand;
std::unique_ptr<Surface> screens;
MP3Stream stream;

bool evenUpdate = false;
uint32_t lastTime;

uint32_t frames[6] = { 0, 4096, 8192, 12288, 16384, 20480 };
uint32_t frame = 0;
uint32_t frameTics = 0;

uint32_t bloodHeight[64];
uint32_t bloodSpeed[64];
uint32_t bloodTics = 0;

void DrawWallSlice(Surface *surface, const uint8_t *texture, uint32_t textureOffsetX, int32_t wallX, int32_t wallY, uint32_t wallHeight)
{
	uint32_t count;
	fixed_t textureOffsetY;
	fixed_t scalar = fixedDiv(64 << FRACBITS, wallHeight << FRACBITS);
	texture = &texture[textureOffsetX * 64];

	if (wallY < 0)
	{
		count = 63;
		textureOffsetY = -wallY * scalar;
		wallY = 0;
	}
	else
	{
		count = wallHeight - 1;
		textureOffsetY = 0;
	}

	uint8_t *p = yTable[wallY] + xTable[wallX];

	do
	{
		*p = 137 + texture[textureOffsetY >> FRACBITS];
		p += surface->bounds.w;
		textureOffsetY += scalar;
	} while (count--);
}

void DrawSprite(Surface *surface, const uint8_t *sprite, int32_t spriteX, int32_t spriteY, uint32_t spriteSize, fixed_t spriteDistance, uint32_t spriteDamage)
{
	if (spriteX + (int32_t)spriteSize <= 0 || spriteX > 63)
		return;

	uint32_t countX;
	uint32_t countY;
	uint32_t yCount;
	fixed_t spriteOffsetX;
	fixed_t spriteOffsetY;
	fixed_t ySpriteOffset;
	fixed_t scalar = fixedDiv(64 << FRACBITS, spriteSize << FRACBITS);

	if (spriteX < 0)
	{
		countX = spriteSize + spriteX - 1;
		spriteOffsetX = -spriteX * scalar;
		spriteX = 0;
	}
	else
	{
		countX = (spriteX + spriteSize > 63 ? 64 - spriteX : spriteSize) - 1;
		spriteOffsetX = 0;
	}

	const uint8_t *spriteColumn = &sprite[(spriteOffsetX >> FRACBITS) * 64];

	if (spriteY < 0)
	{
		countY = yCount = 63;
		spriteOffsetY = ySpriteOffset = -spriteY * scalar;
		spriteY = 0;
	}
	else
	{
		countY = yCount = spriteSize - 1;
		spriteOffsetY = ySpriteOffset = 0;
	}

	uint8_t *p = yTable[spriteY] + xTable[spriteX];
	uint8_t *temp = p;

	do
	{
		if (spriteDistance < zBuffer[spriteX++])
		{
			do
			{
				uint8_t color = spriteColumn[spriteOffsetY >> FRACBITS];
				if (color != 0)
					*p = spriteDamage ? 29 : color;
				p += surface->bounds.w;
				spriteOffsetY += scalar;
			} while (countY--);
		}

		spriteOffsetX += scalar;
		spriteColumn = &sprite[(spriteOffsetX >> FRACBITS) * 64];
		countY = yCount;
		spriteOffsetY = ySpriteOffset;
		p = ++temp;
	} while (countX--);
}

void update(uint32_t time)
{
	if (!stream.get_playing())
	{
		stream.load(music[(level - 1) % 2]);
		stream.play(0);
	}

	stream.update();

	if (buttons.pressed & Button::A)
		restartLevelPressed = true;

	evenUpdate = !evenUpdate;

	if (!evenUpdate)
		return;

	uint32_t currentTime = time;
	uint32_t elapsedTime = currentTime - lastTime;
	lastTime = currentTime;

	moveForwardPressed = buttons & Button::DPAD_UP;
	moveBackwardPressed = buttons & Button::DPAD_DOWN;
	strafeLeftPressed = buttons & Button::B && buttons & Button::DPAD_LEFT;
	strafeRightPressed = buttons & Button::B && buttons & Button::DPAD_RIGHT;
	turnLeftPressed = !(buttons & Button::B) && buttons & Button::DPAD_LEFT;
	turnRightPressed = !(buttons & Button::B) && buttons & Button::DPAD_RIGHT;
	fireWeaponPressed = buttons & Button::A;

	if (state == 1)
	{
		oldCameraX = cameraX;
		oldCameraY = cameraY;

		if (moveForwardPressed)
		{
			cameraX += fixedMul(279620, fixedCos(cameraAngle));
			cameraY -= fixedMul(279620, fixedSin(cameraAngle));
		}

		if (moveBackwardPressed)
		{
			cameraX -= fixedMul(279620, fixedCos(cameraAngle));
			cameraY += fixedMul(279620, fixedSin(cameraAngle));
		}

		if (strafeLeftPressed)
		{
			cameraX += fixedMul(279620, fixedCos(cameraAngle + 64));
			cameraY -= fixedMul(279620, fixedSin(cameraAngle + 64));
		}

		if (strafeRightPressed)
		{
			cameraX -= fixedMul(279620, fixedCos(cameraAngle + 64));
			cameraY += fixedMul(279620, fixedSin(cameraAngle + 64));
		}

		bool fireWeapon = false;

		if (fireWeaponPressed)
		{
			attackTics++;

			if (attackTics > 30)
			{
				fireWeapon = true;
				attackTics = 0;
			}
		}

		if (turnLeftPressed)
			cameraAngle = (cameraAngle + 2) & ANGLESMASK;

		if (turnRightPressed)
			cameraAngle = (cameraAngle - 2) & ANGLESMASK;

		uint32_t tx = cameraX >> 22;
		uint32_t txm = (cameraX - (5 << FRACBITS)) >> 22;
		uint32_t txp = (cameraX + (5 << FRACBITS)) >> 22;
		uint32_t ty = cameraY >> 22;
		uint32_t tym = (cameraY - (5 << FRACBITS)) >> 22;
		uint32_t typ = (cameraY + (5 << FRACBITS)) >> 22;

		if (cameraX - (5 << FRACBITS) < 0 || (cameraX + (5 << FRACBITS)) >> 22 >= mapWidth)
			cameraX = oldCameraX;
		else if (mapData[ty * mapWidth + txp] == 1 || mapData[ty * mapWidth + txm] == 1 || mapData[ty * mapWidth + txp] == 3 || mapData[ty * mapWidth + txm] == 3 || mapData[ty * mapWidth + txp] == 4 || mapData[ty * mapWidth + txm] == 4)
			cameraX = oldCameraX;
		else
		{
			if (mapData[typ * mapWidth + tx] == 1 || mapData[typ * mapWidth + tx] == 3 || mapData[typ * mapWidth + tx] == 4)
				cameraY = (typ << 22) - (5 << FRACBITS);

			if (mapData[tym * mapWidth + tx] == 1 || mapData[tym * mapWidth + tx] == 3 || mapData[tym * mapWidth + tx] == 4)
				cameraY = (tym << 22) + (69 << FRACBITS);
		}

		if (cameraY - (5 << FRACBITS) < 0 || (cameraY + (5 << FRACBITS)) >> 22 >= mapHeight)
			cameraY = oldCameraY;
		else if (mapData[typ * mapWidth + tx] == 1 || mapData[tym * mapWidth + tx] == 1 || mapData[typ * mapWidth + tx] == 3 || mapData[tym * mapWidth + tx] == 3 || mapData[typ * mapWidth + tx] == 4 || mapData[tym * mapWidth + tx] == 4)
			cameraY = oldCameraY;
		else
		{
			if (mapData[ty * mapWidth + txp] == 1 || mapData[ty * mapWidth + txp] == 3 || mapData[ty * mapWidth + txp] == 4)
				cameraX = (txp << 22) - (5 << FRACBITS);

			if (mapData[ty * mapWidth + txm] == 1 || mapData[ty * mapWidth + txm] == 3 || mapData[ty * mapWidth + txm] == 4)
				cameraX = (txm << 22) + (69 << FRACBITS);
		}

		int32_t mapIndex = ty * mapWidth + tx;

		if (mapData[mapIndex] == 5)
		{
			if (health < 100)
			{
				mapData[mapIndex] = 0;

				health += 10;

				if (health > 100)
					health = 100;
			}
		}
		else if (mapData[mapIndex] == 6)
		{
			if (health < 100)
			{
				mapData[mapIndex] = 0;

				health += 25;

				if (health > 100)
					health = 100;
			}
		}
		else if (mapData[mapIndex] == 8)
		{
			level++;

			if (level <= numLevels)
			{
				File file;
				file.open(levels[level - 1], OpenMode::read);
				uint32_t offset = 0;
				int32_t cameraGridX;
				int32_t cameraGridY;
				int32_t exitGridX;
				int32_t exitGridY;
				file.read(offset, sizeof(int32_t), (char *)&cameraGridX);
				offset += sizeof(int32_t);
				file.read(offset, sizeof(int32_t), (char *)&cameraGridY);
				offset += sizeof(int32_t);
				cameraX = (cameraGridX * 64 + 32) << FRACBITS;
				cameraY = (cameraGridY * 64 + 32) << FRACBITS;
				file.read(offset, sizeof(angle_t), (char *)&cameraAngle);
				offset += sizeof(angle_t);
				file.read(offset, sizeof(int32_t), (char *)&exitGridX);
				offset += sizeof(int32_t);
				file.read(offset, sizeof(int32_t), (char *)&exitGridY);
				offset += sizeof(int32_t);
				file.read(offset, sizeof(int32_t), (char *)&mapWidth);
				offset += sizeof(int32_t);
				file.read(offset, sizeof(int32_t), (char *)&mapHeight);
				offset += sizeof(int32_t);
				mapData = new uint32_t[mapWidth * mapHeight];
				mapDataPtr = std::unique_ptr<uint32_t>(mapData);
				file.read(offset, mapWidth * mapHeight * sizeof(uint32_t), (char *)mapData);
				file.close();

				stream.load(music[(level - 1) % 2]);
				stream.play(0);
			}
			else
			{
				level = 1;
				state = 4;
			}
		}

		mapIndex = (ty - 1) * mapWidth + tx;

		if (mapData[mapIndex] == 2)
		{
			door_t *door = &doors[(((ty - 1) & 7) << 3) + (tx & 7)];

			if (door->mapIndex != mapIndex)
			{
				door->mapIndex = mapIndex;
				door->state = 0;
				door->offset = 64 << FRACBITS;
				door->tics = 0;
			}

			if (door->state == 0 || door->state == 1)
			{
				if (door->mapIndex == (ty * mapWidth + txp) || door->mapIndex == (ty * mapWidth + txm))
					cameraX = oldCameraX;

				if (door->mapIndex == (typ * mapWidth + tx) || door->mapIndex == (tym * mapWidth + tx))
					cameraY = oldCameraY;
			}
		}
		else if (mapData[mapIndex] == 3 || mapData[mapIndex] == 4)
		{
			enemy_t *enemy1 = &enemies[(((ty - 1) & 7) << 3) + (tx & 7)];

			if (enemy1->mapIndex != mapIndex)
			{
				enemy1->mapIndex = mapIndex;
				enemy1->type = mapData[mapIndex];
				enemy1->state = 1;
				enemy1->health = 100;
				enemy1->damageTics = 0;
				enemy1->attackTics = 0;
				enemy1->damage = 0;
			}

			if (mapData[(ty + 1) * mapWidth + tx] == 0)
			{
				mapData[(ty + 1) * mapWidth + tx] = 4;

				enemy_t *enemy2 = &enemies[(((ty + 1) & 7) << 3) + (tx & 7)];
				enemy2->mapIndex = (ty + 1) * mapWidth + tx;
				enemy2->type = 1;
				enemy2->state = 1;
				enemy2->health = 100;
				enemy2->damageTics = 0;
				enemy2->attackTics = 0;
				enemy2->damage = 0;

				if (rand() % 2 == 0)
					enemy1->state = 2;
				else
					enemy2->state = 2;
			}

			if (fireWeapon && enemy1->state == 2 && cameraAngle >= 32 && cameraAngle < 96)
			{
				enemy1->health -= 25;
				enemy1->damage = 1;

				if (enemy1->health <= 0)
				{
					enemy1->state = 0;
					enemy1->health = 0;
				}
			}

			if (enemy1->state == 2)
			{
				enemy1->attackTics++;

				if (enemy1->attackTics > 60)
				{
					health -= 10;

					if (health <= 0)
					{
						state = 0;
						health = 0;
					}

					enemy1->attackTics = 0;
				}
			}

			if (enemy1->damage == 1)
			{
				enemy1->damageTics++;

				if (enemy1->damageTics > 15)
				{
					enemy1->damageTics = 0;
					enemy1->damage = 0;

					if (enemy1->state == 0)
					{
						mapData[mapIndex] = 7;
						enemy1->mapIndex = -1;
						enemy1->type = 0;
						enemy1->gridX = 0;
						enemy1->gridY = 0;
						enemy_t *enemy2 = &enemies[(((ty + 1) & 7) << 3) + (tx & 7)];
						enemy2->state = 2;
					}
				}
			}
		}

		mapIndex = (ty + 1) * mapWidth + tx;

		if (mapData[mapIndex] == 2)
		{
			door_t *door = &doors[(((ty + 1) & 7) << 3) + (tx & 7)];

			if (door->mapIndex != mapIndex)
			{
				door->mapIndex = mapIndex;
				door->state = 0;
				door->offset = 64 << FRACBITS;
				door->tics = 0;
			}

			if (door->state == 0 || door->state == 1)
			{
				if (door->mapIndex == (ty * mapWidth + txp) || door->mapIndex == (ty * mapWidth + txm))
					cameraX = oldCameraX;

				if (door->mapIndex == (typ * mapWidth + tx) || door->mapIndex == (tym * mapWidth + tx))
					cameraY = oldCameraY;
			}
		}
		else if (mapData[mapIndex] == 3 || mapData[mapIndex] == 4)
		{
			enemy_t *enemy1 = &enemies[(((ty + 1) & 7) << 3) + (tx & 7)];

			if (enemy1->mapIndex != mapIndex)
			{
				enemy1->mapIndex = mapIndex;
				enemy1->type = mapData[mapIndex];
				enemy1->state = 1;
				enemy1->health = 100;
				enemy1->damageTics = 0;
				enemy1->attackTics = 0;
				enemy1->damage = 0;
			}

			if (mapData[(ty - 1) * mapWidth + tx] == 0)
			{
				mapData[(ty - 1) * mapWidth + tx] = 4;

				enemy_t *enemy2 = &enemies[(((ty - 1) & 7) << 3) + (tx & 7)];
				enemy2->mapIndex = (ty - 1) * mapWidth + tx;
				enemy2->type = 1;
				enemy2->state = 1;
				enemy2->health = 100;
				enemy2->damageTics = 0;
				enemy2->attackTics = 0;
				enemy2->damage = 0;

				if (rand() % 2 == 0)
					enemy1->state = 2;
				else
					enemy2->state = 2;
			}

			if (fireWeapon && enemy1->state == 2 && cameraAngle >= 160 && cameraAngle < 224)
			{
				enemy1->health -= 25;
				enemy1->damage = 1;

				if (enemy1->health <= 0)
				{
					enemy1->state = 0;
					enemy1->health = 0;
				}
			}

			if (enemy1->state == 2)
			{
				enemy1->attackTics++;

				if (enemy1->attackTics > 60)
				{
					health -= 10;

					if (health <= 0)
					{
						state = 0;
						health = 0;
					}

					enemy1->attackTics = 0;
				}
			}

			if (enemy1->damage == 1)
			{
				enemy1->damageTics++;

				if (enemy1->damageTics > 15)
				{
					enemy1->damageTics = 0;
					enemy1->damage = 0;

					if (enemy1->state == 0)
					{
						mapData[mapIndex] = 7;
						enemy1->mapIndex = -1;
						enemy1->type = 0;
						enemy1->gridX = 0;
						enemy1->gridY = 0;
						enemy_t *enemy2 = &enemies[(((ty - 1) & 7) << 3) + (tx & 7)];
						enemy2->state = 2;
					}
				}
			}
		}

		mapIndex = ty * mapWidth + (tx - 1);

		if (mapData[mapIndex] == 2)
		{
			door_t *door = &doors[((ty & 7) << 3) + ((tx - 1) & 7)];

			if (door->mapIndex != mapIndex)
			{
				door->mapIndex = mapIndex;
				door->state = 0;
				door->offset = 64 << FRACBITS;
				door->tics = 0;
			}

			if (door->state == 0 || door->state == 1)
			{
				if (door->mapIndex == (ty * mapWidth + txp) || door->mapIndex == (ty * mapWidth + txm))
					cameraX = oldCameraX;

				if (door->mapIndex == (typ * mapWidth + tx) || door->mapIndex == (tym * mapWidth + tx))
					cameraY = oldCameraY;
			}
		}
		else if (mapData[mapIndex] == 3 || mapData[mapIndex] == 4)
		{
			enemy_t *enemy1 = &enemies[((ty & 7) << 3) + ((tx - 1) & 7)];

			if (enemy1->mapIndex != mapIndex)
			{
				enemy1->mapIndex = mapIndex;
				enemy1->type = mapData[mapIndex];
				enemy1->state = 1;
				enemy1->health = 100;
				enemy1->damageTics = 0;
				enemy1->attackTics = 0;
				enemy1->damage = 0;
			}

			if (mapData[ty * mapWidth + (tx + 1)] == 0)
			{
				mapData[ty * mapWidth + (tx + 1)] = 4;

				enemy_t *enemy2 = &enemies[((ty & 7) << 3) + ((tx + 1) & 7)];
				enemy2->mapIndex = ty * mapWidth + (tx + 1);
				enemy2->type = 1;
				enemy2->state = 1;
				enemy2->health = 100;
				enemy2->damageTics = 0;
				enemy2->attackTics = 0;
				enemy2->damage = 0;

				if (rand() % 2 == 0)
					enemy1->state = 2;
				else
					enemy2->state = 2;
			}

			if (fireWeapon && enemy1->state == 2 && cameraAngle >= 96 && cameraAngle < 160)
			{
				enemy1->health -= 25;
				enemy1->damage = 1;

				if (enemy1->health <= 0)
				{
					enemy1->state = 0;
					enemy1->health = 0;
				}
			}

			if (enemy1->state == 2)
			{
				enemy1->attackTics++;

				if (enemy1->attackTics > 60)
				{
					health -= 10;

					if (health <= 0)
					{
						state = 0;
						health = 0;
					}

					enemy1->attackTics = 0;
				}
			}

			if (enemy1->damage == 1)
			{
				enemy1->damageTics++;

				if (enemy1->damageTics > 15)
				{
					enemy1->damageTics = 0;
					enemy1->damage = 0;

					if (enemy1->state == 0)
					{
						mapData[mapIndex] = 7;
						enemy1->mapIndex = -1;
						enemy1->type = 0;
						enemy1->gridX = 0;
						enemy1->gridY = 0;
						enemy_t *enemy2 = &enemies[((ty & 7) << 3) + ((tx + 1) & 7)];
						enemy2->state = 2;
					}
				}
			}
		}

		mapIndex = ty * mapWidth + (tx + 1);

		if (mapData[mapIndex] == 2)
		{
			door_t *door = &doors[((ty & 7) << 3) + ((tx + 1) & 7)];

			if (door->mapIndex != mapIndex)
			{
				door->mapIndex = mapIndex;
				door->state = 0;
				door->offset = 64 << FRACBITS;
				door->tics = 0;
			}

			if (door->state == 0 || door->state == 1)
			{
				if (door->mapIndex == (ty * mapWidth + txp) || door->mapIndex == (ty * mapWidth + txm))
					cameraX = oldCameraX;

				if (door->mapIndex == (typ * mapWidth + tx) || door->mapIndex == (tym * mapWidth + tx))
					cameraY = oldCameraY;
			}
		}
		else if (mapData[mapIndex] == 3 || mapData[mapIndex] == 4)
		{
			enemy_t *enemy1 = &enemies[((ty & 7) << 3) + ((tx + 1) & 7)];

			if (enemy1->mapIndex != mapIndex)
			{
				enemy1->mapIndex = mapIndex;
				enemy1->type = mapData[mapIndex];
				enemy1->state = 1;
				enemy1->health = 100;
				enemy1->damageTics = 0;
				enemy1->attackTics = 0;
				enemy1->damage = 0;
			}

			if (mapData[ty * mapWidth + (tx - 1)] == 0)
			{
				mapData[ty * mapWidth + (tx - 1)] = 4;

				enemy_t *enemy2 = &enemies[((ty & 7) << 3) + ((tx - 1) & 7)];
				enemy2->mapIndex = ty * mapWidth + (tx - 1);
				enemy2->type = 1;
				enemy2->state = 1;
				enemy2->health = 100;
				enemy2->damageTics = 0;
				enemy2->attackTics = 0;
				enemy2->damage = 0;

				if (rand() % 2 == 0)
					enemy1->state = 2;
				else
					enemy2->state = 2;
			}

			if (fireWeapon && enemy1->state == 2 && (cameraAngle >= 224 || cameraAngle < 32))
			{
				enemy1->health -= 25;
				enemy1->damage = 1;

				if (enemy1->health <= 0)
				{
					enemy1->state = 0;
					enemy1->health = 0;
				}
			}

			if (enemy1->state == 2)
			{
				enemy1->attackTics++;

				if (enemy1->attackTics > 60)
				{
					health -= 10;

					if (health <= 0)
					{
						state = 0;
						health = 0;
					}

					enemy1->attackTics = 0;
				}
			}

			if (enemy1->damage == 1)
			{
				enemy1->damageTics++;

				if (enemy1->damageTics > 15)
				{
					enemy1->damageTics = 0;
					enemy1->damage = 0;

					if (enemy1->state == 0)
					{
						mapData[mapIndex] = 7;
						enemy1->mapIndex = -1;
						enemy1->type = 0;
						enemy1->gridX = 0;
						enemy1->gridY = 0;
						enemy_t *enemy2 = &enemies[((ty & 7) << 3) + ((tx - 1) & 7)];
						enemy2->state = 2;
					}
				}
			}
		}

		for (int32_t i = 0; i < 64; i++)
		{
			door_t *door = &doors[i];

			if (door->mapIndex != -1)
			{
				if (door->state == 0)
				{
					door->tics++;

					if (door->tics == 60)
					{
						door->state = 1;
						door->tics = 0;
					}
				}
				else if (door->state == 1)
				{
					door->offset -= 139810;

					if (door->offset < 0)
					{
						door->state = 2;
						door->offset = 0;
					}
				}
				else if (door->state == 2)
				{
					if (door->mapIndex != (ty * mapWidth + tx))
					{
						door->tics++;

						if (door->tics == 60)
						{
							door->state = 3;
							door->tics = 0;
						}
					}
				}
				else if (door->state == 3)
				{
					door->offset += 139810;

					if (door->offset > (64 << FRACBITS))
					{
						door->mapIndex = -1;
						door->state = 0;
						door->offset = 64 << FRACBITS;
					}
				}
			}
		}

		frameTics++;

		if (frameTics > 15)
		{
			frame = !frame;
			frameTics = 0;
		}

		restartLevelPressed = false;
	}
	else if (state == 0)
	{
		bloodTics++;

		if (bloodTics > 7)
		{
			uint32_t area = 0;

			for (uint32_t i = 0; i < 64; i++)
			{
				bloodHeight[i] += bloodSpeed[i];

				if (bloodHeight[i] > 64)
					bloodHeight[i] = 64;

				area += bloodHeight[i];
			}

			if (area == 4096)
				state = 3;

			bloodTics = 0;
		}

		restartLevelPressed = false;
	}
	else if (state == 2 || state == 3 || state == 4 || state == 5)
	{
		if (state < 4 && restartLevelPressed)
		{
			File file;
			file.open(levels[level - 1], OpenMode::read);
			uint32_t offset = 0;
			int32_t cameraGridX;
			int32_t cameraGridY;
			int32_t exitGridX;
			int32_t exitGridY;
			file.read(offset, sizeof(int32_t), (char *)&cameraGridX);
			offset += sizeof(int32_t);
			file.read(offset, sizeof(int32_t), (char *)&cameraGridY);
			offset += sizeof(int32_t);
			cameraX = (cameraGridX * 64 + 32) << FRACBITS;
			cameraY = (cameraGridY * 64 + 32) << FRACBITS;
			file.read(offset, sizeof(angle_t), (char *)&cameraAngle);
			offset += sizeof(angle_t);
			file.read(offset, sizeof(int32_t), (char *)&exitGridX);
			offset += sizeof(int32_t);
			file.read(offset, sizeof(int32_t), (char *)&exitGridY);
			offset += sizeof(int32_t);
			file.read(offset, sizeof(int32_t), (char *)&mapWidth);
			offset += sizeof(int32_t);
			file.read(offset, sizeof(int32_t), (char *)&mapHeight);
			offset += sizeof(int32_t);
			mapData = new uint32_t[mapWidth * mapHeight];
			mapDataPtr = std::unique_ptr<uint32_t>(mapData);
			file.read(offset, mapWidth * mapHeight * sizeof(uint32_t), (char *)mapData);
			file.close();
			state = 1;
			health = 100;

			for (uint32_t i = 0; i < 64; i++)
				bloodHeight[i] = 0;

			restartLevelPressed = false;
		}
		else if (state == 4 && restartLevelPressed)
		{
			state = 5;
			restartLevelPressed = false;
		}
		else if (restartLevelPressed)
		{
			state = 2;
			stream.load(music[(level - 1) % 2]);
			stream.play(0);
			restartLevelPressed = false;
		}
	}
}

void render(uint32_t time)
{
	if (state == 1 || state == 0)
	{
		plane.minX = 64;
		plane.maxX = -1;

		plane.pad1 = 64;

		for (int32_t i = 0; i < 64; i++)
			plane.top[i] = 64;

		plane.pad2 = 64;

		uint32_t rayAngle = (cameraAngle + 31) & ANGLESMASK;

		for (int32_t i = 0; i < 64; i++)
		{
			fixed_t horizontalIntersectionY;
			fixed_t stepY;

			if (rayAngle < 128)
			{
				horizontalIntersectionY = (cameraY >> 22) * (64 << FRACBITS);
				stepY = -64 << FRACBITS;
			}
			else
			{
				horizontalIntersectionY = (cameraY >> 22) * (64 << FRACBITS) + (64 << FRACBITS);
				stepY = 64 << FRACBITS;
			}

			fixed_t horizontalIntersectionX = cameraX - fixedMul(horizontalIntersectionY - cameraY, fixedCot(rayAngle));
			fixed_t stepX = -fixedMul(stepY, fixedCot(rayAngle));
			fixed_t horizontalIntersectionDistance;
			int32_t horizontalIntersectionType;
			int32_t horizontalDoorOffset;

			if (rayAngle == 0 || rayAngle == 128)
				horizontalIntersectionDistance = INT_MAX;
			else
			{
				while (1)
				{
					int32_t gridX = horizontalIntersectionX >> 22;
					int32_t gridY = (horizontalIntersectionY >> 22) - (stepY < 0 ? 1 : 0);

					if (gridX < 0 || gridY < 0 || gridX >= mapWidth || gridY >= mapHeight)
					{
						horizontalIntersectionDistance = INT_MAX;
						break;
					}

					horizontalIntersectionType = mapData[gridY * mapWidth + gridX];

					if (horizontalIntersectionType == 1)
					{
						horizontalIntersectionDistance = fixedMul(horizontalIntersectionX - cameraX, fixedCos(cameraAngle)) - fixedMul(horizontalIntersectionY - cameraY, fixedSin(cameraAngle));
						break;
					}
					else if (horizontalIntersectionType == 2 && (((horizontalIntersectionX + (stepX >> 1)) >> FRACBITS) & 63) < (horizontalDoorOffset = (doors[((gridY & 7) << 3) + (gridX & 7)].mapIndex == (gridY * mapWidth + gridX) ? doors[((gridY & 7) << 3) + (gridX & 7)].offset >> FRACBITS : 64)))
					{
						horizontalIntersectionX += stepX >> 1;
						horizontalIntersectionY += stepY >> 1;
						horizontalIntersectionDistance = fixedMul(horizontalIntersectionX - cameraX, fixedCos(cameraAngle)) - fixedMul(horizontalIntersectionY - cameraY, fixedSin(cameraAngle));
						break;
					}
					else if (horizontalIntersectionType == 3 || horizontalIntersectionType == 4)
					{
						enemy_t *enemy = &enemies[((gridY & 7) << 3) + (gridX & 7)];
						enemy->type = horizontalIntersectionType - 3;
						enemy->gridX = gridX;
						enemy->gridY = gridY;
						enemy->render = 1;
					}
					else if (horizontalIntersectionType == 5 || horizontalIntersectionType == 6)
					{
						health_t *health = &healths[((gridY & 7) << 3) + (gridX & 7)];
						health->type = horizontalIntersectionType - 5;
						health->gridX = gridX;
						health->gridY = gridY;
						health->render = 1;
					}

					horizontalIntersectionX += stepX;
					horizontalIntersectionY += stepY;
				}
			}

			fixed_t verticalIntersectionX;

			if (rayAngle >= 64 && rayAngle < 192)
			{
				verticalIntersectionX = (cameraX >> 22) * (64 << FRACBITS);
				stepX = -64 << FRACBITS;
			}
			else
			{
				verticalIntersectionX = (cameraX >> 22) * (64 << FRACBITS) + (64 << FRACBITS);
				stepX = 64 << FRACBITS;
			}

			fixed_t verticalIntersectionY = cameraY - fixedMul(verticalIntersectionX - cameraX, fixedTan(rayAngle));
			stepY = -fixedMul(stepX, fixedTan(rayAngle));
			fixed_t verticalIntersectionDistance;
			int32_t verticalIntersectionType;
			int32_t verticalDoorOffset;

			if (rayAngle == 64 || rayAngle == 192)
				verticalIntersectionDistance = INT_MAX;
			else
			{
				while (1)
				{
					int32_t gridX = (verticalIntersectionX >> 22) - (stepX < 0 ? 1 : 0);
					int32_t gridY = verticalIntersectionY >> 22;

					if (gridX < 0 || gridY < 0 || gridX >= mapWidth || gridY >= mapHeight)
					{
						verticalIntersectionDistance = INT_MAX;
						break;
					}

					verticalIntersectionType = mapData[gridY * mapWidth + gridX];

					if (verticalIntersectionType == 1)
					{
						verticalIntersectionDistance = fixedMul(verticalIntersectionX - cameraX, fixedCos(cameraAngle)) - fixedMul((verticalIntersectionY - cameraY), fixedSin(cameraAngle));
						break;
					}
					else if (verticalIntersectionType == 2 && (((verticalIntersectionY + (stepY >> 1)) >> FRACBITS) & 63) < (verticalDoorOffset = (doors[((gridY & 7) << 3) + (gridX & 7)].mapIndex == (gridY * mapWidth + gridX) ? doors[((gridY & 7) << 3) + (gridX & 7)].offset >> FRACBITS : 64)))
					{
						verticalIntersectionX += stepX >> 1;
						verticalIntersectionY += stepY >> 1;
						verticalIntersectionDistance = fixedMul(verticalIntersectionX - cameraX, fixedCos(cameraAngle)) - fixedMul((verticalIntersectionY - cameraY), fixedSin(cameraAngle));
						break;
					}
					else if (verticalIntersectionType == 3 || verticalIntersectionType == 4)
					{
						enemy_t *enemy = &enemies[((gridY & 7) << 3) + (gridX & 7)];
						enemy->type = verticalIntersectionType - 3;
						enemy->gridX = gridX;
						enemy->gridY = gridY;
						enemy->render = 1;
					}
					else if (verticalIntersectionType == 5 || verticalIntersectionType == 6)
					{
						health_t *health = &healths[((gridY & 7) << 3) + (gridX & 7)];
						health->type = verticalIntersectionType - 5;
						health->gridX = gridX;
						health->gridY = gridY;
						health->render = 1;
					}

					verticalIntersectionX += stepX;
					verticalIntersectionY += stepY;
				}
			}

			fixed_t distance;
			const uint8_t *texture;
			int32_t textureOffsetX;

			if (horizontalIntersectionDistance < verticalIntersectionDistance)
			{
				distance = horizontalIntersectionDistance;
				texture = &textures->data[0];
				textureOffsetX = (horizontalIntersectionX >> FRACBITS) & 63;

				if (horizontalIntersectionType == 2)
				{
					texture = &textures->data[8192];
					textureOffsetX += 64 - horizontalDoorOffset;
				}

				if (horizontalIntersectionType != 2 && rayAngle >= 128)
					textureOffsetX = 63 - textureOffsetX;
			}
			else
			{
				distance = verticalIntersectionDistance;
				texture = &textures->data[4096];
				textureOffsetX = (verticalIntersectionY >> FRACBITS) & 63;

				if (verticalIntersectionType == 2)
				{
					texture = &textures->data[12288];
					textureOffsetX += 64 - verticalDoorOffset;
				}

				if (verticalIntersectionType != 2 && rayAngle >= 64 && rayAngle < 192)
					textureOffsetX = 63 - textureOffsetX;
			}

			int32_t wallHeight = fixedDiv(2048 << FRACBITS, distance) >> FRACBITS;

			if ((wallHeight & 1) != 0)
				wallHeight++;

			int32_t wallStart = (64 - wallHeight) >> 1;

			DrawWallSlice(surface.get(), texture, textureOffsetX, i, wallStart, wallHeight);

			if (wallHeight < 64)
			{
				if (i < plane.minX)
					plane.minX = i;

				if (i > plane.maxX)
					plane.maxX = i;

				plane.top[i] = wallStart + wallHeight;
			}

			zBuffer[i] = distance;

			rayAngle = (rayAngle - 1) & ANGLESMASK;
		}

		const uint8_t *floorTexture = &textures->data[16384];
		const uint8_t *ceilingTexture = &textures->data[20480];

		for (int32_t i = 0; i < 32; i++)
			stop[i] = 0;

		for (int32_t x = plane.minX; x <= plane.maxX + 1; x++)
		{
			uint32_t t1 = plane.top[x - 1];
			uint32_t t2 = plane.top[x];

			while (t1 < t2)
			{
				uint32_t index = t1 - 32;

				if (stop[index] == 0)
				{
					fixed_t distance = fixedDiv(fixedDiv(1024 << FRACBITS, (index << FRACBITS) + 32768), fixedCos(31));
					fixed_t x1 = fixedMul(distance, fixedCos((cameraAngle + 31) & ANGLESMASK));
					fixed_t y1 = -fixedMul(distance, fixedSin((cameraAngle + 31) & ANGLESMASK));
					fixed_t x2 = fixedMul(distance, fixedCos((cameraAngle - 32) & ANGLESMASK));
					fixed_t y2 = -fixedMul(distance, fixedSin((cameraAngle - 32) & ANGLESMASK));
					currentX[index] = cameraX + x1;
					currentY[index] = cameraY + y1;
					stepX[index] = fixedDiv(x2 - x1, 64 << FRACBITS);
					stepY[index] = fixedDiv(y2 - y1, 64 << FRACBITS);
					currentX[index] += start[index] * stepX[index];
					currentY[index] += start[index] * stepY[index];
				}
				else
				{
					currentX[index] += (start[index] - stop[index]) * stepX[index];
					currentY[index] += (start[index] - stop[index]) * stepY[index];
				}

				uint32_t count = (x - 1) - start[index];
				uint8_t *p1 = yTable[t1] + xTable[start[index]];
				uint8_t *p2 = yTable[63 - t1] + xTable[start[index]];

				do
				{
					int32_t tx = (currentX[index] >> FRACBITS) & 63;
					int32_t ty = (currentY[index] >> FRACBITS) & 63;
					int32_t textureIndex = ty * 64 + tx;
					*p1 = 137 + floorTexture[textureIndex];
					p1++;
					*p2 = 137 + ceilingTexture[textureIndex];
					p2++;
					currentX[index] += stepX[index];
					currentY[index] += stepY[index];
				} while (count--);

				stop[index] = x;

				t1++;
			}

			while (t2 < t1)
			{
				start[t2 - 32] = x;
				t2++;
			}
		}

		for (int32_t i = 0; i < 64; i++)
		{
			health_t *health = &healths[i];

			if (health->render)
			{
				fixed_t distance = fixedMul(((health->gridX << 22) + (32 << FRACBITS)) - cameraX, fixedCos(cameraAngle)) - fixedMul(((health->gridY << 22) + (32 << FRACBITS)) - cameraY, fixedSin(cameraAngle));
				fixed_t x = fixedMul(((health->gridX << 22) + (32 << FRACBITS)) - cameraX, fixedSin(cameraAngle)) + fixedMul(((health->gridY << 22) + (32 << FRACBITS)) - cameraY, fixedCos(cameraAngle));
				x = fixedDiv(x << 5, distance);
				int32_t spriteSize = fixedDiv(2048 << FRACBITS, distance) >> FRACBITS;
				if ((spriteSize & 1) != 0)
					spriteSize++;
				int32_t spriteX = 32 + (x >> FRACBITS) - (spriteSize >> 1);
				int32_t spriteY = (64 - spriteSize) >> 1;
				const uint8_t *sprite = &sprites->data[frames[4 + health->type]];
				DrawSprite(surface.get(), sprite, spriteX, spriteY, spriteSize, distance, 0);
				health->render = 0;
			}
		}

		for (int32_t i = 0; i < 64; i++)
		{
			enemy_t *enemy = &enemies[i];

			if (enemy->render)
			{
				fixed_t distance = fixedMul(((enemy->gridX << 22) + (32 << FRACBITS)) - cameraX, fixedCos(cameraAngle)) - fixedMul(((enemy->gridY << 22) + (32 << FRACBITS)) - cameraY, fixedSin(cameraAngle));
				fixed_t x = fixedMul(((enemy->gridX << 22) + (32 << FRACBITS)) - cameraX, fixedSin(cameraAngle)) + fixedMul(((enemy->gridY << 22) + (32 << FRACBITS)) - cameraY, fixedCos(cameraAngle));
				x = fixedDiv(x << 5, distance);
				int32_t spriteSize = fixedDiv(2048 << FRACBITS, distance) >> FRACBITS;
				if ((spriteSize & 1) != 0)
					spriteSize++;
				int32_t spriteX = 32 + (x >> FRACBITS) - (spriteSize >> 1);
				int32_t spriteY = (64 - spriteSize) >> 1;
				const uint8_t *sprite = &sprites->data[frames[enemy->type * 2 + frame]];
				DrawSprite(surface.get(), sprite, spriteX, spriteY, spriteSize, distance, enemy->damage);
				enemy->render = 0;
			}
		}

		screen.stretch_blit(surface.get(), Rect(0, 0, 64, 64), Rect(0, 0, 320, 240));

		Rect rect;

		if (fireWeaponPressed)
			rect = { 25, 0, 25, 26 };
		else
			rect = { 0, 0, 25, 26 };

		screen.stretch_blit(hand.get(), rect, Rect(195, 143, 125, 97));

		screen.pen = Pen(255, 0, 0);

		rect = { 80, 225, 5 * (fixedDiv(health << 21, 100 << FRACBITS) >> FRACBITS), 7 };
		screen.rectangle(rect);

		if (state == 0)
		{
			for (uint32_t i = 0; i < 64; i++)
			{
				Rect rect = { 5 * (int)i, 0, 5, (240 * (int)bloodHeight[i]) >> 6 };
				screen.rectangle(rect);
			}
		}
	}
	else if (state == 2)
		screen.stretch_blit(screens.get(), Rect(0, 0, 64, 64), Rect(0, 0, 320, 240));
	else if (state == 3)
		screen.stretch_blit(screens.get(), Rect(0, 64, 64, 64), Rect(0, 0, 320, 240));
	else if (state == 4)
		screen.stretch_blit(screens.get(), Rect(0, 128, 64, 64), Rect(0, 0, 320, 240));
	else if (state == 5)
		screen.stretch_blit(screens.get(), Rect(0, 192, 64, 64), Rect(0, 0, 320, 240));
}

void init()
{
	set_screen_mode(ScreenMode::hires);

	File::add_buffer_file("music1.mp3", asset_music1, asset_music1_length);
	File::add_buffer_file("music2.mp3", asset_music2, asset_music2_length);
	File::add_buffer_file("level1.map", asset_level1, asset_level1_length);
	File::add_buffer_file("level2.map", asset_level2, asset_level2_length);
	File::add_buffer_file("level3.map", asset_level3, asset_level3_length);
	File::add_buffer_file("level4.map", asset_level4, asset_level4_length);

	pixels = std::unique_ptr<uint8_t>(new uint8_t[4096]);

	surface = std::unique_ptr<Surface>(new Surface(pixels.get(), PixelFormat::P, Size(64, 64)));

	textures = std::unique_ptr<Surface>(Surface::load_read_only(asset_textures));

	sprites = std::unique_ptr<Surface>(Surface::load_read_only(asset_sprites));

	hand = std::unique_ptr<Surface>(Surface::load_read_only(asset_hand));

	screens = std::unique_ptr<Surface>(Surface::load_read_only(asset_screens));

	surface->palette = new Pen[148];
	memcpy(&surface->palette[0], sprites->palette, 137 * sizeof(Pen));
	memcpy(&surface->palette[137], textures->palette, 11 * sizeof(Pen));

	palettes[0] = std::unique_ptr<Pen>(surface->palette);
	palettes[1] = std::unique_ptr<Pen>(textures->palette);
	palettes[2] = std::unique_ptr<Pen>(sprites->palette);
	palettes[3] = std::unique_ptr<Pen>(hand->palette);
	palettes[4] = std::unique_ptr<Pen>(screens->palette);

	stream.load("music1.mp3", false);

	for (uint32_t i = 0; i < 64; i++)
		yTable[i] = surface->data + (((surface->bounds.h - 64) >> 1) + i) * surface->row_stride;

	for (uint32_t i = 0; i < 64; i++)
		xTable[i] = ((surface->bounds.w - 64) >> 1) + i;

	stream.play(0);

	srand((unsigned)time(NULL));

	for (uint32_t i = 0; i < 64; i++)
		bloodSpeed[i] = rand() % 4 + 2;

	lastTime = now();
}
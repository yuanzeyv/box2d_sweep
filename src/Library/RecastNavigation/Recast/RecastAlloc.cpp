//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include <stdlib.h>
#include <string.h>
#include "Recast/RecastAlloc.h"
#include "Recast/RecastAssert.h"

static void *rcAllocDefault(size_t size, rcAllocHint)
{
	return malloc(size);
}

static void rcFreeDefault(void *ptr)
{
	free(ptr);
}

static rcAllocFunc* sRecastAllocFunc = rcAllocDefault;
static rcFreeFunc* sRecastFreeFunc = rcFreeDefault;

/// @see rcAlloc, rcFree
void rcAllocSetCustom(rcAllocFunc *allocFunc, rcFreeFunc *freeFunc)
{
	sRecastAllocFunc = allocFunc ? allocFunc : rcAllocDefault;
	sRecastFreeFunc = freeFunc ? freeFunc : rcFreeDefault;
}

/// @see rcAllocSetCustom
void* rcAlloc(size_t size, rcAllocHint hint)
{
	return sRecastAllocFunc(size, hint);
}

/// @par
///
/// @warning This function leaves the value of @p ptr unchanged.  So it still
/// points to the same (now invalid) location, and not to null.
/// 
/// @see rcAllocSetCustom
void rcFree(void* ptr)
{
	if (ptr)
		sRecastFreeFunc(ptr);
}

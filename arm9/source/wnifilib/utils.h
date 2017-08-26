/*
Copyright (C) 2015-2017  Coto

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef __utils_h__
#define __utils_h__

#include "nifi.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif

//misc utilities
extern int split (const char *txt, char delim, char ***tokens);
extern s8 * str_replace (s8 *string, const s8 *substr, const s8 *replacement);

#ifdef __cplusplus
]
#endif

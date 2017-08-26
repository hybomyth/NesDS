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

/**
 * Copyright (C) 2010 chantra <chantra@debuntu.org>
 * Copyright (C) 2012 Iain R. Learmonth <irl@sdf.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	02110-1301, USA.
 */
 
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int split (const char *txt, char delim, char ***tokens)
{
    int *tklen, *t, count = 1;
    s8 **arr, *p = (s8 *) txt;

    while (*p != '\0') if (*p++ == delim) count += 1;
    t = tklen = (int*)calloc (count, sizeof (int));
    for (p = (s8 *) txt; *p != '\0'; p++) *p == delim ? *t++ : (*t)++;
    *tokens = arr = (s8**)malloc (count * sizeof (s8 *));
    t = tklen;
    p = *arr++ = (s8*)calloc (*(t++) + 1, sizeof (s8 *));
    while (*txt != '\0')
    {
        if (*txt == delim)
        {
            p = *arr++ = (s8*)calloc (*(t++) + 1, sizeof (s8 *));
            txt++;
        }
        else *p++ = *txt++;
    }
    free (tklen);
    return count;
}

//fork: https://github.com/irl/la-cucina/blob/master/str_replace.c
s8 * str_replace (s8 *string, const s8 *substr, const s8 *replacement)
{
	s8 *tok = NULL;
	s8 *newstr = NULL;
	s8 *oldstr = NULL;

	/* if either substr or replacement is NULL, duplicate string a let caller handle it */

	if ( substr == NULL || replacement == NULL )
	{
		return strdup (string);
	}

	newstr = strdup (string);

	while ( ( tok = strstr( newstr, substr ) ) )
	{
		oldstr = newstr;
		newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );

		/* If failed to alloc mem, free old string and return NULL */
		if ( newstr == NULL )
		{
			free (oldstr);
			return NULL;
		}

		memcpy ( newstr, oldstr, tok - oldstr );
		memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
		memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
		memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );

		free (oldstr);
	}

	free (string);

	return newstr;
}
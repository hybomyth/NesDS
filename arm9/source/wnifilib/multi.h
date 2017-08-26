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
//DSWNIFI Library revision: 1.1

#ifndef __multi_wnifilib_h__
#define __multi_wnifilib_h__

#include "dswnifi.h"
#include <stdbool.h>

#endif


#ifdef __cplusplus
extern "C"{
#endif

extern int plykeys1;		//player1
extern int plykeys2;		//player2
extern int guest_framecount;	//used by the guest for syncing.
extern int host_framecount;		//emulator framecount:host
extern int guest_framecount;	//emulator framecount:guest
extern int host_vcount;		//host generated REG_VCOUNT
extern int guest_vcount;		//guest generated REG_VCOUNT
extern int topvalue(int a,int b);
extern int bottomvalue(int a,int b);
extern int getintdiff(int a,int b);

extern void sendcmd(u8 * databuf_src);	//framesize is calculated inside
extern void getcmd(u8 * databuf_src);	//framesize is calculated inside (crc over udp requires framesize previously to here calculated anyway)
extern bool do_multi();

#ifdef __cplusplus
}
#endif

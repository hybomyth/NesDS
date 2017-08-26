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


#ifndef __client_http_handler_wnifilib_h__
#define __client_http_handler_wnifilib_h__

#include "dswnifi.h"

#include <netinet/in.h>
#include <netdb.h>

//struct that spinlocks a current request
typedef struct {
    //dswifi socket @ PORT 8888
    struct sockaddr_in sain_UDP_PORT;			//local nds, any IP takes 127.0.0.1 or current IP @ port 8888 // really used for sending udp packets
	struct sockaddr_in server_addr;		//server: APP UDP companion address	/ unused
    int socket_id__multi_notconnected;	//initial stage required PORT (server -- DS)	/used for ds - server
    
	//listener IP-port
	struct sockaddr_in sain_listener;			//local nds, any IP takes 127.0.0.1 or current IP @ port 8889 (this IP, which is special for each ds console connected)
	int socket_multi_listener;		//multi NDS UDP PORT reserved / used for ds - ds comms
    
	//sender IP-port
	struct sockaddr_in sain_sender;	//ndsmulti IP (second DS ip): UDP multiplayer stores other NDS IP / used from guest mode -> socket_id__multi_connected 
	int socket_multi_sender;		//multi NDS UDP PORT reserved / used for ds - ds comms
    
	
	
	
	//host socket entry
    struct hostent myhost;
    
    bool wifi_enabled;
    uint8 http_buffer[4 * 1024];
    

} client_http_handler;


#endif

#ifdef __cplusplus
extern "C"{
#endif

extern s8* server_ip;

//HTTP calls:
extern void request_connection(s8* str_url,int str_url_size);

//generate a GET request to a desired DNS/IP address
extern bool issue_get_response(s8* str_url,int str_url_size);

//generate a POST request where str_params conforms the FORM HTTP 1.0 spec to send to url
extern bool send_response(s8 * str_params);

//coworker that deals with command execution from NDS side
extern client_http_handler client_http_handler_context;

//libnds
extern void getHttp(s8* url);

#ifdef __cplusplus
}
#endif

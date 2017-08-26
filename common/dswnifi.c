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
#include "dswnifi.h"

#include "common_shared.h"
#include "wifi_shared.h"

#ifdef ARM9
#include "nifi.h"
#include "wifi_arm9.h"
#include "dswifi9.h"
#include "client_http_handler.h"

#endif

#ifdef ARM9

//0 idle, 1 nifi, 2 dswnifi
void switch_dswnifi_mode(u8 mode){

	//wifi minimal setup
	if(mode == (u8)dswifi_wifimode){
		MyIPC->dswifiSrv.dsnwifisrv_mode = dswifi_wifimode;
		MyIPC->dswifiSrv.dsnwifisrv_stat = ds_searching_for_multi_servernotaware;
		MyIPC->dswifiSrv.dswifi_setup = false;	//set for RPC services
	}
	//nifi minimal setup
	else if (mode == (u8)dswifi_nifimode){
		//nifi
		MyIPC->dswifiSrv.dsnwifisrv_mode = dswifi_nifimode;
		MyIPC->dswifiSrv.dswifi_setup = false;
	}
	
	//idle mode minimal setup
	else if (mode == (u8)dswifi_idlemode){
		//nifi
		MyIPC->dswifiSrv.dsnwifisrv_mode = dswifi_idlemode;
		MyIPC->dswifiSrv.dswifi_setup = false;
	}
	
	
	//set NIFI mode
	if( (MyIPC->dswifiSrv.dsnwifisrv_mode == dswifi_nifimode) && (MyIPC->dswifiSrv.dswifi_setup == false)){
		initNiFi();
		MyIPC->dswifiSrv.dswifi_setup = true;
	}
	
	//set tcp dswifi
	else if((MyIPC->dswifiSrv.dsnwifisrv_mode == dswifi_wifimode) && (MyIPC->dswifiSrv.dswifi_setup == false)){
		if(Wifi_InitDefault(WFC_CONNECT) == true)
		{
			//char buf[64];
			//sprintf(buf,"connected: IP: %s",(char*)print_ip((u32)Wifi_GetIP()));
			//consoletext(64*2-32,(char *)&buf[0],0);
			
			//works fine both
			//new connection
			request_connection(server_ip,strlen(server_ip));
			
			//this must be called on send tx frame
			//send_response((char *)"<html><body><form action='http://192.168.43.108/'>First name:<br><input type='text' name='firstname' value='Mickey'><br>Last name:<br><input type='text' name='lastname' value='Mouse'><br><br><input type='submit' value='Submit'></form> </body></html>");
			//send_response((char *)"GET /dswifi/example1.php HTTP/1.1\r\n""Host: www.akkit.org\r\n""User-Agent: Nintendo DS\r\n\r\n");
			MyIPC->dswifiSrv.dswifi_setup = true;
		}
		else{
			//printf("     phailed conn! ________________________________");
		}
	}
	
}

#endif

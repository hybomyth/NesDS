#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include <stdio.h>
#include <unistd.h>

static u8 buf[768]; //lol

typedef u16 UnicodeChar;
#define ExtLinkBody_MaxLength (256)
#define ExtLinkBody_ID (0x30545845) // EXT0
typedef struct {
  u32 ID,dummy1,dummy2,dummy3; // dummy is ZERO.
  char DataFullPathFilenameAlias[ExtLinkBody_MaxLength];
  char DataPathAlias[ExtLinkBody_MaxLength];
  char DataFilenameAlias[ExtLinkBody_MaxLength];
  char NDSFullPathFilenameAlias[ExtLinkBody_MaxLength];
  char NDSPathAlias[ExtLinkBody_MaxLength];
  char NDSFilenameAlias[ExtLinkBody_MaxLength];
  UnicodeChar DataFullPathFilenameUnicode[ExtLinkBody_MaxLength];
  UnicodeChar DataPathUnicode[ExtLinkBody_MaxLength];
  UnicodeChar DataFilenameUnicode[ExtLinkBody_MaxLength];
  UnicodeChar NDSFullPathFilenameUnicode[ExtLinkBody_MaxLength];
  UnicodeChar NDSPathUnicode[ExtLinkBody_MaxLength];
  UnicodeChar NDSFilenameUnicode[ExtLinkBody_MaxLength];
} TExtLinkBody;

char* myfgets(char *buf,int n,FILE *fp){ //accepts LF/CRLF
	char *ret=fgets(buf,n,fp);
	if(!ret)return NULL;
	if(strlen(buf)&&buf[strlen(buf)-1]=='\n')buf[strlen(buf)-1]=0;
	if(strlen(buf)&&buf[strlen(buf)-1]=='\r')buf[strlen(buf)-1]=0;
	return ret;
}

void SplitItemFromFullPathAlias(const char *pFullPathAlias,char *pPathAlias,char *pFilenameAlias){
	u32 SplitPos=0;
	{
		u32 idx=0;
		while(1){
			char uc=pFullPathAlias[idx];
			if(uc==0) break;
			if(uc=='/') SplitPos=idx+1;
			idx++;
		}
	}

	if(pPathAlias){
		if(SplitPos<=1){
			pPathAlias[0]='/';
			pPathAlias[1]=0;
		}else{
			u32 idx=0;
			for(;idx<SplitPos-1;idx++){
				pPathAlias[idx]=pFullPathAlias[idx];
			}
			pPathAlias[SplitPos-1]=0;
		}
	}
	if(pFilenameAlias)strcpy(pFilenameAlias,&pFullPathAlias[SplitPos]);
}

bool _readFrontend(char *target){
	FILE *f=fopen("/loadfile.dat","rb");
	if(f){
		int i=0;
		myfgets((char*)buf,768,f);
		fclose(f);
		unlink("/loadfile.dat");
		//if(!memcmp((char*)buf,"fat:",4))i+=4;
		if(!memcmp((char*)buf+i,"//",2))i+=1;
		if(!memcmp((char*)buf+i,"/./",3))i+=2; //menudo dir handling is buggy?
		strcpy(target,(char*)buf+i);
		if(strlen(target)<4||(strcasecmp(target+strlen(target)-4,".nes")&&strcasecmp(target+strlen(target)-4,".fds")))return false;
		return true;
	}
	f=fopen("/plgargs.dat","rb");
	if(f){
		int i=0;
		myfgets((char*)buf,768,f);
		myfgets((char*)buf,768,f); //second line
		fclose(f);
		unlink("/plgargs.dat");
		//if(!memcmp((char*)buf,"fat:",4))i+=4;
		//if(!memcmp((char*)buf+i,"//",2))i+=1;
		//if(!memcmp((char*)buf+i,"/./",3))i+=2;
		strcpy(target,(char*)buf+i);
		if(strlen(target)<4||(strcasecmp(target+strlen(target)-4,".nes")&&strcasecmp(target+strlen(target)-4,".fds")))return false;
		return true;
	}
	f=fopen("/moonshl2/extlink.dat","r+b");
	if(f){
		TExtLinkBody extlink;
		memset(&extlink,0,sizeof(TExtLinkBody));
		fread(&extlink,1,sizeof(TExtLinkBody),f);
		if(extlink.ID!=ExtLinkBody_ID){fclose(f);return false;}
		//strcpy(target,extlink.DataFullPathFilenameAlias);
		ucs2tombs((unsigned char*)target,extlink.DataFullPathFilenameUnicode,768);
		fseek(f,0,SEEK_SET);
		fwrite("____",1,4,f);
		fflush(f);
		fclose(f);
		if(strlen(target)<4||(strcasecmp(target+strlen(target)-4,".nes")&&strcasecmp(target+strlen(target)-4,".fds")))return false;
		return true;
	}
	return false; //it is your choice to boot GUI or to halt.
}

extern int argc;
extern char **argv;
bool readFrontend(char *target)
{
	char dir[768];
	if(argc>1){
		strcpy(target,argv[1]);
	}else{
		if(!_readFrontend(target))return false;
	}
	SplitItemFromFullPathAlias(target,dir,NULL);
	chdir(dir);
	return true;
}


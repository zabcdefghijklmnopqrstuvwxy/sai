#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "ini_rw.h"


int main(void)
{
	int module[16] = {0};
	int setsys[16] = {2,7,18,35};
//	char cStr[256];
//	char cSetStr[256] = "debug";
//	int  cInt;
	int nCnt = 0;;
//	float fDou;
	const char *file = "./B0100/sys.ini";
	INI_LoadFile(file);

	char *sect = "SYS";
	char *key1 = "SysModule";
//	char *key2 = "Int";
//	char *key3 = "Float";
//	char *key4 = "Test";

	INI_GetEnum(sect, key1, module, &nCnt, sizeof(module));
	printf("[%s] %s = %d,%d,%d len is %d\n", sect, key1, module[0],module[1],module[2],nCnt);

	nCnt = 4;
	INI_SetEnum(sect,key1,setsys,nCnt);

	INI_GetEnum(sect, key1, module, &nCnt, sizeof(module));
	printf("[%s] %s = %d,%d,%d len is %d\n", sect, key1, module[0],module[1],module[2],nCnt);

	INI_ResetEnum(sect,key1);
	memset(module,0,sizeof(module));
	INI_GetEnum(sect, key1, module, &nCnt, sizeof(module));
	printf("[%s] %s = %d,%d,%d len is %d\n", sect, key1, module[0],module[1],module[2],nCnt);

#if 0
	INI_GetInt(sect, key2, &cInt);
	printf("[%s] %s = %d\n", sect, key2, cInt);

	INI_GetFloat(sect, key3,&fDou);
	printf("[%s] %s = %f\n", sect, key3, fDou);

	INI_SetString(sect, key4, cSetStr);
	memset(cStr,0,sizeof(cStr));
	INI_GetString(sect, key4, cStr, sizeof(cStr));
	printf("[%s] %s = %s\n", sect, key4, cStr);

        INI_ResetString(sect,key4);
	memset(cStr,0,sizeof(cStr));
	INI_GetString(sect, key4, cStr, sizeof(cStr));
	printf("[%s] %s = %s\n", sect, key4, cStr);

	INI_SetFloat(sect,key3,24.12);
	INI_GetFloat(sect, key3,&fDou);
	printf("[%s] %s = %f\n", sect, key3, fDou);
#endif

	INI_FreeFile();

	return 0;
}


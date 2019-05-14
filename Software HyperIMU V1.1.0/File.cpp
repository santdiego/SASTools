#include "stdafx.h"

void readFile(char *inIP, USHORT *inPort, USHORT *outIP,USHORT *device){
	TCHAR nameFile[50];
	FILE *file;
	errno_t err;
	_stprintf_s(nameFile,sizeof(nameFile),_T("Config.txt"));
	err = _tfopen_s(&file,nameFile, _T("r"));
	if (err == 0)
	{
		fscanf_s(file,"%s\n%d\n%d\n%d\n",inIP,16,inPort,outIP,device);
		fclose(file);
	}
}


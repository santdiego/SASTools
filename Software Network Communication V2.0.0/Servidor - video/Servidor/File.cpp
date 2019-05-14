#include "stdafx.h"

void readFile(char *inIP, USHORT *inPort, char *outIP, USHORT *outPort, char *protocol){
	TCHAR nameFile[50];
	FILE *file;
	errno_t err;
	_stprintf_s(nameFile,sizeof(nameFile),_T("Config.txt"));
	err = _tfopen_s(&file,nameFile, _T("r"));
	if (err == 0)
	{
		fscanf_s(file,"%s\n%d\n%s\n%d\n%s\n",inIP,16,inPort,outIP,16,outPort,protocol,4);
		fclose(file);
	}
}

void writeFile(char *inIP, USHORT inPort, char *outIP, USHORT outPort, char *protocol){
	TCHAR nameFile[50];
	FILE *file;
	errno_t err;
	_stprintf_s(nameFile,sizeof(nameFile),_T("Config.txt"));
	err = _tfopen_s(&file, nameFile, _T("w"));
	if (err == 0)
	{
		fprintf_s(file,"%s\n%d\n%s\n%d\n%s\n",inIP,inPort,outIP,outPort,protocol);
		fclose(file);
	}
	else{
		MessageBox(NULL,_T("Error opening file!"),_T("Error"),MB_OK);
	}
}
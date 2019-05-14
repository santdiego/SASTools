//#include "stdafx.h"
#include "File.h"



void readFile(char *inIP, int *inPort, char *outIP, int *outPort, char *A, int *B,char *C, int *D,char *E,int *F,char*G, int *H){
	TCHAR nameFile[50];
	FILE *file;
	errno_t err;
	_stprintf_s(nameFile,sizeof(nameFile),_T("Config.txt"));
	err = _tfopen_s(&file,nameFile, _T("r"));
	if (err == 0)
	{
		fscanf_s(file,"%s\n%d\n%s\n%d\n%s\n%d\n%s\n%d\n%s\n%d\n%s\n%d\n",inIP,64,inPort,outIP,64,outPort,A,64,B,C,64,D,E,64,F,G,64,H);
		fclose(file);
	}
}

void writeFile(char *inIP, int inPort, char *outIP, int outPort,char *A, int *B,char *C, int *D, char *E, int *F, char*G, int *H){
	TCHAR nameFile[50];
	FILE *file;
	errno_t err;
	_stprintf_s(nameFile,sizeof(nameFile),_T("Config.txt"));
	err = _tfopen_s(&file, nameFile, _T("w"));
	if (err == 0)
	{
		fprintf_s(file,"%s\n%d\n%s\n%d\n%s\n%d\n%s\n%d\n%s\n%d\n%s\n%d\n",inIP,inPort,outIP,outPort,A,B,C,D,E,F,G,H);
		fclose(file);
	}
	else{
		//MessageBox(NULL,_T("Error opening file!"),_T("Error"),MB_OK);
	}
}
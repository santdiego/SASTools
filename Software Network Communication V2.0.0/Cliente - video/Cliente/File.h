#ifndef FILE_H_FILE
	#define FILE_H_FILE

	void readFile(char *inIP, USHORT *inPort, char *outIP, USHORT *outPort, char *serverIP, USHORT *serverPort, char *protocol);
	void writeFile(char *inIP, USHORT inPort, char *outIP, USHORT outPort, char *serverIP, USHORT serverPort, char *protocol);

#endif
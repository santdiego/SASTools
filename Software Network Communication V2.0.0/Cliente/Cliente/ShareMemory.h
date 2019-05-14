// File: ShareMemory.h

#ifndef SHAREMEMORY_H_FILE
	#define SHAREMEMORY_H_FILE

#define NumPairsMemory	1
#define MAXNUMFLOAT 1024
#define MEMDATOSC1 _T("MemoriaDatoCliente1")
#define MEMREDC1 _T("MemoriaRedCliente1")


typedef struct tag_NET_VIRTUAL_DEVICE_IN {

	bool signalWrite;	//Se leen los flancos
	UINT sizeMemoryOut;  //Cantidad de floats

} NET_VIRTUAL_DEVICE_IN;

typedef struct tag_NET_VIRTUAL_DEVICE_OUT {

	bool signalRead;	//Se leen los flancos
	UINT numberSentPackets;
	UINT numberRecvPackets;
	UINT numberLostPackets;
	UINT numberDelPackets;
	float rateSentPackets;
	float rateRecvPackets;
	float rateLostPackets;
	float rateDelPackets;
	float roundTripDelay;

	UINT remoteNumberSentPackets;
	UINT remoteNumberRecvPackets;
	UINT remoteNumberLostPackets;
	UINT remoteNumberDelPackets;
	float remoteRateSentPackets;
	float remoteRateRecvPackets;
	float remoteRateLostPackets;
	float remoteRateDelPackets;
	float remoteRoundTripDelay;

} NET_VIRTUAL_DEVICE_OUT;


typedef struct tag_VIRTUAL_DEVICE_IN {

	float dataMemory[MAXNUMFLOAT];

} VIRTUAL_DEVICE_IN;

typedef struct tag_VIRTUAL_DEVICE_OUT {

	float dataMemory[MAXNUMFLOAT];

} VIRTUAL_DEVICE_OUT;

#endif



#ifndef NET_H_FILE
	#define NET_H_FILE

	#define CODE_CONNECT	0x11
	#define CODE_ACEPT		0x22
	#define CODE_DATA		0x44
	#define CODE_CLOSE		0x88
	#define CODE_VERIF		0xAA

	typedef struct tag_CHANNEL_PROPERTIES {
		UINT numberSentPackets;
		UINT numberRecvPackets;
		UINT numberLostPackets;
		UINT numberDelPackets;
		double rateSentPackets;
		double rateRecvPackets;
		double rateLostPackets;
		double rateDelPackets;
		double roundTripDelay;
	} CHANNEL_PROPERTIES;

	DWORD WINAPI Connect(LPVOID lpParameter);

#endif
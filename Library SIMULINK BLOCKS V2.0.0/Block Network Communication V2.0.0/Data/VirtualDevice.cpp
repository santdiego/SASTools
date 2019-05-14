static VIRTUAL_DEVICE_IN *pVirtualDevice_in[NumPairsMemory];
static VIRTUAL_DEVICE_OUT *pVirtualDevice_out[NumPairsMemory];
static HANDLE hShare_in[NumPairsMemory];
static HANDLE hShare_out[NumPairsMemory];
static TCHAR *NamePairMemory[NumPairsMemory];
static int Index = 0;


static NET_VIRTUAL_DEVICE_IN *pNetVirtualDevice_in[NumPairsMemory];
static NET_VIRTUAL_DEVICE_OUT *pNetVirtualDevice_out[NumPairsMemory];
static HANDLE hNetShare_in[NumPairsMemory];
static HANDLE hNetShare_out[NumPairsMemory];
static TCHAR *NetNamePairMemory[NumPairsMemory];
static int NetIndex = 0;

BOOL OpenPairSharedMemory(TCHAR *cpName)
{
	int l;
	TCHAR name_in[256];
	TCHAR name_out[256];

	for(l = 0; l<Index; l++){
		if(NamePairMemory[l]==cpName){
			MessageBox(NULL,_T("Error: THE PAIR OF SHARE MEMORY WAS ALREADY OPEN"), _T("Error"),MB_OK);
			return FALSE;
		}
	}
		
	StringCchPrintf( name_in, 128, TEXT("%s_in"),cpName); 
	StringCchPrintf( name_out, 128, TEXT("%s_out"),cpName); 
	
	hShare_in[Index] = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, PAGE_READWRITE, 0, sizeof(VIRTUAL_DEVICE_IN), name_in);
	hShare_out[Index] = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, PAGE_READWRITE, 0, sizeof(VIRTUAL_DEVICE_OUT), name_out);

	if(hShare_in[Index] == NULL || hShare_out[Index] == NULL){
		MessageBox(NULL,_T("Error: Open Virtual Device::CreateFileMapping"),_T("Error"),MB_OK);
		return FALSE;
	}else{
		pVirtualDevice_in[Index] = (VIRTUAL_DEVICE_IN *) MapViewOfFile(hShare_in[Index],FILE_MAP_ALL_ACCESS, 0, 0, sizeof(VIRTUAL_DEVICE_IN));
		pVirtualDevice_out[Index] = (VIRTUAL_DEVICE_OUT *) MapViewOfFile(hShare_out[Index],FILE_MAP_ALL_ACCESS, 0, 0, sizeof(VIRTUAL_DEVICE_OUT));
	}

	if(pVirtualDevice_in[Index] == NULL || pVirtualDevice_out[Index] == NULL){
		MessageBox(NULL,_T("Error: Open Virtual Device::MapViewOfFile"),_T("Error"),MB_OK);
		return FALSE;
	}

	NamePairMemory[Index]=cpName;
	Index=Index+1;
		
	return TRUE;
}

BOOL ClosePairSharedMemory(TCHAR *cpName)
{
	int h = -1, l, k;
	for(l = 0; l<Index; l++){
		if(_tcscmp(NamePairMemory[l],cpName)==0){
			h=l;
		}
	}

	if(h != -1){
		
		if( FlushViewOfFile(pVirtualDevice_in[h], sizeof(VIRTUAL_DEVICE_IN)) == 0 || FlushViewOfFile(pVirtualDevice_out[h], sizeof(VIRTUAL_DEVICE_OUT)) == 0){
			MessageBox(NULL,_T("Error: Close Virtual Device::FlushViewOfFile"),_T("Error"),MB_OK);
			return FALSE;
		}

		if( UnmapViewOfFile(pVirtualDevice_in[h]) == 0 || UnmapViewOfFile(pVirtualDevice_out[h]) == 0){
			MessageBox(NULL,_T("Error: Close Virtual Device::UnmapViewOfFile"),_T("Error"),MB_OK);
			return FALSE;
		}

		if(CloseHandle(hShare_in[h]) == 0 || CloseHandle(hShare_out[h]) == 0){
			MessageBox(NULL,_T("Error: Close Virtual Device::CloseHandle"),_T("Error"),MB_OK);
			return FALSE;
		}

		for(k = h; k<(Index-1); k++){
			pVirtualDevice_in[k] = pVirtualDevice_in[k+1];
			pVirtualDevice_out[k] = pVirtualDevice_out[k+1];
			hShare_in[k] = hShare_in[k+1];
			hShare_out[k] = hShare_out[k+1];
			NamePairMemory[k] = NamePairMemory[k+1];
		}

		pVirtualDevice_in[Index-1] = NULL;
		pVirtualDevice_out[Index-1] = NULL;
		hShare_in[Index-1] = NULL;
		hShare_out[Index-1] = NULL;
		NamePairMemory[Index-1] = NULL;

		Index=Index-1;

		return TRUE;

	}else{
		MessageBox(NULL,_T("Error: THE PAIR SHARE MEMORY WAS ALREADY CLOSE"),_T("Error"),MB_OK);
		return FALSE;
	}
}

BOOL ReadShareMemory(TCHAR *cpName, VIRTUAL_DEVICE_OUT *pfData)
{
	int h = -1, l;
	for(l = 0; l<Index; l++){
		if(_tcscmp(NamePairMemory[l],cpName)==0){
			h=l;
		}
	}

	if(h != -1){
		CopyMemory((VIRTUAL_DEVICE_OUT *)pfData, (VIRTUAL_DEVICE_OUT *)pVirtualDevice_out[h], sizeof(VIRTUAL_DEVICE_OUT));
	}
	else{
		MessageBox(NULL,_T("Error: Read Virtual Device CANT FIND THE SHARE MEMORY"),_T("Error"),MB_OK);
		return FALSE;
	}

return TRUE;
}


BOOL WriteShareMemory(TCHAR *cpName, VIRTUAL_DEVICE_IN *pfData)
{
	int h = -1, l;
	for(l = 0; l<Index; l++){
		if(_tcscmp(NamePairMemory[l],cpName)==0){
			h=l;
		}
	}

	if(h != -1){

		CopyMemory((VIRTUAL_DEVICE_IN *)pVirtualDevice_in[h], (VIRTUAL_DEVICE_IN *)pfData, sizeof(VIRTUAL_DEVICE_IN));

		if( FlushViewOfFile(pVirtualDevice_in[h], sizeof(VIRTUAL_DEVICE_IN)) == 0){
			MessageBox(NULL,_T("Error: Write Virtual Device::FlushViewOfFile"),_T("Error"),MB_OK);
			return FALSE;
		}
		
	}
	else{
		MessageBox(NULL,_T("Error: Write Virtual Device CANT FIND THE SHARE MEMORY"),_T("Error"),MB_OK);
		return FALSE;
	}
	
	return TRUE;

}


BOOL NetOpenPairSharedMemory(TCHAR *cpName)
{
	int l;
	TCHAR name_in[256];
	TCHAR name_out[256];

	for(l = 0; l<NetIndex; l++){
		if(NetNamePairMemory[l]==cpName){
			MessageBox(NULL,_T("Error: Net THE PAIR OF SHARE MEMORY WAS ALREADY OPEN"), _T("Error"),MB_OK);
			return FALSE;
		}
	}
		
	StringCchPrintf( name_in, 128, TEXT("%s_in"),cpName); 
	StringCchPrintf( name_out, 128, TEXT("%s_out"),cpName); 

	hNetShare_in[NetIndex] = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, PAGE_READWRITE, 0, sizeof(NET_VIRTUAL_DEVICE_IN), name_in);
	hNetShare_out[NetIndex] = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, PAGE_READWRITE, 0, sizeof(NET_VIRTUAL_DEVICE_OUT), name_out);

	if(hNetShare_in[NetIndex] == NULL || hNetShare_out[NetIndex] == NULL){
		MessageBox(NULL,_T("Error: Net Open Virtual Device::CreateFileMapping"),_T("Error"),MB_OK);
		return FALSE;
	}else{
		pNetVirtualDevice_in[NetIndex] = (NET_VIRTUAL_DEVICE_IN *) MapViewOfFile(hNetShare_in[NetIndex],FILE_MAP_ALL_ACCESS, 0, 0, sizeof(NET_VIRTUAL_DEVICE_IN));
		pNetVirtualDevice_out[NetIndex] = (NET_VIRTUAL_DEVICE_OUT *) MapViewOfFile(hNetShare_out[NetIndex],FILE_MAP_ALL_ACCESS, 0, 0, sizeof(NET_VIRTUAL_DEVICE_OUT));
	}

	if(pNetVirtualDevice_in[NetIndex] == NULL || pNetVirtualDevice_out[NetIndex] == NULL){
		MessageBox(NULL,_T("Error: Net Open Virtual Device::MapViewOfFile"),_T("Error"),MB_OK);
		return FALSE;
	}

	NetNamePairMemory[NetIndex]=cpName;
	NetIndex=NetIndex+1;
		
	return TRUE;
}

BOOL NetClosePairSharedMemory(TCHAR *cpName)
{
	int h = -1, l, k;
	for(l = 0; l<NetIndex; l++){
		if(_tcscmp(NetNamePairMemory[l],cpName)==0){
			h=l;
		}
	}

	if(h != -1) {
		
		if( FlushViewOfFile(pNetVirtualDevice_in[h], sizeof(NET_VIRTUAL_DEVICE_IN)) == 0 || FlushViewOfFile(pNetVirtualDevice_out[h], sizeof(NET_VIRTUAL_DEVICE_OUT)) == 0){
			MessageBox(NULL,_T("Error: Close Virtual Device::FlushViewOfFile"),_T("Error"),MB_OK);
			return FALSE;
		}

		if( UnmapViewOfFile(pNetVirtualDevice_in[h]) == 0 || UnmapViewOfFile(pNetVirtualDevice_out[h]) == 0){
			MessageBox(NULL,_T("Error: Close Virtual Device::UnmapViewOfFile"),_T("Error"),MB_OK);
			return FALSE;
		}

		if(CloseHandle(hNetShare_in[h]) == 0 || CloseHandle(hNetShare_out[h]) == 0){
			MessageBox(NULL,_T("Error: Close Virtual Device::CloseHandle"),_T("Error"),MB_OK);
			return FALSE;
		}

		for(k = h; k<(NetIndex-1); k++){
			pNetVirtualDevice_in[k] = pNetVirtualDevice_in[k+1];
			pNetVirtualDevice_out[k] = pNetVirtualDevice_out[k+1];
			hNetShare_in[k] = hNetShare_in[k+1];
			hNetShare_out[k] = hNetShare_out[k+1];
			NetNamePairMemory[k] = NetNamePairMemory[k+1];
		}

		pNetVirtualDevice_in[NetIndex-1] = NULL;
		pNetVirtualDevice_out[NetIndex-1] = NULL;
		hNetShare_in[NetIndex-1] = NULL;
		hNetShare_out[NetIndex-1] = NULL;
		NetNamePairMemory[NetIndex-1] = NULL;

		NetIndex=NetIndex-1;

		return TRUE;

	}else{
		MessageBox(NULL,_T("Error: THE PAIR SHARE MEMORY WAS ALREADY CLOSE"),_T("Error"),MB_OK);
		return FALSE;
	}
}

BOOL NetReadShareMemory(TCHAR *cpName, NET_VIRTUAL_DEVICE_OUT *pfData)
{
	int h = -1, l;
	for(l = 0; l<NetIndex; l++){
		if(_tcscmp(NetNamePairMemory[l],cpName)==0){
			h=l;
		}
	}
	if(h != -1){
		CopyMemory((NET_VIRTUAL_DEVICE_OUT *)pfData, (NET_VIRTUAL_DEVICE_OUT *)pNetVirtualDevice_out[h], sizeof(NET_VIRTUAL_DEVICE_OUT));
	}
	else{
		MessageBox(NULL,_T("Error: Read Virtual Device CANT FIND THE SHARE MEMORY"),_T("Error"),MB_OK);
		return FALSE;
	}

return TRUE;
}


BOOL NetWriteShareMemory(TCHAR *cpName, NET_VIRTUAL_DEVICE_IN *pfData)
{
	int h = -1, l;
	for(l = 0; l<NetIndex; l++){
		if(_tcscmp(NetNamePairMemory[l],cpName)==0){
			h=l;
		}
	}

	if(h != -1){

		CopyMemory((NET_VIRTUAL_DEVICE_IN *)pNetVirtualDevice_in[h], (NET_VIRTUAL_DEVICE_IN *)pfData, sizeof(NET_VIRTUAL_DEVICE_IN));

		if( FlushViewOfFile(pNetVirtualDevice_in[h], sizeof(NET_VIRTUAL_DEVICE_IN)) == 0){
			MessageBox(NULL,_T("Error: Write Virtual Device::FlushViewOfFile"),_T("Error"),MB_OK);
			return FALSE;
		}
		
	}
	else{
		MessageBox(NULL,_T("Error: Write Virtual Device CANT FIND THE SHARE MEMORY"),_T("Error"),MB_OK);
		return FALSE;
	}
	
	return TRUE;
} 

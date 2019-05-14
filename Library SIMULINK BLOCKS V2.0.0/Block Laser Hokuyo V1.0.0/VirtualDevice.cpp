static VIRTUAL_DEVICE_OUT *pVirtualDevice_out[numMemory];
static HANDLE hShare_out[numMemory];
static TCHAR *nameMemory[numMemory];
static int Index = 0;

BOOL OpenSharedMemory(TCHAR *cpName)
{
	int l;
	TCHAR name_out[256];

	for(l = 0; l<Index; l++){
		if(nameMemory[l]==cpName){
			MessageBox(NULL,_T("Error: THE SHARE MEMORY WAS ALREADY OPEN"), _T("Error"),MB_OK);
			return FALSE;
		}
	}
		
	StringCchPrintf( name_out, 256, TEXT("%s_out"),cpName); 
	
	hShare_out[Index] = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, PAGE_READWRITE, 0, sizeof(VIRTUAL_DEVICE_OUT), name_out);

	if( hShare_out[Index] == NULL){
		MessageBox(NULL,_T("Error: Open Virtual Device::CreateFileMapping"),_T("Error"),MB_OK);
		return FALSE;
	}else{
		pVirtualDevice_out[Index] = (VIRTUAL_DEVICE_OUT *) MapViewOfFile(hShare_out[Index],FILE_MAP_ALL_ACCESS, 0, 0, sizeof(VIRTUAL_DEVICE_OUT));
	}

	if(pVirtualDevice_out[Index] == NULL){
		MessageBox(NULL,_T("Error: Open Virtual Device::MapViewOfFile"),_T("Error"),MB_OK);
		return FALSE;
	}

	nameMemory[Index]=cpName;
	Index=Index+1;
		
	return TRUE;
}

BOOL CloseSharedMemory(TCHAR *cpName)
{
	int h = -1, l, k;
	for(l = 0; l<Index; l++){
		if(_tcscmp(nameMemory[l],cpName)==0){
			h=l;
		}
	}

	if(h != -1){
		
		if( FlushViewOfFile(pVirtualDevice_out[h], sizeof(VIRTUAL_DEVICE_OUT)) == 0){
			MessageBox(NULL,_T("Error: Close Virtual Device::FlushViewOfFile"),_T("Error"),MB_OK);
			return FALSE;
		}

		if( UnmapViewOfFile(pVirtualDevice_out[h]) == 0){
			MessageBox(NULL,_T("Error: Close Virtual Device::UnmapViewOfFile"),_T("Error"),MB_OK);
			return FALSE;
		}

		if( CloseHandle(hShare_out[h]) == 0){
			MessageBox(NULL,_T("Error: Close Virtual Device::CloseHandle"),_T("Error"),MB_OK);
			return FALSE;
		}

		for(k = h; k<(Index-1); k++){
			pVirtualDevice_out[k] = pVirtualDevice_out[k+1];
			hShare_out[k] = hShare_out[k+1];
			nameMemory[k] = nameMemory[k+1];
		}

		pVirtualDevice_out[Index-1] = NULL;
		hShare_out[Index-1] = NULL;
		nameMemory[Index-1] = NULL;

		Index=Index-1;

		return TRUE;

	}else{
		MessageBox(NULL,_T("Error: THE SHARE MEMORY WAS ALREADY CLOSE"),_T("Error"),MB_OK);
		return FALSE;
	}
}

BOOL ReadShareMemory(TCHAR *cpName, VIRTUAL_DEVICE_OUT *pfData)
{
	int h = -1, l;
	for(l = 0; l<Index; l++){
		if(_tcscmp(nameMemory[l],cpName)==0){
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
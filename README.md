# SystemInfo :: C++ Win32 Project
### This app can show system information like CPU, GPU, HDD , RAM informations developed on C++ and Win32 API.
<p align = "center">
<img src="https://github.com/Santho07/System-Information-Project/blob/master/System%20Info.jpg" width="496" height="559">
<p/>

## Featuers 

- CPU Description 
- Vendor Name 
- Number of Cores
- GPU Name 
- GPU Memomry (Dedicated Video Memory, Shared Video Memory, Shared System Memory)
- RAM Info

## Tools & Technology
- Visual C++
- Win32 API
- Microsoft Visual Studio 2017

## How To Run the project
Download the project then run main.cpp in c++ compiler or open the project in visual studio by opening SystemInfo.vcxproj file.
Then buil and run the project from visual studio.

## Functions Overview 
### CPU Information
To extract CPU information using Win32 API, CPUID instructions are used. 
### CPUID
In the x86 architecture, the CPUID instruction (identified by a CPUID opcode) is a processor supplementary instruction (its name derived from CPU IDentification) allowing software to discover details of the processor. It was introduced by Intel in 1993 when it introduced the Pentium and SL-enhanced 486 processors.[1]
A program can use the CPUID to determine processor type and whether features such as MMX/SSE are implemented.
[More on WIKIPEDIA: CPUID](https://en.wikipedia.org/wiki/CPUID)

### GPU Information
To extract GPU information, d3d9.h Library support is used. To obtain video memory information, dxgi.h library is used.
```C++
void showGpuInfo(HWND hwnd, HDC hdc) {

	/* **** show GPU name *******/
	D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
	initD3D(hwnd);

	d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &AdapterIdentifier);
	LPCWSTR msg = convertCharToLPCWSTR(AdapterIdentifier.Description);
	TextOutW(hdc, rect.left+150, rect.top+120, msg, wcslen(msg));

	/***** Show GPU MEMORY **********/
	ID3D10Device* g_pd3dDevice;
	D3D10CreateDevice(NULL, D3D10_DRIVER_TYPE_REFERENCE, NULL, 0,
		D3D10_SDK_VERSION, &g_pd3dDevice);
	IDXGIDevice * pDXGIDevice;
	HRESULT hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
	IDXGIAdapter * pDXGIAdapter;
	pDXGIDevice->GetAdapter(&pDXGIAdapter);
	DXGI_ADAPTER_DESC adapterDesc;
	pDXGIAdapter->GetDesc(&adapterDesc); //form the adapter desc we extract memory information
  ```

## HDD Information
To show HDD information, GlobalMemoryStatusEx(); method is used of win32 api library.
```C++ 
void showHDDInfo(HDC hdc) {

	DISK_GEOMETRY pdg = { 0 }; // disk drive geometry structure
	BOOL bResult = FALSE;      // generic results flag
	ULONGLONG DiskSize = 0;    // size of the drive, in bytes

	bResult = GetDriveGeometry(wszDrive, &pdg);

	if (bResult)
	{
		/* wprintf(L"Drive path      = %ws\n", wszDrive);
		wprintf(L"Cylinders       = %I64d\n", pdg.Cylinders);
		wprintf(L"Tracks/cylinder = %ld\n", (ULONG)pdg.TracksPerCylinder);
		wprintf(L"Sectors/track   = %ld\n", (ULONG)pdg.SectorsPerTrack);
		wprintf(L"Bytes/sector    = %ld\n", (ULONG)pdg.BytesPerSector); */
		TCHAR s4[] = TEXT("Drive Path: ");
		TextOut(hdc, rect.left + 120, rect.top + 240, s4, wcslen(s4));
		TextOut(hdc, rect.left+240, rect.top+240, wszDrive, wcslen(wszDrive));

		DiskSize = pdg.Cylinders.QuadPart * (ULONG)pdg.TracksPerCylinder *
			(ULONG)pdg.SectorsPerTrack * (ULONG)pdg.BytesPerSector;
		double f = (double)DiskSize / (1024 * 1024 * 1024); 
		TCHAR buffer[1024];
		LPCTSTR pszFormat = TEXT("%lf");
		StringCchPrintf(buffer, sizeof(buffer) / sizeof(TCHAR), pszFormat, f);
		TCHAR s5[] = TEXT("HDD Capacity (GB) : ");
		TextOut(hdc, rect.left + 120, rect.top + 260, s5, wcslen(s5));
		TextOutW(hdc, rect.left+270, rect.top+260, buffer, wcslen(buffer));
	}
}
```

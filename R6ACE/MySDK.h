#pragma once
#include <windows.h>
#include <iostream>
#include "auth.hpp"
#include "skStr.h"


std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);

using namespace KeyAuth;

std::string name = "r6"; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = "KksyZqGi2O"; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = "f3a10c1c98fbbe1001cfd6a54d3469ba02eb1dbd32681512c0c18f9cc5dbb886"; // app secret, the blurred text on licenses tab and other tabs
std::string version = "1.0"; // leave alone unless you've changed version on website
std::string url = "https://keyauth.win/api/1.2/"; // change if you're self-hosting

api KeyAuthApp(name, ownerid, secret, version, url);

bool validation()
{
	AllocConsole();
	freopen("CON", "w", stdout);
	freopen("CON", "r", stdin);
	freopen("CON", "w", stderr);
	SetConsoleTitleA("cwp");

	KeyAuthApp.init();
	std::string key;

	std::cout << "Please Enter Your Key: ";
	std::cin >> key;
	KeyAuthApp.license(key);

	if (!KeyAuthApp.data.success)
	{
		std::cout << "\nInvalid key or hardware changed!";
		Sleep(1500);
		exit(0);
	}
	std::cout << "\nClose This Window And Start Your Game Now!\n";
	FreeConsole();
	return true;
}
//bool validation()
//{
//	AllocConsole();
//	freopen("CON", "w", stdout);
//	freopen("CON", "r", stdin);
//	freopen("CON", "w", stderr);
//	SetConsoleTitleA(skCrypt("R6 Cheat by Roncik"));
//
//	std::cout << "zzzz";
//	KeyAuthApp.init();
//	if (!KeyAuthApp.data.success)
//	{
//		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
//		Sleep(1500);
//		exit(0);
//	}
//
//	/*
//		Optional - check if HWID or IP blacklisted
//
//	if (KeyAuthApp.checkblack()) {
//		abort();
//	}
//	*/
//
//	std::cout << skCrypt("\n\n App data:");
//	std::cout << skCrypt("\n Number of users: ") << KeyAuthApp.data.numUsers;
//	std::cout << skCrypt("\n Number of online users: ") << KeyAuthApp.data.numOnlineUsers;
//	std::cout << skCrypt("\n Number of keys: ") << KeyAuthApp.data.numKeys;
//	std::cout << skCrypt("\n Application Version: ") << KeyAuthApp.data.version;
//	std::cout << skCrypt("\n Customer panel link: ") << KeyAuthApp.data.customerPanelLink;
//	std::cout << skCrypt("\n Checking session validation status (remove this if causing your loader to be slow)");
//	KeyAuthApp.check();
//	std::cout << skCrypt("\n Current Session Validation Status: ") << KeyAuthApp.data.message;
//
//	std::cout << skCrypt("\n\n [1] Login\n [2] Register\n [3] Upgrade\n [4] License key only\n\n Choose option: ");
//
//	int option;
//	std::string username;
//	std::string password;
//	std::string key;
//
//	std::cin >> option;
//	switch (option)
//	{
//	case 1:
//		std::cout << skCrypt("\n\n Enter username: ");
//		std::cin >> username;
//		std::cout << skCrypt("\n Enter password: ");
//		std::cin >> password;
//		KeyAuthApp.login(username, password);
//		break;
//	case 2:
//		std::cout << skCrypt("\n\n Enter username: ");
//		std::cin >> username;
//		std::cout << skCrypt("\n Enter password: ");
//		std::cin >> password;
//		std::cout << skCrypt("\n Enter license: ");
//		std::cin >> key;
//		KeyAuthApp.regstr(username, password, key);
//		break;
//	case 3:
//		std::cout << skCrypt("\n\n Enter username: ");
//		std::cin >> username;
//		std::cout << skCrypt("\n Enter license: ");
//		std::cin >> key;
//		KeyAuthApp.upgrade(username, key);
//		break;
//	case 4:
//		std::cout << skCrypt("\n Enter license: ");
//		std::cin >> key;
//		KeyAuthApp.license(key);
//		break;
//	default:
//		std::cout << skCrypt("\n\n Status: Failure: Invalid Selection");
//		Sleep(3000);
//		exit(0);
//	}
//
//	if (!KeyAuthApp.data.success)
//	{
//		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
//		Sleep(1500);
//		exit(0);
//	}
//
//
//	std::string subs;
//	for (std::string value : KeyAuthApp.data.subscriptions)subs += value + " ";
//	std::cout << subs;
//	std::cout << skCrypt("\n Checking session validation status (remove this if causing your loader to be slow)");
//	KeyAuthApp.check();
//	std::cout << skCrypt("\n Current Session Validation Status: ") << KeyAuthApp.data.message;
//}

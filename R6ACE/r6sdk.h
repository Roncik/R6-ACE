#pragma once
#include "DriverComms.h"
#include "Values.h"
#define BYTE0(dwTemp)       (*(char  *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
#define BYTE4(dwTemp)       (*((char *)(&dwTemp) + 4))
#define BYTE5(dwTemp)       (*((char *)(&dwTemp) + 5))
#define BYTE6(dwTemp)       (*((char *)(&dwTemp) + 6))
#define BYTE7(dwTemp)       (*((char *)(&dwTemp) + 7))

#define LODWORD(x)  (*((DWORD*)&(x)))
#define HIDWORD(x)  (*((DWORD*)&(x)+1))

template<class T> T __ROL__(T value, int count)
{
	const unsigned int nbits = sizeof(T) * 8;

	if (count > 0)
	{
		count %= nbits;
		T high = value >> (nbits - count);
		if (T(-1) < 0) // signed value
			high &= ~((T(-1) << count));
		value <<= count;
		value |= high;
	}
	else
	{
		count = -count % nbits;
		T low = value << (nbits - count);
		value >>= count;
		value |= low;
	}
	return value;
}

inline uint8_t  __ROL1__(uint8_t  value, int count) { return __ROL__((uint8_t)value, count); }
inline uint16_t __ROL2__(uint16_t value, int count) { return __ROL__((uint16_t)value, count); }
inline uint32_t __ROL4__(uint32_t value, int count) { return __ROL__((uint32_t)value, count); }
inline uint64_t __ROL8__(uint64_t value, int count) { return __ROL__((uint64_t)value, count); }
inline uint8_t  __ROR1__(uint8_t  value, int count) { return __ROL__((uint8_t)value, -count); }
inline uint16_t __ROR2__(uint16_t value, int count) { return __ROL__((uint16_t)value, -count); }
inline uint32_t __ROR4__(uint32_t value, int count) { return __ROL__((uint32_t)value, -count); }
inline uint64_t __ROR8__(uint64_t value, int count) { return __ROL__((uint64_t)value, -count); }

bool IsValidPTR(uint64_t value)
{
	if (value <= 0x00000FFFFFFFFFFF && value >= 0x0000000FFFFFFFFF)
		return true;
	else
		return false;
}

bool IsValidPlayer(PlayerData player)
{
	if (
		IsValidPTR(player.Pawn) &&
		(player.HeadPosition.x != 0.f)&&
		player.TeamID != val.LocalTeamID)
		return true;
	else
		return false;
}

uint64_t DecryptPtr(uint64_t encryptedValue, uint64_t addr, int v1, int v2, int type) //update 16.11.22
{
	uint64_t table = val.basemodule + 0x6822180;

	uint64_t decodedAnd = encryptedValue & 0xFFFFFFFFFFFFll;
	uint64_t decodedShift = encryptedValue >> 0x30;

	uint64_t finalDecoded = 0;

	uint64_t decrypted = decodedAnd;
	if (type == 0) // RoundManager - update 16.11.22
	{
		finalDecoded = *(unsigned __int16*)((char*)table
			+ (((v1 * (DWORD)decodedShift
				- v2 * (unsigned int)((unsigned __int64)addr >> 3)) >> 22) & 0xFFFFFFFC));

		decrypted ^= (finalDecoded | (finalDecoded << 16) | (finalDecoded << 32));
	}
	else if (type == 1)
	{
		finalDecoded = (ReadPM<uint32_t>(table + (((v1 * (DWORD)decodedShift + v2 * (unsigned int)((unsigned __int64)addr >> 3)) >> 22) & 0xFFFFFFFC)) >> 16);

		decrypted ^= ((unsigned int)finalDecoded | (ReadPM<uint32_t>(table + (((v1 * (DWORD)decodedShift + v2 * (unsigned int)((unsigned __int64)addr >> 3)) >> 22) & 0xFFFFFFFC)) << 16) | (finalDecoded << 32));
	}
	else if (type == 2) // GameManager - update 16.11.22 
	{
		finalDecoded = (ReadPM<uint32_t>(table + (((v1 * (uint32_t)decodedShift + v2 * (uint32_t)(addr >> 3)) >> 0x16) & 0xFFFFFFFC)));
		decrypted ^= ((unsigned __int16)finalDecoded | (finalDecoded >> 16 << 16) | (finalDecoded >> 16 << 32));
	}
	else if (type == 3) // OutlineProbManager - update 16.11.22
	{
		finalDecoded = *(unsigned int*)((char*)table
			+ (((v1 * (DWORD)decodedShift - v2 * (unsigned int)((unsigned __int64)addr >> 3)) >> 22) & 0xFFFFFFFC));
		decrypted ^= ((finalDecoded >> 16) | (finalDecoded >> 16 << 16) | ((unsigned __int64)(unsigned __int16)finalDecoded << 32));
	}
	return decrypted;
}





//uint64_t GetGameManager()  //update 16.11.22
//{
//	uint64_t addr = (val.basemodule + 0x87F1848);
//	if (addr)
//	{
//		uint64_t manager = __ROL8__(__ROL8__(ReadPM<uint64_t>(addr) ^ 0x87B58C551F4C2D9Dui64, 58) - 38i64, 55);
//		return (DecryptPtr(manager, addr, 0xD5A0824D, 0x1188A652, 2));
//	}
//	return 0;
//}
uint64_t GetGameManager()  //update 16.11.22
{
	uint64_t m_game_raw = val.basemodule + 0x818E428;
	uint64_t decryptionkey = val.basemodule + 0x69BB2B0;
	uint64_t v376 = __ROL8__(ReadPM<uint64_t>(m_game_raw) ^ 0xA63509C9880F980Eui64, 55) ^ 0x1B4F1BAFE364A51Ai64;
	uint64_t v377 = (unsigned __int16)__ROL2__(v376, 8) | v376 & 0xFF0000 | (v376 << 16) & 0xFF0000000000i64 | (v376 >> 16) & 0xFF000000 | (v376 >> 16) & 0xFF00000000i64;
	uint64_t v378 = ((v376 << 16) & 0xFF000000000000i64 | v376 & 0xFF00000000000000ui64) >> 48;
	uint64_t v379 = ReadPM<uint32_t>((decryptionkey + (((1506340551 * (DWORD)v378 + 127937202 * (unsigned int)((unsigned __int64)(m_game_raw) >> 3)) >> 22) & 0xFFFFFFFC)));
	uint64_t v380 = v377 ^ (((unsigned __int64)BYTE2(v379) << 16) | ((unsigned __int64)BYTE2(v379) << 8) | ((unsigned __int64)(unsigned __int8)v379 << 40) | ((unsigned __int64)(unsigned __int8)v379 << 32) | BYTE2(v379) | ((unsigned __int64)(unsigned __int8)v379 << 24));
	uint64_t m_game = v380;
	if (IsValidPTR(m_game))
		return m_game;
	else
		return val.gamemgr;
}

uint64_t GetProfileManager() //update 16.11.22
{
	uint64_t addr = (val.basemodule + 0x751BEB0);
	if (addr)
	{
		uint64_t manager = __ROR8__(((ReadPM<uint64_t>(addr) - 86) ^ 0x20) + 0x5D471D7DDEAE48E7i64, 1);
		manager = DecryptPtr(manager, addr, 0xC762F6E6, 0x45011DA2, 1);
		manager = ReadPM<uint64_t>(manager + 0x30) ^ 0xCB9A2F54EC0644B4ui64;
		return manager;
	}
	return 0;
}

inline uint64_t getPlayerList(uintptr_t gamemgr)
{
	return ((ReadPM<uintptr_t>(gamemgr+0xD8) ^ 0xABD41CFCF86723E1) + 0xC7E3CC14FBE41772) ^ 0x7FD2AB99CBEEFFAD;
}

inline uint32_t getPlayerCount(uintptr_t gamemgr)
{
	auto v5 = ((ReadPM<uint64_t>(gamemgr + 0x58) ^ 0x45) - 0x6737F61F2051E9D7i64);
	return static_cast<uint32_t>(v5 & 0x3FFFFFFF);
}

inline uint64_t getLocalPlayer()
{
	unsigned __int64 v0; // r9
	unsigned __int64 v1; // rax
	unsigned __int64 v2; // r10
	unsigned __int64 v3; // rcx
	unsigned __int64 v4; // rdx
	unsigned __int64 v5; // rcx

	v0 = (__ROL8__(ReadPM<uintptr_t>(val.basemodule + 0x782F1D8), 32) ^ 0x6Ci64) + 0x3897E5956BD0F50i64;
	v1 = ((unsigned __int64)(unsigned __int8)v0 << 32) | BYTE1(v0) | (v0 >> 8) & 0xFF00 | (v0 >> 16) & 0xFF000000 | HIDWORD(v0) & 0xFF0000 | (v0 >> 16) & 0xFF0000000000i64;
	v0 >>= 24;
	v2 = (val.basemodule + 0x782F1D8) >> 3;
	v3 = (unsigned __int64)ReadPM<uint32_t>(val.basemodule + 0x69BB2B0
		+ (((988644971 * (unsigned __int16)v0
			- 1949467660 * (unsigned int)((val.basemodule + 0x782F1D8) >> 3)) >> 22) & 0xFFFFFFFC)) >> 24;
	v4 = (unsigned __int64)ReadPM<uint32_t>(val.basemodule + 0x69BB2B0
		+ (((988644971 * (unsigned __int16)v0
			- 1949467660 * (unsigned int)((val.basemodule + 0x782F1D8) >> 3)) >> 22) & 0xFFFFFFFC)) >> 8;
	v5 = v1 ^ (((unsigned __int64)(unsigned __int8)v4 << 32) | ((unsigned __int64)(unsigned __int8)v4 << 24) | v3 | (unsigned __int16)v4 & 0xFF00 | (v3 << 16) | (v3 << 40));
	uint64_t result = (v5);

	/*if (result)
		return result;
	else
		return 0;*/
	uint64_t profile_manager = result;
	uint64_t returnval = ReadPM<uintptr_t>(__ROL8__(ReadPM<uintptr_t>(profile_manager + 0x28) ^ 0x4BA1CCEBAB4A7ED9i64, 32) - 0x417764DDE49FA91i64);;
	if (IsValidPTR(returnval) && result)
	{
		returnval = __ROL8__(ReadPM<uintptr_t>(returnval + 0x28) ^ 0x147B74CCDFA0FF8Di64, 9) - 60i64;
		return returnval;
	}
	else
		return val.LocalPlayer;
}

inline uint64_t GetPawn(uint64_t player)
{
	return  __ROL8__(ReadPM<uintptr_t>(player + 0x38) - 0x22i64, 0x37) - 0x18i64;
}

inline uint64_t GetActor(uint64_t pawn)
{
	return ((ReadPM<uintptr_t>(pawn + 0x18) ^ 0xE02A8E8DC664A902ui64) - 0x5F) ^ 0x2333F563B66FDC1Fi64;
}

inline Vec3 GetPos(uint64_t actor)
{
	return ReadPM<Vec3>(actor + 0x50);
}

inline uintptr_t CameraManager()
{
	uint64_t camera_mgr = ReadPM<uint64_t>(val.basemodule + 0x6756BB8);
	camera_mgr = (((ReadPM<uint64_t>(camera_mgr + 0x40) + 0x19A9BFFE16D27803i64) ^ 0xAA3060F8F1063F18ui64) + 0x3AB701F3CB3A062Fi64);
	camera_mgr = ReadPM<uint64_t>(camera_mgr);
	return camera_mgr;

}

uintptr_t getReplication(uintptr_t player)
{
	uintptr_t value = __ROL8__(ReadPM<uint64_t>(player + 0x98) ^ 0xE83922DA7E8D36F0ui64, 32) + 0x546A7F6A667D697Ai64;
	return value;
}

uint32_t getTeamId(uintptr_t replication)
{
	if (replication)
		return __ROL1__(ReadPM<BYTE>(replication + 0x92E), 4) + 8;
	else
		return -1;
}

uint64_t current_weapon(uint64_t actor)
{
	const uint8_t index = ReadPM<uint8_t>(actor + 0x225);

	const uintptr_t component = ReadPM<uint64_t>(actor + 0xD8);
	const uintptr_t array_entry = ReadPM<uint64_t>(component + (index * sizeof(uintptr_t)));

	//uintptr_t entry = ReadPM<uint64_t>(array_entry + 0x318) ^ 0x2C579ECA354514CDi64;
	//uintptr_t entry = (__ROL8__((ReadPM<uintptr_t>(array_entry + 0x70)+0x8D75703CF9F87DE1),0x31))+0x9C365B44DEDE18F4;
	uintptr_t entry = __ROL8__(((ReadPM<uintptr_t>(array_entry + 0x88)+0x87AF31FD84213E32)^0x40887706E6646F78),0x8);
	entry = ReadPM<uint64_t>(entry);

	const uintptr_t weapon_outer = ReadPM<uint64_t>(entry + 0x198);

	auto weapon = _rotl64(ReadPM<uint64_t>(weapon_outer + 0x268) ^ 0xE4725402997E0340ui64, 10) - 0x7002FACF220273B2i64;

	return weapon;
}
//inline uint64_t current_weapon(uint64_t actor)
//{
//	uintptr_t component = ReadPM<uintptr_t>(actor + 0xD8);
//	if (!component) {
//		return 0;
//	}
//	BYTE index = ReadPM<BYTE>(actor + 0x223);
//	if (!index) {
//		return 0;
//	}
//	uintptr_t weaponArray = ReadPM<uintptr_t>(component + (index * sizeof(uintptr_t)));
//	if (!weaponArray) {
//		return 0;
//	}
//	uintptr_t entry = ((ReadPM<uintptr_t>(weaponArray + 0x70) - 77i64) ^ 0x7F) - 0x17952C0716F342A2i64;
//
//	entry = ReadPM<uintptr_t>(entry);
//	if (!entry) {
//		return 0;
//	}
//	uintptr_t array_entry = ReadPM<uintptr_t>(entry + 0x198);
//	if (!array_entry) {
//		return 0;
//	}
//	uintptr_t current_weapon = (__ROL8__(ReadPM<uintptr_t>(array_entry + 0x268) ^ 0x3D9D63846A29925Ci64, 36) - 41i64);
//	if (!current_weapon) {
//		return 0;
//	}
//	return current_weapon;
//}

inline uint64_t GetOutlineComponent(uint64_t& actor)
{
	return  ((ReadPM<uintptr_t>(actor + 0x1C8) ^ 0x52042F7BFF6EF580i64) - 0x2647C3739F80E60Di64) ^ 0x616C499CC18F3E64i64;
}



inline Vec3 GetPosition(uint64_t actor)
{
	return ReadPM<Vec3>(actor + 0x50);
}

static uint64_t decrypt_camera_manager(const uint64_t value)
{
	return ReadPM<uint64_t>(_rotl64(0xA5F8DEA6C0859E3B + ReadPM<uint64_t>(value + 0x40), 9) - 7597356491596278441);
}

static uint64_t getCameraManager()
{
	
		const auto v9 = (val.basemodule + (0x7FF7B1F30028 - 0x7FF7AB110000)) >> 3;
		auto v16 = _rotl64(ReadPM<uint64_t>(val.basemodule + (0x7FF7B1F30028 - 0x7FF7AB110000)) - 4059317596833740613, 9) - 5169483632978739241;
		const auto v17 = (v16 >> 40) & 0xFF0000 | BYTE6(v16) | (v16 << 16) & 0xFF00000000 | (v16 << 40) & 0xFF0000000000 | (unsigned int)v16 & 0xFF00FF00;
		v16 >>= 32;
		const auto v18 = ReadPM<uint32_t>((val.basemodule + (0x7FF7B1ACB2B0 - 0x7FF7AB110000)) + (((-339064247 * (unsigned int)(unsigned __int16)v16 - 1490964117 * (DWORD)v9) >> 22) & 0xFFFFFFFC));
		const auto result = v17 ^ (((unsigned __int64)(unsigned __int8)((unsigned __int16)ReadPM<uint32_t>((val.basemodule + (0x7FF7B1ACB2B0 - 0x7FF7AB110000))
			+ (((-339064247
				* (unsigned int)(unsigned __int16)v16
				- 1490964117 * (DWORD)v9) >> 22) & 0xFFFFFFFC)) >> 8) << 32) | (unsigned __int16)_rotl16(ReadPM<uint32_t>((val.basemodule + (0x7FF7B1ACB2B0 - 0x7FF7AB110000)) + (((-339064247 * (unsigned int)(unsigned __int16)v16 - 1490964117 * (DWORD)v9) >> 22) & 0xFFFFFFFC)), 8) | (unsigned __int64)((ReadPM<uint32_t>((val.basemodule + (0x7FF7B1ACB2B0 - 0x7FF7AB110000)) + (((-339064247 * (unsigned int)(unsigned __int16)v16 - 1490964117 * (DWORD)v9) >> 22) & 0xFFFFFFFC)) >> 8) & 0xFF0000) | ((unsigned __int64)(v18 & 0xFF0000) << 8) | ((unsigned __int64)(unsigned __int8)v18 << 40));

		if (result == 0 || result % 8 != 0) return 0;

		uint64_t cameramgr = decrypt_camera_manager(result);
		if (IsValidPTR(cameramgr))
			return cameramgr;
		else
			return val.CameraManager;
}

static uint64_t view_data_decryption_xor_value, view_data_decryption_add_values[2];

static void init_view_data_decryption(void)
{
	view_data_decryption_xor_value = 0xB2351CF63952EFAF;
	view_data_decryption_add_values[0] = 0x36A8DD82F5258E2C;
	view_data_decryption_add_values[1] = 0x931DBF4A492ACDE8;
}

static uint64_t decrypt_view_data(const uint64_t x)
{
	const auto temp = ((x + view_data_decryption_add_values[0]) ^ view_data_decryption_xor_value) + view_data_decryption_add_values[1];
	view_data_decryption_xor_value -= 0x67F9ACDEDE860D8F;
	view_data_decryption_add_values[0] += 0x729DF044C4A5FC0E;
	view_data_decryption_add_values[1] += (0x0C582924F53CC7ED6 - 0x931DBF4A492ACDE8);
	return temp;
}


bool WorldToScreen(Vec3 world, Vec3& screen)
{
	//CameraData = ReadPM<CameraMgr>(val.CameraManager+0xF0);
	val.CameraData = ReadPM<CameraMgr>(val.CameraManager + 0xF0);
	init_view_data_decryption();

	__m128i pRight = val.CameraData.pRight;
	pRight.m128i_i64[0] = decrypt_view_data(pRight.m128i_i64[0]);
	pRight.m128i_i64[1] = decrypt_view_data(pRight.m128i_i64[1]);
	Vec3 vecRight = *(Vec3*)&(pRight);

	__m128i pUp = val.CameraData.pUp;
	pUp.m128i_i64[0] = decrypt_view_data(pUp.m128i_i64[0]);
	pUp.m128i_i64[1] = decrypt_view_data(pUp.m128i_i64[1]);
	Vec3 vecUp = *(Vec3*)&(pUp);

	__m128i pForward = val.CameraData.pForward;
	pForward.m128i_i64[0] = decrypt_view_data(pForward.m128i_i64[0]);
	pForward.m128i_i64[1] = decrypt_view_data(pForward.m128i_i64[1]);
	Vec3 vecForward = *(Vec3*)&(pForward);

	__m128i pTranslation = val.CameraData.pTranslation;
	pTranslation.m128i_i64[0] = decrypt_view_data(pTranslation.m128i_i64[0]);
	pTranslation.m128i_i64[1] = decrypt_view_data(pTranslation.m128i_i64[1]);
	Vec3 vecTranslation = *(Vec3*)&(pTranslation);

	Vec3 temp = world - vecTranslation;
	float x = temp.Dot(vecRight);
	float y = temp.Dot(vecUp);
	float z = temp.Dot(vecForward * -1.f);

	//uint64_t fovX_offset = 0x2AC;
	//uint64_t fovY_offset = 0x2A8;
	uint32_t encrypted_fovx = val.CameraData.encrypted_fovx;
	uint32_t encrypted_fovy = val.CameraData.encrypted_fovy;
	uint32_t fovX_ = __ROR4__(__ROR4__(encrypted_fovx, 22) - 0x6DDB55C7, 13); // 0x2A8 
	uint32_t fovY_ = __ROR4__(encrypted_fovy, 15) ^ 0x886E5B73; //0x2B4

	float fovX = *(float*)&fovX_;
	float fovY = *(float*)&fovY_;

	int width = Target.width;
	int height = Target.height;

	screen.x = ((width / 2.f) * (1.f + x / -fovX / z)) - Overlay.OffsetLeft; // fovx
	screen.y = ((height / 2.f) * (1.f - y / -fovY / z)) - Overlay.OffsetTop; // fovy
	if (screen.x < 0.0f || screen.x > width || screen.y < 0.0f || screen.y > height)
		return false;
	return z >= 1.0f;
}

bool WorldToScreennoread(Vec3 world, Vec3& screen)
{
	//CameraData = ReadPM<CameraMgr>(val.CameraManager+0xF0);
	//val.CameraData = ReadPM<CameraMgr>(val.CameraManager + 0xF0);
	init_view_data_decryption();

	__m128i pRight = val.CameraData.pRight;
	pRight.m128i_i64[0] = decrypt_view_data(pRight.m128i_i64[0]);
	pRight.m128i_i64[1] = decrypt_view_data(pRight.m128i_i64[1]);
	Vec3 vecRight = *(Vec3*)&(pRight);

	__m128i pUp = val.CameraData.pUp;
	pUp.m128i_i64[0] = decrypt_view_data(pUp.m128i_i64[0]);
	pUp.m128i_i64[1] = decrypt_view_data(pUp.m128i_i64[1]);
	Vec3 vecUp = *(Vec3*)&(pUp);

	__m128i pForward = val.CameraData.pForward;
	pForward.m128i_i64[0] = decrypt_view_data(pForward.m128i_i64[0]);
	pForward.m128i_i64[1] = decrypt_view_data(pForward.m128i_i64[1]);
	Vec3 vecForward = *(Vec3*)&(pForward);

	__m128i pTranslation = val.CameraData.pTranslation;
	pTranslation.m128i_i64[0] = decrypt_view_data(pTranslation.m128i_i64[0]);
	pTranslation.m128i_i64[1] = decrypt_view_data(pTranslation.m128i_i64[1]);
	Vec3 vecTranslation = *(Vec3*)&(pTranslation);

	Vec3 temp = world - vecTranslation;
	float x = temp.Dot(vecRight);
	float y = temp.Dot(vecUp);
	float z = temp.Dot(vecForward * -1.f);

	//uint64_t fovX_offset = 0x2AC;
	//uint64_t fovY_offset = 0x2A8;
	uint32_t encrypted_fovx = val.CameraData.encrypted_fovx;
	uint32_t encrypted_fovy = val.CameraData.encrypted_fovy;
	uint32_t fovX_ = __ROR4__(__ROR4__(encrypted_fovx, 22) - 0x6DDB55C7, 13); // 0x2A8 
	uint32_t fovY_ = __ROR4__(encrypted_fovy, 15) ^ 0x886E5B73; //0x2B4

	float fovX = *(float*)&fovX_;
	float fovY = *(float*)&fovY_;

	int width = Target.width;
	int height = Target.height;

	screen.x = ((width / 2.f) * (1.f + x / -fovX / z)) - Overlay.OffsetLeft; // fovx
	screen.y = ((height / 2.f) * (1.f - y / -fovY / z)) - Overlay.OffsetTop; // fovy
	if (screen.x < 0.0f || screen.x > width || screen.y < 0.0f || screen.y > height)
		return false;
	return z >= 1.0f;
}

uint64_t get_skeleton(uint64_t pawn)
{
	uint64_t encryption = __ROL8__((ReadPM<uint64_t>(pawn + 0x9B0) ^ 0x92BD5279C3EB4i64) - 0x2C5694CB2E3743DBi64, 21);

	//uint64_t skeleton = ReadPM<uint64_t>(encryption);
	return encryption;
}

inline void TransformsCalculation(__m128* ResultPosition, PlayerData player, int index) //23.10.2021
{
	__m128 v5; // xmm2
	__m128 v6; // xmm3
	__m128 v7; // xmm0
	__m128 v8; // xmm4
	__m128 v9; // xmm1

	__m128 v10 = { 0.500f, 0.500f, 0.500f, 0.500f };
	__m128 v11 = { 2.000f, 2.000f, 2.000f, 0.000f };

	__m128 v12 = player.Pbones.v12;
	__m128 v13 = player.Pbones.v13;

	v5 = v13;

	v6 = _mm_mul_ps(player.PlayerBones.Bone[index].boneinfo, v5);
	v6.m128_f32[0] = v6.m128_f32[0]
		+ (float)(_mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0x55)) + _mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0xAA)));
	v7 = _mm_shuffle_ps(v13, v5, 0xFF);
	v8 = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(v5, v5, 0xD2), player.PlayerBones.Bone[index].boneinfo),
		_mm_mul_ps(_mm_shuffle_ps(player.PlayerBones.Bone[index].boneinfo, player.PlayerBones.Bone[index].boneinfo, 0xD2), v5));
	v9 = _mm_shuffle_ps(v12, v12, 0x93);
	v9.m128_f32[0] = 0.0;
	*(__m128*)ResultPosition = _mm_add_ps(
		_mm_shuffle_ps(v9, v9, 0x39),
		_mm_mul_ps(
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(_mm_shuffle_ps(v8, v8, 0xD2), v7),
					_mm_mul_ps(_mm_shuffle_ps(v6, v6, 0), v5)),
				_mm_mul_ps(
					_mm_sub_ps(_mm_mul_ps(v7, v7), (__m128)v10),
					player.PlayerBones.Bone[index].boneinfo)),
			(__m128)v11));
}


inline pbones get_player_pbones(std::uintptr_t skeleton)
{
	std::uintptr_t pBonesChain2 = skeleton;
	pBonesChain2 = ReadPM<std::uintptr_t>(pBonesChain2);

	//std::uintptr_t pBones = ReadPM<uintptr_t>(pBonesChain2 + 0x250);
	std::uintptr_t pBones;

	if (ReadPM<uint8_t>(pBonesChain2 + 0x250))
		pBones = __ROL8__(((__ROL8__(ReadPM<uintptr_t>(pBonesChain2 + 0x260), 0x31)) + 0xD5402E41778AC67F), 0xD);
	else
		pBones = ((ReadPM<uintptr_t>(pBonesChain2 + 0x258) ^ 0xA46CBD37D0033F34) + 0x2C20CA48311363A7) ^ 0xB3D4D75892238818;

	pbones output = ReadPM<pbones>(pBones);
	return output;
}
inline BonesArray get_player_bones(pbones pboness)
{
	std::uintptr_t pBonesData = pboness.pBonesData;
	BonesArray output = ReadPM<BonesArray>(pBonesData);
	return output;
}

Vec3 get_bone_by_id(int index, PlayerData player)
{
	__m128 Output{};
	TransformsCalculation(&Output, player, index);
	return { Output.m128_f32[0], Output.m128_f32[1], Output.m128_f32[2] };
}

inline float GetHealth(uintptr_t actor)
{
	return __ROR4__((ReadPM<uint32_t>(actor+0x1c0) ^ 0xCE986F1F), 0x18) + 0x2C4B4E05; // 0x1C0
}

//inline float GetHealth(uintptr_t actor)
//{
//	uintptr_t entity_info = ReadPM<uintptr_t>(actor + 0xD8);
//	uintptr_t main_component = ReadPM<uintptr_t>(entity_info + 0x8);
//	return (((ReadPM<int>(main_component + 0x1EC)) ^ 0x4C) - 1) ^ 0xE8AC4AC0;
//}

inline int GetDistance2D(Vec3 first, Vec3 second)
{
	return sqrt(pow((first.x - second.x), 2) + pow((first.y - second.y), 2));
}
inline int GetDistance3D(Vec3 first, Vec3 second)
{
	return sqrt(pow((first.x - second.x), 2) + pow((first.y - second.y), 2) + pow((first.z - second.z), 2));
}
inline float GetDistance3Dflt(Vec3 first, Vec3 second)
{
	return sqrt(pow((first.x - second.x), 2) + pow((first.y - second.y), 2) + pow((first.z - second.z), 2));
}
void AIMBOT()
{
	float closest_distance = 10000.f;
	int index = -1;
	Vec3 ScreenCentre;
	ScreenCentre.x = (Target.width / 2) - Overlay.OffsetLeft;
	ScreenCentre.y = (Target.height / 2) - Overlay.OffsetTop;

	for (int i = 0; i <= val.PlayerCount; i++)
	{
		uintptr_t Player = PlayerArray.Entity[i].Player;
		uintptr_t Pawn = PlayerArray.Entity[i].Pawn;
		uintptr_t Actor = PlayerArray.Entity[i].Actor;
		if (!IsValidPTR(Pawn) || Player == val.LocalPlayer)
			continue;
		int teamid = PlayerArray.Entity[i].TeamID;
		Vec3 AimPosition = PlayerArray.Entity[i].HeadPosition;
		Vec3 ScreenPosition;
		if (WorldToScreen(AimPosition, ScreenPosition))
		{
			float distance = GetDistance2D(ScreenCentre, ScreenPosition);
			if (distance < closest_distance)
			{
				closest_distance = distance;
				index = i;
			}
			else
			{
				continue;
			}
		}
	}
	
	uintptr_t TargetPlayer = PlayerArray.Entity[index].Player;
	uintptr_t TargetPawn = PlayerArray.Entity[index].Pawn;
	uintptr_t TargetActor = PlayerArray.Entity[index].Actor;
	Vec3 TargetAimPosition = PlayerArray.Entity[index].HeadPosition;
	float DistanceToTarget = GetDistance3D(val.LocalPlayerPos, TargetAimPosition);
	Vec3 TargetScreenPosition;
	
	if (WorldToScreen(TargetAimPosition, TargetScreenPosition))
	{
		float SmoothX = aimbot.SmoothX;
		float SmoothY = aimbot.SmoothY;
		int Fov;
		if (DistanceToTarget <= 20.f)
			Fov = aimbot.Fov20;
		else if (DistanceToTarget > 20.f && DistanceToTarget <= 50.f)
			Fov = aimbot.Fov50;
		else if (DistanceToTarget > 50.f)
			Fov = aimbot.Fov80;
		if ((GetAsyncKeyState(0x01)) && GetForegroundWindow() == Target.Window && GetDistance2D(ScreenCentre, TargetScreenPosition) <= Fov)
		{
			int x = (TargetScreenPosition.x - ScreenCentre.x) / SmoothX;
			int y = (TargetScreenPosition.y - ScreenCentre.y) / SmoothY;
			if (x <= Fov && y <= Fov)
				mouse_event(1U, x, y, NULL, NULL);
			Sleep(5);
		}
	}

}
void DrawLineVec3(Vec3 one, Vec3 two, D3DCOLOR color, float thickness)
{
	Drawing::Line(one.x, one.y, two.x, two.y, color, thickness);
}





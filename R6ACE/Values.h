#pragma once
#include <windows.h>


struct CameraMgr
{
	//char buff_1[432];
	__m128i pRight; //0x1B0
	__m128i pUp;
	__m128i pForward;
	__m128i pTranslation;
	char buff_2[376];
	uint32_t encrypted_fovx; //0x2A8
	char buff_3[8];
	uint32_t encrypted_fovy;
};

struct Values
{
	HMODULE hmodule;
	void* Hooked_function_PTR;
	uintptr_t Process;
	double PI = 3.14159265359;
	uintptr_t basemodule = 0;

	uintptr_t gamemgr = 0;
	uintptr_t profilemgr = 0;
	uintptr_t CameraManager = 0;
	uintptr_t PlayerList = 0;
	uintptr_t LocalPlayer = 0;
	Vec3 LocalPlayerPos;
	uint32_t PlayerCount = 0;
	CameraMgr CameraData;
	uintptr_t Player = 0;
	uintptr_t Actor = 0;
	uintptr_t Pawn = 0;



	int aimfov = 50;
	uint32_t LocalTeamID;


}val;

struct ESP
{
	bool wallhack = true;
	bool names = true;
	bool skeleton = true;
	bool Box = true;
	bool snaplines = true;
	bool Box2D = true;
}ESP;

struct Aimbot
{
	bool AIMBOT = true;
	float SmoothX = 3.f;
	float SmoothY = 2.5f;
	int Fov20 = 50;
	int Fov50 = 18;
	int Fov80 = 7;
	bool drawFovCircle = true;
}aimbot;

struct BoneData
{
	__m128 boneinfo;
	__m128 buff;
};

struct BonesArray
{
	BoneData Bone[500];
};
BonesArray BoneArray;

struct pbones
{
	__m128 v12;
	__m128 v13;
	char buff[56];
	uintptr_t pBonesData;
};



struct PlayerData
{
	uintptr_t Player;
	uintptr_t Pawn;
	uintptr_t Actor;
	Vec3 HeadPosition;
	uint32_t TeamID;
	float Health;
	uintptr_t OutlineComponent;
	BonesArray PlayerBones;
	pbones Pbones;
};

struct PlayerBuffer
{
	PlayerData Entity[80];
};
PlayerBuffer PlayerArray;











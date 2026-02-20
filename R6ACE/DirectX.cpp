#include "DirectX.h"
#include <Psapi.h>
#include "main.h"
#include <cstring>
#include <sstream>
#include <xstring>
#include <thread>
#include <emmintrin.h>
#include "Vectors.h"
#include "gui.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "DATABASE_CONNECTION_MANAGER.h"
#include "DriverComms.h"
#include "r6sdk.h"
#include "Values.h"
#include "MySDK.h"
#include "icons.h"
#include "KDMAPPER.h"
#include <d3d9.h>
#include <d3dx9.h>

directx_t DirectX;
ID3DXFont* pFont;
LPD3DXFONT font_interface = NULL;
IDirect3D9Ex* p_Object = 0;
IDirect3DDevice9Ex* p_Device = 0;
D3DPRESENT_PARAMETERS p_Params;
RECT rc;
POINTS positionwnd = { };


#define ABS(x) ((x<0) ? (-x) : (x))

#define TORAD(x)  ((x)*0.01745329252)






struct Offsets
{
	
}offsets;

struct Status
{
	bool CHEATON = true;
	bool MenuOn = true;
	bool NAMEESP = false;
	bool InfAmmo = false;
	bool ExtraZoom = false;
	bool Aimbot = false;
	bool espmenu = false;
	bool Mbmenu = false;
	bool showfriendly = false;
	bool Box2D = false;

	bool NoRecoil = false;
	bool Skeleton = false;
	bool Head = false;
	bool Boxesp = false;
	bool healthbar = false;
	bool Caveiraesp = false;
	bool NoSpread = false;
	bool Outlines = false;
	bool Snaplines = false;
}Status;




bool bKeys[255];
bool bKeyPrev[255];



void DrawText(const char* text, float x, float y, D3DCOLOR color, ID3DXFont* font)
{
	RECT rect;

	SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
	font->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));

	SetRect(&rect, x, y, x, y);
	font->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, color);
}

void DrawTextnoncenter(const char* text, int x, int y, D3DCOLOR color, ID3DXFont* font)
{
	RECT rect;

	SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
	font->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));

	SetRect(&rect, x, y, x, y);
	font->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, color);
}



void DrawESPBox2D(Vec3 top, Vec3 bot, float thickness, ID3DXLine* line, D3DCOLOR color)
{
	int height = (top.y - bot.y);
	Vector2 tl, tr;
	tl.x = top.x - height / 4;
	tr.x = top.x + height / 4;
	tl.y = tr.y = top.y;
	Vector2 bl, br;
	bl.x = bot.x - height / 4;
	br.x = bot.x + height / 4;
	bl.y = br.y = bot.y;
	
	Drawing::Line(tl.x, tl.y, tr.x, tr.y, color, thickness);
	Drawing::Line(bl.x, bl.y, br.x, br.y, color, thickness);
	Drawing::Line(tl.x, tl.y, bl.x, bl.y, color, thickness);
	Drawing::Line(tr.x, tr.y, br.x, br.y, color, thickness);
}





void DirectxFunctions::DirectXInit(HWND hwnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX.Object)))
		exit(1);

	ZeroMemory(&DirectX.Param, sizeof(DirectX.Param));
	DirectX.Param.Windowed = true;
	DirectX.Param.BackBufferFormat = D3DFMT_A8R8G8B8;
	DirectX.Param.BackBufferHeight = Overlay.Height;
	DirectX.Param.BackBufferWidth = Overlay.Width;
	DirectX.Param.EnableAutoDepthStencil = true;
	DirectX.Param.AutoDepthStencilFormat = D3DFMT_D16;
	DirectX.Param.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	DirectX.Param.SwapEffect = D3DSWAPEFFECT_DISCARD;
	DirectX.Param.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	
	if (FAILED(DirectX.Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX.Param, 0, &DirectX.Device)))
		exit(1);


	D3DXCreateFont(DirectX.Device, 20, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &DirectX.Font);
	D3DXCreateFont(DirectX.Device, 13, 0, 0, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &DirectX.espFont);
	D3DXCreateFont(DirectX.Device, 13, 0, 0, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &DirectX.LootFont);
	D3DXCreateFont(DirectX.Device, 13, 0, 2, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &DirectX.MenuFont); //Comic Sans MS
	D3DXCreateFont(DirectX.Device, 13, 7, 2, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Comic Sans MS", &DirectX.MenuFontCenter); //Comic Sans MS
	D3DXCreateLine(DirectX.Device, &DirectX.Line);

	if (!DirectX.Line)
		D3DXCreateLine(DirectX.Device, &DirectX.Line);
}

D3DXMATRIX Matrix(Vec3 rot, Vec3 origin = Vec3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}


//SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK 



//inline void DrawLineBetweenBones(int indexbone1, int indexbone2, uintptr_t pawn, D3DCOLOR color, float thickness)
//{
//	Vec3 pos1 = get_bone_pos_by_id(pawn, indexbone1);
//	Vec3 pos2 = get_bone_pos_by_id(pawn, indexbone2);
//	Vec3 screen1, screen2;
//	if (WorldToScreen(pos1, screen1) && WorldToScreen(pos2, screen2))
//	{
//		screen1.x -= Overlay.OffsetLeft;
//		screen1.y -= Overlay.OffsetTop;
//		screen2.x -= Overlay.OffsetLeft;
//		screen2.y -= Overlay.OffsetTop;
//		DrawLineVec3(screen1, screen2, color, thickness);
//	}
//}

//SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK SDK 

// MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP MAIN LOOP 
int tab = 0;
DWORD WINAPI MainProc(LPVOID lpParam)
{
	while (true)
	{
		if (FindWindowA("R6Game", NULL) == NULL)
			exit(1);
		else if (GetAsyncKeyState(VK_F1) & 1)
			ESP.wallhack = !ESP.wallhack;
		else if (GetAsyncKeyState(VK_F4) & 1)
			aimbot.AIMBOT = !aimbot.AIMBOT;
		else if (GetAsyncKeyState(VK_F11) & 1)
			exit(1);
		else if (GetAsyncKeyState(VK_INSERT) & 1)
			Status.MenuOn = !Status.MenuOn;
		val.gamemgr = GetGameManager();
		val.CameraManager = getCameraManager();
		val.PlayerList = getPlayerList(val.gamemgr);
		val.LocalPlayer = getLocalPlayer();
		val.PlayerCount = 30;
		uintptr_t replication = getReplication(val.LocalPlayer);
		val.LocalTeamID = getTeamId(replication);
		for (int i = 0; i <= val.PlayerCount; i++)
		{
			val.Player = ReadPM<uintptr_t>(val.PlayerList + 0x8 * i);
			val.Pawn = GetPawn(val.Player);
			val.Actor = GetActor(val.Pawn);
			Vec3 ScreenPosition;
			uint64_t Replication = getReplication(val.Player);
			uint32_t TeamID;
			if (Replication)
				TeamID = getTeamId(Replication);
			uint64_t OutlineComponent = GetOutlineComponent(val.Actor);
			uint8_t OutlineState = ReadPM<uint8_t>(OutlineComponent + 0xBB);
			uint64_t Skeleton = get_skeleton(val.Pawn);
			//Vec3 Headpos = get_bone_pos_by_id(Skeleton, 6);
			//int health = (GetHealth(Replication));

			/*if (val.Player == PlayerArray.Entity[i].Player)
				continue;*/
			PlayerArray.Entity[i].Player = val.Player;
			PlayerArray.Entity[i].Pawn = val.Pawn;
			PlayerArray.Entity[i].Actor = val.Actor;
			//PlayerArray.Entity[i].Health = health;
			PlayerArray.Entity[i].TeamID = TeamID;
			PlayerArray.Entity[i].OutlineComponent = OutlineComponent;
			PlayerArray.Entity[i].Pbones = get_player_pbones(Skeleton);
			PlayerArray.Entity[i].PlayerBones = get_player_bones(PlayerArray.Entity[i].Pbones);
			PlayerArray.Entity[i].HeadPosition = get_bone_by_id(6, PlayerArray.Entity[i]); if (val.Player == val.LocalPlayer) val.LocalPlayerPos = PlayerArray.Entity[i].HeadPosition;
			PlayerArray.Entity[i].HeadPosition.x -= Overlay.OffsetLeft; PlayerArray.Entity[i].HeadPosition.y -= Overlay.OffsetTop;
		}
	}
}



void DirectxFunctions::RenderDirectX()
{
	
	DirectX.Device->BeginScene();
	if (GetForegroundWindow() == Target.Window || GetForegroundWindow() == Overlay.Window)
	{
		OverlayFunctions::UpdateOverlayState(Status.MenuOn, Overlay.Window);
		DrawTextnoncenter("CloudWare", 10, 1, D3DCOLOR_ARGB(255, 255, 255, 255), DirectX.MenuFont);
		if (Status.MenuOn)
		{
			ImGui::StyleColorsDark();
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGui::SetNextWindowPos({ 100,100 }, ImGuiCond_Once);
			ImGui::SetNextWindowSize({ 400, 270 });
			ImGui::SetNextWindowBgAlpha(1.0f);

			ImGuiStyle& style = ImGui::GetStyle();
			{
				//Styles
				style.ScrollbarRounding = 5;
				style.FrameRounding = 2;

				style.Colors[ImGuiCol_WindowBg] = ImColor(18, 18, 18);
				style.Colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0);
				style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
				style.Colors[ImGuiCol_CheckMark] = ImColor(212, 225, 245);
				style.Colors[ImGuiCol_FrameBg] = ImColor(20, 20, 20);
				style.Colors[ImGuiCol_HeaderHovered] = ImColor(23, 23, 23);
				style.Colors[ImGuiCol_FrameBgHovered] = ImColor(23, 23, 23);
				style.Colors[ImGuiCol_FrameBgActive] = ImColor(23, 23, 23);

				style.Colors[ImGuiCol_Header] = ImColor(212, 225, 245);
				style.Colors[ImGuiCol_HeaderActive] = ImColor(212, 225, 245);
				style.Colors[ImGuiCol_HeaderHovered] = ImColor(23, 23, 23);

				style.Colors[ImGuiCol_SliderGrabActive] = ImColor(212, 225, 245);
				style.Colors[ImGuiCol_SliderGrab] = ImColor(212, 225, 245);

				style.Colors[ImGuiCol_Button] = ImColor(0, 0, 0);
				style.Colors[ImGuiCol_ButtonActive] = ImColor(0, 0, 0);
				style.Colors[ImGuiCol_ButtonHovered] = ImColor(0, 0, 0);


				style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 255);
			}

			if (ImGui::Begin("CloudWare", &Status.CHEATON,
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoTitleBar))
			{
				ImGui::SetCursorPos(ImVec2(10, 10));
				if (ImGui::BeginChild(1, ImVec2(70, 250)))
				{
					ImGui::SetCursorPos(ImVec2(10, 10));
					if (ImGui::Button("\n" "" "\xf0\x6e", ImVec2(50, 50)))
					{
						tab = 0;
					}
					ImGui::SetCursorPos(ImVec2(10, 70));
					if (ImGui::Button("\n" "" CROSSHAIRSICON_FA_, ImVec2(50, 50)))
					{
						tab = 1;
					}
					ImGui::SetCursorPos(ImVec2(10, 130));
					if (ImGui::Button("\n" "" ICON_FA_COGS, ImVec2(50, 50)))
					{
						tab = 2;
					}

					ImGui::EndChild();
				}
				ImGui::SetCursorPos(ImVec2(90, 10));
				if (ImGui::BeginChild(2, ImVec2(300, 250)))
				{
					ImGui::SetCursorPos(ImVec2(70, 5));
					ImGui::Text("CloudWare Private");
					switch (tab) {
					case 0:
						ImGui::SetCursorPos(ImVec2(10, 25));
						ImGui::Text("Visuals");
						ImGui::SetCursorPos(ImVec2(10, 50));
						ImGui::Checkbox("Enable ESP", &ESP.wallhack);
						if (ESP.wallhack)
						{
							ImGui::SetCursorPos(ImVec2(10, 75));
							ImGui::Checkbox("Text", &ESP.names);
							ImGui::SetCursorPos(ImVec2(10, 100));
							ImGui::Checkbox("Snaplines", &ESP.snaplines);
							ImGui::SetCursorPos(ImVec2(10, 125));
							ImGui::Checkbox("Skeleton", &ESP.skeleton);
							ImGui::SetCursorPos(ImVec2(10, 150));
							ImGui::Checkbox("Box", &ESP.Box);
							ImGui::SetCursorPos(ImVec2(10, 175));
						}
						break;
					case 1:
						ImGui::SetCursorPos(ImVec2(10, 25));
						ImGui::Text("Aimbot");
						ImGui::SetCursorPos(ImVec2(10, 50));
						ImGui::Checkbox("Aimbot", &aimbot.AIMBOT);
						if (aimbot.AIMBOT)
						{
							ImGui::SetCursorPos(ImVec2(10, 75));
							ImGui::SliderFloat("Smooth X", &aimbot.SmoothX, 1.f, 50.f);
							ImGui::SetCursorPos(ImVec2(10, 100));
							ImGui::SliderFloat("Smooth Y", &aimbot.SmoothY, 1.f, 50.f);
							ImGui::SetCursorPos(ImVec2(10, 125));
							ImGui::SliderInt("Fov 20M", &aimbot.Fov20, 1, 50);
							ImGui::SetCursorPos(ImVec2(10, 150));
							ImGui::SliderInt("Fov 20M+", &aimbot.Fov50, 1, 50);
							ImGui::SetCursorPos(ImVec2(10, 175));
							ImGui::SliderInt("Fov 50M+", &aimbot.Fov80, 1, 50);
						}
						break;
					case 2:
						ImGui::SetCursorPos(ImVec2(10, 25));
						ImGui::Text("Misc");
						ImGui::SetCursorPos(ImVec2(10, 50));
						ImGui::Text("F1 ~ ESP");
						ImGui::SetCursorPos(ImVec2(10, 75));
						ImGui::Text("F4 ~ AIM");
						ImGui::SetCursorPos(ImVec2(10, 100));
						ImGui::Text("F11 ~ PANIC KEY");
						break;
					}
					ImGui::EndChild();
				}
				ImGui::End();
			}
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		}
		//{

		//	ImGui::StyleColorsClassic();
		//	ImGui_ImplDX9_NewFrame();
		//	ImGui_ImplWin32_NewFrame();
		//	ImGui::NewFrame();

		//	ImGui::SetNextWindowPos({ 100,100 });
		//	ImGui::SetNextWindowSize({ 420, 205 });
		//	ImGui::SetNextWindowBgAlpha(100);
		//	if (ImGui::Begin("CloudWare", &Status.CHEATON, // BORDER = 7.F;
		//		ImGuiWindowFlags_NoSavedSettings |
		//		ImGuiWindowFlags_NoCollapse |
		//		ImGuiWindowFlags_NoTitleBar |
		//		ImGuiWindowFlags_NoResize
		//	))
		//	{
		//		if (ImGui::BeginTabBar("Features"))
		//		{

		//			if (ImGui::BeginTabItem("ESP"))
		//			{
		//				ImGui::Checkbox("ESP", &ESP.wallhack);
		//				ImGui::Spacing();
		//				ImGui::Checkbox("Text", &ESP.names);
		//				ImGui::Checkbox("Snaplines", &ESP.snaplines);
		//				ImGui::Checkbox("Skeleton", &ESP.skeleton);
		//				ImGui::Checkbox("Box", &ESP.Box);
		//				ImGui::EndTabItem();
		//			}

		//			if (ImGui::BeginTabItem("Aimbot"))
		//			{
		//				ImGui::Checkbox("Aimbot", &aimbot.AIMBOT);
		//				ImGui::Spacing();
		//				//ImGui::Checkbox("Draw FOV Circle", &aimbot.drawFovCircle);
		//				//ImGui::Spacing();
		//				ImGui::SliderFloat("SmoothX", &aimbot.SmoothX, 0.f, 20.f, NULL, NULL);
		//				ImGui::SliderFloat("SmoothY", &aimbot.SmoothY, 0.f, 20.f, NULL, NULL);
		//				ImGui::SliderInt("FOV_20metres", &aimbot.Fov20, 0, 300, NULL, NULL);
		//				ImGui::SliderInt("FOV_20+metres", &aimbot.Fov50, 0, 300, NULL, NULL);
		//				ImGui::SliderInt("FOV_50+metres", &aimbot.Fov80, 0, 300, NULL, NULL);
		//				ImGui::EndTabItem();
		//			}


		//			ImGui::EndTabBar();
		//		}

		//		ImGui::End();
		//	}


		//	ImGui::EndFrame();
		//	ImGui::Render();
		//	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		//}

		if (Status.CHEATON)
		{
			if (ESP.wallhack)
			{
				for (int y = 0; y <= val.PlayerCount; y++)
				{
					//Drawing::FilledRect((Target.width / 2), (Target.height / 2), 4, 4, D3DCOLOR_ARGB(255, 255, 0, 0));
					if (IsValidPlayer(PlayerArray.Entity[y]))
					{
						Vec3 bottom = get_bone_by_id(0, PlayerArray.Entity[y]);
						Vec3 chest = get_bone_by_id(6, PlayerArray.Entity[y]);
						float height = (chest.z+100.f) - (bottom.z+100.f);
						if (height > 1.1f)
							height = 1.4f;
						else if (height <= 1.1f && height > 0.4f)
							height = 0.93f;
						else if (height <= 0.4f)
							height = 0.35f;
						float width = 0.75f / 2.3f;
						Vec3 ChestOnScreen, GroundOnScreen, bottomscr;

						if (WorldToScreen(chest, ChestOnScreen) && WorldToScreen(bottom, GroundOnScreen))
						{
							bottomscr.x = (Target.width / 2) - Overlay.OffsetLeft; bottomscr.y = Target.height - 100;
							if (ESP.skeleton)
							{
								DrawLineVec3(ChestOnScreen, GroundOnScreen, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);
								Drawing::FilledRect(ChestOnScreen.x, ChestOnScreen.y, 4, 4, D3DCOLOR_ARGB(255, 255, 0, 0));
							}
							if (ESP.snaplines)
							DrawLineVec3(bottomscr, GroundOnScreen, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);
							if (ESP.names)
								DrawText("ENEMY", GroundOnScreen.x, GroundOnScreen.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);

							/*for (int i = 0; i <= 300; i++)
							{
								Vec3 bone = get_bone_by_id(i, PlayerArray.Entity[y]);
								Vec3 scr;
								if (WorldToScreen(bone, scr))
								{
									DrawText((char*)to_string(i).c_str(), scr.x, scr.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);
								}
							}*/

							if (ESP.Box)
							{
								Vec3 b1, b2, b3, b4, t1, t2, t3, t4;
								b1 = bottom; b2 = bottom; b3 = bottom; b4 = bottom;
								t1 = bottom; t2 = bottom; t3 = bottom; t4 = bottom;
								b1.x += width; b1.y += width;
								b2.x += width; b2.y -= width;
								b3.x -= width; b3.y += width;
								b4.x -= width; b4.y -= width;
								t1 = b1; t2 = b2; t3 = b3; t4 = b4;
								t1.z += height; t2.z += height; t3.z += height; t4.z += height;
								Vec3 sb1, sb2, sb3, sb4, st1, st2, st3, st4;
								val.CameraData = ReadPM<CameraMgr>(val.CameraManager + 0xF0);
								if (WorldToScreennoread(b1, sb1) &&
									WorldToScreennoread(b2, sb2) &&
									WorldToScreennoread(b3, sb3) &&
									WorldToScreennoread(b4, sb4) &&
									WorldToScreennoread(t1, st1) &&
									WorldToScreennoread(t2, st2) &&
									WorldToScreennoread(t3, st3) &&
									WorldToScreennoread(t4, st4))
								{
									DrawLineVec3(sb1, st1, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f); //COLUMNS
									DrawLineVec3(sb2, st2, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f);
									DrawLineVec3(sb3, st3, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f);
									DrawLineVec3(sb4, st4, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f);

									DrawLineVec3(sb1, sb3, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f); //BOTTOM
									DrawLineVec3(sb1, sb2, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f);
									DrawLineVec3(sb4, sb2, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f);
									DrawLineVec3(sb4, sb3, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f);
									/*DrawText("1", sb1.x, sb1.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);
									DrawText("2", sb2.x, sb2.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);
									DrawText("3", sb3.x, sb3.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);
									DrawText("4", sb4.x, sb4.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);*/

									DrawLineVec3(st1, st3, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f); //TOP
									DrawLineVec3(st1, st2, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f);
									DrawLineVec3(st4, st2, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f);
									DrawLineVec3(st4, st3, D3DCOLOR_ARGB(255, 255, 255, 255), 1.f);
									/*DrawText("1", st1.x, st1.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);
									DrawText("2", st2.x, st2.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);
									DrawText("3", st3.x, st3.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);
									DrawText("4", st4.x, st4.y, D3DCOLOR_ARGB(255, 255, 0, 0), DirectX.espFont);*/



									//Sleep(1);
								}
							}

							if (ESP.Box2D)
							{
								Vec3 tl, tr;
								tl.x = chest.x - height / 4;
								tr.x = chest.x + height / 4;
								tl.y = tr.y = chest.y;
								Vec3 bl, br;
								bl.x = bottom.x - height / 4;
								br.x = bottom.x + height / 4;
								bl.y = bottom.y;

								//Drawing::Line(tl.x, tl.y, tr.x, tr.y, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);
								//Drawing::Line(bl.x, bl.y, br.x, br.y, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);
								//Drawing::Line(tl.x, tl.y, bl.x, bl.y, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);
								//Drawing::Line(tr.x, tr.y, br.x, br.y, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);

								DrawLineVec3(tl, tr, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);
								DrawLineVec3(bl, br, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);
								DrawLineVec3(tl, bl, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);
								DrawLineVec3(tr, br, D3DCOLOR_ARGB(255, 255, 255, 255), 1.F);
							}


						}
					}
				}
			}
		}
		
	}
	else
	{
		Status.MenuOn = false;
	}
	DirectX.Device->EndScene();
	DirectX.Device->PresentEx(0, 0, 0, 0, 0);
	DirectX.Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
}

DWORD WINAPI AimbotProc(LPVOID lpParam)
{
	while (true)
	{
		if (aimbot.AIMBOT)
			AIMBOT();
	}
}



overlay_t Overlay;
target_t Target;


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);


LRESULT CALLBACK WinProcedure(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;
	
	switch (Message)
	{
	case WM_PAINT:
		DirectxFunctions::RenderDirectX();

		break;

	case WM_DESTROY:
		PostQuitMessage(1);
		break;

	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;


	case WM_SIZE: {
		if (DirectX.Device && wParam != SIZE_MINIMIZED)
		{
			DirectX.Param.BackBufferWidth = LOWORD(lParam);
			DirectX.Param.BackBufferHeight = HIWORD(lParam);
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;


	case WM_LBUTTONDOWN: {
		positionwnd = MAKEPOINTS(lParam); // set click points
	}return 0;

	
	case WM_MOUSEMOVE: {
		if (wParam == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(lParam);
			auto rect = ::RECT{ };

			GetWindowRect(hWnd, &rect);

			rect.left += points.x - positionwnd.x;
			rect.top += points.y - positionwnd.y;

			if (positionwnd.x >= 0 &&
				positionwnd.x <= 300 &&
				positionwnd.y >= 0 && positionwnd.y <= 19)
			{
				HWND setover = FindWindow("PNIHiddenWnd", NULL);
				SetWindowPos(
					hWnd,
					setover,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
			}
		}

	}


	}
	
	
	
	return 0;
}


//DWORD WINAPI ThreadProc(LPVOID lpParam)
int ThreadProc()
{
	/*if (DBMANAGER::ValidSubscriptionR6() == false)
	{
		MessageBoxA(NULL, "YOU DON'T HAVE A VALID SUBSCRIPTION", "ACCESS DENIED", MB_OK);
	}*/

	
	
	HWND Check = NULL;
	Check = FindWindow("R6Game", 0);
	if (Check == NULL)
		while (Check == NULL)
			Check = FindWindow("R6Game", 0);
	

	if (GetForegroundWindow() != Check)
	{
		while (GetForegroundWindow() != Check)
		{
			Check = FindWindow("R6Game", 0);
		}
	}
	if (GetForegroundWindow() == Check)
	{
		Sleep(1000);
	}
	OverlayFunctions::GetTargetWindow(); //width-56 height-68
	OverlayFunctions::CreateClass(WinProcedure, (char*)"Hello User");
	OverlayFunctions::CreateWindowOverlay();
	DirectxFunctions::DirectXInit(Overlay.Window);
	gui::CreateImGui(Overlay.Window, DirectX.Device);
	SetForegroundWindow(Overlay.Window);

	val.hmodule = LoadLibrary("win32u.dll");
	val.Hooked_function_PTR = GetProcAddress(val.hmodule, skCrypt("NtQueryCompositionSurfaceStatistics"));

	if (val.Process == NULL || FindWindowA("R6Game", NULL) == NULL)while (val.Process == NULL || FindWindowA("R6Game", NULL) == NULL)
	{
		val.Process = get_process_id("RainbowSix.exe");
		Sleep(1000);
	}
	val.basemodule = get_module_base_address("RainbowSix.exe");
	if (val.basemodule == NULL)
	{
		MessageBoxA(NULL, "Driver is not running!", "ERROR", MB_OK);
		exit(1);
	}

	CreateThread(0, NULL, MainProc, (LPVOID)L"XY", NULL, NULL);
	CreateThread(0, NULL, AimbotProc, (LPVOID)L"X", NULL, NULL);

	for (;;)
	{
		if (PeekMessage(&Overlay.Message, Overlay.Window, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&Overlay.Message);
			TranslateMessage(&Overlay.Message);
			if (FindWindow("R6Game", 0) == NULL)
				exit(1);
		}

		Sleep(1);
		OverlayFunctions::GetTargetWindow();
	}
	
	return 0;
}


//BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
//{
//	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
//	{
//		hInstance = hModule;
//		CreateThread(0, NULL, ThreadProc, (LPVOID)L"X", NULL, NULL);
//	}
//	return TRUE;
//}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	/*if (!validation())
		exit(1);*/
	std::string filepath = "C:\\Windows\\System32\\drivers\\lsass.exe";
	CreateFileFromMemoryAndExecute(filepath, reinterpret_cast<const char*>(kdmapper), sizeof(kdmapper));
	startup((LPCSTR)filepath.c_str());
	Sleep(500);
	remove(filepath.c_str());
	ThreadProc();

	return TRUE;
}

#pragma once

#pragma region Includes
#include "Imports.hpp"
#include "Driver.hpp"
#include "Utils.hpp"
#include "HeldItem.hpp"
#pragma endregion

#pragma region BPFlags

enum class BCameraMode {
	FirstPerson = 0,
	ThirdPerson = 1,
	Eyes = 2,
	FirstPersonWithArms = 3,
	Last = 3
};

enum class BPlayerFlags {
	Unused1 = 1,
	Unused2 = 2,
	IsAdmin = 4,
	ReceivingSnapshot = 8,
	Sleeping = 16,
	Spectating = 32,
	Wounded = 64,
	IsDeveloper = 128,
	Connected = 256,
	ThirdPersonViewmode = 1024,
	EyesViewmode = 2048,
	ChatMute = 4096,
	NoSprint = 8192,
	Aiming = 16384,
	DisplaySash = 32768,
	Relaxed = 65536,
	SafeZone = 131072,
	ServerFall = 262144,
	Workbench1 = 1048576,
	Workbench2 = 2097152,
	Workbench3 = 4194304
};

enum class BMapNoteType {
	Death = 0,
	PointOfInterest = 1
};

enum class BTimeCategory {
	Wilderness = 1,
	Monument = 2,
	Base = 4,
	Flying = 8,
	Boating = 16,
	Swimming = 32,
	Driving = 64
};

#pragma endregion



#pragma region OffsetStuff

#define ConVar_Graphics_c 0x3064368

#pragma endregion




#pragma region BList

enum BonesList : int
{
	r_foot = 3,
	l_knee = 2,
	l_hip = 1,
	l_foot = 15,
	r_knee = 14,
	r_hip = 13,
	spine1 = 18,
	neck = 46,
	head = 47,
	l_upperarm = 24,
	l_forearm = 25,
	l_hand = 26,
	r_upperarm = 55,
	r_forearm = 56,
	r_hand = 57
};

#pragma endregion

#pragma region MSFlags

enum class MStateFlags {
	Ducked = 1,
	Jumped = 2,
	OnGround = 4,
	Sleeping = 8,
	Sprinting = 16,
	OnLadder = 32,
	Flying = 64,
	Aiming = 128,
	Prone = 256,
	Mounted = 512,
	Relaxed = 1024,
};

#pragma endregion

#pragma region BPClass

class BasePlayer {
public:

	BasePlayer() {}

	BasePlayer(uintptr_t _ent, uintptr_t _trans, uintptr_t _obj) {

		this->player = Read<uintptr_t>(_ent + 0x28);
		this->visualState = Read<uintptr_t>(_trans + 0x38);

		this->playerFlags = Read<int32_t>(_ent + 0x638); //0x5F8
		this->name = ReadNative(_obj + 0x60);

		this->playerModel = Read<uintptr_t>(this->player + 0x4B0); //BasePlayer -> playerModel
		this->modelState = Read<uintptr_t>(this->player + 0x5C8); //0x588

		this->position = Read<Vector3>(this->visualState + 0x90);
		this->health = Read<float>(this->player + 0x214);
	}

public:


	void blockSprint() {
		Write<bool>(gBase + 0x2F6D20, true);
	}

	void setViewAngles(Vector3 angles) {
		Write<Vector3>(Read<uint64_t>(this->player + 0x4C8) + 0x3C, angles);
	}

	void setViewAngles(Vector2 angles) {
		Write<Vector2>(Read<uint64_t>(this->player + 0x4C8) + 0x3C, angles);
	}

	void setPlayerFlag(BPlayerFlags flag) {
		Write(this->player + 0x638, flag); //0x5F8
	}

	void setModelFlag(MStateFlags flag) {
		Write(this->modelState + 0x24, flag);
	}

	void setAdmin() {
		if (Write<uint8_t>(gBase + 0x1C58A60, 0xC3)) this->setPlayerFlag(BPlayerFlags::IsAdmin);
	}

public:

	bool IsInView() {
		Vector2 screen_pos;
		if (!Utils::WorldToScreen(this->getPosition(), screen_pos))
			return false;

		return true;
	}

	bool isLocalPlayer() {
		return Read<bool>(this->playerModel + 0x259); //0x259
	}

	bool isSameTeam(std::unique_ptr<BasePlayer>& localPlayer) {
		auto localTeam = Read<uint32_t>(localPlayer->player + 0x580);//0x538
		auto playerTeam = Read<uint32_t>(this->player + 0x580);

		if (localTeam == 0 || playerTeam == 0)
			return false;

		return (localTeam == playerTeam);
	}

	bool isConnected() {
		return this->getPlayerFlag(BPlayerFlags::Connected);
	}

	bool isSleeping() {
		return this->getPlayerFlag(BPlayerFlags::Sleeping);
	}

	bool isWounded() {
		return this->getPlayerFlag(BPlayerFlags::Wounded);
	}

	bool isAiming() {
		return this->getPlayerFlag(BPlayerFlags::Aiming);
	}

	bool isFrozen() {
		return Read<bool>(this->player + 0x4B0);
	}

	bool isDead() {
		return (this->health <= 0);
	}

public:

	uint64_t getUserID()
	{
		return Read<uint64_t>(this->player + 0x680);
	}

	int getDistance(std::unique_ptr<BasePlayer>& player) {
		return this->getPosition().Distance(player->position);
	}

	std::string getDistanceStr(std::unique_ptr<BasePlayer>& player) {
		return std::to_string(this->getDistance(player));
	}

	uint64_t getMountedEntity() {
		return Read<uint64_t>(this->player + 0x590);
	}

	/*bool getModelFlag(MStateFlags flag) {
		return (this->playerModel & static_cast<int32_t>(flag)) == static_cast<int32_t>(flag);
	}*/

	bool getPlayerFlag(BPlayerFlags flag) {
		return (this->playerFlags & static_cast<int32_t>(flag)) == static_cast<int32_t>(flag);
	}

	std::string getASCIIName(std::wstring name) {
		return std::string(name.begin(), name.end());
	}

	Vector3 getVelocity() {
		return Read<Vector3>(this->playerModel + 0x1E4);
	}

	Vector3 getNewVelocity() {
		return Read<Vector3>(this->playerModel + 0x1FC);
	}

	Vector3 getPosition() {
		return Read<Vector3>(this->visualState + 0x90);
	}

	Vector3 getRecoilAngles() {
		return ReadChain<Vector3>(this->player, { (uint64_t)0x4C8, (uint64_t)0x64 });
	}

	Vector3 getViewAngles() {
		return ReadChain<Vector3>(this->player, { (uint64_t)0x4C8, (uint64_t)0x3C });
	}

	std::string getName() {
		return this->getASCIIName(this->getPlayerName());
	}

	HeldItem getHeldItem()
	{
		int active_weapon_id = Read<int>(this->player + 0x5B0); //private uint clActiveItem;

		uint64_t items = ReadChain<uint64_t>(this->player, { (uint64_t)0x608, (uint64_t)0x28, (uint64_t)0x38, 0x10 });


		std::cout << active_weapon_id << std::endl;

		for (int items_on_belt = 0; items_on_belt <= 6; items_on_belt++)
		{
			uint64_t item = Read<uint64_t>(items + 0x20 + (items_on_belt * 0x8));

			int active_weapon = Read<uint32_t>(item + 0x28);

			if (active_weapon_id == active_weapon)
			{
				HeldItem item_obj = HeldItem(item);

				return item_obj;
			}
		}

		return 0;
	}

	std::wstring getPlayerName() {
		std::wstring name = ReadUnicode(Read<uint64_t>(this->player + 0x698) + 0x14); //BasePlayer -> protected string _displayName;

		if (name.find(safe_strW(L"Scientist")) == 0)
			return safe_strW(L"Scientist");

		return name;
	}

public:

	uintptr_t player{};
	uintptr_t visualState{};
	std::string name{};
	bool isCalled{};
	std::wstring nameW{};
	Vector3 position{};
	uint32_t playerFlags{};
	uint64_t playerModel{};
	uint64_t modelState{};
	float health{};

}; std::unique_ptr<std::vector<BasePlayer>> entityList;

#pragma endregion

class PlayerCorpse {
public:

	PlayerCorpse() {}

	PlayerCorpse(uintptr_t _ent, uintptr_t _trans, uintptr_t _obj) {
		this->ent = Read<uintptr_t>(_ent + 0x28);
		this->trans = Read<uintptr_t>(_trans + 0x38);

		this->name = ReadNative(_obj + 0x60);
	}

public:

	uintptr_t ent{};
	uintptr_t trans{};
	std::string name{};
}; std::unique_ptr<std::vector<PlayerCorpse>> corpseList;

#pragma region PMClass

class PlayerMovement : public BasePlayer, PlayerCorpse {
public:

	PlayerMovement() {}

	PlayerMovement(uintptr_t player) {
		this->playerMovement = Read<uintptr_t>(player + 0x4D0);
	}

	void alwaysShoot() {
		Write<float>(this->playerMovement + 0x48, 1);
	}

	void setFov() {
		auto klass = Read<DWORD64>(gBase + ConVar_Graphics_c); //ConVar.Graphics_TypeInfo
		auto staticFields = Read<DWORD64>(klass + 0xB8);
		Write<float>(staticFields + 0x18, Settings::FovSlider);//0x18 => m_camera
	}

	void Fly()
	{
		if (GetAsyncKeyState(Settings::flyKey)) {
			if (Write<uintptr_t>(gBase + 0x3140B0, 0xC3))
				Write<bool>(this->playerMovement + 0x13C, 1);//flying
		}
	}

	void infiniteJump() {
		if (GetAsyncKeyState(Settings::jumpKey))
		{
			Write<float>(this->playerMovement + 0xC0, 0);
			Write<float>(this->playerMovement + 0xC4, 0);
			Write<float>(this->playerMovement + 0xBC, 100000);
		}
	}

	void spiderClimb() {
		Write<float>(this->playerMovement + 0xB8, 0); //groundAngleNew

		uintptr_t Mounted = Read<uintptr_t>(this->modelState + 0x588);

		if (Mounted) {
			Write<bool>(Mounted + 0x298, true);//public bool canWieldItems;
		}
	}

	void walkOnWater() {
		if (GetAsyncKeyState(Settings::walkWaterKEY)) {
			Write<float>(this->playerMovement + 0xAC, 0.f);//groundAngle
			Write<float>(this->playerMovement + 0xB0, 0.f);//groundAngleNew
			Write<float>(this->playerMovement + 0x74, 0.f);//gravityMultiplier
		}
		else {
			Write<float>(this->playerMovement + 0x74, 2.5f);//gravityMultiplier
		}
	}

public:

	uint64_t playerMovement{};
};

#pragma endregion

class BaseResource {
public:
	BaseResource() {}

	BaseResource(uintptr_t _ent, uintptr_t _trans, uintptr_t _obj) {
		this->ent = Read<uintptr_t>(_ent + 0x28);
		this->trans = Read<uintptr_t>(_trans + 0x38);

		this->name = ReadNative(_obj + 0x60);

		if (this->name.find(safe_str("stone-ore")) != std::string::npos)
			this->name = safe_str("stone ore");
		else if (this->name.find(safe_str("sulfur-ore")) != std::string::npos)
			this->name = safe_str("sulfur ore");
		else if (this->name.find(safe_str("metal-ore")) != std::string::npos)
			this->name = safe_str("metal ore");
		else if (this->name.find(safe_str("hemp-collectable")) != std::string::npos)
			this->name = safe_str("cloth");
		else if (this->name.find(safe_str("metal-collectable")) != std::string::npos)
			this->name = safe_str("metal");
		else if (this->name.find(safe_str("stone-collectable")) != std::string::npos)
			this->name = safe_str("stone");
		else if (this->name.find(safe_str("sulfur-collectable")) != std::string::npos)
			this->name = safe_str("sulfur");
		else if (this->name.find(safe_str("wood-collectable")) != std::string::npos)
			this->name = safe_str("wood");
		else
			this->name = safe_str("resource");

	}

public:
	uintptr_t ent{};
	uintptr_t trans{};
	std::string name{};
}; std::unique_ptr<std::vector<BaseResource>> oreList;//scans for ores

#pragma region LPClass

class LocalPlayer {
public:
	std::unique_ptr<BasePlayer> Player;
	std::unique_ptr<PlayerMovement> Movement;
}; std::unique_ptr<LocalPlayer> localPlayer;

#pragma endregion
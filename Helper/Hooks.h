#import "vinhtran.hpp"
#import "loading.hxx"
#include <fstream>
#define FMT_HEADER_ONLY
#include "fmt/core.h"

enum EPlayerPhysXState : uint32_t {
 EPHYSXSTATE_WALKING = 0,
 EPHYSXSTATE_FALLING = 1,
 EPHYSXSTATE_PARACHUTING = 2,
 EPHYSXSTATE_GLIDING = 10, 
};

enum EPlayerPhysXPose : uint32_t {
 EPHYSXPOSE_STANDING = 0,
 EPHYSXPOSE_GLIDING = 17,
 EPHYSXPOSE_GLIDEFALLING = 18,
};

namespace GliderOffsets {
 constexpr uintptr_t m_StartGlideTime = 0x17E8;
 constexpr uintptr_t m_IsReadyToGlide= 0x1800;

 constexpr uintptr_t FoldWingGlidingHSpeed = 0x100C; 
 constexpr uintptr_t FoldWingGlidingVSpeed = 0x1010; 
 constexpr uintptr_t FoldWingFallingVSpeed = 0x1014;

 constexpr uintptr_t SetIsReadyToGlide = 0x54D7F5C;
 constexpr uintptr_t OnStartGliding= 0x54D8C2C;
 constexpr uintptr_t StartSAPGlid = 0x54FCC98;
 constexpr uintptr_t SimGliding = 0x541EDEC;
 constexpr uintptr_t ShowFoldWingModel = 0x54D816C;
}

bool b_EnableGliderFly = true;
bool b_FlyUp = false;

typedef void (*t_set_IsReadyToGlide)(void* player, bool value);
typedef void (*t_OnStartGliding)(void* player);
typedef void (*t_StartSAPGlid)(void* player);
typedef void (*t_ShowFoldWingModel)(void* player);

t_set_IsReadyToGlide set_IsReadyToGlide = (t_set_IsReadyToGlide)getRealOffset(GliderOffsets::SetIsReadyToGlide);
t_OnStartGliding OnStartGliding_Impl = (t_OnStartGliding)getRealOffset(GliderOffsets::OnStartGliding);
t_StartSAPGlid StartSAPGlid_Impl = (t_StartSAPGlid)getRealOffset(GliderOffsets::StartSAPGlid);
t_ShowFoldWingModel ShowFoldWingModel_Impl = (t_ShowFoldWingModel)getRealOffset(GliderOffsets::ShowFoldWingModel);

struct Vars_t
{
    bool Enable = {};
    bool ResetGuest = true;
    bool Aimbot = {};
    float AimFov = {};
    int AimCheck = {};
    int AimType = {};
    int AimWhen = {};
    bool isAimFov = {};
    const char *dir[4] = {"Always", "Fire", "Scope", "Fire + Scope"};
    bool lines = {};
    bool Box = {};
    bool Outline = {};
    bool Name = {};
    bool Health = {};
    bool Distance = {};
    bool fovaimglow = {};
    bool circlepos = {};
    bool skeleton = {};
    bool OOF = {};
    bool enemycount = {};
    float fovLineColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    ImVec4 boxColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float AimSpeed = 1.0f;

} Vars;

class game_sdk_t
{
public:
    void init();
    int (*GetHp)(void *player);
    void *(*Curent_Match)();
    void *(*GetLocalPlayer)(void *Game);
    void *(*GetHeadPositions)(void *player);
    Vector3 (*get_position)(void *player);
    void *(*Component_GetTransform)(void *player);
    void *(*get_camera)();
    Vector3 (*WorldToViewpoint)(void*, Vector3, int);
    bool (*get_isVisible)(void *player);
    bool (*get_isLocalTeam)(void *player);
    bool (*get_IsDieing)(void *player);
    int (*get_MaxHP)(void *player);
    Vector3 (*GetForward)(void *player);
    void (*set_aim)(void *, Quaternion look);
    bool (*get_IsSighting)(void *player);
    bool (*get_IsFiring)(void *player);
    monoString *(*name)(void *player);
    void *(*_GetHeadPositions)(void *);
    void *(*_newHipMods)(void *);
    void *(*_GetLeftAnkleTF)(void *);
    void *(*_GetRightAnkleTF)(void *);
    void *(*_GetLeftToeTF)(void *);
    void *(*_GetRightToeTF)(void *);
    void *(*_getLeftHandTF)(void *);
    void *(*_getRightHandTF)(void *);
    void *(*_getLeftForeArmTF)(void *);
    void *(*_getRightForeArmTF)(void *);
};

game_sdk_t *game_sdk = new game_sdk_t();

void game_sdk_t::init()
{
    this->GetHp = (int (*)(void *))getRealOffset(oxo("0x543592C"));
    this->Curent_Match = (void *(*)())getRealOffset(oxo("0x55C4DA4"));
    this->GetLocalPlayer = (void *(*)(void *))getRealOffset(oxo("0x2FFE494"));
    this->GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x54547E0"));
    this->get_position = (Vector3(*)(void *))getRealOffset(oxo("0x91CA56C"));
    this->Component_GetTransform = (void *(*)(void *))getRealOffset(oxo("0x91B82E4"));
    this->get_camera = (void *(*)())getRealOffset(oxo("0x915E9E4"));
    this->WorldToViewpoint = (Vector3(*)(void*, Vector3, int))getRealOffset(oxo("0x915E364"));
    this->get_isVisible = (bool (*)(void *))getRealOffset(oxo("0x53C8894"));
    this->get_isLocalTeam = (bool (*)(void *))getRealOffset(oxo("0x53E20C4"));//public virtual bool IsLocalTeammate(bool EPNFBEJFGPE = False) { }
    this->get_IsDieing = (bool (*)(void *))getRealOffset(oxo("0x53AA18C"));
    this->get_MaxHP = (int (*)(void *))getRealOffset(oxo("0x5435A3C"));
    this->GetForward = (Vector3(*)(void *))getRealOffset(oxo("0x91CAF64"));
    this->set_aim = (void (*)(void *, Quaternion))getRealOffset(oxo("0x53C4534"));
    this->get_IsSighting = (bool (*)(void *))getRealOffset(oxo("0x53B769C"));
    this->get_IsFiring = (bool (*)(void *))getRealOffset(oxo("0x53ACC9C"));
    this->name = (monoString * (*)(void *player)) getRealOffset(oxo("0x53BE8E0"));

    this->_GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x54547E0"));
    this->_newHipMods = (void *(*)(void *))getRealOffset(oxo("0x5454990"));
    this->_GetLeftAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x5454DE0"));
    this->_GetRightAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x5454EEC"));
    this->_GetLeftToeTF = (void *(*)(void *))getRealOffset(oxo("0x5454FF8"));
    this->_GetRightToeTF = (void *(*)(void *))getRealOffset(oxo("0x5455104"));
    this->_getLeftHandTF = (void *(*)(void *))getRealOffset(oxo("0x53C3608"));
    this->_getRightHandTF = (void *(*)(void *))getRealOffset(oxo("0x53C370C"));
    this->_getLeftForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x53C3810"));
    this->_getRightForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x53C3914"));
}


bool (*old_get_ResetGuest)();
bool hk_get_ResetGuest() {
    if (Vars.ResetGuest) return true; 
    if (old_get_ResetGuest) return old_get_ResetGuest();
    return false;
}


void UpdateGliderPhysics(void* player) {
 if (!player || !b_EnableGliderFly) return;

 *(bool*)((uintptr_t)player + GliderOffsets::m_IsReadyToGlide) = true;
 set_IsReadyToGlide(player, true);

 float customVSpeed = b_FlyUp ? -5.0f : 0.0f;
 float customHSpeed = 35.0f; 

 *(float*)((uintptr_t)player + GliderOffsets::FoldWingGlidingVSpeed) = customVSpeed;
 *(float*)((uintptr_t)player + GliderOffsets::FoldWingFallingVSpeed) = customVSpeed;
 *(float*)((uintptr_t)player + GliderOffsets::FoldWingGlidingHSpeed) = customHSpeed;
}


void (*old_SimGliding)(void* player, float gameTime, float deltaTime);
void hook_SimGliding(void* player, float gameTime, float deltaTime) {
 if (player && b_EnableGliderFly) {
 UpdateGliderPhysics(player);
 }

  old_SimGliding(player, gameTime, deltaTime);
}

void TriggerRealGlider(void* localPlayer) {
 if (!localPlayer) return;

ShowFoldWingModel_Impl(localPlayer);

 StartSAPGlid_Impl(localPlayer);
 OnStartGliding_Impl(localPlayer);

 UpdateGliderPhysics(localPlayer);
}

namespace Camera$$WorldToScreen
{
ImVec2 Regular(Vector3 pos) {
    auto cam = game_sdk->get_camera();
    if (!cam) return {0,0};

    Vector3 worldPoint = game_sdk->WorldToViewpoint(cam,pos, 2);
    Vector3 location;

    int ScreenWidth = ImGui::GetIO().DisplaySize.x;
    int ScreenHeight = ImGui::GetIO().DisplaySize.y;

    location.x = ScreenWidth * worldPoint.x;
    location.y = ScreenHeight - worldPoint.y * ScreenHeight;
    location.z  = worldPoint.z;

    return {location.x, location.y};
}

ImVec2 Checker(Vector3 pos, bool &checker) {
    auto cam = game_sdk->get_camera();
    if (!cam) return {0, 0};
   
    Vector3 worldPoint = game_sdk->WorldToViewpoint(cam,pos, 4);
    Vector3 location;
 
    int ScreenWidth = ImGui::GetIO().DisplaySize.x;
    int ScreenHeight = ImGui::GetIO().DisplaySize.y;
 
    location.x = ScreenWidth * worldPoint.x;
    location.y = ScreenHeight - worldPoint.y * ScreenHeight;
    location.z = worldPoint.z;
 
    checker = location.z > 1;
 
    return {location.x, location.y};
}
}

Vector3 GetBonePosition(void *player, void *(*transformGetter)(void *)) {
    if (!player || !transformGetter)
        return Vector3();
    void *transform = transformGetter(player);
    return transform ? game_sdk->get_position(game_sdk->Component_GetTransform(transform)) : Vector3();
}

Vector3 GetHitboxPosition(void* player, int hitbox) {
    if (!player) return Vector3::zero();
    
    switch (hitbox) {
        case 0: return GetBonePosition(player, game_sdk->GetHeadPositions);
        case 1: {
            Vector3 headPos = GetBonePosition(player, game_sdk->GetHeadPositions);
            return headPos == Vector3::zero() ? headPos : Vector3(headPos.x, headPos.y - 0.1f, headPos.z);
        }
        case 2: {
            Vector3 headPos = GetBonePosition(player, game_sdk->GetHeadPositions);
            return headPos == Vector3::zero() ? headPos : Vector3(headPos.x, headPos.y - 0.2f, headPos.z);
        }
        default: return GetBonePosition(player, game_sdk->GetHeadPositions);
    }
}

Vector3 getPosition(void *player) {
    return game_sdk->get_position(game_sdk->Component_GetTransform(player));
}

Vector3 GetHeadPosition(void *player) {
    return game_sdk->get_position(game_sdk->GetHeadPositions(player));
}

static Vector3 CameraMain(void *player) {
    return game_sdk->get_position(*(void **)((uint64_t)player + oxo("0x380")));//public Transform MainCameraTransform;
}

Quaternion GetRotationToTheLocation(Vector3 Target, float Height, Vector3 MyEnemy) {
    Vector3 direction = (Target + Vector3(0, Height, 0)) - MyEnemy;
    return Quaternion::LookRotation(direction, Vector3(0, 1, 0));
}

Quaternion GetCurrentRotation(void* player) {
    void* transform = game_sdk->Component_GetTransform(player);
    if (!transform) return Quaternion();
    return Quaternion::LookRotation(game_sdk->GetForward(transform), Vector3(0, 1, 0));
}

#include "Helper/Ext.h"

class tanghinh {
public:
    static Vector3 Transform_GetPosition(void *player) {
       Vector3 out = Vector3::zero();
        void (*_Transform_GetPosition)(void *transform, Vector3 *out) = (void (*)(void *, Vector3 *))getRealOffset(oxo("0x91CA5D0"));//private void get_position_Injected(out Vector3 ret) { }
        _Transform_GetPosition(player, &out);
        return out;
    }

    static void *Player_GetHeadCollider(void *player)
    {
        void *(*_Player_GetHeadCollider)(void *players) = (void *(*)(void *))getRealOffset(oxo("0x53C2630"));//public virtual Collider get_HeadCollider() { }
        return _Player_GetHeadCollider(player);
    }

    static bool Physics_Raycast(Vector3 camLocation, Vector3 headLocation, unsigned int LayerID, void *collider)
    {
        bool (*_Physics_Raycast)(Vector3 camLocation, Vector3 headLocation, unsigned int LayerID, void *collider) = (bool (*)(Vector3, Vector3, unsigned int, void *))getRealOffset(oxo("0x5FE855C"));//public class HPFKOGPDBBE // TypeDefIndex: 33080 public static bool FOHHPOKDOND(Vector3 OENCHMFCMMO, Vector3 MHAIPDKIHNL, uint CBCKBCPFOAI, ref GMPGMPFNMFP NKEPDPFIOFJ) { }
        return _Physics_Raycast(camLocation, headLocation, LayerID, collider);
    }

    static bool isVisible(void *enemy) {
        if (enemy != NULL) {
            void *hitObj = NULL;
            auto Camera = Transform_GetPosition(game_sdk->Component_GetTransform(game_sdk->get_camera()));
            auto Target = Transform_GetPosition(game_sdk->Component_GetTransform(Player_GetHeadCollider(enemy)));
            return !Physics_Raycast(Camera, Target, 12, &hitObj);
        }
        return false;
    }
};


void DrawSkeleton(void *player, ImDrawList *drawList)
{
    if (!player || !drawList)
        return;
    bool isPlayerVisible = tanghinh::isVisible(player);
    Vector3 headPos = GetBonePosition(player, game_sdk->_GetHeadPositions);
    Vector3 hipPos = GetBonePosition(player, game_sdk->_newHipMods);
    Vector3 leftAnklePos = GetBonePosition(player, game_sdk->_GetLeftAnkleTF);
    Vector3 rightAnklePos = GetBonePosition(player, game_sdk->_GetRightAnkleTF);
    Vector3 leftToePos = GetBonePosition(player, game_sdk->_GetLeftToeTF);
    Vector3 rightToePos = GetBonePosition(player, game_sdk->_GetRightToeTF);
    Vector3 leftHandPos = GetBonePosition(player, game_sdk->_getLeftHandTF);
    Vector3 rightHandPos = GetBonePosition(player, game_sdk->_getRightHandTF);
    Vector3 leftForeArmPos = GetBonePosition(player, game_sdk->_getLeftForeArmTF);
    Vector3 rightForeArmPos = GetBonePosition(player, game_sdk->_getRightForeArmTF);

    // Chuyển đổi vị trí xương sang tọa độ màn hình
    bool visible;
    ImVec2 headScreen = Camera$$WorldToScreen::Checker(headPos, visible);
    if (!visible)
        return;

    ImVec2 hipScreen = Camera$$WorldToScreen::Regular(hipPos);
    ImVec2 leftAnkleScreen = Camera$$WorldToScreen::Regular(leftAnklePos);
    ImVec2 rightAnkleScreen = Camera$$WorldToScreen::Regular(rightAnklePos);
    ImVec2 leftToeScreen = Camera$$WorldToScreen::Regular(leftToePos);
    ImVec2 rightToeScreen = Camera$$WorldToScreen::Regular(rightToePos);
    ImVec2 leftHandScreen = Camera$$WorldToScreen::Regular(leftHandPos);
    ImVec2 rightHandScreen = Camera$$WorldToScreen::Regular(rightHandPos);
    ImVec2 leftForeArmScreen = Camera$$WorldToScreen::Regular(leftForeArmPos);
    ImVec2 rightForeArmScreen = Camera$$WorldToScreen::Regular(rightForeArmPos);
    ImColor boneColor = isPlayerVisible ? ImColor(0, 255, 0) : ImColor(255, 255, 255);
    float thickness = 1.0f;

    // Vẽ đầu
    drawList->AddCircle(headScreen, 2.0f, boneColor, 12, thickness);
    // Vẽ thân
    drawList->AddLine(headScreen, hipScreen, boneColor, thickness);
    // Vẽ tay
    drawList->AddLine(headScreen, leftForeArmScreen, boneColor, thickness);
    drawList->AddLine(headScreen, rightForeArmScreen, boneColor, thickness);
    drawList->AddLine(leftForeArmScreen, leftHandScreen, boneColor, thickness);
    drawList->AddLine(rightForeArmScreen, rightHandScreen, boneColor, thickness);
    // Vẽ chân
    drawList->AddLine(hipScreen, leftAnkleScreen, boneColor, thickness);
    drawList->AddLine(hipScreen, rightAnkleScreen, boneColor, thickness);
    drawList->AddLine(leftAnkleScreen, leftToeScreen, boneColor, thickness);
    drawList->AddLine(rightAnkleScreen, rightToeScreen, boneColor, thickness);
}

bool isFov(Vector3 vec1, Vector3 vec2, int radius)
{
    int x = vec1.x;
    int y = vec1.y;
    int x0 = vec2.x;
    int y0 = vec2.y;
    if ((pow(x - x0, 2) + pow(y - y0, 2)) <= pow(radius, 2))
    {
        return true;
    }
    return false;
}

void *GetClosestEnemy()
{
    try
    {
        float shortestDistance = 9999.0f;
        void *closestEnemy = NULL;
        void *get_MatchGame = game_sdk->Curent_Match();
        if (!get_MatchGame)
            return NULL;
        void *LocalPlayer = game_sdk->GetLocalPlayer(get_MatchGame);
        if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
            return NULL;
        if (!Vars.Enable)
            return NULL;
        Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)get_MatchGame + oxo("0x148"));
        if (!players )
            return NULL;
        for (int u = 0; u < players->getSize(); u++)
        {
            void *Player = players->getValues()[u];
            if (!Player)
                continue;
            if (Player == LocalPlayer)
                continue;
            if (!game_sdk->get_MaxHP(Player))
                continue;
            if (game_sdk->get_IsDieing(Player))
                continue;
            if (!game_sdk->get_isVisible(Player))
                continue;
            if (game_sdk->get_isLocalTeam(Player))
                continue;
            Vector3 PlayerPos = getPosition(Player);
            Vector3 LocalPlayerPos = getPosition(LocalPlayer);
            ImVec2 screenPos = Camera$$WorldToScreen::Regular(PlayerPos);
            bool isFov1 = isFov(Vector3(screenPos.x, screenPos.y), Vector3(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), Vars.AimFov);
            float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
            if (distance < 200)
            {
                Vector3 targetDir = Vector3::Normalized(PlayerPos - LocalPlayerPos);
                float angle = Vector3::Angle(targetDir, game_sdk->GetForward(game_sdk->Component_GetTransform(game_sdk->get_camera()))) * 100.0f;
                if (angle <= Vars.AimFov && isFov1 && angle < shortestDistance)
                {
                    if (tanghinh::isVisible(Player))
                    {
                        shortestDistance = angle;
                        closestEnemy = Player;
                    }
                }
            }
        }
        return closestEnemy;
    }
    catch (...)
    {
        return NULL;
    }
}
void ProcessAimbot()
{
    if (!Vars.Aimbot)
        return;
    void *CurrentMatch = game_sdk->Curent_Match();
    if (!CurrentMatch)
        return;
    void *LocalPlayer = game_sdk->GetLocalPlayer(CurrentMatch);
    if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
        return;
    if (game_sdk->get_IsDieing(LocalPlayer))
        return;
    void *closestEnemy = GetClosestEnemy();
    if (!closestEnemy || !game_sdk->Component_GetTransform(closestEnemy))
        return;
    if (game_sdk->get_IsDieing(closestEnemy))
        return;
    if (!game_sdk->get_isVisible(closestEnemy))
        return;
    Vector3 EnemyLocation = GetHeadPosition(closestEnemy);
    if (EnemyLocation == Vector3::zero())
        return;
    Vector3 PlayerLocation = CameraMain(LocalPlayer);
  if (PlayerLocation == Vector3::zero())
        return;
    Quaternion PlayerLook = GetRotationToTheLocation(EnemyLocation, 0.1f, PlayerLocation);

    bool IsScopeOn = game_sdk->get_IsSighting(LocalPlayer);
    bool IsFiring = game_sdk->get_IsFiring(LocalPlayer);

    bool shouldAim =
        (Vars.AimWhen == 0) ||
        (Vars.AimWhen == 1 && IsFiring) ||
        (Vars.AimWhen == 2 && IsScopeOn);

    if (shouldAim)
    {
        game_sdk->set_aim(LocalPlayer, PlayerLook);
    }
}
void get_players()
{
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    if (!draw_list)
        return;
    if (!Vars.Enable)
        return;
    try
    {
        ProcessAimbot();

        void *current_Match = game_sdk->Curent_Match();
        if (!current_Match)
            return;

        void *local_player = game_sdk->GetLocalPlayer(current_Match);
        if (!local_player)
            return;

        TriggerRealGlider(local_player);

        Dictionary<uint8_t *, void **> *players = *(Dictionary<uint8_t *, void **> **)((long)current_Match + 0x148);
        if (!players )
            return;

        void *camera = game_sdk->get_camera();
        if (!camera)
            return;

        for (int u = 0; u < players->getSize(); u++)
        {
            void *closestEnemy = players->getValues()[u];
            if (!closestEnemy)
                continue;
            if (!game_sdk->Component_GetTransform(closestEnemy))
                continue;
            if (closestEnemy == local_player)
                continue;
            if (!game_sdk->get_MaxHP(closestEnemy))
                continue;
            if (game_sdk->get_IsDieing(closestEnemy))
                continue;
            if (!game_sdk->get_isVisible(closestEnemy))
                continue;
            if (game_sdk->get_isLocalTeam(closestEnemy))
                continue;

            Vector3 pos = getPosition(closestEnemy);
            Vector3 pos2 = getPosition(local_player);
            float distance = Vector3::Distance(pos, pos2);
            if (distance > 200.0f)
                continue;
            ImColor line_color = ImColor(255, 255, 255);
            bool w2sc;
            ImVec2 top_pos = Camera$$WorldToScreen::Regular(pos + Vector3(0, 1.6, 0));
            ImVec2 bot_pos = Camera$$WorldToScreen::Regular(pos);
            ImVec2 pos_3 = Camera$$WorldToScreen::Checker(pos, w2sc);
            auto pmtXtop = top_pos.x;
            auto pmtXbottom = bot_pos.x;
            if (top_pos.x > bot_pos.x)
            {
                pmtXtop = bot_pos.x;
                pmtXbottom = top_pos.x;
            }
            Camera$$WorldToScreen::Checker(pos + Vector3(0, 0.75f, 0), w2sc);
            float calculatedPosition = fabs((top_pos.y - bot_pos.y) * (0.0092f / 0.019f) / 2);

            ImRect rect(
                ImVec2(pmtXtop - calculatedPosition, top_pos.y),
                ImVec2(pmtXbottom + calculatedPosition, bot_pos.y));
            const auto &viewpos = game_sdk->get_position(game_sdk->Component_GetTransform(game_sdk->get_camera()));
            if (w2sc)
            {
                if (Vars.lines)
                {
                    if (game_sdk->get_IsDieing(closestEnemy))
                    {
                        draw_list->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 0), ImVec2(rect.GetCenter().x, rect.Min.y), ImColor(255, 0, 0));
                    }
                    else
                    {
                        draw_list->AddLine(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 0), ImVec2(rect.GetCenter().x, rect.Min.y), line_color);
                    }
                }
                if (Vars.Box)
                {
                    if (game_sdk->get_IsDieing(closestEnemy))
                    {
                        draw_list->AddRect(rect.Min, rect.Max, ImColor(255, 0, 0));
                    }
                    else
                    {
                        draw_list->AddRect(rect.Min, rect.Max, ImColor(255, 255, 255));
                    }
                    
                    if (Vars.Outline)
                    {
                        draw_list->AddRect(rect.Min - ImVec2(1, 1), rect.Max + ImVec2(1, 1), ImColor(0, 0, 0), 0.65, 0, 1);
                        draw_list->AddRect(rect.Min + ImVec2(1, 1), rect.Max - ImVec2(1, 1), ImColor(0, 0, 0), 0.65, 0, 1);
                    }
                }
                if (Vars.Name)
                {
                    auto pname = game_sdk->name(closestEnemy);
                    std::string names = "null";
                    if (pname)
                        names = pname->toCPPString();
                    std::transform(names.begin(), names.end(), names.begin(), ::tolower);
                    auto playername = names;
                    std::string name = names;
                    ImVec2 text_size = verdana_smol->CalcTextSizeA(8, FLT_MAX, 0, names.c_str());
                    ImVec2 name_pos = {
                        rect.Min.x + (rect.GetWidth() / 2) - text_size.x / 2,
                        rect.Min.y - 2 - text_size.y};
                    AddText(verdana_smol, 8, false, Vars.Outline, name_pos, ImColor(255, 255, 255), name);
                }
                if (Vars.Health)
                {
                    auto health = game_sdk->GetHp(closestEnemy);
                    auto maxhealth = game_sdk->get_MaxHP(closestEnemy);
                    float health_multiplier = (float)health / (float)maxhealth;
                    float health_bar_pos = rect.Min.x - 4;
                    draw_list->AddLine({health_bar_pos, rect.Min.y - 1}, {health_bar_pos, rect.Max.y}, ImColor(0, 0, 0, 100), 3);
                    draw_list->AddLine({health_bar_pos - 0.5f, rect.Max.y}, {health_bar_pos - 0.5f, rect.Max.y - (rect.GetHeight() + 1) * health_multiplier}, ImColor(0, 255, 0), 3);
                    if (Vars.Outline)
                        draw_list->AddRect({health_bar_pos - 2, rect.Min.y - 1}, {health_bar_pos + 2, rect.Max.y + 1}, ImColor(0, 0, 0));
                    std::string hpstr = fmt::format(oxorany("{}HP"), static_cast<int>(health));
                    ImVec2 text_size_hp = pixel_smol->CalcTextSizeA(8, FLT_MAX, 0, hpstr.c_str());
                    ImVec2 text_pos = {
                        rect.Min.x + (rect.GetWidth() / 2) - text_size_hp.x / 2,
                        rect.Max.y};
                    AddText(pixel_smol, 8, false, true, text_pos, ImColor(0, 255, 0), hpstr.c_str());
                }
                if (Vars.Distance)
                {
                    std::string distancestr = fmt::format(oxorany("{}M"), static_cast<int>(distance));
                    ImVec2 distance_pos = {
                        rect.Max.x + 4,
                        rect.Min.y};
                    AddText(pixel_smol, 8, false, true, distance_pos, ImColor(255, 255, 255), distancestr.c_str());
                }
                if (Vars.circlepos)
                {
                    Draw3DCircle(pos, 1.0f, 0.5f, ImColor(255, 0, 0), 36, false, 0.5f);
                }
                if (Vars.skeleton)
                {
                    DrawSkeleton(closestEnemy, draw_list);
                }
            }
            if (Vars.OOF)
            {
                if ((pos_3.x < 0 || pos_3.x > disp.width) || (pos_3.y < 0 || pos_3.y > disp.height) || !w2sc)
                {
                    constexpr int maxpixels = 150;
                    int pixels = maxpixels;
                    if (w2sc)
                    {
                        if (pos_3.x < 0)
                            pixels = clamp((int)-pos_3.x, 0, (int)maxpixels);
                        if (pos_3.y < 0)
                            pixels = clamp((int)-pos_3.y, 0, (int)maxpixels);

                        if (pos_3.x > disp.width)
                            pixels = clamp((int)pos_3.x - (int)disp.width, 0, (int)maxpixels);
                        if (pos_3.y > disp.height)
                            pixels = clamp((int)pos_3.y - (int)disp.height, 0, (int)maxpixels);
                    }

                    float opacity = (float)pixels / (float)maxpixels;

                    float size = 3.5f;
                    Vector3 viewdir = game_sdk->GetForward(game_sdk->Component_GetTransform(game_sdk->get_camera()));
                    Vector3 targetdir = Vector3::Normalized(pos - viewpos);

                    float viewangle = atan2(viewdir.z, viewdir.x) * Rad2Deg;
                    float targetangle = atan2(targetdir.z, targetdir.x) * Rad2Deg;

                    if (viewangle < 0)
                        viewangle += 360;
                    if (targetangle < 0)
                        targetangle += 360;

                    float angle = targetangle - viewangle;

                    while (angle < 0)
                        angle += 360;
                    while (angle > 360)
                        angle -= 360;

                    angle = 360 - angle;
                    angle -= 90;
                    OtFovV1(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2, 90 + distance * 2,
                            angle - size,
                            angle + size,
                            ImColor(1.f, 1.f, 1.f, 1.f * opacity), 1);
                }
            }
        }
    }
    catch (...)
    {
        return;
    }
}
void aimbot()
{
    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
    if (!Vars.Aimbot)
        return;
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    if (!draw_list)
        return;
    void *Match = game_sdk->Curent_Match();
    if (!Match)
        return;
    if (Vars.isAimFov)
    {
        if (Vars.fovaimglow)
            drawcircleglow(draw_list, center, Vars.AimFov, ImColor(Vars.fovLineColor[0], Vars.fovLineColor[1], Vars.fovLineColor[2], Vars.fovLineColor[3]), 999, 1, 12);
        else
            draw_list->AddCircle(center, Vars.AimFov, ImColor(Vars.fovLineColor[0], Vars.fovLineColor[1], Vars.fovLineColor[2], Vars.fovLineColor[3]), 100);
    }
    void *LocalPlayer = game_sdk->GetLocalPlayer(Match);
    if (!LocalPlayer)
        return;
    void *playertarget = GetClosestEnemy();
    if (!playertarget)
        return;
    ImVec2 EnemyLocation = Camera$$WorldToScreen::Regular(GetHeadPosition(playertarget));
    drawlineglow(draw_list, ImVec2(center.x, center.y), EnemyLocation, ImColor(255, 255, 255), 1, 3);
}

#import "Helper/AimkillSend.h"

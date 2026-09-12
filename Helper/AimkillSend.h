#pragma once

#import <QuartzCore/QuartzCore.h>
#include <cstdint>
#include <chrono>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include <cstdlib>

// ============================================================================
// AIMKILL SEND + BODY KILL  (ported from AIMKILL_SEND_CLEAN.txt to iOS)
// ----------------------------------------------------------------------------
// All offsets below were verified against dump54.txt (iOS OB54 Il2CppDumper
// dump, which matches every working method offset of this mod, e.g.
// CurrentMatch=0x55C4DA4, GetLocalPlayer=0x2FFE494, get_CurHP=0x543592C).
// The earlier OffsetsffNormal field values (0x458/0x46C/0x4BC/0x99) belonged
// to a different build and are replaced here with the dump54 values.
// ============================================================================

namespace AimkillOffsets {

    // ---- Player field offsets (dump54 Player class, TypeDefIndex 30884) ----
    constexpr uintptr_t Player_HeadTF = 0x638;           // Player PEMOFNFCLFB (head bone)
    constexpr uintptr_t Player_RootTF = 0x660;           // Player KNFKIDHJCCO (root bone)
    constexpr uintptr_t Player_PlayerAttributes = 0x700; // Player KDJHNBAECLM
    constexpr uintptr_t Player_FastSwitch = 0x794;       // Player KDNABNMDIPA (float)
    constexpr uintptr_t Player_HitObjectInfoWp = 0xDC8;  // Player AKFLHNOIHED (GMPGMPFNMFP hit info)
    constexpr uintptr_t PlayerAttributes_NoReload = 0xD9;// PlayerAttributes ShootNoReload

    // ---- Method offsets (dump54) ----
    constexpr uintptr_t GetWeaponOnHand = 0x53BE110;        // Player.GetWeaponOnHand() -> FDAEPHMIEPC
    constexpr uintptr_t get_PlayerID = 0x5392374;           // Player.get_PlayerID() -> BHGGAEEHJCO
    constexpr uintptr_t PlayerIDToUInt = 0x60A37EC;         // BHGGAEEHJCO.LMCLIKOCCJM(BHGGAEEHJCO) -> uint
    constexpr uintptr_t WeaponFire = 0x58B3258;             // FDAEPHMIEPC.KOODEKEKFEK(weapon, hitInfo) -> int
    constexpr uintptr_t GKHECDLGAJA = 0x5433094;            // Player.KNBEPECEJEP(hitInfo) (OB54 rename of GKHECDLGAJA)
    constexpr uintptr_t StartFiring = 0x56E6494;            // PlayerNetwork.StartFiring(FDAEPHMIEPC)
    constexpr uintptr_t StopFire = 0x56E7078;               // PlayerNetwork.StopFire(FDAEPHMIEPC)
    constexpr uintptr_t GameFacade_Send = 0x55CE9F8;        // GameFacade.Send(uint, UDPClientMessageBase, byte, bool)
    constexpr uintptr_t set_position_Injected = 0x91CA6A8;  // UnityEngine.Transform::set_position_Injected(ref Vector3)
    constexpr uintptr_t GetHipTF = 0x5454990;               // Player.GetHipTF()
    constexpr uintptr_t get_LockedAimingCollider = 0x5379D40;// AttackableEntity.get_LockedAimingCollider()
    constexpr uintptr_t Component_GetGameObject = 0x91B8334;// UnityEngine.Component.get_gameObject()
    constexpr uintptr_t GetDamage = 0x588AC8C;              // FDAEPHMIEPC.BAPINEFALEM() -> int
    constexpr uintptr_t SyncSwapWeapon = 0x0;               // RUDP swap-sync message (not available on iOS) - keep 0

    // ---- GMPGMPFNMFP hit-info field offsets (dump54, TypeDefIndex 33076) ----
    constexpr uintptr_t Hit_GameObject = 0x18;          // HLIJMDODPIM
    constexpr uintptr_t Hit_HeadCollider = 0x20;        // OCEBCHENIOK
    constexpr uintptr_t Hit_Ignore = 0x70;              // DEDOKPCAHAC
    constexpr uintptr_t Hit_SpecialHitType = 0x80;      // LNOIFBAFGOK
    constexpr uintptr_t Hit_HitLoc = 0x28;              // MBGBCLNJOMK
    constexpr uintptr_t Hit_Normal = 0x34;              // DGFLGBEOGPG
    constexpr uintptr_t Hit_RayDir = 0x40;              // IKDEGKIICJP
    constexpr uintptr_t Hit_StartPos = 0x4C;            // LMAEGPEAECO
    constexpr uintptr_t Hit_OrgStrtPos = 0x74;          // KPEICEMCHIF
    constexpr uintptr_t Hit_Part = 0x64;                // FLCLOHCBJEI (JKCLPFEFMNG: 0=Default, 1=Head, 2=Body)
    constexpr uintptr_t HitInfo_Damage = 0x58;          // BGKGJKDILEA (int damage)
    constexpr uintptr_t HitInfo_Distance = 0x5C;        // PGCPFOAJHBM (float distance)
}

struct AimkillMaster_t {
    bool SafeAimkill = false;
    bool AimkillBody = false;
    bool Aimkillsend = false;
    bool AimkillFiring = false;
    bool BodySilentAim = false;
    bool SafeSilentAim = false;
    bool NoBulletTracking = false;
    bool hidedamage = false;
    bool fastfiremax = false;
    bool fireScaleHack = false;
} AimkillMaster;

static double Aimkill_Now() {
    return CACurrentMediaTime();
}

static void* Aimkill_LocalPlayer() {
    void* match = game_sdk->Curent_Match();
    if (!match) return nullptr;
    void* lp = game_sdk->GetLocalPlayer(match);
    return lp;
}

static bool Aimkill_InActiveMatch() {
    return Aimkill_LocalPlayer() != nullptr;
}

static bool Aimkill_IsDieing(void* player) {
    if (!player) return true;
    if (*(void**)player == nullptr) return true;
    return game_sdk->get_IsDieing(player);
}

static bool Aimkill_IsTeammate(void* player) {
    if (!player) return true;
    return game_sdk->get_isLocalTeam(player);
}

static void* Aimkill_GetWeaponOnHand(void* player) {
    if (!player || AimkillOffsets::GetWeaponOnHand == 0) return nullptr;
    void* (*fn)(void*) = (void* (*)(void*))getRealOffset(AimkillOffsets::GetWeaponOnHand);
    return fn ? fn(player) : nullptr;
}

static void* Aimkill_GetHeadCollider(void* player) {
    if (!player) return nullptr;
    return tanghinh::Player_GetHeadCollider(player);
}

static Vector3 Aimkill_ComponentPos(void* component) {
    if (!component) return Vector3::zero();
    void* tf = game_sdk->Component_GetTransform(component);
    return tf ? game_sdk->get_position(tf) : Vector3::zero();
}

static Vector3 Aimkill_GetHeadPosition(void* player) {
    return GetHeadPosition(player);
}

static Vector3 Aimkill_LocalCameraPosition() {
    void* cam = game_sdk->get_camera();
    if (!cam) return Vector3::zero();
    void* tf = game_sdk->Component_GetTransform(cam);
    return tf ? game_sdk->get_position(tf) : Vector3::zero();
}

static bool Aimkill_Raycast(Vector3 origin, Vector3 target, unsigned layer, void*& hitObj) {
    return tanghinh::Physics_Raycast(origin, target, layer, &hitObj);
}

static bool Aimkill_IsVisible(void* enemy) {
    if (!enemy) return false;
    void* local = Aimkill_LocalPlayer();
    if (!local) return false;
    return tanghinh::isVisible(enemy);
}

// ---- BODY KILL helpers ----

static void* Aimkill_GetBodyCollider(void* player, void* headCollider) {
    if (!player) return nullptr;
    if (AimkillOffsets::get_LockedAimingCollider != 0) {
        void* (*fn)(void*) = (void* (*)(void*))getRealOffset(AimkillOffsets::get_LockedAimingCollider);
        if (fn) {
            void* col = fn(player);
            if (col) return col;
        }
    }
    return headCollider;
}

static Vector3 Aimkill_GetBodyPosition(void* player) {
    if (!player) return Vector3::zero();

    if (AimkillOffsets::GetHipTF != 0) {
        void* (*fn)(void*) = (void* (*)(void*))getRealOffset(AimkillOffsets::GetHipTF);
        if (fn) {
            void* hip = fn(player);
            Vector3 p = Aimkill_ComponentPos(hip);
            if (p != Vector3::zero()) {
                p.y += 0.25f;
                return p;
            }
        }
    }

    void* headComponent = *(void**)((uintptr_t)player + AimkillOffsets::Player_HeadTF);
    void* rootComponent = *(void**)((uintptr_t)player + AimkillOffsets::Player_RootTF);
    Vector3 headPos = Aimkill_ComponentPos(headComponent);
    if (headPos == Vector3::zero()) return Vector3::zero();

    if (rootComponent) {
        Vector3 rootPos = Aimkill_ComponentPos(rootComponent);
        Vector3 body = rootPos + (headPos - rootPos) * 0.55f;
        body.y += 0.25f;
        return body;
    }

    Vector3 bodyPos = headPos;
    bodyPos.y -= 0.50f;
    return bodyPos;
}

// ---- PULL & RESTORE (needs set_position_Injected) ----

static bool Aimkill_IsPullTargetValid(void* enemy) {
    if (!enemy) return false;
    if (*(void**)enemy == nullptr) return false;
    if (game_sdk->GetHp(enemy) <= 0) return false;
    if (Aimkill_IsDieing(enemy)) return false;
    if (!*(void**)((uintptr_t)enemy + AimkillOffsets::Player_HeadTF)) return false;
    if (!game_sdk->Component_GetTransform(enemy)) return false;
    return true;
}

static void Aimkill_SetPosition(void* tf, Vector3 position) {
    if (!tf) return;
    if (AimkillOffsets::set_position_Injected == 0) return;
    void (*fn)(void*, Vector3*) = (void (*)(void*, Vector3*))getRealOffset(AimkillOffsets::set_position_Injected);
    if (!fn) return;
    fn(tf, &position);
}

static bool Aimkill_SafeSilentPull(void* enemy, float* outBestXZRadius, bool apply = true) {
    if (!Aimkill_IsPullTargetValid(enemy)) return false;
    if (Aimkill_IsVisible(enemy)) {
        if (outBestXZRadius) *outBestXZRadius = 0.0f;
        return true;
    }
    if (AimkillOffsets::set_position_Injected == 0) return false;

    Vector3 enemyRoot = getPosition(enemy);
    Vector3 enemyHead = Aimkill_GetHeadPosition(enemy);
    Vector3 headOffset = enemyHead - enemyRoot;

    void* lp = Aimkill_LocalPlayer();
    if (!lp) return false;
    Vector3 lpHead = Aimkill_GetHeadPosition(lp);

    auto testPull = [&](float dx, float dy, float dz) -> bool {
        if (!Aimkill_IsPullTargetValid(enemy)) return false;
        Vector3 newRoot = enemyRoot;
        newRoot.x += dx; newRoot.y += dy; newRoot.z += dz;
        Vector3 newHead = newRoot + headOffset;

        void* hitObj = nullptr;
        if (Aimkill_Raycast(lpHead, newHead, 12, hitObj)) return false;

        if (outBestXZRadius) *outBestXZRadius = sqrtf(dx * dx + dz * dz);
        if (apply) {
            void* tf = game_sdk->Component_GetTransform(enemy);
            if (!tf) return false;
            Aimkill_SetPosition(tf, newRoot);
        }
        return true;
    };

    static const float zF[] = {0.8f, 1.2f, -0.8f, -1.2f};
    for (float z : zF) if (testPull(0, 0, z)) return true;
    static const float xS[] = {-0.5f, -0.8f, 0.5f, 0.8f};
    for (float x : xS) if (testPull(x, 0, 0)) return true;
    for (float z : zF) for (float x : xS) if (testPull(x, 0, z)) return true;
    return false;
}

static bool Aimkill_SilentGhostPullV2(void* enemy, float* outBestXZRadius, bool apply = true) {
    if (!Aimkill_IsPullTargetValid(enemy)) return false;
    if (Aimkill_IsVisible(enemy)) {
        if (outBestXZRadius) *outBestXZRadius = 0.0f;
        return true;
    }
    if (AimkillOffsets::set_position_Injected == 0) return false;

    Vector3 enemyRoot = getPosition(enemy);
    Vector3 enemyHead = Aimkill_GetHeadPosition(enemy);
    Vector3 headOffset = enemyHead - enemyRoot;

    void* lp = Aimkill_LocalPlayer();
    if (!lp) return false;
    Vector3 lpHead = Aimkill_GetHeadPosition(lp);

    auto testPull = [&](float dx, float dy, float dz) -> bool {
        if (!Aimkill_IsPullTargetValid(enemy)) return false;
        Vector3 newRoot = enemyRoot;
        newRoot.x += dx; newRoot.y += dy; newRoot.z += dz;
        Vector3 newHead = newRoot + headOffset;

        void* hitObj = nullptr;
        if (Aimkill_Raycast(lpHead, newHead, 12, hitObj)) return false;

        if (outBestXZRadius) *outBestXZRadius = sqrtf(dx * dx + dz * dz);
        if (apply) {
            void* tf = game_sdk->Component_GetTransform(enemy);
            if (!tf) return false;
            Aimkill_SetPosition(tf, newRoot);
        }
        return true;
    };

    static const float zF[] = {1.5f, 3.0f, 4.5f, -1.5f, -3.0f, -4.5f};
    for (float z : zF) if (testPull(0, 0, z)) return true;
    static const float xS[] = {-2.0f, -0.8f, 0.8f, 2.0f, -3.2f, 3.2f};
    for (float x : xS) if (testPull(x, 0, 0)) return true;
    for (float z : zF) for (float x : xS) if (testPull(x, 0, z)) return true;
    for (int i = 0; i < 30; i++) {
        float dy = -1.4f - 0.015f * i;
        if (testPull(0, dy, 0)) return true;
    }
    return false;
}

static bool Aimkill_MethodPull(void* enemy, void** outTf, Vector3* outOriginal) {
    if (!enemy || !Aimkill_IsPullTargetValid(enemy)) return false;
    void* tf = game_sdk->Component_GetTransform(enemy);
    if (!tf) return false;
    if (outTf) *outTf = tf;
    if (outOriginal) *outOriginal = game_sdk->get_position(tf);
    if (Aimkill_IsVisible(enemy)) return false;

    if (AimkillMaster.SafeSilentAim) return Aimkill_SafeSilentPull(enemy, nullptr, true);
    return Aimkill_SilentGhostPullV2(enemy, nullptr, true);
}

static void Aimkill_MethodRestore(void* enemy, void* tf, const Vector3& original) {
    if (enemy && tf && Aimkill_IsPullTargetValid(enemy)) {
        Aimkill_SetPosition(tf, original);
    }
}

// ---- WEAPON SYNC / FIRING helpers ----

static int Aimkill_GetDamage(void* weapon) {
    if (weapon && AimkillOffsets::GetDamage != 0) {
        int (*fn)(void*) = (int (*)(void*))getRealOffset(AimkillOffsets::GetDamage);
        if (fn) {
            int d = fn(weapon);
            if (d > 0) return d;
        }
    }
    return 30;
}

static uint32_t Aimkill_GetPlayerID(void* player) {
    if (!player) return 0;
    if (AimkillOffsets::get_PlayerID == 0) return 0;
    uint32_t (*fn)(void*, void* out) = (uint32_t (*)(void*, void*))getRealOffset(AimkillOffsets::get_PlayerID);
    if (!fn) return 0;
    uint8_t out[0x18] = {};
    fn(player, out);
    if (AimkillOffsets::PlayerIDToUInt == 0) return 0;
    uint32_t (*conv)(void*) = (uint32_t (*)(void*))getRealOffset(AimkillOffsets::PlayerIDToUInt);
    return conv ? conv(out) : 0;
}

static void Aimkill_StartFiring(void* player, void* weapon) {
    if (!player || !weapon || AimkillOffsets::StartFiring == 0) return;
    void (*fn)(void*, void*) = reinterpret_cast<void (*)(void*, void*)>(getRealOffset(AimkillOffsets::StartFiring));
    if (fn) fn(player, weapon);
}

static void Aimkill_StopFire(void* player, void* weapon) {
    if (!player || !weapon || AimkillOffsets::StopFire == 0) return;
    void (*fn)(void*, void*) = reinterpret_cast<void (*)(void*, void*)>(getRealOffset(AimkillOffsets::StopFire));
    if (fn) fn(player, weapon);
}

static void* Aimkill_GKHECDLGAJA(void* pthis, void* a1) {
    if (!pthis || !a1 || AimkillOffsets::GKHECDLGAJA == 0) return nullptr;
    return ((void* (*)(void*, void*))getRealOffset(AimkillOffsets::GKHECDLGAJA))(pthis, a1);
}

static void Aimkill_SyncSwapWeapon(void* localPlayer, void* weaponOnHand) {
    if (!localPlayer || !weaponOnHand || AimkillOffsets::SyncSwapWeapon == 0) return;
    void* addr = (void*)getRealOffset(AimkillOffsets::SyncSwapWeapon);
    if (!addr || AimkillOffsets::GameFacade_Send == 0) return;
    void (*send)(int, void*, int, int) = (void (*)(int, void*, int, int))getRealOffset(AimkillOffsets::GameFacade_Send);
    if (send) send(108, addr, 2, 0);
}

static void Aimkill_FastSwitch() {
    void* lp = Aimkill_LocalPlayer();
    if (!lp) return;
    if (!AimkillMaster.fastswitch && !AimkillMaster.SafeAimkill &&
        !AimkillMaster.Aimkillsend && !AimkillMaster.AimkillFiring) return;
    if (AimkillOffsets::Player_FastSwitch == 0) return;
    float* fs = (float*)((uintptr_t)lp + AimkillOffsets::Player_FastSwitch);
    if (fs) *fs = 0.0f;
}

// ---- HIT INFO builder ----

static bool Aimkill_FillHitInfoDirectly(void* hitInfo, void* targetCollider,
                                        Vector3 enemyPos, Vector3 localHeadPos,
                                        Vector3 direction, float dist, int baseDamage) {
    if (!hitInfo || !targetCollider) return false;

    if (AimkillOffsets::Hit_GameObject != 0) {
        void* go = nullptr;
        void* (*getGo)(void*) = (void* (*)(void*))getRealOffset(AimkillOffsets::Component_GetGameObject);
        if (getGo) go = getGo(targetCollider);
        if (!go) return false;
        *(void**)((uintptr_t)hitInfo + AimkillOffsets::Hit_GameObject) = go;
    }
    if (AimkillOffsets::Hit_HeadCollider != 0)
        *(void**)((uintptr_t)hitInfo + AimkillOffsets::Hit_HeadCollider) = targetCollider;
    if (AimkillOffsets::Hit_HitLoc != 0)
        *(Vector3*)((uintptr_t)hitInfo + AimkillOffsets::Hit_HitLoc) = enemyPos;
    if (AimkillOffsets::Hit_Normal != 0)
        *(Vector3*)((uintptr_t)hitInfo + AimkillOffsets::Hit_Normal) = direction;
    if (AimkillOffsets::Hit_RayDir != 0)
        *(Vector3*)((uintptr_t)hitInfo + AimkillOffsets::Hit_RayDir) = direction;
    if (AimkillOffsets::Hit_StartPos != 0)
        *(Vector3*)((uintptr_t)hitInfo + AimkillOffsets::Hit_StartPos) = localHeadPos;
    if (AimkillOffsets::Hit_OrgStrtPos != 0)
        *(Vector3*)((uintptr_t)hitInfo + AimkillOffsets::Hit_OrgStrtPos) = localHeadPos;
    if (AimkillOffsets::Hit_Part != 0)
        *(int*)((uintptr_t)hitInfo + AimkillOffsets::Hit_Part) = AimkillMaster.AimkillBody ? 2 : 1;
    if (AimkillOffsets::Hit_Ignore != 0)
        *(bool*)((uintptr_t)hitInfo + AimkillOffsets::Hit_Ignore) = false;

    *(float*)((uintptr_t)hitInfo + AimkillOffsets::HitInfo_Distance) = dist;
    *(int*)((uintptr_t)hitInfo + AimkillOffsets::HitInfo_Damage) = baseDamage;
    return true;
}

// ---- RATE LIMITER (anti-detection) ----

namespace {
    const int kSafeMaxPacketsPerEnemy = 15;
    const int kSafeBurstPacketMax = 8;
    const long long kSafeMinPacketGapMs = 10;
    const long long kSafeMaxPacketGapMs = 80;
    const long long kSafeBurstResetMs = 2000;

    struct SafeAimkillTracker {
        void* trackedEnemy = nullptr;
        int packetsToEnemy = 0;
        int burstPackets = 0;
        long long nextRequiredGapMs = 200;
        std::chrono::steady_clock::time_point lastPacketAt{};
        std::chrono::steady_clock::time_point burstWindowStart{};
    } g_safeAimkill;

    long long SafeAimkillNextGapMs() {
        static bool seeded = false;
        if (!seeded) {
            srand((unsigned int)(Aimkill_Now() * 1000.0f));
            seeded = true;
        }
        long long span = kSafeMaxPacketGapMs - kSafeMinPacketGapMs;
        if (span < 0) span = 0;
        return kSafeMinPacketGapMs + (rand() % (int)(span + 1));
    }

    bool SafeAimkillCanSendPacket(void* enemy) {
        if (!enemy) return false;
        auto now = std::chrono::steady_clock::now();

        if (g_safeAimkill.trackedEnemy != enemy) {
            g_safeAimkill.trackedEnemy = enemy;
            g_safeAimkill.packetsToEnemy = 0;
        }
        if (Aimkill_IsDieing(enemy) || game_sdk->GetHp(enemy) <= 0) {
            g_safeAimkill.trackedEnemy = nullptr;
            g_safeAimkill.packetsToEnemy = 0;
            return false;
        }
        if (g_safeAimkill.packetsToEnemy >= kSafeMaxPacketsPerEnemy) return false;

        if (g_safeAimkill.burstWindowStart.time_since_epoch().count() == 0) {
            g_safeAimkill.burstWindowStart = now;
            g_safeAimkill.burstPackets = 0;
        } else {
            long long burstAge = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_safeAimkill.burstWindowStart).count();
            if (burstAge >= kSafeBurstResetMs) {
                g_safeAimkill.burstWindowStart = now;
                g_safeAimkill.burstPackets = 0;
            }
        }
        if (g_safeAimkill.burstPackets >= kSafeBurstPacketMax) return false;

        if (g_safeAimkill.lastPacketAt.time_since_epoch().count() != 0) {
            long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_safeAimkill.lastPacketAt).count();
            if (elapsed < g_safeAimkill.nextRequiredGapMs) return false;
        }
        return true;
    }

    void SafeAimkillRecordPacketSent() {
        auto now = std::chrono::steady_clock::now();
        if (g_safeAimkill.burstWindowStart.time_since_epoch().count() == 0) {
            g_safeAimkill.burstWindowStart = now;
            g_safeAimkill.burstPackets = 0;
        }
        g_safeAimkill.packetsToEnemy++;
        g_safeAimkill.burstPackets++;
        g_safeAimkill.lastPacketAt = now;
        g_safeAimkill.nextRequiredGapMs = SafeAimkillNextGapMs();
    }
}

// ---- EXECUTION ROUTINES ----

static bool g_inSafeAimkill = false;
static bool g_inAimkillSend = false;

void Aimkill_StartSafeAimkill(void* enemy) {
    if (!AimkillMaster.SafeAimkill) return;
    if (!enemy) return;
    if (Aimkill_IsDieing(enemy) || game_sdk->GetHp(enemy) <= 0) return;
    if (!Aimkill_InActiveMatch()) return;
    if (AimkillOffsets::Player_HitObjectInfoWp == 0) return;

    if (!SafeAimkillCanSendPacket(enemy)) return;
    if (g_inSafeAimkill) return;
    g_inSafeAimkill = true;

    void* localPlayer = Aimkill_LocalPlayer();
    if (!localPlayer || Aimkill_IsDieing(localPlayer) || localPlayer == enemy) { g_inSafeAimkill = false; return; }

    void* weaponOnHand = Aimkill_GetWeaponOnHand(localPlayer);
    if (!weaponOnHand) { g_inSafeAimkill = false; return; }

    void* attrs = *(void**)((uintptr_t)localPlayer + AimkillOffsets::Player_PlayerAttributes);
    if (attrs && AimkillOffsets::PlayerAttributes_NoReload != 0)
        *(bool*)((uintptr_t)attrs + AimkillOffsets::PlayerAttributes_NoReload) = true;

    void* enemyTf = nullptr;
    Vector3 originalPos = Vector3::zero();
    bool wasPulled = false;
    if (!Aimkill_IsVisible(enemy)) {
        wasPulled = Aimkill_MethodPull(enemy, &enemyTf, &originalPos);
        if (!wasPulled) { g_inSafeAimkill = false; return; }
    }

    void* headCollider = Aimkill_GetHeadCollider(enemy);
    if (!headCollider) {
        if (wasPulled) Aimkill_MethodRestore(enemy, enemyTf, originalPos);
        g_inSafeAimkill = false;
        return;
    }

    void* targetCollider = AimkillMaster.AimkillBody ? Aimkill_GetBodyCollider(enemy, headCollider) : headCollider;
    if (!targetCollider) {
        if (wasPulled) Aimkill_MethodRestore(enemy, enemyTf, originalPos);
        g_inSafeAimkill = false;
        return;
    }

    void* hitObjectInfo = *(void**)((uintptr_t)localPlayer + AimkillOffsets::Player_HitObjectInfoWp);
    if (!hitObjectInfo) {
        if (wasPulled) Aimkill_MethodRestore(enemy, enemyTf, originalPos);
        g_inSafeAimkill = false;
        return;
    }

    Vector3 localHead = Aimkill_GetHeadPosition(localPlayer);
    Vector3 hitPos = AimkillMaster.AimkillBody ? Aimkill_GetBodyPosition(enemy) : Aimkill_GetHeadPosition(enemy);
    float dx = hitPos.x - localHead.x;
    float dy = hitPos.y - localHead.y;
    float dz = hitPos.z - localHead.z;
    float dist = sqrtf(dx * dx + dy * dy + dz * dz);
    Vector3 direction = Vector3::zero();
    if (dist > 0.0001f) direction = Vector3(dx / dist, dy / dist, dz / dist);

    int baseDamage = Aimkill_GetDamage(weaponOnHand);
    if (baseDamage <= 0) baseDamage = 30;
    int damageToSend = baseDamage * 2;
    if (damageToSend > 250) damageToSend = 250;

    Aimkill_FillHitInfoDirectly(hitObjectInfo, targetCollider, hitPos, localHead, direction, dist, damageToSend);
    Aimkill_GKHECDLGAJA(localPlayer, hitObjectInfo);

    if (AimkillOffsets::WeaponFire != 0) {
        Aimkill_StartFiring(localPlayer, weaponOnHand);
        bool (*wf)(void*, void*) = (bool (*)(void*, void*))getRealOffset(AimkillOffsets::WeaponFire);
        if (wf) wf(weaponOnHand, hitObjectInfo);
        Aimkill_StopFire(localPlayer, weaponOnHand);
        Aimkill_SyncSwapWeapon(localPlayer, weaponOnHand);
    }

    if (wasPulled) Aimkill_MethodRestore(enemy, enemyTf, originalPos);
    SafeAimkillRecordPacketSent();
    g_inSafeAimkill = false;
}

void Aimkill_StartAimKillSend(void* ClosestEnemy) {
    if (!AimkillMaster.Aimkillsend && !AimkillMaster.AimkillBody) return;
    if (!Aimkill_InActiveMatch()) return;
    if (AimkillOffsets::Player_HitObjectInfoWp == 0) return;
    if (g_inAimkillSend) return;
    g_inAimkillSend = true;

    void* localPlayer = Aimkill_LocalPlayer();
    if (!localPlayer || Aimkill_IsDieing(localPlayer)) { g_inAimkillSend = false; return; }

    void* weaponOnHand = Aimkill_GetWeaponOnHand(localPlayer);
    if (!weaponOnHand) { g_inAimkillSend = false; return; }

    void* hitObjectInfo = *(void**)((uintptr_t)localPlayer + AimkillOffsets::Player_HitObjectInfoWp);
    if (!hitObjectInfo) { g_inAimkillSend = false; return; }

    Vector3 firePos = Aimkill_GetHeadPosition(localPlayer);

    std::vector<void*> targetsToProcess;
    if (ClosestEnemy && !Aimkill_IsDieing(ClosestEnemy) && game_sdk->GetHp(ClosestEnemy) > 0 &&
        !Aimkill_IsTeammate(ClosestEnemy)) {
        targetsToProcess.push_back(ClosestEnemy);
    }

    // fallback: closest enemy to screen center
    if (targetsToProcess.empty()) {
        void* currentMatch = game_sdk->Curent_Match();
        if (currentMatch) {
            Dictionary<uint8_t*, void**>* players = *(Dictionary<uint8_t*, void**>**)((uintptr_t)currentMatch + 0x148);
            float best = 99999.0f;
            void* bestEnemy = nullptr;
            if (players) {
                for (int u = 0; u < players->getSize(); u++) {
                    void* p = players->getValues()[u];
                    if (!p || p == localPlayer || Aimkill_IsDieing(p) || game_sdk->GetHp(p) <= 0 || Aimkill_IsTeammate(p))
                        continue;
                    if (!Aimkill_IsVisible(p)) continue;
                    float d = Vector3::Distance(Aimkill_GetHeadPosition(p), firePos);
                    if (d < best) { best = d; bestEnemy = p; }
                }
            }
            if (bestEnemy) targetsToProcess.push_back(bestEnemy);
        }
    }

    for (void* target : targetsToProcess) {
        if (!target || target == localPlayer || Aimkill_IsDieing(target) || game_sdk->GetHp(target) <= 0 || Aimkill_IsTeammate(target))
            continue;

        void* enemyTf = nullptr;
        Vector3 originalPos = Vector3::zero();
        bool wasPulled = Aimkill_MethodPull(target, &enemyTf, &originalPos);
        bool visible = Aimkill_IsVisible(target);
        if (!visible && !wasPulled) {
            if (wasPulled && enemyTf) Aimkill_MethodRestore(target, enemyTf, originalPos);
            continue;
        }

        Vector3 hitPos = AimkillMaster.AimkillBody ? Aimkill_GetBodyPosition(target) : Aimkill_GetHeadPosition(target);
        float dx = hitPos.x - firePos.x;
        float dy = hitPos.y - firePos.y;
        float dz = hitPos.z - firePos.z;
        float dist = sqrtf(dx * dx + dy * dy + dz * dz);
        Vector3 direction = Vector3::zero();
        if (dist > 0.0001f) direction = Vector3(dx / dist, dy / dist, dz / dist);

        void* headCollider = Aimkill_GetHeadCollider(target);
        if (!headCollider) {
            if (wasPulled && enemyTf) Aimkill_MethodRestore(target, enemyTf, originalPos);
            continue;
        }

        void* targetCollider = AimkillMaster.AimkillBody ? Aimkill_GetBodyCollider(target, headCollider) : headCollider;
        if (!targetCollider) {
            if (wasPulled && enemyTf) Aimkill_MethodRestore(target, enemyTf, originalPos);
            continue;
        }

        void* attrs = *(void**)((uintptr_t)localPlayer + AimkillOffsets::Player_PlayerAttributes);
        if (attrs && AimkillOffsets::PlayerAttributes_NoReload != 0)
            *(bool*)((uintptr_t)attrs + AimkillOffsets::PlayerAttributes_NoReload) = true;

        Aimkill_GKHECDLGAJA(localPlayer, hitObjectInfo);

        if (!AimkillMaster.AimkillFiring) Aimkill_StartFiring(localPlayer, weaponOnHand);

        int lethalDamage = AimkillMaster.AimkillBody ? 500 : 999;
        Aimkill_FillHitInfoDirectly(hitObjectInfo, targetCollider, hitPos, firePos, direction, dist, lethalDamage);
        Aimkill_GKHECDLGAJA(localPlayer, hitObjectInfo);

        if (AimkillOffsets::WeaponFire != 0) {
            bool (*wf)(void*, void*) = (bool (*)(void*, void*))getRealOffset(AimkillOffsets::WeaponFire);
            if (wf && !Aimkill_IsDieing(target) && game_sdk->GetHp(target) > 0)
                wf(weaponOnHand, hitObjectInfo);
        }

        Aimkill_StopFire(localPlayer, weaponOnHand);
        Aimkill_SyncSwapWeapon(localPlayer, weaponOnHand);

        if (wasPulled && enemyTf) Aimkill_MethodRestore(target, enemyTf, originalPos);
        break;
    }

    g_inAimkillSend = false;
}

// ---- MAIN UPDATE LOOP ----

void AimkillSendUpdateLoop(void* cachedTarget) {
    void* localPlayer = Aimkill_LocalPlayer();
    if (!localPlayer) return;

    void* weaponOnHand = Aimkill_GetWeaponOnHand(localPlayer);
    if (!weaponOnHand) return;

    Aimkill_FastSwitch();

    if (AimkillMaster.SafeAimkill) {
        if (cachedTarget) Aimkill_StartSafeAimkill(cachedTarget);
    } else if (AimkillMaster.Aimkillsend || AimkillMaster.AimkillBody) {
        Aimkill_StartAimKillSend(cachedTarget);
    }
}

// ---- OPTIONAL HOOKS (only installed when offsets are provided) ----

inline bool (*old_Aimkill_NoBulletTract)(void* weapon, void* hitInfo) = nullptr;
inline bool hk_Aimkill_NoBulletTract(void* weapon, void* hitInfo) {
    if (hitInfo && (AimkillMaster.SafeAimkill || AimkillMaster.Aimkillsend || AimkillMaster.AimkillBody)) {
        void* lp = Aimkill_LocalPlayer();
        if (lp) {
            void* gun = Aimkill_GetWeaponOnHand(lp);
            if (gun) {
                void* target = Aimkill_GetHeadCollider(lp);
                if (target) Aimkill_FillHitInfoDirectly(hitInfo, target, Aimkill_GetHeadPosition(gun), Aimkill_GetHeadPosition(lp), Vector3::zero(), 0.01f, AimkillMaster.AimkillBody ? 250 : 500);
            }
        }
    }
    return old_Aimkill_NoBulletTract ? old_Aimkill_NoBulletTract(weapon, hitInfo) : false;
}
/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __UNIT_H
#define __UNIT_H

#include "EnumFlag.h"
#include "EventProcessor.h"
#include "FollowerRefMgr.h"
#include "FollowerReference.h"
#include "HostileRefMgr.h"
#include "ItemTemplate.h"
#include "MotionMaster.h"
#include "Object.h"
#include "Optional.h"
#include "SpellAuraDefines.h"
#include "SpellDefines.h"
#include "TaskScheduler.h"
#include "ThreatMgr.h"
#include <functional>
#include <utility>

#define WORLD_TRIGGER   12999

#define BASE_MINDAMAGE 1.0f
#define BASE_MAXDAMAGE 2.0f
#define BASE_ATTACK_TIME 2000

enum UnitBytes1Offsets : uint8
{
    UNIT_BYTES_1_OFFSET_STAND_STATE = 0,
    UNIT_BYTES_1_OFFSET_PET_TALENTS = 1,
    UNIT_BYTES_1_OFFSET_VIS_FLAG    = 2,
    UNIT_BYTES_1_OFFSET_ANIM_TIER   = 3
};

// byte value (UNIT_FIELD_BYTES_1, 0)
enum UnitStandStateType
{
    UNIT_STAND_STATE_STAND             = 0,
    UNIT_STAND_STATE_SIT               = 1,
    UNIT_STAND_STATE_SIT_CHAIR         = 2,
    UNIT_STAND_STATE_SLEEP             = 3,
    UNIT_STAND_STATE_SIT_LOW_CHAIR     = 4,
    UNIT_STAND_STATE_SIT_MEDIUM_CHAIR  = 5,
    UNIT_STAND_STATE_SIT_HIGH_CHAIR    = 6,
    UNIT_STAND_STATE_DEAD              = 7,
    UNIT_STAND_STATE_KNEEL             = 8,
    UNIT_STAND_STATE_SUBMERGED         = 9
};

// byte flag value (UNIT_FIELD_BYTES_1, 2)
enum UnitStandFlags
{
    UNIT_STAND_FLAGS_UNK1         = 0x01,
    UNIT_STAND_FLAGS_CREEP        = 0x02,
    UNIT_STAND_FLAGS_UNTRACKABLE  = 0x04,
    UNIT_STAND_FLAGS_UNK4         = 0x08,
    UNIT_STAND_FLAGS_UNK5         = 0x10,
    UNIT_STAND_FLAGS_ALL          = 0xFF
};

// byte flags value (UNIT_FIELD_BYTES_1, 3)
enum UnitBytes1_Flags
{
    UNIT_BYTE1_FLAG_GROUND          = 0x00,
    UNIT_BYTE1_FLAG_ALWAYS_STAND    = 0x01,
    UNIT_BYTE1_FLAG_HOVER           = 0x02,
    UNIT_BYTE1_FLAG_FLY             = 0x03,
    UNIT_BYTE1_FLAG_SUBMERGED       = 0x04,
    UNIT_BYTE1_FLAG_ALL             = 0xFF
};

// high byte (3 from 0..3) of UNIT_FIELD_BYTES_2
enum ShapeshiftForm
{
    FORM_NONE               = 0x00,
    FORM_CAT                = 0x01,
    FORM_TREE               = 0x02,
    FORM_TRAVEL             = 0x03,
    FORM_AQUA               = 0x04,
    FORM_BEAR               = 0x05,
    FORM_AMBIENT            = 0x06,
    FORM_GHOUL              = 0x07,
    FORM_DIREBEAR           = 0x08,
    FORM_STEVES_GHOUL       = 0x09,
    FORM_THARONJA_SKELETON  = 0x0A,
    FORM_TEST_OF_STRENGTH   = 0x0B,
    FORM_BLB_PLAYER         = 0x0C,
    FORM_SHADOW_DANCE       = 0x0D,
    FORM_CREATUREBEAR       = 0x0E,
    FORM_CREATURECAT        = 0x0F,
    FORM_GHOSTWOLF          = 0x10,
    FORM_BATTLESTANCE       = 0x11,
    FORM_DEFENSIVESTANCE    = 0x12,
    FORM_BERSERKERSTANCE    = 0x13,
    FORM_TEST               = 0x14,
    FORM_ZOMBIE             = 0x15,
    FORM_METAMORPHOSIS      = 0x16,
    FORM_UNDEAD             = 0x19,
    FORM_MASTER_ANGLER      = 0x1A,
    FORM_FLIGHT_EPIC        = 0x1B,
    FORM_SHADOW             = 0x1C,
    FORM_FLIGHT             = 0x1D,
    FORM_STEALTH            = 0x1E,
    FORM_MOONKIN            = 0x1F,
    FORM_SPIRITOFREDEMPTION = 0x20
};

// low byte (0 from 0..3) of UNIT_FIELD_BYTES_2
enum SheathState
{
    SHEATH_STATE_UNARMED  = 0,                              // non prepared weapon
    SHEATH_STATE_MELEE    = 1,                              // prepared melee weapon
    SHEATH_STATE_RANGED   = 2                               // prepared ranged weapon
};

#define MAX_SHEATH_STATE    3

// byte (1 from 0..3) of UNIT_FIELD_BYTES_2
enum UnitPVPStateFlags
{
    UNIT_BYTE2_FLAG_PVP         = 0x01,
    UNIT_BYTE2_FLAG_UNK1        = 0x02,
    UNIT_BYTE2_FLAG_FFA_PVP     = 0x04,
    UNIT_BYTE2_FLAG_SANCTUARY   = 0x08,
    UNIT_BYTE2_FLAG_UNK4        = 0x10,
    UNIT_BYTE2_FLAG_UNK5        = 0x20,
    UNIT_BYTE2_FLAG_UNK6        = 0x40,
    UNIT_BYTE2_FLAG_UNK7        = 0x80
};

// byte (2 from 0..3) of UNIT_FIELD_BYTES_2
enum UnitRename
{
    UNIT_CAN_BE_RENAMED     = 0x01,
    UNIT_CAN_BE_ABANDONED   = 0x02,
};

static constexpr uint32 MAX_CREATURE_SPELLS = 8;
static constexpr uint32 infinityCooldownDelay = 0x9A7EC800; // used for set "infinity cooldowns" for spells and check, MONTH*IN_MILLISECONDS
static constexpr uint32 infinityCooldownDelayCheck = 0x4D3F6400; // MONTH*IN_MILLISECONDS/2;

#define MAX_SPELL_CHARM         4
#define MAX_SPELL_VEHICLE       6
#define MAX_SPELL_POSSESS       8
#define MAX_SPELL_CONTROL_BAR   10

#define MAX_AGGRO_RADIUS 45.0f  // yards

enum VictimState
{
    VICTIMSTATE_INTACT         = 0, // set when attacker misses
    VICTIMSTATE_HIT            = 1, // victim got clear/blocked hit
    VICTIMSTATE_DODGE          = 2,
    VICTIMSTATE_PARRY          = 3,
    VICTIMSTATE_INTERRUPT      = 4,
    VICTIMSTATE_BLOCKS         = 5, // unused? not set when blocked, even on full block
    VICTIMSTATE_EVADES         = 6,
    VICTIMSTATE_IS_IMMUNE      = 7,
    VICTIMSTATE_DEFLECTS       = 8
};

enum HitInfo
{
    HITINFO_NORMALSWING         = 0x00000000,
    HITINFO_UNK1                = 0x00000001,               // req correct packet structure
    HITINFO_AFFECTS_VICTIM      = 0x00000002,
    HITINFO_OFFHAND             = 0x00000004,
    HITINFO_UNK2                = 0x00000008,
    HITINFO_MISS                = 0x00000010,
    HITINFO_FULL_ABSORB         = 0x00000020,
    HITINFO_PARTIAL_ABSORB      = 0x00000040,
    HITINFO_FULL_RESIST         = 0x00000080,
    HITINFO_PARTIAL_RESIST      = 0x00000100,
    HITINFO_CRITICALHIT         = 0x00000200,               // critical hit
    HITINFO_UNK10               = 0x00000400,
    HITINFO_UNK11               = 0x00000800,
    HITINFO_UNK12               = 0x00001000,
    HITINFO_BLOCK               = 0x00002000,               // blocked damage
    HITINFO_UNK14               = 0x00004000,               // set only if meleespellid is present//  no world text when victim is hit for 0 dmg(HideWorldTextForNoDamage?)
    HITINFO_UNK15               = 0x00008000,               // player victim?// something related to blod sprut visual (BloodSpurtInBack?)
    HITINFO_GLANCING            = 0x00010000,
    HITINFO_CRUSHING            = 0x00020000,
    HITINFO_NO_ANIMATION        = 0x00040000,
    HITINFO_UNK19               = 0x00080000,
    HITINFO_UNK20               = 0x00100000,
    HITINFO_SWINGNOHITSOUND     = 0x00200000,               // unused?
    HITINFO_UNK22               = 0x00400000,
    HITINFO_RAGE_GAIN           = 0x00800000,
    HITINFO_FAKE_DAMAGE         = 0x01000000                // enables damage animation even if no damage done, set only if no damage
};

//i would like to remove this: (it is defined in item.h
enum InventorySlot
{
    NULL_BAG                   = 0,
    NULL_SLOT                  = 255
};

struct FactionTemplateEntry;
struct SpellValue;

class AuraApplication;
class Aura;
class UnitAura;
class AuraEffect;
class Creature;
class Spell;
class SpellInfo;
class DynamicObject;
class GameObject;
class Item;
class Pet;
class PetAura;
class Minion;
class Guardian;
class UnitAI;
class Totem;
class Transport;
class StaticTransport;
class MotionTransport;
class Vehicle;
class TransportBase;
class SpellCastTargets;

typedef std::list<Unit*> UnitList;
typedef std::list< std::pair<Aura*, uint8> > DispelChargesList;

enum UnitModifierType
{
    BASE_VALUE = 0,
    BASE_PCT = 1,
    TOTAL_VALUE = 2,
    TOTAL_PCT = 3,
    MODIFIER_TYPE_END = 4
};

enum WeaponDamageRange
{
    MINDAMAGE,
    MAXDAMAGE,

    MAX_WEAPON_DAMAGE_RANGE
};

enum UnitMods
{
    UNIT_MOD_STAT_STRENGTH,                                 // UNIT_MOD_STAT_STRENGTH..UNIT_MOD_STAT_SPIRIT must be in existed order, it's accessed by index values of Stats enum.
    UNIT_MOD_STAT_AGILITY,
    UNIT_MOD_STAT_STAMINA,
    UNIT_MOD_STAT_INTELLECT,
    UNIT_MOD_STAT_SPIRIT,
    UNIT_MOD_HEALTH,
    UNIT_MOD_MANA,                                          // UNIT_MOD_MANA..UNIT_MOD_RUNIC_POWER must be in existed order, it's accessed by index values of Powers enum.
    UNIT_MOD_RAGE,
    UNIT_MOD_FOCUS,
    UNIT_MOD_ENERGY,
    UNIT_MOD_HAPPINESS,
    UNIT_MOD_RUNE,
    UNIT_MOD_RUNIC_POWER,
    UNIT_MOD_ARMOR,                                         // UNIT_MOD_ARMOR..UNIT_MOD_RESISTANCE_ARCANE must be in existed order, it's accessed by index values of SpellSchools enum.
    UNIT_MOD_RESISTANCE_HOLY,
    UNIT_MOD_RESISTANCE_FIRE,
    UNIT_MOD_RESISTANCE_NATURE,
    UNIT_MOD_RESISTANCE_FROST,
    UNIT_MOD_RESISTANCE_SHADOW,
    UNIT_MOD_RESISTANCE_ARCANE,
    UNIT_MOD_ATTACK_POWER,
    UNIT_MOD_ATTACK_POWER_RANGED,
    UNIT_MOD_DAMAGE_MAINHAND,
    UNIT_MOD_DAMAGE_OFFHAND,
    UNIT_MOD_DAMAGE_RANGED,
    UNIT_MOD_END,
    // synonyms
    UNIT_MOD_STAT_START = UNIT_MOD_STAT_STRENGTH,
    UNIT_MOD_STAT_END = UNIT_MOD_STAT_SPIRIT + 1,
    UNIT_MOD_RESISTANCE_START = UNIT_MOD_ARMOR,
    UNIT_MOD_RESISTANCE_END = UNIT_MOD_RESISTANCE_ARCANE + 1,
    UNIT_MOD_POWER_START = UNIT_MOD_MANA,
    UNIT_MOD_POWER_END = UNIT_MOD_RUNIC_POWER + 1
};

enum BaseModGroup
{
    CRIT_PERCENTAGE,
    RANGED_CRIT_PERCENTAGE,
    OFFHAND_CRIT_PERCENTAGE,
    SHIELD_BLOCK_VALUE,
    BASEMOD_END
};

enum BaseModType
{
    FLAT_MOD,
    PCT_MOD
};

#define MOD_END (PCT_MOD+1)

enum class DeathState : uint8
{
    Alive         = 0,
    JustDied      = 1,
    Corpse        = 2,
    Dead          = 3,
    JustRespawned = 4,
};

enum UnitState
{
    UNIT_STATE_DIED            = 0x00000001,                     // player has fake death aura
    UNIT_STATE_MELEE_ATTACKING = 0x00000002,                     // player is melee attacking someone
    //UNIT_STATE_MELEE_ATTACK_BY = 0x00000004,                     // player is melee attack by someone
    UNIT_STATE_STUNNED         = 0x00000008,
    UNIT_STATE_ROAMING         = 0x00000010,
    UNIT_STATE_CHASE           = 0x00000020,
    //UNIT_STATE_SEARCHING       = 0x00000040,
    UNIT_STATE_FLEEING         = 0x00000080,
    UNIT_STATE_IN_FLIGHT       = 0x00000100,                     // player is in flight mode
    UNIT_STATE_FOLLOW          = 0x00000200,
    UNIT_STATE_ROOT            = 0x00000400,
    UNIT_STATE_CONFUSED        = 0x00000800,
    UNIT_STATE_DISTRACTED      = 0x00001000,
    UNIT_STATE_ISOLATED        = 0x00002000,                     // area auras do not affect other players
    UNIT_STATE_ATTACK_PLAYER   = 0x00004000,
    UNIT_STATE_CASTING         = 0x00008000,
    UNIT_STATE_POSSESSED       = 0x00010000,
    UNIT_STATE_CHARGING        = 0x00020000,
    UNIT_STATE_JUMPING         = 0x00040000,
    UNIT_STATE_MOVE            = 0x00100000,
    UNIT_STATE_ROTATING        = 0x00200000,
    UNIT_STATE_EVADE           = 0x00400000,
    UNIT_STATE_ROAMING_MOVE    = 0x00800000,
    UNIT_STATE_CONFUSED_MOVE   = 0x01000000,
    UNIT_STATE_FLEEING_MOVE    = 0x02000000,
    UNIT_STATE_CHASE_MOVE      = 0x04000000,
    UNIT_STATE_FOLLOW_MOVE     = 0x08000000,
    UNIT_STATE_IGNORE_PATHFINDING = 0x10000000,                 // do not use pathfinding in any MovementGenerator
    UNIT_STATE_NO_ENVIRONMENT_UPD = 0x20000000, // pussywizard

    UNIT_STATE_ALL_STATE_SUPPORTED = UNIT_STATE_DIED | UNIT_STATE_MELEE_ATTACKING | UNIT_STATE_STUNNED | UNIT_STATE_ROAMING | UNIT_STATE_CHASE
                                     | UNIT_STATE_FLEEING | UNIT_STATE_IN_FLIGHT | UNIT_STATE_FOLLOW | UNIT_STATE_ROOT | UNIT_STATE_CONFUSED
                                     | UNIT_STATE_DISTRACTED | UNIT_STATE_ISOLATED | UNIT_STATE_ATTACK_PLAYER | UNIT_STATE_CASTING
                                     | UNIT_STATE_POSSESSED | UNIT_STATE_CHARGING | UNIT_STATE_JUMPING | UNIT_STATE_MOVE | UNIT_STATE_ROTATING
                                     | UNIT_STATE_EVADE | UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CONFUSED_MOVE | UNIT_STATE_FLEEING_MOVE
                                     | UNIT_STATE_CHASE_MOVE | UNIT_STATE_FOLLOW_MOVE | UNIT_STATE_IGNORE_PATHFINDING | UNIT_STATE_NO_ENVIRONMENT_UPD,

    UNIT_STATE_UNATTACKABLE         = UNIT_STATE_IN_FLIGHT,
    // for real move using movegen check and stop (except unstoppable flight)
    UNIT_STATE_MOVING               = UNIT_STATE_ROAMING_MOVE | UNIT_STATE_CONFUSED_MOVE | UNIT_STATE_FLEEING_MOVE | UNIT_STATE_CHASE_MOVE | UNIT_STATE_FOLLOW_MOVE,
    UNIT_STATE_CONTROLLED           = (UNIT_STATE_CONFUSED | UNIT_STATE_STUNNED | UNIT_STATE_FLEEING),
    UNIT_STATE_LOST_CONTROL         = (UNIT_STATE_CONTROLLED | UNIT_STATE_JUMPING | UNIT_STATE_CHARGING),
    UNIT_STATE_SIGHTLESS            = (UNIT_STATE_LOST_CONTROL | UNIT_STATE_EVADE),
    UNIT_STATE_CANNOT_AUTOATTACK    = (UNIT_STATE_LOST_CONTROL | UNIT_STATE_CASTING),
    UNIT_STATE_CANNOT_TURN          = (UNIT_STATE_LOST_CONTROL | UNIT_STATE_ROTATING | UNIT_STATE_ROOT),
    // stay by different reasons
    UNIT_STATE_NOT_MOVE             = UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DIED | UNIT_STATE_DISTRACTED,
    UNIT_STATE_IGNORE_ANTISPEEDHACK = UNIT_STATE_FLEEING | UNIT_STATE_CONFUSED | UNIT_STATE_CHARGING | UNIT_STATE_DISTRACTED | UNIT_STATE_POSSESSED,
    UNIT_STATE_ALL_STATE            = 0xffffffff                      //(UNIT_STATE_STOPPED | UNIT_STATE_MOVING | UNIT_STATE_IN_COMBAT | UNIT_STATE_IN_FLIGHT)
};

enum UnitMoveType
{
    MOVE_WALK           = 0,
    MOVE_RUN            = 1,
    MOVE_RUN_BACK       = 2,
    MOVE_SWIM           = 3,
    MOVE_SWIM_BACK      = 4,
    MOVE_TURN_RATE      = 5,
    MOVE_FLIGHT         = 6,
    MOVE_FLIGHT_BACK    = 7,
    MOVE_PITCH_RATE     = 8
};

#define MAX_MOVE_TYPE     9

extern float baseMoveSpeed[MAX_MOVE_TYPE];
extern float playerBaseMoveSpeed[MAX_MOVE_TYPE];

enum WeaponAttackType : uint8
{
    BASE_ATTACK   = 0,
    OFF_ATTACK    = 1,
    RANGED_ATTACK = 2,
    MAX_ATTACK
};

enum CombatRating : uint8
{
    CR_WEAPON_SKILL             = 0,
    CR_DEFENSE_SKILL            = 1,
    CR_DODGE                    = 2,
    CR_PARRY                    = 3,
    CR_BLOCK                    = 4,
    CR_HIT_MELEE                = 5,
    CR_HIT_RANGED               = 6,
    CR_HIT_SPELL                = 7,
    CR_CRIT_MELEE               = 8,
    CR_CRIT_RANGED              = 9,
    CR_CRIT_SPELL               = 10,
    CR_HIT_TAKEN_MELEE          = 11,
    CR_HIT_TAKEN_RANGED         = 12,
    CR_HIT_TAKEN_SPELL          = 13,
    CR_CRIT_TAKEN_MELEE         = 14,
    CR_CRIT_TAKEN_RANGED        = 15,
    CR_CRIT_TAKEN_SPELL         = 16,
    CR_HASTE_MELEE              = 17,
    CR_HASTE_RANGED             = 18,
    CR_HASTE_SPELL              = 19,
    CR_WEAPON_SKILL_MAINHAND    = 20,
    CR_WEAPON_SKILL_OFFHAND     = 21,
    CR_WEAPON_SKILL_RANGED      = 22,
    CR_EXPERTISE                = 23,
    CR_ARMOR_PENETRATION        = 24
};

#define MAX_COMBAT_RATING         25

enum DamageEffectType : uint8
{
    DIRECT_DAMAGE           = 0,                            // used for normal weapon damage (not for class abilities or spells)
    SPELL_DIRECT_DAMAGE     = 1,                            // spell/class abilities damage
    DOT                     = 2,
    HEAL                    = 3,
    NODAMAGE                = 4,                            // used also in case when damage applied to health but not applied to spell channelInterruptFlags/etc
    SELF_DAMAGE             = 5
};

// Used for IsClass hook
enum ClassContext : uint8
{
    CLASS_CONTEXT_NONE              = 0, // Default
    CLASS_CONTEXT_INIT              = 1,
    CLASS_CONTEXT_TELEPORT          = 2,
    CLASS_CONTEXT_QUEST             = 3,
    CLASS_CONTEXT_STATS             = 4,
    CLASS_CONTEXT_TAXI              = 5,
    CLASS_CONTEXT_SKILL             = 6,
    CLASS_CONTEXT_TALENT_POINT_CALC = 7,
    CLASS_CONTEXT_ABILITY           = 8,
    CLASS_CONTEXT_ABILITY_REACTIVE  = 9,
    CLASS_CONTEXT_PET               = 10,
    CLASS_CONTEXT_PET_CHARM         = 11,
    CLASS_CONTEXT_EQUIP_RELIC       = 12,
    CLASS_CONTEXT_EQUIP_SHIELDS     = 13,
    CLASS_CONTEXT_EQUIP_ARMOR_CLASS = 14,
    CLASS_CONTEXT_WEAPON_SWAP       = 15,
    CLASS_CONTEXT_GRAVEYARD         = 16,
    CLASS_CONTEXT_CLASS_TRAINER     = 17
};

// Value masks for UNIT_FIELD_FLAGS
// EnumUtils: DESCRIBE THIS
enum UnitFlags : uint32
{
    UNIT_FLAG_NONE                          = 0x00000000,
    UNIT_FLAG_SERVER_CONTROLLED             = 0x00000001,           // set only when unit movement is controlled by server - by SPLINE/MONSTER_MOVE packets, together with UNIT_FLAG_STUNNED; only set to units controlled by client; client function CGUnit_C::IsClientControlled returns false when set for owner
    UNIT_FLAG_NON_ATTACKABLE                = 0x00000002,           // not attackable
    UNIT_FLAG_DISABLE_MOVE                  = 0x00000004,
    UNIT_FLAG_PLAYER_CONTROLLED             = 0x00000008,           // controlled by player, use _IMMUNE_TO_PC instead of _IMMUNE_TO_NPC
    UNIT_FLAG_RENAME                        = 0x00000010,
    UNIT_FLAG_PREPARATION                   = 0x00000020,           // don't take reagents for spells with SPELL_ATTR5_NO_REAGENT_COST_WITH_AURA
    UNIT_FLAG_UNK_6                         = 0x00000040,
    UNIT_FLAG_NOT_ATTACKABLE_1              = 0x00000080,           // ?? (UNIT_FLAG_PLAYER_CONTROLLED | UNIT_FLAG_NOT_ATTACKABLE_1) is NON_PVP_ATTACKABLE
    UNIT_FLAG_IMMUNE_TO_PC                  = 0x00000100,           // disables combat/assistance with PlayerCharacters (PC) - see Unit::_IsValidAttackTarget, Unit::_IsValidAssistTarget
    UNIT_FLAG_IMMUNE_TO_NPC                 = 0x00000200,           // disables combat/assistance with NonPlayerCharacters (NPC) - see Unit::_IsValidAttackTarget, Unit::_IsValidAssistTarget
    UNIT_FLAG_LOOTING                       = 0x00000400,           // loot animation
    UNIT_FLAG_PET_IN_COMBAT                 = 0x00000800,           // in combat?, 2.0.8
    UNIT_FLAG_PVP                           = 0x00001000,           // changed in 3.0.3
    UNIT_FLAG_SILENCED                      = 0x00002000,           // silenced, 2.1.1
    UNIT_FLAG_CANNOT_SWIM                   = 0x00004000,           // 2.0.8
    UNIT_FLAG_SWIMMING                      = 0x00008000,           // shows swim animation in water
    UNIT_FLAG_NON_ATTACKABLE_2              = 0x00010000,           // removes attackable icon, if on yourself, cannot assist self but can cast TARGET_SELF spells - added by SPELL_AURA_MOD_UNATTACKABLE
    UNIT_FLAG_PACIFIED                      = 0x00020000,           // 3.0.3 ok
    UNIT_FLAG_STUNNED                       = 0x00040000,           // 3.0.3 ok
    UNIT_FLAG_IN_COMBAT                     = 0x00080000,
    UNIT_FLAG_TAXI_FLIGHT                   = 0x00100000,           // disable casting at client side spell not allowed by taxi flight (mounted?), probably used with 0x4 flag
    UNIT_FLAG_DISARMED                      = 0x00200000,           // 3.0.3, disable melee spells casting..., "Required melee weapon" added to melee spells tooltip.
    UNIT_FLAG_CONFUSED                      = 0x00400000,
    UNIT_FLAG_FLEEING                       = 0x00800000,
    UNIT_FLAG_POSSESSED                     = 0x01000000,           // under direct client control by a player (possess or vehicle)
    UNIT_FLAG_NOT_SELECTABLE                = 0x02000000,
    UNIT_FLAG_SKINNABLE                     = 0x04000000,
    UNIT_FLAG_MOUNT                         = 0x08000000,
    UNIT_FLAG_UNK_28                        = 0x10000000,
    UNIT_FLAG_PREVENT_EMOTES_FROM_CHAT_TEXT = 0x20000000,           // Prevent automatically playing emotes from parsing chat text, for example "lol" in /say, ending message with ? or !, or using /yell
    UNIT_FLAG_SHEATHE                       = 0x40000000,
    UNIT_FLAG_IMMUNE                        = 0x80000000,           // Immune to damage
};

DEFINE_ENUM_FLAG(UnitFlags);

// Value masks for UNIT_FIELD_FLAGS_2
enum UnitFlags2 : uint32
{
    UNIT_FLAG2_NONE                         = 0x00000000,
    UNIT_FLAG2_FEIGN_DEATH                  = 0x00000001,
    UNIT_FLAG2_HIDE_BODY                    = 0x00000002,   // Hide unit model (show only player equip)
    UNIT_FLAG2_IGNORE_REPUTATION            = 0x00000004,
    UNIT_FLAG2_COMPREHEND_LANG              = 0x00000008,
    UNIT_FLAG2_MIRROR_IMAGE                 = 0x00000010,
    UNIT_FLAG2_DO_NOT_FADE_IN               = 0x00000020,   // Unit model instantly appears when summoned (does not fade in)
    UNIT_FLAG2_FORCE_MOVEMENT               = 0x00000040,
    UNIT_FLAG2_DISARM_OFFHAND               = 0x00000080,
    UNIT_FLAG2_DISABLE_PRED_STATS           = 0x00000100,   // Player has disabled predicted stats (Used by raid frames)
    UNIT_FLAG2_DISARM_RANGED                = 0x00000400,   // this does not disable ranged weapon display (maybe additional flag needed?)
    UNIT_FLAG2_REGENERATE_POWER             = 0x00000800,
    UNIT_FLAG2_RESTRICT_PARTY_INTERACTION   = 0x00001000,   // Restrict interaction to party or raid
    UNIT_FLAG2_PREVENT_SPELL_CLICK          = 0x00002000,   // Prevent spellclick
    UNIT_FLAG2_ALLOW_ENEMY_INTERACT         = 0x00004000,
    UNIT_FLAG2_CANNOT_TURN                  = 0x00008000,
    UNIT_FLAG2_UNK2                         = 0x00010000,
    UNIT_FLAG2_PLAY_DEATH_ANIM              = 0x00020000,   // Plays special death animation upon death
    UNIT_FLAG2_ALLOW_CHEAT_SPELLS           = 0x00040000,   // Allows casting spells with AttributesEx7 & SPELL_ATTR7_DEBUG_SPELL
    UNIT_FLAG2_UNUSED_6                     = 0x01000000,
};

DEFINE_ENUM_FLAG(UnitFlags2);

/// Non Player Character flags
// EnumUtils: DESCRIBE THIS
enum NPCFlags : uint32
{
    UNIT_NPC_FLAG_NONE                  = 0x00000000,       // SKIP
    UNIT_NPC_FLAG_GOSSIP                = 0x00000001,       // TITLE has gossip menu DESCRIPTION 100%
    UNIT_NPC_FLAG_QUESTGIVER            = 0x00000002,       // TITLE is quest giver DESCRIPTION guessed, probably ok
    UNIT_NPC_FLAG_UNK1                  = 0x00000004,
    UNIT_NPC_FLAG_UNK2                  = 0x00000008,
    UNIT_NPC_FLAG_TRAINER               = 0x00000010,       // TITLE is trainer DESCRIPTION 100%
    UNIT_NPC_FLAG_TRAINER_CLASS         = 0x00000020,       // TITLE is class trainer DESCRIPTION 100%
    UNIT_NPC_FLAG_TRAINER_PROFESSION    = 0x00000040,       // TITLE is profession trainer DESCRIPTION 100%
    UNIT_NPC_FLAG_VENDOR                = 0x00000080,       // TITLE is vendor (generic) DESCRIPTION 100%
    UNIT_NPC_FLAG_VENDOR_AMMO           = 0x00000100,       // TITLE is vendor (ammo) DESCRIPTION 100%, general goods vendor
    UNIT_NPC_FLAG_VENDOR_FOOD           = 0x00000200,       // TITLE is vendor (food) DESCRIPTION 100%
    UNIT_NPC_FLAG_VENDOR_POISON         = 0x00000400,       // TITLE is vendor (poison) DESCRIPTION guessed
    UNIT_NPC_FLAG_VENDOR_REAGENT        = 0x00000800,       // TITLE is vendor (reagents) DESCRIPTION 100%
    UNIT_NPC_FLAG_REPAIR                = 0x00001000,       // TITLE can repair DESCRIPTION 100%
    UNIT_NPC_FLAG_FLIGHTMASTER          = 0x00002000,       // TITLE is flight master DESCRIPTION 100%
    UNIT_NPC_FLAG_SPIRITHEALER          = 0x00004000,       // TITLE is spirit healer DESCRIPTION guessed
    UNIT_NPC_FLAG_SPIRITGUIDE           = 0x00008000,       // TITLE is spirit guide DESCRIPTION guessed
    UNIT_NPC_FLAG_INNKEEPER             = 0x00010000,       // TITLE is innkeeper
    UNIT_NPC_FLAG_BANKER                = 0x00020000,       // TITLE is banker DESCRIPTION 100%
    UNIT_NPC_FLAG_PETITIONER            = 0x00040000,       // TITLE handles guild/arena petitions DESCRIPTION 100% 0xC0000 = guild petitions, 0x40000 = arena team petitions
    UNIT_NPC_FLAG_TABARDDESIGNER        = 0x00080000,       // TITLE is guild tabard designer DESCRIPTION 100%
    UNIT_NPC_FLAG_BATTLEMASTER          = 0x00100000,       // TITLE is battlemaster DESCRIPTION 100%
    UNIT_NPC_FLAG_AUCTIONEER            = 0x00200000,       // TITLE is auctioneer DESCRIPTION 100%
    UNIT_NPC_FLAG_STABLEMASTER          = 0x00400000,       // TITLE is stable master DESCRIPTION 100%
    UNIT_NPC_FLAG_GUILD_BANKER          = 0x00800000,       // TITLE is guild banker DESCRIPTION cause client to send 997 opcode
    UNIT_NPC_FLAG_SPELLCLICK            = 0x01000000,       // TITLE has spell click enabled DESCRIPTION cause client to send 1015 opcode (spell click)
    UNIT_NPC_FLAG_PLAYER_VEHICLE        = 0x02000000,       // TITLE is player vehicle DESCRIPTION players with mounts that have vehicle data should have it set
    UNIT_NPC_FLAG_MAILBOX               = 0x04000000,       // TITLE is mailbox

    UNIT_NPC_FLAG_VENDOR_MASK           = UNIT_NPC_FLAG_VENDOR | UNIT_NPC_FLAG_VENDOR_AMMO | UNIT_NPC_FLAG_VENDOR_POISON | UNIT_NPC_FLAG_VENDOR_REAGENT
};

DEFINE_ENUM_FLAG(NPCFlags);

enum MovementFlags
{
    MOVEMENTFLAG_NONE                  = 0x00000000,
    MOVEMENTFLAG_FORWARD               = 0x00000001,
    MOVEMENTFLAG_BACKWARD              = 0x00000002,
    MOVEMENTFLAG_STRAFE_LEFT           = 0x00000004,
    MOVEMENTFLAG_STRAFE_RIGHT          = 0x00000008,
    MOVEMENTFLAG_LEFT                  = 0x00000010,
    MOVEMENTFLAG_RIGHT                 = 0x00000020,
    MOVEMENTFLAG_PITCH_UP              = 0x00000040,
    MOVEMENTFLAG_PITCH_DOWN            = 0x00000080,
    MOVEMENTFLAG_WALKING               = 0x00000100,               // Walking
    MOVEMENTFLAG_ONTRANSPORT           = 0x00000200,               // Used for flying on some creatures
    MOVEMENTFLAG_DISABLE_GRAVITY       = 0x00000400,               // Former MOVEMENTFLAG_LEVITATING. This is used when walking is not possible.
    MOVEMENTFLAG_ROOT                  = 0x00000800,               // Must not be set along with MOVEMENTFLAG_MASK_MOVING
    MOVEMENTFLAG_FALLING               = 0x00001000,               // damage dealt on that type of falling
    MOVEMENTFLAG_FALLING_FAR           = 0x00002000,
    MOVEMENTFLAG_PENDING_STOP          = 0x00004000,
    MOVEMENTFLAG_PENDING_STRAFE_STOP   = 0x00008000,
    MOVEMENTFLAG_PENDING_FORWARD       = 0x00010000,
    MOVEMENTFLAG_PENDING_BACKWARD      = 0x00020000,
    MOVEMENTFLAG_PENDING_STRAFE_LEFT   = 0x00040000,
    MOVEMENTFLAG_PENDING_STRAFE_RIGHT  = 0x00080000,
    MOVEMENTFLAG_PENDING_ROOT          = 0x00100000,
    MOVEMENTFLAG_SWIMMING              = 0x00200000,               // appears with fly flag also
    MOVEMENTFLAG_ASCENDING             = 0x00400000,               // press "space" when flying
    MOVEMENTFLAG_DESCENDING            = 0x00800000,
    MOVEMENTFLAG_CAN_FLY               = 0x01000000,               // Appears when unit can fly AND also walk
    MOVEMENTFLAG_FLYING                = 0x02000000,               // unit is actually flying. pretty sure this is only used for players. creatures use disable_gravity
    MOVEMENTFLAG_SPLINE_ELEVATION      = 0x04000000,               // used for flight paths
    MOVEMENTFLAG_SPLINE_ENABLED        = 0x08000000,               // used for flight paths
    MOVEMENTFLAG_WATERWALKING          = 0x10000000,               // prevent unit from falling through water
    MOVEMENTFLAG_FALLING_SLOW          = 0x20000000,               // active rogue safe fall spell (passive)
    MOVEMENTFLAG_HOVER                 = 0x40000000,               // hover, cannot jump

    /// @todo: Check if PITCH_UP and PITCH_DOWN really belong here..
    MOVEMENTFLAG_MASK_MOVING =
        MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT |
        MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING |
        MOVEMENTFLAG_SPLINE_ELEVATION,

    MOVEMENTFLAG_MASK_TURNING =
        MOVEMENTFLAG_LEFT | MOVEMENTFLAG_RIGHT,

    MOVEMENTFLAG_MASK_MOVING_FLY =
        MOVEMENTFLAG_FLYING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING,

    /// @todo if needed: add more flags to this masks that are exclusive to players
    MOVEMENTFLAG_MASK_PLAYER_ONLY =
        MOVEMENTFLAG_FLYING,

    /// Movement flags that have change status opcodes associated for players
    MOVEMENTFLAG_MASK_HAS_PLAYER_STATUS_OPCODE = MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_ROOT |
            MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_WATERWALKING | MOVEMENTFLAG_FALLING_SLOW | MOVEMENTFLAG_HOVER
};

enum MovementFlags2
{
    MOVEMENTFLAG2_NONE                     = 0x00000000,
    MOVEMENTFLAG2_NO_STRAFE                = 0x00000001,
    MOVEMENTFLAG2_NO_JUMPING               = 0x00000002,
    MOVEMENTFLAG2_UNK3                     = 0x00000004,        // Overrides various clientside checks
    MOVEMENTFLAG2_FULL_SPEED_TURNING       = 0x00000008,
    MOVEMENTFLAG2_FULL_SPEED_PITCHING      = 0x00000010,
    MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING    = 0x00000020,
    MOVEMENTFLAG2_UNK7                     = 0x00000040,
    MOVEMENTFLAG2_UNK8                     = 0x00000080,
    MOVEMENTFLAG2_UNK9                     = 0x00000100,
    MOVEMENTFLAG2_UNK10                    = 0x00000200,
    MOVEMENTFLAG2_INTERPOLATED_MOVEMENT    = 0x00000400,
    MOVEMENTFLAG2_INTERPOLATED_TURNING     = 0x00000800,
    MOVEMENTFLAG2_INTERPOLATED_PITCHING    = 0x00001000,
    MOVEMENTFLAG2_UNK14                    = 0x00002000,
    MOVEMENTFLAG2_UNK15                    = 0x00004000,
    MOVEMENTFLAG2_UNK16                    = 0x00008000,
};

enum SplineFlags
{
    SPLINEFLAG_NONE                 = 0x00000000,
    SPLINEFLAG_FORWARD              = 0x00000001,
    SPLINEFLAG_BACKWARD             = 0x00000002,
    SPLINEFLAG_STRAFE_LEFT          = 0x00000004,
    SPLINEFLAG_STRAFE_RIGHT         = 0x00000008,
    SPLINEFLAG_TURN_LEFT            = 0x00000010,
    SPLINEFLAG_TURN_RIGHT           = 0x00000020,
    SPLINEFLAG_PITCH_UP             = 0x00000040,
    SPLINEFLAG_PITCH_DOWN           = 0x00000080,
    SPLINEFLAG_DONE                 = 0x00000100,
    SPLINEFLAG_FALLING              = 0x00000200,
    SPLINEFLAG_NO_SPLINE            = 0x00000400,
    SPLINEFLAG_TRAJECTORY           = 0x00000800,
    SPLINEFLAG_WALK_MODE            = 0x00001000,
    SPLINEFLAG_FLYING               = 0x00002000,
    SPLINEFLAG_KNOCKBACK            = 0x00004000,
    SPLINEFLAG_FINAL_POINT          = 0x00008000,
    SPLINEFLAG_FINAL_TARGET         = 0x00010000,
    SPLINEFLAG_FINAL_FACING         = 0x00020000,
    SPLINEFLAG_CATMULL_ROM          = 0x00040000,
    SPLINEFLAG_CYCLIC               = 0x00080000,
    SPLINEFLAG_ENTER_CYCLE          = 0x00100000,
    SPLINEFLAG_ANIMATION_TIER       = 0x00200000,
    SPLINEFLAG_FROZEN               = 0x00400000,
    SPLINEFLAG_TRANSPORT            = 0x00800000,
    SPLINEFLAG_TRANSPORT_EXIT       = 0x01000000,
    SPLINEFLAG_UNKNOWN7             = 0x02000000,
    SPLINEFLAG_UNKNOWN8             = 0x04000000,
    SPLINEFLAG_ORIENTATION_INVERTED = 0x08000000,
    SPLINEFLAG_USE_PATH_SMOOTHING   = 0x10000000,
    SPLINEFLAG_ANIMATION            = 0x20000000,
    SPLINEFLAG_UNCOMPRESSED_PATH    = 0x40000000,
    SPLINEFLAG_UNKNOWN10            = 0x80000000,
};

enum SplineType
{
    SPLINETYPE_NORMAL               = 0,
    SPLINETYPE_STOP                 = 1,
    SPLINETYPE_FACING_SPOT          = 2,
    SPLINETYPE_FACING_TARGET        = 3,
    SPLINETYPE_FACING_ANGLE         = 4,
};

enum UnitTypeMask
{
    UNIT_MASK_NONE                  = 0x00000000,
    UNIT_MASK_SUMMON                = 0x00000001,
    UNIT_MASK_MINION                = 0x00000002,
    UNIT_MASK_GUARDIAN              = 0x00000004,
    UNIT_MASK_TOTEM                 = 0x00000008,
    UNIT_MASK_PET                   = 0x00000010,
    UNIT_MASK_VEHICLE               = 0x00000020,
    UNIT_MASK_PUPPET                = 0x00000040,
    UNIT_MASK_HUNTER_PET            = 0x00000080,
    UNIT_MASK_CONTROLABLE_GUARDIAN  = 0x00000100,
    UNIT_MASK_ACCESSORY             = 0x00000200,
};

namespace Movement
{
    class MoveSpline;
}

enum DiminishingLevels
{
    DIMINISHING_LEVEL_1             = 0,
    DIMINISHING_LEVEL_2             = 1,
    DIMINISHING_LEVEL_3             = 2,
    DIMINISHING_LEVEL_IMMUNE        = 3,
    DIMINISHING_LEVEL_4             = 3,
    DIMINISHING_LEVEL_TAUNT_IMMUNE  = 4,
};

struct DiminishingReturn
{
    DiminishingReturn(DiminishingGroup group, uint32 t, uint32 count)
        : DRGroup(group), stack(0), hitTime(t), hitCount(count)
    {}

    DiminishingGroup        DRGroup: 16;
    uint16                  stack: 16;
    uint32                  hitTime;
    uint32                  hitCount;
};

enum MeleeHitOutcome : uint8
{
    MELEE_HIT_EVADE, MELEE_HIT_MISS, MELEE_HIT_DODGE, MELEE_HIT_BLOCK, MELEE_HIT_PARRY,
    MELEE_HIT_GLANCING, MELEE_HIT_CRIT, MELEE_HIT_CRUSHING, MELEE_HIT_NORMAL
};

enum ExtraAttackSpells
{
    SPELL_SWORD_SPECIALIZATION   = 16459,
    SPELL_HACK_AND_SLASH         = 66923
};

class DispelInfo
{
public:
    explicit DispelInfo(Unit* dispeller, uint32 dispellerSpellId, uint8 chargesRemoved) :
        _dispellerUnit(dispeller), _dispellerSpell(dispellerSpellId), _chargesRemoved(chargesRemoved) {}

    [[nodiscard]] Unit* GetDispeller() const { return _dispellerUnit; }
    [[nodiscard]] uint32 GetDispellerSpellId() const { return _dispellerSpell; }
    [[nodiscard]] uint8 GetRemovedCharges() const { return _chargesRemoved; }
    void SetRemovedCharges(uint8 amount)
    {
        _chargesRemoved = amount;
    }
private:
    Unit* _dispellerUnit;
    uint32 _dispellerSpell;
    uint8 _chargesRemoved;
};

struct CleanDamage
{
    CleanDamage(uint32 mitigated, uint32 absorbed, WeaponAttackType _attackType, MeleeHitOutcome _hitOutCome) :
        absorbed_damage(absorbed), mitigated_damage(mitigated), attackType(_attackType), hitOutCome(_hitOutCome) {}

    uint32 absorbed_damage;
    uint32 mitigated_damage;

    WeaponAttackType attackType;
    MeleeHitOutcome hitOutCome;
};

struct CalcDamageInfo;
struct SpellNonMeleeDamage;

class DamageInfo
{
private:
    Unit* const m_attacker;
    Unit* const m_victim;
    uint32 m_damage;
    SpellInfo const* const m_spellInfo;
    SpellSchoolMask const m_schoolMask;
    DamageEffectType const m_damageType;
    WeaponAttackType m_attackType;
    uint32 m_absorb;
    uint32 m_resist;
    uint32 m_block;
    uint32 m_cleanDamage;

    // amalgamation constructor (used for proc)
    DamageInfo(DamageInfo const& dmg1, DamageInfo const& dmg2);

    //npcbot
    uint32 m_procEx = 0;
    //end npcbot

public:
    explicit DamageInfo(Unit* _attacker, Unit* _victim, uint32 _damage, SpellInfo const* _spellInfo, SpellSchoolMask _schoolMask, DamageEffectType _damageType, uint32 cleanDamage = 0);
    explicit DamageInfo(CalcDamageInfo const& dmgInfo); // amalgamation wrapper
    DamageInfo(CalcDamageInfo const& dmgInfo, uint8 damageIndex);
    DamageInfo(SpellNonMeleeDamage const& spellNonMeleeDamage, DamageEffectType damageType);

    void ModifyDamage(int32 amount);
    void AbsorbDamage(uint32 amount);
    void ResistDamage(uint32 amount);
    void BlockDamage(uint32 amount);

    [[nodiscard]] Unit* GetAttacker() const { return m_attacker; };
    [[nodiscard]] Unit* GetVictim() const { return m_victim; };
    [[nodiscard]] SpellInfo const* GetSpellInfo() const { return m_spellInfo; };
    [[nodiscard]] SpellSchoolMask GetSchoolMask() const { return m_schoolMask; };
    [[nodiscard]] DamageEffectType GetDamageType() const { return m_damageType; };
    [[nodiscard]] WeaponAttackType GetAttackType() const { return m_attackType; };
    [[nodiscard]] uint32 GetDamage() const { return m_damage; };
    [[nodiscard]] uint32 GetAbsorb() const { return m_absorb; };
    [[nodiscard]] uint32 GetResist() const { return m_resist; };
    [[nodiscard]] uint32 GetBlock() const { return m_block; };

    [[nodiscard]] uint32 GetUnmitigatedDamage() const;

    //npcbot
    [[nodiscard]] uint32 GetHitMask() const { return m_procEx; }
    //end npcbot
};

class HealInfo
{
private:
    Unit* const m_healer;
    Unit* const m_target;
    uint32 m_heal;
    uint32 m_effectiveHeal;
    uint32 m_absorb;
    SpellInfo const* const m_spellInfo;
    SpellSchoolMask const m_schoolMask;
public:
    explicit HealInfo(Unit* _healer, Unit* _target, uint32 _heal, SpellInfo const* _spellInfo, SpellSchoolMask _schoolMask)
        : m_healer(_healer), m_target(_target), m_heal(_heal), m_spellInfo(_spellInfo), m_schoolMask(_schoolMask)
    {
        m_absorb = 0;
        m_effectiveHeal = 0;
    }

    void AbsorbHeal(uint32 amount)
    {
        amount = std::min(amount, GetHeal());
        m_absorb += amount;
        m_heal -= amount;

        amount = std::min(amount, GetEffectiveHeal());
        m_effectiveHeal -= amount;
    }

    void SetHeal(uint32 amount)
    {
        m_heal = amount;
    }

    void SetEffectiveHeal(uint32 amount)
    {
        m_effectiveHeal = amount;
    }

    [[nodiscard]] Unit* GetHealer() const { return m_healer; }
    [[nodiscard]] Unit* GetTarget() const { return m_target; }
    [[nodiscard]] uint32 GetHeal() const { return m_heal; }
    [[nodiscard]] uint32 GetEffectiveHeal() const { return m_effectiveHeal; }
    [[nodiscard]] uint32 GetAbsorb() const { return m_absorb; }
    [[nodiscard]] SpellInfo const* GetSpellInfo() const { return m_spellInfo; };
    [[nodiscard]] SpellSchoolMask GetSchoolMask() const { return m_schoolMask; };
};

class ProcEventInfo
{
private:
    Unit* const _actor;
    Unit* const _actionTarget;
    Unit* const _procTarget;
    uint32 _typeMask;
    uint32 _spellTypeMask;
    uint32 _spellPhaseMask;
    uint32 _hitMask;
    uint32 _cooldown;
    Spell const* _spell;
    DamageInfo* _damageInfo;
    HealInfo* _healInfo;
    SpellInfo const* const _triggeredByAuraSpell;
    int8 _procAuraEffectIndex;
    std::optional<float> _chance;

public:
    explicit ProcEventInfo(Unit* actor, Unit* actionTarget, Unit* procTarget, uint32 typeMask, uint32 spellTypeMask, uint32 spellPhaseMask, uint32 hitMask, Spell const* spell, DamageInfo* damageInfo, HealInfo* healInfo, SpellInfo const* triggeredByAuraSpell = nullptr, int8 procAuraEffectIndex = -1);
    Unit* GetActor() { return _actor; };
    [[nodiscard]] Unit* GetActionTarget() const { return _actionTarget; }
    [[nodiscard]] Unit* GetProcTarget() const { return _procTarget; }
    [[nodiscard]] uint32 GetTypeMask() const { return _typeMask; }
    [[nodiscard]] uint32 GetSpellTypeMask() const { return _spellTypeMask; }
    [[nodiscard]] uint32 GetSpellPhaseMask() const { return _spellPhaseMask; }
    [[nodiscard]] uint32 GetHitMask() const { return _hitMask; }
    [[nodiscard]] SpellInfo const* GetSpellInfo() const;
    [[nodiscard]] SpellSchoolMask GetSchoolMask() const { return SPELL_SCHOOL_MASK_NONE; }
    [[nodiscard]] Spell const* GetProcSpell() const { return _spell; }
    [[nodiscard]] DamageInfo* GetDamageInfo() const { return _damageInfo; }
    [[nodiscard]] HealInfo* GetHealInfo() const { return _healInfo; }
    [[nodiscard]] SpellInfo const* GetTriggerAuraSpell() const { return _triggeredByAuraSpell; }
    [[nodiscard]] int8 GetTriggerAuraEffectIndex() const { return _procAuraEffectIndex; }
    [[nodiscard]] uint32 GetProcCooldown() const { return _cooldown; }
    void SetProcCooldown(uint32 cooldown) { _cooldown = cooldown; }
    [[nodiscard]] std::optional<float> GetProcChance() const { return _chance; }
    void SetProcChance(float chance) { _chance = chance; }
    void ResetProcChance() { _chance.reset(); }
};

// Struct for use in Unit::CalculateMeleeDamage
// Need create structure like in SMSG_ATTACKERSTATEUPDATE opcode
struct CalcDamageInfo
{
    Unit*  attacker;             // Attacker
    Unit*  target;               // Target for damage

    struct
    {
        uint32 damageSchoolMask;
        uint32 damage;
        uint32 absorb;
        uint32 resist;
    } damages[MAX_ITEM_PROTO_DAMAGES];

    uint32 blocked_amount;
    uint32 HitInfo;
    uint32 TargetState;
    // Helper
    WeaponAttackType attackType; //
    uint32 procAttacker;
    uint32 procVictim;
    uint32 procEx;
    uint32 cleanDamage;          // Used only for rage calculation
    MeleeHitOutcome hitOutCome;  /// @todo: remove this field (need use TargetState)
};

// Spell damage info structure based on structure sending in SMSG_SPELLNONMELEEDAMAGELOG opcode
struct SpellNonMeleeDamage
{
    SpellNonMeleeDamage(Unit* _attacker, Unit* _target, SpellInfo const* _spellInfo, uint32 _schoolMask)
        : target(_target), attacker(_attacker), spellInfo(_spellInfo), damage(0), overkill(0), schoolMask(_schoolMask),
          absorb(0), resist(0), physicalLog(false), unused(false), blocked(0), HitInfo(0), cleanDamage(0)
    {}

    Unit* target;
    Unit* attacker;
    SpellInfo const* spellInfo;
    uint32 damage;
    uint32 overkill;
    uint32 schoolMask;
    uint32 absorb;
    uint32 resist;
    bool physicalLog;
    bool unused;
    uint32 blocked;
    uint32 HitInfo;
    // Used for help
    uint32 cleanDamage;
};

struct SpellPeriodicAuraLogInfo
{
    SpellPeriodicAuraLogInfo(AuraEffect const* _auraEff, uint32 _damage, uint32 _overDamage, uint32 _absorb, uint32 _resist, float _multiplier, bool _critical)
        : auraEff(_auraEff), damage(_damage), overDamage(_overDamage), absorb(_absorb), resist(_resist), multiplier(_multiplier), critical(_critical) {}

    AuraEffect const* auraEff;
    uint32 damage;
    uint32 overDamage;                                      // overkill/overheal
    uint32 absorb;
    uint32 resist;
    float  multiplier;
    bool   critical;
};

void createProcFlags(SpellInfo const* spellInfo, WeaponAttackType attackType, bool positive, uint32& procAttacker, uint32& procVictim);
uint32 createProcExtendMask(SpellNonMeleeDamage* damageInfo, SpellMissInfo missCondition);

struct RedirectThreatInfo
{
    RedirectThreatInfo()  = default;
    ObjectGuid _targetGUID;
    uint32 _threatPct{0};

    [[nodiscard]] ObjectGuid GetTargetGUID() const { return _targetGUID; }
    [[nodiscard]] uint32 GetThreatPct() const { return _threatPct; }

    void Set(ObjectGuid guid, uint32 pct)
    {
        _targetGUID = guid;
        _threatPct = pct;
    }

    void ModifyThreatPct(int32 amount)
    {
        amount += _threatPct;
        _threatPct = uint32(std::max(0, amount));
    }
};

#define MAX_DECLINED_NAME_CASES 5

struct DeclinedName
{
    std::string name[MAX_DECLINED_NAME_CASES];
};

enum CurrentSpellTypes : uint8
{
    CURRENT_MELEE_SPELL             = 0,
    CURRENT_GENERIC_SPELL           = 1,
    CURRENT_CHANNELED_SPELL         = 2,
    CURRENT_AUTOREPEAT_SPELL        = 3
};

#define CURRENT_FIRST_NON_MELEE_SPELL 1
#define CURRENT_MAX_SPELL             4

struct GlobalCooldown
{
    explicit GlobalCooldown(uint32 _dur = 0, uint32 _time = 0) : duration(_dur), cast_time(_time) {}

    uint32 duration;
    uint32 cast_time;
};

typedef std::unordered_map<uint32 /*category*/, GlobalCooldown> GlobalCooldownList;

class GlobalCooldownMgr                                     // Shared by Player and CharmInfo
{
public:
    GlobalCooldownMgr() = default;

public:
    bool HasGlobalCooldown(SpellInfo const* spellInfo) const;
    void AddGlobalCooldown(SpellInfo const* spellInfo, uint32 gcd);
    void CancelGlobalCooldown(SpellInfo const* spellInfo);

private:
    GlobalCooldownList m_GlobalCooldowns;
};

enum ActiveStates : uint8
{
    ACT_PASSIVE  = 0x01,                                    // 0x01 - passive
    ACT_DISABLED = 0x81,                                    // 0x80 - castable
    ACT_ENABLED  = 0xC1,                                    // 0x40 | 0x80 - auto cast + castable
    ACT_COMMAND  = 0x07,                                    // 0x01 | 0x02 | 0x04
    ACT_REACTION = 0x06,                                    // 0x02 | 0x04
    ACT_DECIDE   = 0x00                                     // custom
};

enum ReactStates : uint8
{
    REACT_PASSIVE    = 0,
    REACT_DEFENSIVE  = 1,
    REACT_AGGRESSIVE = 2
};

enum CommandStates
{
    COMMAND_STAY    = 0,
    COMMAND_FOLLOW  = 1,
    COMMAND_ATTACK  = 2,
    COMMAND_ABANDON = 3
};

#define UNIT_ACTION_BUTTON_ACTION(X) (uint32(X) & 0x00FFFFFF)
#define UNIT_ACTION_BUTTON_TYPE(X)   ((uint32(X) & 0xFF000000) >> 24)
#define MAKE_UNIT_ACTION_BUTTON(A, T) (uint32(A) | (uint32(T) << 24))

struct UnitActionBarEntry
{
    UnitActionBarEntry() : packedData(uint32(ACT_DISABLED) << 24) {}

    uint32 packedData;

    // helper
    [[nodiscard]] ActiveStates GetType() const { return ActiveStates(UNIT_ACTION_BUTTON_TYPE(packedData)); }
    [[nodiscard]] uint32 GetAction() const { return UNIT_ACTION_BUTTON_ACTION(packedData); }
    [[nodiscard]] bool IsActionBarForSpell() const
    {
        ActiveStates Type = GetType();
        return Type == ACT_DISABLED || Type == ACT_ENABLED || Type == ACT_PASSIVE;
    }

    void SetActionAndType(uint32 action, ActiveStates type)
    {
        packedData = MAKE_UNIT_ACTION_BUTTON(action, type);
    }

    void SetType(ActiveStates type)
    {
        packedData = MAKE_UNIT_ACTION_BUTTON(UNIT_ACTION_BUTTON_ACTION(packedData), type);
    }

    void SetAction(uint32 action)
    {
        packedData = (packedData & 0xFF000000) | UNIT_ACTION_BUTTON_ACTION(action);
    }
};

typedef std::list<Player*> SharedVisionList;

enum CharmType
{
    CHARM_TYPE_CHARM,
    CHARM_TYPE_POSSESS,
    CHARM_TYPE_VEHICLE,
    CHARM_TYPE_CONVERT,
};

typedef UnitActionBarEntry CharmSpellInfo;

enum ActionBarIndex
{
    ACTION_BAR_INDEX_START = 0,
    ACTION_BAR_INDEX_PET_SPELL_START = 3,
    ACTION_BAR_INDEX_PET_SPELL_END = 7,
    ACTION_BAR_INDEX_END = 10,
};

#define MAX_UNIT_ACTION_BAR_INDEX (ACTION_BAR_INDEX_END-ACTION_BAR_INDEX_START)

struct CharmInfo
{
public:
    explicit CharmInfo(Unit* unit);
    ~CharmInfo();
    void RestoreState();
    [[nodiscard]] uint32 GetPetNumber() const { return _petnumber; }
    void SetPetNumber(uint32 petnumber, bool statwindow);

    void SetCommandState(CommandStates st) { _CommandState = st; }
    [[nodiscard]] CommandStates GetCommandState() const { return _CommandState; }
    [[nodiscard]] bool HasCommandState(CommandStates state) const { return (_CommandState == state); }

    void InitPossessCreateSpells();
    void InitCharmCreateSpells();
    void InitPetActionBar();
    void InitEmptyActionBar(bool withAttack = true);

    //return true if successful
    bool AddSpellToActionBar(SpellInfo const* spellInfo, ActiveStates newstate = ACT_DECIDE);
    bool RemoveSpellFromActionBar(uint32 spell_id);
    void LoadPetActionBar(const std::string& data);
    void BuildActionBar(WorldPacket* data);
    void SetSpellAutocast(SpellInfo const* spellInfo, bool state);
    void SetActionBar(uint8 index, uint32 spellOrAction, ActiveStates type)
    {
        PetActionBar[index].SetActionAndType(spellOrAction, type);
    }
    [[nodiscard]] UnitActionBarEntry const* GetActionBarEntry(uint8 index) const { return &(PetActionBar[index]); }

    void ToggleCreatureAutocast(SpellInfo const* spellInfo, bool apply);

    CharmSpellInfo* GetCharmSpell(uint8 index) { return &(_charmspells[index]); }

    GlobalCooldownMgr& GetGlobalCooldownMgr() { return _GlobalCooldownMgr; }

    void SetIsCommandAttack(bool val);
    bool IsCommandAttack();
    void SetIsCommandFollow(bool val);
    bool IsCommandFollow();
    void SetIsAtStay(bool val);
    bool IsAtStay();
    void SetIsFollowing(bool val);
    bool IsFollowing();
    void SetIsReturning(bool val);
    bool IsReturning();
    void SaveStayPosition(bool atCurrentPos);
    void GetStayPosition(float& x, float& y, float& z);
    void RemoveStayPosition();
    bool HasStayPosition();

    void SetForcedSpell(uint32 id) { _forcedSpellId = id; }
    int32 GetForcedSpell() { return _forcedSpellId; }
    void SetForcedTargetGUID(ObjectGuid guid = ObjectGuid::Empty) { _forcedTargetGUID = guid; }
    ObjectGuid GetForcedTarget() { return _forcedTargetGUID; }

    // Player react states
    void SetPlayerReactState(ReactStates s) { _oldReactState = s; }
    [[nodiscard]] ReactStates GetPlayerReactState() const { return _oldReactState; }

private:
    Unit* _unit;
    UnitActionBarEntry PetActionBar[MAX_UNIT_ACTION_BAR_INDEX];
    CharmSpellInfo _charmspells[4];
    CommandStates _CommandState;
    uint32 _petnumber;

    //for restoration after charmed
    ReactStates     _oldReactState;

    bool _isCommandAttack;
    bool _isCommandFollow;
    bool _isAtStay;
    bool _isFollowing;
    bool _isReturning;
    int32 _forcedSpellId;
    ObjectGuid _forcedTargetGUID;
    float _stayX;
    float _stayY;
    float _stayZ;

    GlobalCooldownMgr _GlobalCooldownMgr;
};

struct AttackPosition {
    AttackPosition(Position pos) : _pos(std::move(pos)), _taken(false) {}
    bool operator==(const int val)
    {
        return !val;
    };
    int operator=(const int val)
    {
        if (!val)
        {
            // _pos = nullptr;
            _taken = false;
            return 0; // nullptr
        }
        return 0; // nullptr
    };
    Position _pos;
    bool _taken;
};

// for clearing special attacks
#define REACTIVE_TIMER_START 5000

enum ReactiveType
{
    REACTIVE_DEFENSE        = 0,
    REACTIVE_HUNTER_PARRY   = 1,
    REACTIVE_OVERPOWER      = 2,
    REACTIVE_WOLVERINE_BITE = 3,

    MAX_REACTIVE
};

#define SUMMON_SLOT_PET     0
#define SUMMON_SLOT_TOTEM   1
#define MAX_TOTEM_SLOT      5
#define SUMMON_SLOT_MINIPET 5
#define SUMMON_SLOT_QUEST   6
#define MAX_SUMMON_SLOT     7

#define MAX_GAMEOBJECT_SLOT 4

enum PlayerTotemType
{
    SUMMON_TYPE_TOTEM_FIRE  = 63,
    SUMMON_TYPE_TOTEM_EARTH = 81,
    SUMMON_TYPE_TOTEM_WATER = 82,
    SUMMON_TYPE_TOTEM_AIR   = 83,
};

/// Spell cooldown flags sent in SMSG_SPELL_COOLDOWN
enum SpellCooldownFlags
{
    SPELL_COOLDOWN_FLAG_NONE                    = 0x0,
    SPELL_COOLDOWN_FLAG_INCLUDE_GCD             = 0x1,  ///< Starts GCD in addition to normal cooldown specified in the packet
    SPELL_COOLDOWN_FLAG_INCLUDE_EVENT_COOLDOWNS = 0x2   ///< Starts GCD for spells that should start their cooldown on events, requires SPELL_COOLDOWN_FLAG_INCLUDE_GCD set
};

typedef std::unordered_map<uint32, uint32> PacketCooldowns;

// delay time next attack to prevent client attack animation problems
#define ATTACK_DISPLAY_DELAY 200
#define MAX_PLAYER_STEALTH_DETECT_RANGE 30.0f               // max distance for detection targets by player

struct SpellProcEventEntry;                                 // used only privately

// pussywizard:
class MMapTargetData
{
public:
    MMapTargetData() = default;
    MMapTargetData(uint32 endTime, const Position* o, const Position* t)
    {
        _endTime = endTime;
        _posOwner.Relocate(o);
        _posTarget.Relocate(t);
    }
    MMapTargetData(const MMapTargetData& c)
    {
        _endTime = c._endTime;
        _posOwner.Relocate(c._posOwner);
        _posTarget.Relocate(c._posTarget);
    }
    MMapTargetData(MMapTargetData&&) = default;
    MMapTargetData& operator=(const MMapTargetData&) = default;
    MMapTargetData& operator=(MMapTargetData&&) = default;
    [[nodiscard]] bool PosChanged(const Position& o, const Position& t) const
    {
        return _posOwner.GetExactDistSq(&o) > 0.5f * 0.5f || _posTarget.GetExactDistSq(&t) > 0.5f * 0.5f;
    }
    uint32 _endTime;
    Position _posOwner;
    Position _posTarget;
};

class SafeUnitPointer
{
public:
    explicit SafeUnitPointer(Unit* defVal) :  ptr(defVal), defaultValue(defVal) {}
    SafeUnitPointer(const SafeUnitPointer& /*p*/) { ABORT(); }
    void Initialize(Unit* defVal) { defaultValue = defVal; ptr = defVal; }
    ~SafeUnitPointer();
    void SetPointedTo(Unit* u);
    void UnitDeleted();
    Unit* operator->() const { return ptr; }
    void operator=(Unit* u) { SetPointedTo(u); }
    operator Unit* () const { return ptr; }
private:
    Unit* ptr;
    Unit* defaultValue;
};

// BuildValuesCachePosPointers is marks of the position of some data inside of BuildValue cache.
struct BuildValuesCachePosPointers
{
    BuildValuesCachePosPointers() :
        UnitNPCFlagsPos(-1), UnitFieldAuraStatePos(-1), UnitFieldFlagsPos(-1), UnitFieldDisplayPos(-1),
        UnitDynamicFlagsPos(-1), UnitFieldBytes2Pos(-1), UnitFieldFactionTemplatePos(-1) {}

    void ApplyOffset(uint32 offset)
    {
        if (UnitNPCFlagsPos >= 0)
            UnitNPCFlagsPos += offset;

        if (UnitFieldAuraStatePos >= 0)
            UnitFieldAuraStatePos += offset;

        if (UnitFieldFlagsPos >= 0)
            UnitFieldFlagsPos += offset;

        if (UnitFieldDisplayPos >= 0)
            UnitFieldDisplayPos += offset;

        if (UnitDynamicFlagsPos >= 0)
            UnitDynamicFlagsPos += offset;

        if (UnitFieldBytes2Pos >= 0)
            UnitFieldBytes2Pos += offset;

        if (UnitFieldFactionTemplatePos >= 0)
            UnitFieldFactionTemplatePos += offset;

        for (auto it = other.begin(); it != other.end(); ++it)
            it->second += offset;
    }

    int32 UnitNPCFlagsPos;
    int32 UnitFieldAuraStatePos;
    int32 UnitFieldFlagsPos;
    int32 UnitFieldDisplayPos;
    int32 UnitDynamicFlagsPos;
    int32 UnitFieldBytes2Pos;
    int32 UnitFieldFactionTemplatePos;

    std::unordered_map<uint16 /*index*/, uint32 /*pos*/> other;
};

// BuildValuesCachedBuffer cache for calculated BuildValue.
struct BuildValuesCachedBuffer
{
    BuildValuesCachedBuffer(uint32 bufferSize) :
        buffer(bufferSize), posPointers() {}

    ByteBuffer buffer;

    BuildValuesCachePosPointers posPointers;
};

class Unit : public WorldObject
{
public:
    typedef std::unordered_set<Unit*> AttackerSet;
    typedef std::set<Unit*> ControlSet;

    typedef std::multimap<uint32,  Aura*> AuraMap;
    typedef std::pair<AuraMap::const_iterator, AuraMap::const_iterator> AuraMapBounds;
    typedef std::pair<AuraMap::iterator, AuraMap::iterator> AuraMapBoundsNonConst;

    typedef std::multimap<uint32,  AuraApplication*> AuraApplicationMap;
    typedef std::pair<AuraApplicationMap::const_iterator, AuraApplicationMap::const_iterator> AuraApplicationMapBounds;
    typedef std::pair<AuraApplicationMap::iterator, AuraApplicationMap::iterator> AuraApplicationMapBoundsNonConst;

    typedef std::multimap<AuraStateType,  AuraApplication*> AuraStateAurasMap;
    typedef std::pair<AuraStateAurasMap::const_iterator, AuraStateAurasMap::const_iterator> AuraStateAurasMapBounds;

    typedef std::list<AuraEffect*> AuraEffectList;
    typedef std::list<Aura*> AuraList;
    typedef std::list<AuraApplication*> AuraApplicationList;
    typedef std::list<DiminishingReturn> Diminishing;
    typedef GuidUnorderedSet ComboPointHolderSet;

    typedef std::map<uint8, AuraApplication*> VisibleAuraMap;

    ~Unit() override;

    UnitAI* GetAI() { return i_AI; }
    void SetAI(UnitAI* newAI) { i_AI = newAI; }

    void AddToWorld() override;
    void RemoveFromWorld() override;

    void CleanupBeforeRemoveFromMap(bool finalCleanup);
    void CleanupsBeforeDelete(bool finalCleanup = true) override;                        // used in ~Creature/~Player (or before mass creature delete to remove cross-references to already deleted units)

    uint32 GetDynamicFlags() const override { return GetUInt32Value(UNIT_DYNAMIC_FLAGS); }
    void ReplaceAllDynamicFlags(uint32 flag) override { SetUInt32Value(UNIT_DYNAMIC_FLAGS, flag); }

    DiminishingLevels GetDiminishing(DiminishingGroup group);
    void IncrDiminishing(DiminishingGroup group);
    float ApplyDiminishingToDuration(DiminishingGroup group, int32& duration, Unit* caster, DiminishingLevels Level, int32 limitduration);
    void ApplyDiminishingAura(DiminishingGroup group, bool apply);
    void ClearDiminishings() { m_Diminishing.clear(); }

    // target dependent range checks
    float GetSpellMaxRangeForTarget(Unit const* target, SpellInfo const* spellInfo) const;
    float GetSpellMinRangeForTarget(Unit const* target, SpellInfo const* spellInfo) const;

    void Update(uint32 time) override;

    void setAttackTimer(WeaponAttackType type, int32 time) { m_attackTimer[type] = time; }  /// @todo - Look to convert to std::chrono
    void resetAttackTimer(WeaponAttackType type = BASE_ATTACK);
    [[nodiscard]] int32 getAttackTimer(WeaponAttackType type) const { return m_attackTimer[type]; }
    [[nodiscard]] bool isAttackReady(WeaponAttackType type = BASE_ATTACK) const { return m_attackTimer[type] <= 0; }
    [[nodiscard]] bool haveOffhandWeapon() const;
    [[nodiscard]] bool CanDualWield() const { return m_canDualWield; }
    virtual void SetCanDualWield(bool value) { m_canDualWield = value; }
    [[nodiscard]] float GetCombatReach() const override { return m_floatValues[UNIT_FIELD_COMBATREACH]; }
    [[nodiscard]] float GetMeleeReach() const { float reach = m_floatValues[UNIT_FIELD_COMBATREACH]; return reach > MIN_MELEE_REACH ? reach : MIN_MELEE_REACH; }
    [[nodiscard]] bool IsWithinRange(Unit const* obj, float dist) const;
    bool IsWithinCombatRange(Unit const* obj, float dist2compare) const;
    bool IsWithinMeleeRange(Unit const* obj, float dist = 0.f) const;
    //npcbot: TC method transfer
    bool IsWithinMeleeRangeAt(Position const& pos, Unit const* obj) const;
    //end npcbot
    float GetMeleeRange(Unit const* target) const;
    virtual SpellSchoolMask GetMeleeDamageSchoolMask(WeaponAttackType attackType = BASE_ATTACK, uint8 damageIndex = 0) const = 0;
    bool GetRandomContactPoint(Unit const* target, float& x, float& y, float& z, bool force = false) const;
    uint32 m_extraAttacks;
    bool m_canDualWield;

    void _addAttacker(Unit* pAttacker)                  // must be called only from Unit::Attack(Unit*)
    {
        m_attackers.insert(pAttacker);
    }
    void _removeAttacker(Unit* pAttacker)               // must be called only from Unit::AttackStop()
    {
        m_attackers.erase(pAttacker);
    }
    [[nodiscard]] Unit* getAttackerForHelper() const                 // If someone wants to help, who to give them
    {
        if (GetVictim() != nullptr)
            return GetVictim();

        if (!IsEngaged())
            return nullptr;

        if (!m_attackers.empty())
            return *(m_attackers.begin());

        return nullptr;
    }
    bool Attack(Unit* victim, bool meleeAttack);
    void CastStop(uint32 except_spellid = 0, bool withInstant = true);
    bool AttackStop();
    void RemoveAllAttackers();
    [[nodiscard]] AttackerSet const& getAttackers() const { return m_attackers; }
    [[nodiscard]] bool GetMeleeAttackPoint(Unit* attacker, Position& pos);
    [[nodiscard]] bool isAttackingPlayer() const;
    [[nodiscard]] Unit* GetVictim() const { return m_attacking; }

    void CombatStop(bool includingCast = false);
    void CombatStopWithPets(bool includingCast = false);
    void StopAttackFaction(uint32 faction_id);
    void StopAttackingInvalidTarget();
    Unit* SelectNearbyTarget(Unit* exclude = nullptr, float dist = NOMINAL_MELEE_RANGE) const;
    Unit* SelectNearbyNoTotemTarget(Unit* exclude = nullptr, float dist = NOMINAL_MELEE_RANGE) const;
    void SendMeleeAttackStop(Unit* victim = nullptr);
    void SendMeleeAttackStart(Unit* victim, Player* sendTo = nullptr);

    void AddUnitState(uint32 f) { m_state |= f; }
    [[nodiscard]] bool HasUnitState(const uint32 f) const { return (m_state & f); }
    void ClearUnitState(uint32 f) { m_state &= ~f; }
    [[nodiscard]] uint32 GetUnitState() const { return m_state; }
    [[nodiscard]] bool CanFreeMove() const
    {
        //npcbot: skip owner guid condition for bots
        if (IsNPCBotOrPet())
            return !HasUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_FLEEING | UNIT_STATE_IN_FLIGHT |
                                 UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED);
        //end npcbot
        return !HasUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_FLEEING | UNIT_STATE_IN_FLIGHT |
                             UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED) && !GetOwnerGUID();
    }

    [[nodiscard]] uint32 HasUnitTypeMask(uint32 mask) const { return mask & m_unitTypeMask; }
    void AddUnitTypeMask(uint32 mask) { m_unitTypeMask |= mask; }
    [[nodiscard]] uint32 GetUnitTypeMask() const { return m_unitTypeMask; }
    [[nodiscard]] bool IsSummon() const { return m_unitTypeMask & UNIT_MASK_SUMMON; }
    [[nodiscard]] bool IsGuardian() const { return m_unitTypeMask & UNIT_MASK_GUARDIAN; }
    [[nodiscard]] bool IsControllableGuardian() const { return m_unitTypeMask & UNIT_MASK_CONTROLABLE_GUARDIAN; }
    [[nodiscard]] bool IsPet() const { return m_unitTypeMask & UNIT_MASK_PET; }
    [[nodiscard]] bool IsHunterPet() const { return m_unitTypeMask & UNIT_MASK_HUNTER_PET; }
    [[nodiscard]] bool IsTotem() const { return m_unitTypeMask & UNIT_MASK_TOTEM; }
    [[nodiscard]] bool IsVehicle() const { return m_unitTypeMask & UNIT_MASK_VEHICLE; }

    /// @deprecated Use GetLevel() instead!
    [[nodiscard]] uint8 getLevel() const { return uint8(GetUInt32Value(UNIT_FIELD_LEVEL)); }
    [[nodiscard]] uint8 GetLevel() const { return getLevel(); }
    uint8 getLevelForTarget(WorldObject const* /*target*/) const override { return GetLevel(); }
    void SetLevel(uint8 lvl, bool showLevelChange = true);
    [[nodiscard]] uint8 getRace(bool original = false) const;
    void setRace(uint8 race);
    [[nodiscard]] uint32 getRaceMask() const { return 1 << (getRace(true) - 1); }
    [[nodiscard]] uint8 getClass() const { return GetByteValue(UNIT_FIELD_BYTES_0, 1); }
    [[nodiscard]] virtual bool IsClass(Classes unitClass, [[maybe_unused]] ClassContext context = CLASS_CONTEXT_NONE) const { return (getClass() == unitClass); }
    [[nodiscard]] uint32 getClassMask() const { return 1 << (getClass() - 1); }
    [[nodiscard]] uint8 getGender() const { return GetByteValue(UNIT_FIELD_BYTES_0, 2); }
    [[nodiscard]] DisplayRace GetDisplayRaceFromModelId(uint32 modelId) const;
    [[nodiscard]] DisplayRace GetDisplayRace() const { return GetDisplayRaceFromModelId(GetDisplayId()); };

    //npcbot: compatibility accessors
    [[nodiscard]] inline uint8 GetRace(bool original = false) const { return getRace(original); }
    [[nodiscard]] inline uint32 GetRaceMask() const { return getRaceMask(); }
    [[nodiscard]] inline uint8 GetClass() const { return getClass(); }
    [[nodiscard]] inline uint32 GetClassMask() const { return getClassMask(); }
    [[nodiscard]] inline uint8 GetGender() const { return getGender(); }
    inline void SetPowerType(Powers power) { setPowerType(power); }
    [[nodiscard]] inline Powers GetPowerType() const { return getPowerType(); }
    [[nodiscard]] uint8 GetStandState() const { return getStandState(); }
    //end npcbot

    [[nodiscard]] float GetStat(Stats stat) const { return float(GetUInt32Value(static_cast<uint16>(UNIT_FIELD_STAT0) + stat)); }
    void SetStat(Stats stat, int32 val) { SetStatInt32Value(static_cast<uint16>(UNIT_FIELD_STAT0) + stat, val); }
    [[nodiscard]] uint32 GetArmor() const { return GetResistance(SPELL_SCHOOL_NORMAL); }
    void SetArmor(int32 val) { SetResistance(SPELL_SCHOOL_NORMAL, val); }

    [[nodiscard]] uint32 GetResistance(SpellSchools school) const { return GetUInt32Value(static_cast<uint16>(UNIT_FIELD_RESISTANCES) + school); }
    [[nodiscard]] uint32 GetResistance(SpellSchoolMask mask) const;
    void SetResistance(SpellSchools school, int32 val) { SetStatInt32Value(static_cast<uint16>(UNIT_FIELD_RESISTANCES) + school, val); }
    static float GetEffectiveResistChance(Unit const* owner, SpellSchoolMask schoolMask, Unit const* victim);

    [[nodiscard]] uint32 GetHealth()    const { return GetUInt32Value(UNIT_FIELD_HEALTH); }
    [[nodiscard]] uint32 GetMaxHealth() const { return GetUInt32Value(UNIT_FIELD_MAXHEALTH); }

    [[nodiscard]] bool IsFullHealth() const { return GetHealth() == GetMaxHealth(); }
    [[nodiscard]] bool HealthBelowPct(int32 pct) const { return GetHealth() < CountPctFromMaxHealth(pct); }
    [[nodiscard]] bool HealthBelowPctDamaged(int32 pct, uint32 damage) const { return int64(GetHealth()) - int64(damage) < int64(CountPctFromMaxHealth(pct)); }
    [[nodiscard]] bool HealthAbovePct(int32 pct) const { return GetHealth() > CountPctFromMaxHealth(pct); }
    [[nodiscard]] bool HealthAbovePctHealed(int32 pct, uint32 heal) const { return uint64(GetHealth()) + uint64(heal) > CountPctFromMaxHealth(pct); }
    [[nodiscard]] float GetHealthPct() const { return GetMaxHealth() ? 100.f * GetHealth() / GetMaxHealth() : 0.0f; }
    [[nodiscard]] uint32 CountPctFromMaxHealth(int32 pct) const { return CalculatePct(GetMaxHealth(), pct); }
    [[nodiscard]] uint32 CountPctFromCurHealth(int32 pct) const { return CalculatePct(GetHealth(), pct); }
    [[nodiscard]] float GetPowerPct(Powers power) const { return GetMaxPower(power) ? 100.f * GetPower(power) / GetMaxPower(power) : 0.0f; }

    void SetHealth(uint32 val);
    void SetMaxHealth(uint32 val);
    inline void SetFullHealth() { SetHealth(GetMaxHealth()); }
    int32 ModifyHealth(int32 val);
    int32 GetHealthGain(int32 dVal);

    [[nodiscard]] Powers getPowerType() const { return Powers(GetByteValue(UNIT_FIELD_BYTES_0, 3)); }
    void setPowerType(Powers power);
    [[nodiscard]] virtual bool HasActivePowerType(Powers power) { return getPowerType() == power; }
    [[nodiscard]] uint32 GetPower(Powers power) const { return GetUInt32Value(static_cast<uint16>(UNIT_FIELD_POWER1) + power); }
    [[nodiscard]] uint32 GetMaxPower(Powers power) const { return GetUInt32Value(static_cast<uint16>(UNIT_FIELD_MAXPOWER1) + power); }
    void SetPower(Powers power, uint32 val, bool withPowerUpdate = true, bool fromRegenerate = false);
    void SetMaxPower(Powers power, uint32 val);
    // returns the change in power
    int32 ModifyPower(Powers power, int32 val, bool withPowerUpdate = true);
    int32 ModifyPowerPct(Powers power, float pct, bool apply = true);

    [[nodiscard]] uint32 GetAttackTime(WeaponAttackType att) const
    {
        float f_BaseAttackTime = GetFloatValue(static_cast<uint16>(UNIT_FIELD_BASEATTACKTIME) + att) / m_modAttackSpeedPct[att];
        return (uint32)f_BaseAttackTime;
    }

    void SetAttackTime(WeaponAttackType att, uint32 val) { SetFloatValue(static_cast<uint16>(UNIT_FIELD_BASEATTACKTIME) + att, val * m_modAttackSpeedPct[att]); }
    void ApplyAttackTimePercentMod(WeaponAttackType att, float val, bool apply);
    void ApplyCastTimePercentMod(float val, bool apply);

    UnitFlags GetUnitFlags() const { return UnitFlags(GetUInt32Value(UNIT_FIELD_FLAGS)); }
    bool HasUnitFlag(UnitFlags flags) const { return HasFlag(UNIT_FIELD_FLAGS, flags); }
    void SetUnitFlag(UnitFlags flags) { SetFlag(UNIT_FIELD_FLAGS, flags); }
    void RemoveUnitFlag(UnitFlags flags) { RemoveFlag(UNIT_FIELD_FLAGS, flags); }
    void ReplaceAllUnitFlags(UnitFlags flags) { SetUInt32Value(UNIT_FIELD_FLAGS, flags); }

    UnitFlags2 GetUnitFlags2() const { return UnitFlags2(GetUInt32Value(UNIT_FIELD_FLAGS_2)); }
    bool HasUnitFlag2(UnitFlags2 flags) const { return HasFlag(UNIT_FIELD_FLAGS_2, flags); }
    void SetUnitFlag2(UnitFlags2 flags) { SetFlag(UNIT_FIELD_FLAGS_2, flags); }
    void RemoveUnitFlag2(UnitFlags2 flags) { RemoveFlag(UNIT_FIELD_FLAGS_2, flags); }
    void ReplaceAllUnitFlags2(UnitFlags2 flags) { SetUInt32Value(UNIT_FIELD_FLAGS_2, flags); }

    [[nodiscard]] SheathState GetSheath() const { return SheathState(GetByteValue(UNIT_FIELD_BYTES_2, 0)); }
    virtual void SetSheath(SheathState sheathed) { SetByteValue(UNIT_FIELD_BYTES_2, 0, sheathed); }

    // faction template id
    [[nodiscard]] uint32 GetFaction() const { return GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE); }
    void SetFaction(uint32 faction);
    [[nodiscard]] FactionTemplateEntry const* GetFactionTemplateEntry() const;

    ReputationRank GetReactionTo(Unit const* target, bool checkOriginalFaction = false) const;
    ReputationRank GetFactionReactionTo(FactionTemplateEntry const* factionTemplateEntry, Unit const* target) const;

    bool IsHostileTo(Unit const* unit) const;
    [[nodiscard]] bool IsHostileToPlayers() const;
    bool IsFriendlyTo(Unit const* unit) const;
    [[nodiscard]] bool IsNeutralToAll() const;
    bool IsInPartyWith(Unit const* unit) const;
    bool IsInRaidWith(Unit const* unit) const;
    void GetPartyMembers(std::list<Unit*>& units);
    [[nodiscard]] bool IsContestedGuard() const
    {
        if (FactionTemplateEntry const* entry = GetFactionTemplateEntry())
            return entry->IsContestedGuardFaction();

        return false;
    }
    [[nodiscard]] bool IsInSanctuary() const { return HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_SANCTUARY); }
    [[nodiscard]] bool IsPvP() const { return HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP); }
    [[nodiscard]] bool IsFFAPvP() const { return HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP); }
    void SetPvP(bool state)
    {
        if (state)
            SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);
        else
            RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);
    }

    [[nodiscard]] uint32 GetCreatureType() const;
    [[nodiscard]] uint32 GetCreatureTypeMask() const
    {
        uint32 creatureType = GetCreatureType();
        return (creatureType >= 1) ? (1 << (creatureType - 1)) : 0;
    }

    [[nodiscard]] uint8 getStandState() const { return GetByteValue(UNIT_FIELD_BYTES_1, 0); }
    [[nodiscard]] bool IsSitState() const;
    [[nodiscard]] bool IsStandState() const;
    void SetStandState(uint8 state);

    void  SetStandFlags(uint8 flags) { SetByteFlag(UNIT_FIELD_BYTES_1,  UNIT_BYTES_1_OFFSET_VIS_FLAG, flags); }
    void  RemoveStandFlags(uint8 flags) { RemoveByteFlag(UNIT_FIELD_BYTES_1,  UNIT_BYTES_1_OFFSET_VIS_FLAG, flags); }

    [[nodiscard]] bool IsMounted() const { return HasUnitFlag(UNIT_FLAG_MOUNT); }
    [[nodiscard]] uint32 GetMountID() const { return GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID); }
    void Mount(uint32 mount, uint32 vehicleId = 0, uint32 creatureEntry = 0);
    void Dismount();

    uint16 GetMaxSkillValueForLevel(Unit const* target = nullptr) const { return (target ? getLevelForTarget(target) : GetLevel()) * 5; }
    static void DealDamageMods(Unit const* victim, uint32& damage, uint32* absorb);
    static uint32 DealDamage(Unit* attacker, Unit* victim, uint32 damage, CleanDamage const* cleanDamage = nullptr, DamageEffectType damagetype = DIRECT_DAMAGE, SpellSchoolMask damageSchoolMask = SPELL_SCHOOL_MASK_NORMAL, SpellInfo const* spellProto = nullptr, bool durabilityLoss = true, bool allowGM = false, Spell const* spell = nullptr);
    static void Kill(Unit* killer, Unit* victim, bool durabilityLoss = true, WeaponAttackType attackType = BASE_ATTACK, SpellInfo const* spellProto = nullptr, Spell const* spell = nullptr);
    void KillSelf(bool durabilityLoss = true, WeaponAttackType attackType = BASE_ATTACK, SpellInfo const* spellProto = nullptr, Spell const* spell = nullptr) { Kill(this, this, durabilityLoss, attackType, spellProto, spell); };
    static int32 DealHeal(Unit* healer, Unit* victim, uint32 addhealth);

    static void ProcDamageAndSpell(Unit* actor, Unit* victim, uint32 procAttacker, uint32 procVictim, uint32 procEx, uint32 amount, WeaponAttackType attType = BASE_ATTACK, SpellInfo const* procSpellInfo = nullptr, SpellInfo const* procAura = nullptr, int8 procAuraEffectIndex = -1, Spell const* procSpell = nullptr, DamageInfo* damageInfo = nullptr, HealInfo* healInfo = nullptr, uint32 procPhase = 2 /*PROC_SPELL_PHASE_HIT*/);
    void ProcDamageAndSpellFor(bool isVictim, Unit* target, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, SpellInfo const* procSpellInfo, uint32 damage, SpellInfo const* procAura = nullptr, int8 procAuraEffectIndex = -1, Spell const* procSpell = nullptr, DamageInfo* damageInfo = nullptr, HealInfo* healInfo = nullptr, uint32 procPhase = 2 /*PROC_SPELL_PHASE_HIT*/);

    void GetProcAurasTriggeredOnEvent(std::list<AuraApplication*>& aurasTriggeringProc, std::list<AuraApplication*>* procAuras, ProcEventInfo eventInfo);
    void TriggerAurasProcOnEvent(CalcDamageInfo& damageInfo);
    void TriggerAurasProcOnEvent(std::list<AuraApplication*>* myProcAuras, std::list<AuraApplication*>* targetProcAuras, Unit* actionTarget, uint32 typeMaskActor, uint32 typeMaskActionTarget, uint32 spellTypeMask, uint32 spellPhaseMask, uint32 hitMask, Spell* spell, DamageInfo* damageInfo, HealInfo* healInfo);
    void TriggerAurasProcOnEvent(ProcEventInfo& eventInfo, std::list<AuraApplication*>& procAuras);

    void HandleEmoteCommand(uint32 emoteId);
    void AttackerStateUpdate (Unit* victim, WeaponAttackType attType = BASE_ATTACK, bool extra = false, bool ignoreCasting = false);

    void CalculateMeleeDamage(Unit* victim, CalcDamageInfo* damageInfo, WeaponAttackType attackType = BASE_ATTACK, const bool sittingVictim = false);
    void DealMeleeDamage(CalcDamageInfo* damageInfo, bool durabilityLoss);

    void HandleProcExtraAttackFor(Unit* victim, uint32 count);
    void SetLastExtraAttackSpell(uint32 spellId) { _lastExtraAttackSpell = spellId; }
    [[nodiscard]] uint32 GetLastExtraAttackSpell() const { return _lastExtraAttackSpell; }
    void AddExtraAttacks(uint32 count);
    void SetLastDamagedTargetGuid(ObjectGuid const& guid) { _lastDamagedTargetGuid = guid; }
    [[nodiscard]] ObjectGuid const& GetLastDamagedTargetGuid() const { return _lastDamagedTargetGuid; }

    void CalculateSpellDamageTaken(SpellNonMeleeDamage* damageInfo, int32 damage, SpellInfo const* spellInfo, WeaponAttackType attackType = BASE_ATTACK, bool crit = false);
    void DealSpellDamage(SpellNonMeleeDamage* damageInfo, bool durabilityLoss, Spell const* spell = nullptr);

    // player or player's pet resilience (-1%)
    [[nodiscard]] float GetMeleeCritChanceReduction() const { return GetCombatRatingReduction(CR_CRIT_TAKEN_MELEE); }
    [[nodiscard]] float GetRangedCritChanceReduction() const { return GetCombatRatingReduction(CR_CRIT_TAKEN_RANGED); }
    [[nodiscard]] float GetSpellCritChanceReduction() const { return GetCombatRatingReduction(CR_CRIT_TAKEN_SPELL); }

    // player or player's pet resilience (-1%)
    [[nodiscard]] uint32 GetMeleeCritDamageReduction(uint32 damage) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_MELEE, 2.2f, 33.0f, damage); }
    [[nodiscard]] uint32 GetRangedCritDamageReduction(uint32 damage) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_RANGED, 2.2f, 33.0f, damage); }
    [[nodiscard]] uint32 GetSpellCritDamageReduction(uint32 damage) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_SPELL, 2.2f, 33.0f, damage); }

    // player or player's pet resilience (-1%), cap 100%
    [[nodiscard]] uint32 GetMeleeDamageReduction(uint32 damage) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_MELEE, 2.0f, 100.0f, damage); }
    [[nodiscard]] uint32 GetRangedDamageReduction(uint32 damage) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_RANGED, 2.0f, 100.0f, damage); }
    [[nodiscard]] uint32 GetSpellDamageReduction(uint32 damage) const { return GetCombatRatingDamageReduction(CR_CRIT_TAKEN_SPELL, 2.0f, 100.0f, damage); }

    static void ApplyResilience(Unit const* victim, float* crit, int32* damage, bool isCrit, CombatRating type);

    [[nodiscard]] float MeleeSpellMissChance(Unit const* victim, WeaponAttackType attType, int32 skillDiff, uint32 spellId) const;
    [[nodiscard]] SpellMissInfo MeleeSpellHitResult(Unit* victim, SpellInfo const* spell);
    [[nodiscard]] SpellMissInfo MagicSpellHitResult(Unit* victim, SpellInfo const* spell);
    [[nodiscard]] SpellMissInfo SpellHitResult(Unit* victim, SpellInfo const* spell, bool canReflect = false);
    [[nodiscard]] SpellMissInfo SpellHitResult(Unit* victim, Spell const* spell, bool canReflect = false);

    [[nodiscard]] float GetUnitDodgeChance()    const;
    [[nodiscard]] float GetUnitParryChance()    const;
    [[nodiscard]] float GetUnitBlockChance()    const;
    [[nodiscard]] float GetUnitMissChance(WeaponAttackType attType)     const;
    float GetUnitCriticalChance(WeaponAttackType attackType, Unit const* victim) const;
    int32 GetMechanicResistChance(SpellInfo const* spell);
    [[nodiscard]] bool CanUseAttackType(uint8 attacktype) const
    {
        switch (attacktype)
        {
            case BASE_ATTACK:
                return !HasUnitFlag(UNIT_FLAG_DISARMED);
            case OFF_ATTACK:
                return !HasUnitFlag2(UNIT_FLAG2_DISARM_OFFHAND);
            case RANGED_ATTACK:
                return !HasUnitFlag2(UNIT_FLAG2_DISARM_RANGED);
        }
        return true;
    }

    [[nodiscard]] virtual uint32 GetShieldBlockValue() const = 0;
    [[nodiscard]] uint32 GetShieldBlockValue(uint32 soft_cap, uint32 hard_cap) const
    {
        uint32 value = GetShieldBlockValue();
        if (value >= hard_cap)
        {
            value = (soft_cap + hard_cap) / 2;
        }
        else if (value > soft_cap)
        {
            value = soft_cap + ((value - soft_cap) / 2);
        }

        return value;
    }
    uint32 GetUnitMeleeSkill(Unit const* target = nullptr) const { return (target ? getLevelForTarget(target) : GetLevel()) * 5; }
    uint32 GetDefenseSkillValue(Unit const* target = nullptr) const;
    uint32 GetWeaponSkillValue(WeaponAttackType attType, Unit const* target = nullptr) const;
    [[nodiscard]] float GetWeaponProcChance() const;
    float GetPPMProcChance(uint32 WeaponSpeed, float PPM,  SpellInfo const* spellProto) const;

    MeleeHitOutcome RollMeleeOutcomeAgainst (Unit const* victim, WeaponAttackType attType) const;
    MeleeHitOutcome RollMeleeOutcomeAgainst (Unit const* victim, WeaponAttackType attType, int32 crit_chance, int32 miss_chance, int32 dodge_chance, int32 parry_chance, int32 block_chance) const;

    NPCFlags GetNpcFlags() const { return NPCFlags(GetUInt32Value(UNIT_NPC_FLAGS)); }
    bool HasNpcFlag(NPCFlags flags) const { return HasFlag(UNIT_NPC_FLAGS, flags) != 0; }
    void SetNpcFlag(NPCFlags flags) { SetFlag(UNIT_NPC_FLAGS, flags); }
    void RemoveNpcFlag(NPCFlags flags) { RemoveFlag(UNIT_NPC_FLAGS, flags); }
    void ReplaceAllNpcFlags(NPCFlags flags) { SetUInt32Value(UNIT_NPC_FLAGS, flags); }

    [[nodiscard]] bool IsVendor()       const { return HasNpcFlag(UNIT_NPC_FLAG_VENDOR); }
    [[nodiscard]] bool IsTrainer()      const { return HasNpcFlag(UNIT_NPC_FLAG_TRAINER); }
    [[nodiscard]] bool IsQuestGiver()   const { return HasNpcFlag(UNIT_NPC_FLAG_QUESTGIVER); }
    [[nodiscard]] bool IsGossip()       const { return HasNpcFlag(UNIT_NPC_FLAG_GOSSIP); }
    [[nodiscard]] bool IsTaxi()         const { return HasNpcFlag(UNIT_NPC_FLAG_FLIGHTMASTER); }
    [[nodiscard]] bool IsGuildMaster()  const { return HasNpcFlag(UNIT_NPC_FLAG_PETITIONER); }
    [[nodiscard]] bool IsBattleMaster() const { return HasNpcFlag(UNIT_NPC_FLAG_BATTLEMASTER); }
    [[nodiscard]] bool IsBanker()       const { return HasNpcFlag(UNIT_NPC_FLAG_BANKER); }
    [[nodiscard]] bool IsInnkeeper()    const { return HasNpcFlag(UNIT_NPC_FLAG_INNKEEPER); }
    [[nodiscard]] bool IsSpiritHealer() const { return HasNpcFlag(UNIT_NPC_FLAG_SPIRITHEALER); }
    [[nodiscard]] bool IsSpiritGuide()  const { return HasNpcFlag(UNIT_NPC_FLAG_SPIRITGUIDE); }
    [[nodiscard]] bool IsTabardDesigner()const { return HasNpcFlag(UNIT_NPC_FLAG_TABARDDESIGNER); }
    [[nodiscard]] bool IsAuctioner()    const { return HasNpcFlag(UNIT_NPC_FLAG_AUCTIONEER); }
    [[nodiscard]] bool IsArmorer()      const { return HasNpcFlag(UNIT_NPC_FLAG_REPAIR); }
    [[nodiscard]] bool IsServiceProvider() const
    {
        return HasFlag(UNIT_NPC_FLAGS,
                       UNIT_NPC_FLAG_VENDOR | UNIT_NPC_FLAG_TRAINER | UNIT_NPC_FLAG_FLIGHTMASTER |
                       UNIT_NPC_FLAG_PETITIONER | UNIT_NPC_FLAG_BATTLEMASTER | UNIT_NPC_FLAG_BANKER |
                       UNIT_NPC_FLAG_INNKEEPER | UNIT_NPC_FLAG_SPIRITHEALER |
                       UNIT_NPC_FLAG_SPIRITGUIDE | UNIT_NPC_FLAG_TABARDDESIGNER | UNIT_NPC_FLAG_AUCTIONEER);
    }
    [[nodiscard]] bool IsSpiritService() const { return HasNpcFlag(UNIT_NPC_FLAG_SPIRITHEALER | UNIT_NPC_FLAG_SPIRITGUIDE); }
    [[nodiscard]] bool IsCritter() const { return GetCreatureType() == CREATURE_TYPE_CRITTER; }

    [[nodiscard]] bool IsInFlight()  const { return HasUnitState(UNIT_STATE_IN_FLIGHT); }

    void SetImmuneToAll(bool apply, bool keepCombat = false) { SetImmuneToPC(apply, keepCombat); SetImmuneToNPC(apply, keepCombat); }
    bool IsImmuneToAll() const { return IsImmuneToPC() && IsImmuneToNPC(); }
    void SetImmuneToPC(bool apply, bool keepCombat = false);
    bool IsImmuneToPC() const { return HasUnitFlag(UNIT_FLAG_IMMUNE_TO_PC); }
    void SetImmuneToNPC(bool apply, bool keepCombat = false);
    bool IsImmuneToNPC() const { return HasUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC); }

    bool IsEngaged() const { return IsInCombat(); }
    bool IsEngagedBy(Unit const* who) const { return IsInCombatWith(who); }

    [[nodiscard]] bool IsInCombat() const { return HasUnitFlag(UNIT_FLAG_IN_COMBAT); }
    bool IsInCombatWith(Unit const* who) const;

    [[nodiscard]] bool IsPetInCombat() const { return HasUnitFlag(UNIT_FLAG_PET_IN_COMBAT); }
    void CombatStart(Unit* target, bool initialAggro = true);
    void CombatStartOnCast(Unit* target, bool initialAggro = true, uint32 duration = 0);
    void SetInCombatState(bool PvP, Unit* enemy = nullptr, uint32 duration = 0);
    void SetInCombatWith(Unit* enemy, uint32 duration = 0);
    void ClearInCombat();
    void ClearInPetCombat();
    [[nodiscard]] uint32 GetCombatTimer() const { return m_CombatTimer; }
    void SetCombatTimer(uint32 timer) { m_CombatTimer = timer; }

    [[nodiscard]] bool HasAuraTypeWithFamilyFlags(AuraType auraType, uint32 familyName, uint32 familyFlags) const;
    [[nodiscard]] bool virtual HasSpell(uint32 /*spellID*/) const { return false; }
    [[nodiscard]] bool HasBreakableByDamageAuraType(AuraType type, uint32 excludeAura = 0) const;
    bool HasBreakableByDamageCrowdControlAura(Unit* excludeCasterChannel = nullptr) const;

    [[nodiscard]] bool HasStealthAura()      const { return HasAuraType(SPELL_AURA_MOD_STEALTH); }
    [[nodiscard]] bool HasInvisibilityAura() const { return HasAuraType(SPELL_AURA_MOD_INVISIBILITY); }
    [[nodiscard]] bool isFeared()  const { return HasAuraType(SPELL_AURA_MOD_FEAR); }
    [[nodiscard]] bool isInRoots() const { return HasAuraType(SPELL_AURA_MOD_ROOT); }
    [[nodiscard]] bool IsPolymorphed() const;

    [[nodiscard]] bool isFrozen() const;

    bool isTargetableForAttack(bool checkFakeDeath = true, Unit const* byWho = nullptr) const;

    bool IsValidAttackTarget(Unit const* target, SpellInfo const* bySpell = nullptr) const;
    bool _IsValidAttackTarget(Unit const* target, SpellInfo const* bySpell, WorldObject const* obj = nullptr) const;

    bool IsValidAssistTarget(Unit const* target) const;
    bool _IsValidAssistTarget(Unit const* target, SpellInfo const* bySpell) const;

    [[nodiscard]] virtual bool IsInWater() const;
    [[nodiscard]] virtual bool IsUnderWater() const;
    bool isInAccessiblePlaceFor(Creature const* c) const;

    void SendHealSpellLog(HealInfo const& healInfo, bool critical = false);
    int32 HealBySpell(HealInfo& healInfo, bool critical = false);
    void SendEnergizeSpellLog(Unit* victim, uint32 SpellID, uint32 Damage, Powers powertype);
    void EnergizeBySpell(Unit* victim, uint32 SpellID, uint32 Damage, Powers powertype);

    SpellCastResult CastSpell(SpellCastTargets const& targets, SpellInfo const* spellInfo, CustomSpellValues const* value, TriggerCastFlags triggerFlags = TRIGGERED_NONE, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastSpell(Unit* victim, uint32 spellId, bool triggered, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastSpell(Unit* victim, uint32 spellId, TriggerCastFlags triggerFlags = TRIGGERED_NONE, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastSpell(Unit* victim, SpellInfo const* spellInfo, bool triggered, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastSpell(Unit* victim, SpellInfo const* spellInfo, TriggerCastFlags triggerFlags = TRIGGERED_NONE, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastSpell(float x, float y, float z, uint32 spellId, bool triggered, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastSpell(GameObject* go, uint32 spellId, bool triggered, Item* castItem = nullptr, AuraEffect* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastCustomSpell(Unit* victim, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, bool triggered, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastCustomSpell(uint32 spellId, SpellValueMod mod, int32 value, Unit* victim, bool triggered, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastCustomSpell(uint32 spellId, SpellValueMod mod, int32 value, Unit* victim = nullptr, TriggerCastFlags triggerFlags = TRIGGERED_NONE, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    SpellCastResult CastCustomSpell(uint32 spellId, CustomSpellValues const& value, Unit* victim = nullptr, TriggerCastFlags triggerFlags = TRIGGERED_NONE, Item* castItem = nullptr, AuraEffect const* triggeredByAura = nullptr, ObjectGuid originalCaster = ObjectGuid::Empty);
    Aura* AddAura(uint32 spellId, Unit* target);
    Aura* AddAura(SpellInfo const* spellInfo, uint8 effMask, Unit* target);
    void SetAuraStack(uint32 spellId, Unit* target, uint32 stack);
    void SendPlaySpellVisual(uint32 id);
    void SendPlaySpellImpact(ObjectGuid guid, uint32 id);
    void BuildCooldownPacket(WorldPacket& data, uint8 flags, uint32 spellId, uint32 cooldown);
    void BuildCooldownPacket(WorldPacket& data, uint8 flags, PacketCooldowns const& cooldowns);

    void DeMorph();

    void SendAttackStateUpdate(CalcDamageInfo* damageInfo);
    void SendAttackStateUpdate(uint32 HitInfo, Unit* target, uint8 SwingType, SpellSchoolMask damageSchoolMask, uint32 Damage, uint32 AbsorbDamage, uint32 Resist, VictimState TargetState, uint32 BlockedAmount);
    void SendSpellNonMeleeDamageLog(SpellNonMeleeDamage* log);
    void SendSpellNonMeleeReflectLog(SpellNonMeleeDamage* log, Unit* attacker);
    void SendSpellNonMeleeDamageLog(Unit* target, SpellInfo const* spellInfo, uint32 Damage, SpellSchoolMask damageSchoolMask, uint32 AbsorbedDamage, uint32 Resist, bool PhysicalDamage, uint32 Blocked, bool CriticalHit = false, bool Split = false);
    void SendPeriodicAuraLog(SpellPeriodicAuraLogInfo* pInfo);
    void SendSpellMiss(Unit* target, uint32 spellID, SpellMissInfo missInfo);
    void SendSpellDamageResist(Unit* target, uint32 spellId);
    void SendSpellDamageImmune(Unit* target, uint32 spellId);

    void NearTeleportTo(Position& pos, bool casting = false, bool vehicleTeleport = false, bool withPet = false, bool removeTransport = false);
    void NearTeleportTo(float x, float y, float z, float orientation, bool casting = false, bool vehicleTeleport = false, bool withPet = false, bool removeTransport = false);
    void SendTameFailure(uint8 result);
    void SendTeleportPacket(Position& pos);
    virtual bool UpdatePosition(float x, float y, float z, float ang, bool teleport = false);
    // returns true if unit's position really changed
    bool UpdatePosition(const Position& pos, bool teleport = false) { return UpdatePosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), teleport); }
    void UpdateOrientation(float orientation);
    void UpdateHeight(float newZ);

    void KnockbackFrom(float x, float y, float speedXY, float speedZ);
    void JumpTo(float speedXY, float speedZ, bool forward = true);
    void JumpTo(WorldObject* obj, float speedZ);

    void SendMonsterMove(float NewPosX, float NewPosY, float NewPosZ, uint32 TransitTime, SplineFlags sf = SPLINEFLAG_WALK_MODE); // pussywizard: need to just send packet, with no movement/spline
    void MonsterMoveWithSpeed(float x, float y, float z, float speed);
    //void SetFacing(float ori, WorldObject* obj = nullptr);
    //void SendMonsterMove(float NewPosX, float NewPosY, float NewPosZ, uint8 type, uint32 MovementFlags, uint32 Time, Player* player = nullptr);
    void SendMovementFlagUpdate(bool self = false);

    virtual bool SetWalk(bool enable);
    virtual bool SetDisableGravity(bool disable, bool packetOnly = false, bool updateAnimationTier = true);
    virtual bool SetSwim(bool enable);
    virtual bool SetCanFly(bool enable, bool packetOnly = false);
    virtual bool SetWaterWalking(bool enable, bool packetOnly = false);
    virtual bool SetFeatherFall(bool enable, bool packetOnly = false);
    virtual bool SetHover(bool enable, bool packetOnly = false, bool updateAnimationTier = true);

    // pussywizard:
    void SendMovementWaterWalking(Player* sendTo);
    void SendMovementFeatherFall(Player* sendTo);
    void SendMovementHover(Player* sendTo);

    void SetInFront(WorldObject const* target);
    void SetFacingTo(float ori);
    void SetFacingToObject(WorldObject* object);

    void SendChangeCurrentVictimOpcode(HostileReference* pHostileReference);
    void SendClearThreatListOpcode();
    void SendRemoveFromThreatListOpcode(HostileReference* pHostileReference);
    void SendThreatListUpdate();

    void SendClearTarget();

    void BuildHeartBeatMsg(WorldPacket* data) const;

    [[nodiscard]] bool IsAlive() const { return (m_deathState == DeathState::Alive); };
    [[nodiscard]] bool isDying() const { return (m_deathState == DeathState::JustDied); };
    [[nodiscard]] bool isDead() const { return (m_deathState == DeathState::Dead || m_deathState == DeathState::Corpse); };
    //npcbot
    /*
    DeathState getDeathState() { return m_deathState; };
    */
    DeathState getDeathState() const { return m_deathState; };
    //end npcbot
    virtual void setDeathState(DeathState s, bool despawn = false); // overwrited in Creature/Player/Pet

    [[nodiscard]] ObjectGuid GetOwnerGUID() const { return GetGuidValue(UNIT_FIELD_SUMMONEDBY); }
    void SetOwnerGUID(ObjectGuid owner);
    [[nodiscard]] ObjectGuid GetCreatorGUID() const { return GetGuidValue(UNIT_FIELD_CREATEDBY); }
    void SetCreatorGUID(ObjectGuid creator) { SetGuidValue(UNIT_FIELD_CREATEDBY, creator); }
    [[nodiscard]] ObjectGuid GetMinionGUID() const { return GetGuidValue(UNIT_FIELD_SUMMON); }
    void SetMinionGUID(ObjectGuid guid) { SetGuidValue(UNIT_FIELD_SUMMON, guid); }
    [[nodiscard]] ObjectGuid GetCharmerGUID() const { return GetGuidValue(UNIT_FIELD_CHARMEDBY); }
    void SetCharmerGUID(ObjectGuid owner) { SetGuidValue(UNIT_FIELD_CHARMEDBY, owner); }
    [[nodiscard]] ObjectGuid GetCharmGUID() const { return  GetGuidValue(UNIT_FIELD_CHARM); }
    void SetPetGUID(ObjectGuid guid) { m_SummonSlot[SUMMON_SLOT_PET] = guid; }
    [[nodiscard]] ObjectGuid GetPetGUID() const { return m_SummonSlot[SUMMON_SLOT_PET]; }
    void SetCritterGUID(ObjectGuid guid) { SetGuidValue(UNIT_FIELD_CRITTER, guid); }
    [[nodiscard]] ObjectGuid GetCritterGUID() const { return GetGuidValue(UNIT_FIELD_CRITTER); }


    //npcbot
    void SetControlledByPlayer(bool set) { m_ControlledByPlayer = set; }
    GameObject* GetFirstGameObjectById(uint32 id) const;
    void SetCreator(Unit* creator);
    Unit* GetCreator() const { return m_creator; }
    Unit* m_creator = nullptr;
    //end npcbot

    [[nodiscard]] bool IsControlledByPlayer() const { return m_ControlledByPlayer; }
    [[nodiscard]] bool IsCreatedByPlayer() const { return m_CreatedByPlayer; }
    [[nodiscard]] ObjectGuid GetCharmerOrOwnerGUID() const { return GetCharmerGUID() ? GetCharmerGUID() : GetOwnerGUID(); }
    [[nodiscard]] ObjectGuid GetCharmerOrOwnerOrOwnGUID() const
    {
        if (ObjectGuid guid = GetCharmerOrOwnerGUID())
            return guid;

        return GetGUID();
    }
    [[nodiscard]] bool IsCharmedOwnedByPlayerOrPlayer() const { return GetCharmerOrOwnerOrOwnGUID().IsPlayer(); }

    [[nodiscard]] Player* GetSpellModOwner() const;

    [[nodiscard]] Unit* GetOwner() const;
    [[nodiscard]] Guardian* GetGuardianPet() const;
    [[nodiscard]] Minion* GetFirstMinion() const;
    [[nodiscard]] Unit* GetCharmer() const;
    [[nodiscard]] Unit* GetCharm() const;
    [[nodiscard]] Unit* GetCharmerOrOwner() const { return GetCharmerGUID() ? GetCharmer() : GetOwner(); }
    [[nodiscard]] Unit* GetCharmerOrOwnerOrSelf() const
    {
        if (Unit* u = GetCharmerOrOwner())
            return u;

        return (Unit*)this;
    }
    [[nodiscard]] Player* GetCharmerOrOwnerPlayerOrPlayerItself() const;
    [[nodiscard]] Player* GetAffectingPlayer() const;

    void SetMinion(Minion* minion, bool apply);
    void GetAllMinionsByEntry(std::list<Creature*>& Minions, uint32 entry);
    void RemoveAllMinionsByEntry(uint32 entry);
    void SetCharm(Unit* target, bool apply);
    Unit* GetNextRandomRaidMemberOrPet(float radius);
    bool SetCharmedBy(Unit* charmer, CharmType type, AuraApplication const* aurApp = nullptr);
    void RemoveCharmedBy(Unit* charmer);
    void RestoreFaction();

    ControlSet m_Controlled;
    [[nodiscard]] Unit* GetFirstControlled() const;
    void RemoveAllControlled(bool onDeath = false);

    [[nodiscard]] bool IsCharmed() const { return GetCharmerGUID(); }
    [[nodiscard]] bool isPossessed() const { return HasUnitState(UNIT_STATE_POSSESSED); }
    [[nodiscard]] bool isPossessedByPlayer() const { return HasUnitState(UNIT_STATE_POSSESSED) && GetCharmerGUID().IsPlayer(); }
    [[nodiscard]] bool isPossessing() const
    {
        if (Unit* u = GetCharm())
            return u->isPossessed();
        else
            return false;
    }
    bool isPossessing(Unit* u) const { return u->isPossessed() && GetCharmGUID() == u->GetGUID(); }

    CharmInfo* GetCharmInfo() { return m_charmInfo; }
    CharmInfo* InitCharmInfo();
    void DeleteCharmInfo();
    void UpdateCharmAI();
    //Player* GetMoverSource() const;
    SafeUnitPointer m_movedByPlayer;
    SharedVisionList const& GetSharedVisionList() { return m_sharedVision; }
    void AddPlayerToVision(Player* player);
    void RemovePlayerFromVision(Player* player);
    [[nodiscard]] bool HasSharedVision() const { return !m_sharedVision.empty(); }
    void RemoveBindSightAuras();
    void RemoveCharmAuras();

    Pet* CreateTamedPetFrom(Creature* creatureTarget, uint32 spell_id = 0);
    Pet* CreateTamedPetFrom(uint32 creatureEntry, uint32 spell_id = 0);
    bool InitTamedPet(Pet* pet, uint8 level, uint32 spell_id);

    // aura apply/remove helpers - you should better not use these
    Aura* _TryStackingOrRefreshingExistingAura(SpellInfo const* newAura, uint8 effMask, Unit* caster, int32* baseAmount = nullptr, Item* castItem = nullptr, ObjectGuid casterGUID = ObjectGuid::Empty, bool periodicReset = false);
    void _AddAura(UnitAura* aura, Unit* caster);
    AuraApplication* _CreateAuraApplication(Aura* aura, uint8 effMask);
    void _ApplyAuraEffect(Aura* aura, uint8 effIndex);
    void _ApplyAura(AuraApplication* aurApp, uint8 effMask);
    void _UnapplyAura(AuraApplicationMap::iterator& i, AuraRemoveMode removeMode);
    void _UnapplyAura(AuraApplication* aurApp, AuraRemoveMode removeMode);
    void _RemoveNoStackAuraApplicationsDueToAura(Aura* aura);
    void _RemoveNoStackAurasDueToAura(Aura* aura);
    bool _IsNoStackAuraDueToAura(Aura* appliedAura, Aura* existingAura) const;
    void _RegisterAuraEffect(AuraEffect* aurEff, bool apply);

    // m_ownedAuras container management
    AuraMap&       GetOwnedAuras()       { return m_ownedAuras; }
    [[nodiscard]] AuraMap const& GetOwnedAuras() const { return m_ownedAuras; }

    void RemoveOwnedAura(AuraMap::iterator& i, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT);
    void RemoveOwnedAura(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, uint8 reqEffMask = 0, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT);
    void RemoveOwnedAura(Aura* aura, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT);

    Aura* GetOwnedAura(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, ObjectGuid itemCasterGUID = ObjectGuid::Empty, uint8 reqEffMask = 0, Aura* except = nullptr) const;

    // m_appliedAuras container management
    AuraApplicationMap&       GetAppliedAuras()       { return m_appliedAuras; }
    [[nodiscard]] AuraApplicationMap const& GetAppliedAuras() const { return m_appliedAuras; }

    void RemoveAura(AuraApplicationMap::iterator& i, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    void RemoveAura(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, uint8 reqEffMask = 0, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT);
    void RemoveAura(AuraApplication* aurApp, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);
    void RemoveAura(Aura* aur, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT);

    // Convenience methods removing auras by predicate
    void RemoveAppliedAuras(std::function<bool(AuraApplication const*)> const& check);
    void RemoveOwnedAuras(std::function<bool(Aura const*)> const& check);

    // Optimized overloads taking advantage of map key
    void RemoveAppliedAuras(uint32 spellId, std::function<bool(AuraApplication const*)> const& check);
    void RemoveOwnedAuras(uint32 spellId, std::function<bool(Aura const*)> const& check);

    void RemoveAurasDueToSpell(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, uint8 reqEffMask = 0, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT);
    void RemoveAuraFromStack(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, AuraRemoveMode removeMode = AURA_REMOVE_BY_DEFAULT);
    void RemoveAurasDueToSpellByDispel(uint32 spellId, uint32 dispellerSpellId, ObjectGuid casterGUID, Unit* dispeller, uint8 chargesRemoved = 1);
    void RemoveAurasDueToSpellBySteal(uint32 spellId, ObjectGuid casterGUID, Unit* stealer);
    void RemoveAurasDueToItemSpell(uint32 spellId, ObjectGuid castItemGuid);
    void RemoveAurasByType(AuraType auraType, ObjectGuid casterGUID = ObjectGuid::Empty, Aura* except = nullptr, bool negative = true, bool positive = true);
    void RemoveNotOwnSingleTargetAuras();
    void RemoveAurasWithInterruptFlags(uint32 flag, uint32 except = 0, bool isAutoshot = false);
    void RemoveAurasWithAttribute(uint32 flags);
    void RemoveAurasWithFamily(SpellFamilyNames family, uint32 familyFlag1, uint32 familyFlag2, uint32 familyFlag3, ObjectGuid casterGUID);
    void RemoveAurasWithMechanic(uint32 mechanic_mask, AuraRemoveMode removemode = AURA_REMOVE_BY_DEFAULT, uint32 except = 0);
    void RemoveMovementImpairingAuras(bool withRoot);
    void RemoveAurasByShapeShift();

    void RemoveAreaAurasDueToLeaveWorld();
    void RemoveAllAuras();
    void RemoveArenaAuras();
    void RemoveAllAurasOnDeath();
    void RemoveAllAurasRequiringDeadTarget();
    void RemoveAllAurasExceptType(AuraType type);
    //void RemoveAllAurasExceptType(AuraType type1, AuraType type2); // pussywizard: replaced with RemoveEvadeAuras()
    void RemoveEvadeAuras();
    void DelayOwnedAuras(uint32 spellId, ObjectGuid caster, int32 delaytime);

    void _RemoveAllAuraStatMods();
    void _ApplyAllAuraStatMods();

    [[nodiscard]] AuraEffectList const& GetAuraEffectsByType(AuraType type) const { return m_modAuras[type]; }
    AuraList&       GetSingleCastAuras()       { return m_scAuras; }
    [[nodiscard]] AuraList const& GetSingleCastAuras() const { return m_scAuras; }

    [[nodiscard]] AuraEffect* GetAuraEffect(uint32 spellId, uint8 effIndex, ObjectGuid casterGUID = ObjectGuid::Empty) const;
    [[nodiscard]] AuraEffect* GetAuraEffectOfRankedSpell(uint32 spellId, uint8 effIndex, ObjectGuid casterGUID = ObjectGuid::Empty) const;
    [[nodiscard]] AuraEffect* GetAuraEffect(AuraType type, SpellFamilyNames name, uint32 iconId, uint8 effIndex) const; // spell mustn't have familyflags
    [[nodiscard]] AuraEffect* GetAuraEffect(AuraType type, SpellFamilyNames family, uint32 familyFlag1, uint32 familyFlag2, uint32 familyFlag3, ObjectGuid casterGUID = ObjectGuid::Empty) const;
    [[nodiscard]] AuraEffect* GetAuraEffectDummy(uint32 spellid) const;
    [[nodiscard]] inline AuraEffect* GetDummyAuraEffect(SpellFamilyNames name, uint32 iconId, uint8 effIndex) const { return GetAuraEffect(SPELL_AURA_DUMMY, name, iconId, effIndex);}

    AuraApplication* GetAuraApplication(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, ObjectGuid itemCasterGUID = ObjectGuid::Empty, uint8 reqEffMask = 0, AuraApplication* except = nullptr) const;
    [[nodiscard]] Aura* GetAura(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, ObjectGuid itemCasterGUID = ObjectGuid::Empty, uint8 reqEffMask = 0) const;

    AuraApplication* GetAuraApplicationOfRankedSpell(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, ObjectGuid itemCasterGUID = ObjectGuid::Empty, uint8 reqEffMask = 0, AuraApplication* except = nullptr) const;
    [[nodiscard]] Aura* GetAuraOfRankedSpell(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, ObjectGuid itemCasterGUID = ObjectGuid::Empty, uint8 reqEffMask = 0) const;

    void GetDispellableAuraList(Unit* caster, uint32 dispelMask, DispelChargesList& dispelList, SpellInfo const* dispelSpell);

    [[nodiscard]] bool HasAuraEffect(uint32 spellId, uint8 effIndex, ObjectGuid caster = ObjectGuid::Empty) const;
    [[nodiscard]] uint32 GetAuraCount(uint32 spellId) const;
    [[nodiscard]] bool HasAura(uint32 spellId, ObjectGuid casterGUID = ObjectGuid::Empty, ObjectGuid itemCasterGUID = ObjectGuid::Empty, uint8 reqEffMask = 0) const;
    [[nodiscard]] bool HasAuraType(AuraType auraType) const;
    [[nodiscard]] bool HasAuraTypeWithCaster(AuraType auratype, ObjectGuid caster) const;
    [[nodiscard]] bool HasAuraTypeWithMiscvalue(AuraType auratype, int32 miscvalue) const;
    bool HasAuraTypeWithAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const;
    [[nodiscard]] bool HasAuraTypeWithValue(AuraType auratype, int32 value) const;
    [[nodiscard]] bool HasAuraTypeWithTriggerSpell(AuraType auratype, uint32 triggerSpell) const;
    bool HasNegativeAuraWithInterruptFlag(uint32 flag, ObjectGuid guid = ObjectGuid::Empty);
    [[nodiscard]] bool HasVisibleAuraType(AuraType auraType) const;
    bool HasNegativeAuraWithAttribute(uint32 flag, ObjectGuid guid = ObjectGuid::Empty);
    [[nodiscard]] bool HasAuraWithMechanic(uint32 mechanicMask) const;

    AuraEffect* IsScriptOverriden(SpellInfo const* spell, int32 script) const;
    uint32 GetDiseasesByCaster(ObjectGuid casterGUID, uint8 mode = 0);
    [[nodiscard]] uint32 GetDoTsByCaster(ObjectGuid casterGUID) const;

    [[nodiscard]] int32 GetTotalAuraModifierAreaExclusive(AuraType auratype) const;
    [[nodiscard]] int32 GetTotalAuraModifier(AuraType auratype) const;
    [[nodiscard]] float GetTotalAuraMultiplier(AuraType auratype) const;
    int32 GetMaxPositiveAuraModifier(AuraType auratype);
    [[nodiscard]] int32 GetMaxNegativeAuraModifier(AuraType auratype) const;

    [[nodiscard]] int32 GetTotalAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const;
    [[nodiscard]] float GetTotalAuraMultiplierByMiscMask(AuraType auratype, uint32 misc_mask) const;
    int32 GetMaxPositiveAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask, const AuraEffect* except = nullptr) const;
    [[nodiscard]] int32 GetMaxNegativeAuraModifierByMiscMask(AuraType auratype, uint32 misc_mask) const;

    [[nodiscard]] int32 GetTotalAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const;
    [[nodiscard]] float GetTotalAuraMultiplierByMiscValue(AuraType auratype, int32 misc_value) const;
    [[nodiscard]] int32 GetMaxPositiveAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const;
    [[nodiscard]] int32 GetMaxNegativeAuraModifierByMiscValue(AuraType auratype, int32 misc_value) const;

    int32 GetTotalAuraModifierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const;
    float GetTotalAuraMultiplierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const;
    int32 GetMaxPositiveAuraModifierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const;
    int32 GetMaxNegativeAuraModifierByAffectMask(AuraType auratype, SpellInfo const* affectedSpell) const;

    [[nodiscard]] float GetResistanceBuffMods(SpellSchools school, bool positive) const { return GetFloatValue(positive ? static_cast<uint16>(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE) + school : static_cast<uint16>(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE) +  + school); }
    void SetResistanceBuffMods(SpellSchools school, bool positive, float val) { SetFloatValue(positive ? static_cast<uint16>(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE) + school : static_cast<uint16>(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE) +  + school, val); }
    void ApplyResistanceBuffModsMod(SpellSchools school, bool positive, float val, bool apply) { ApplyModSignedFloatValue(positive ? static_cast<uint16>(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE) + school : static_cast<uint16>(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE) +  + school, val, apply); }
    void ApplyResistanceBuffModsPercentMod(SpellSchools school, bool positive, float val, bool apply) { ApplyPercentModFloatValue(positive ? static_cast<uint16>(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE) + school : static_cast<uint16>(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE) +  + school, val, apply); }
    void InitStatBuffMods()
    {
        for (uint8 i = STAT_STRENGTH; i < MAX_STATS; ++i) SetFloatValue(static_cast<uint16>(UNIT_FIELD_POSSTAT0) +  i, 0);
        for (uint8 i = STAT_STRENGTH; i < MAX_STATS; ++i) SetFloatValue(static_cast<uint16>(UNIT_FIELD_NEGSTAT0) +  i, 0);
    }
    void ApplyStatBuffMod(Stats stat, float val, bool apply) { ApplyModSignedFloatValue((val > 0 ? static_cast<uint16>(UNIT_FIELD_POSSTAT0) +  stat : static_cast<uint16>(UNIT_FIELD_NEGSTAT0) +  stat), val, apply); }
    void ApplyStatPercentBuffMod(Stats stat, float val, bool apply);

    void SetCreateStat(Stats stat, float val) { m_createStats[stat] = val; }
    void SetCreateHealth(uint32 val) { SetUInt32Value(UNIT_FIELD_BASE_HEALTH, val); }
    [[nodiscard]] uint32 GetCreateHealth() const { return GetUInt32Value(UNIT_FIELD_BASE_HEALTH); }
    void SetCreateMana(uint32 val) { SetUInt32Value(UNIT_FIELD_BASE_MANA, val); }
    [[nodiscard]] uint32 GetCreateMana() const { return GetUInt32Value(UNIT_FIELD_BASE_MANA); }
    [[nodiscard]] uint32 GetCreatePowers(Powers power) const;
    [[nodiscard]] float GetPosStat(Stats stat) const { return GetFloatValue(static_cast<uint16>(UNIT_FIELD_POSSTAT0) +  stat); }
    [[nodiscard]] float GetNegStat(Stats stat) const { return GetFloatValue(static_cast<uint16>(UNIT_FIELD_NEGSTAT0) +  stat); }
    [[nodiscard]] float GetCreateStat(Stats stat) const { return m_createStats[stat]; }

    void SetCurrentCastedSpell(Spell* pSpell);
    virtual void ProhibitSpellSchool(SpellSchoolMask /*idSchoolMask*/, uint32 /*unTimeMs*/) { }
    void InterruptSpell(CurrentSpellTypes spellType, bool withDelayed = true, bool withInstant = true, bool bySelf = false);
    void FinishSpell(CurrentSpellTypes spellType, bool ok = true);

    // set withDelayed to true to account delayed spells as casted
    // delayed+channeled spells are always accounted as casted
    // we can skip channeled or delayed checks using flags
    [[nodiscard]] bool IsNonMeleeSpellCast(bool withDelayed, bool skipChanneled = false, bool skipAutorepeat = false, bool isAutoshoot = false, bool skipInstant = true) const;

    // set withDelayed to true to interrupt delayed spells too
    // delayed+channeled spells are always interrupted
    void InterruptNonMeleeSpells(bool withDelayed, uint32 spellid = 0, bool withInstant = true, bool bySelf = false);

    [[nodiscard]] Spell* GetCurrentSpell(CurrentSpellTypes spellType) const { return m_currentSpells[spellType]; }
    [[nodiscard]] Spell* GetCurrentSpell(uint32 spellType) const { return m_currentSpells[spellType]; }
    [[nodiscard]] Spell* FindCurrentSpellBySpellId(uint32 spell_id) const;
    [[nodiscard]] int32 GetCurrentSpellCastTime(uint32 spell_id) const;

    [[nodiscard]] virtual bool IsMovementPreventedByCasting() const;

    ObjectGuid m_SummonSlot[MAX_SUMMON_SLOT];
    ObjectGuid m_ObjectSlot[MAX_GAMEOBJECT_SLOT];

    [[nodiscard]] ShapeshiftForm GetShapeshiftForm() const { return ShapeshiftForm(GetByteValue(UNIT_FIELD_BYTES_2, 3)); }
    void SetShapeshiftForm(ShapeshiftForm form)
    {
        SetByteValue(UNIT_FIELD_BYTES_2, 3, form);
    }

    [[nodiscard]] bool IsInFeralForm() const
    {
        ShapeshiftForm form = GetShapeshiftForm();
        return form == FORM_CAT || form == FORM_BEAR || form == FORM_DIREBEAR || form == FORM_GHOSTWOLF; // Xinef: added shamans Ghost Wolf, should behave exactly like druid forms
    }

    [[nodiscard]] bool IsInDisallowedMountForm() const;

    float m_modMeleeHitChance;
    float m_modRangedHitChance;
    float m_modSpellHitChance;
    int32 m_baseSpellCritChance;

    float m_threatModifier[MAX_SPELL_SCHOOL];
    float m_modAttackSpeedPct[3];

    // Event handler
    EventProcessor m_Events;
    TaskScheduler m_scheduler;
    
    // stat system
    bool HandleStatModifier(UnitMods unitMod, UnitModifierType modifierType, float amount, bool apply);
    void SetModifierValue(UnitMods unitMod, UnitModifierType modifierType, float value) { m_auraModifiersGroup[unitMod][modifierType] = value; }
    [[nodiscard]] float GetModifierValue(UnitMods unitMod, UnitModifierType modifierType) const;
    [[nodiscard]] float GetTotalStatValue(Stats stat, float additionalValue = 0.0f) const;
    [[nodiscard]] float GetTotalAuraModValue(UnitMods unitMod) const;
    [[nodiscard]] SpellSchools GetSpellSchoolByAuraGroup(UnitMods unitMod) const;
    [[nodiscard]] Stats GetStatByAuraGroup(UnitMods unitMod) const;
    [[nodiscard]] Powers GetPowerTypeByAuraGroup(UnitMods unitMod) const;
    [[nodiscard]] bool CanModifyStats() const { return m_canModifyStats; }
    void SetCanModifyStats(bool modifyStats) { m_canModifyStats = modifyStats; }
    virtual bool UpdateStats(Stats stat) = 0;
    virtual bool UpdateAllStats() = 0;
    virtual void UpdateResistances(uint32 school) = 0;
    virtual void UpdateAllResistances();
    virtual void UpdateArmor() = 0;
    virtual void UpdateMaxHealth() = 0;
    virtual void UpdateMaxPower(Powers power) = 0;
    virtual void UpdateAttackPowerAndDamage(bool ranged = false) = 0;
    virtual void UpdateDamagePhysical(WeaponAttackType attType);
    float GetTotalAttackPowerValue(WeaponAttackType attType, Unit* pVictim = nullptr) const;
    [[nodiscard]] float GetWeaponDamageRange(WeaponAttackType attType, WeaponDamageRange type, uint8 damageIndex = 0) const;
    void SetBaseWeaponDamage(WeaponAttackType attType, WeaponDamageRange damageRange, float value, uint8 damageIndex = 0) { m_weaponDamage[attType][damageRange][damageIndex] = value; }
    virtual void CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, bool addTotalPct, float& minDamage, float& maxDamage, uint8 damageIndex = 0) = 0;
    uint32 CalculateDamage(WeaponAttackType attType, bool normalized, bool addTotalPct, uint8 itemDamagesMask = 0);
    float GetAPMultiplier(WeaponAttackType attType, bool normalized);

    bool isInFrontInMap(Unit const* target, float distance, float arc = M_PI) const;
    bool isInBackInMap(Unit const* target, float distance, float arc = M_PI) const;

    // Visibility system
    [[nodiscard]] bool IsVisible() const { return m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GM) <= SEC_PLAYER; }
    void SetVisible(bool x);
    void SetModelVisible(bool on);

    // common function for visibility checks for player/creatures with detection code
    [[nodiscard]] uint32 GetPhaseByAuras() const;
    void SetPhaseMask(uint32 newPhaseMask, bool update) override;// overwrite WorldObject::SetPhaseMask
    void UpdateObjectVisibility(bool forced = true, bool fromUpdate = false) override;

    SpellImmuneList m_spellImmune[MAX_SPELL_IMMUNITY];
    uint32 m_lastSanctuaryTime;

    // Threat related methods
    [[nodiscard]] bool CanHaveThreatList() const;
    void AddThreat(Unit* victim, float fThreat, SpellSchoolMask schoolMask = SPELL_SCHOOL_MASK_NORMAL, SpellInfo const* threatSpell = nullptr);
    float ApplyTotalThreatModifier(float fThreat, SpellSchoolMask schoolMask = SPELL_SCHOOL_MASK_NORMAL);
    void TauntApply(Unit* victim);
    void TauntFadeOut(Unit* taunter);
    ThreatMgr& GetThreatMgr() { return m_ThreatMgr; }
    ThreatMgr const& GetThreatMgr() const { return m_ThreatMgr; }
    void addHatedBy(HostileReference* pHostileReference) { m_HostileRefMgr.insertFirst(pHostileReference); };
    void removeHatedBy(HostileReference* /*pHostileReference*/) { /* nothing to do yet */ }
    HostileRefMgr& getHostileRefMgr() { return m_HostileRefMgr; }

    VisibleAuraMap const* GetVisibleAuras() { return &m_visibleAuras; }
    AuraApplication* GetVisibleAura(uint8 slot)
    {
        VisibleAuraMap::iterator itr = m_visibleAuras.find(slot);
        if (itr != m_visibleAuras.end())
            return itr->second;
        return nullptr;
    }
    void SetVisibleAura(uint8 slot, AuraApplication* aur) { m_visibleAuras[slot] = aur; UpdateAuraForGroup(slot);}
    void RemoveVisibleAura(uint8 slot) { m_visibleAuras.erase(slot); UpdateAuraForGroup(slot);}

    [[nodiscard]] uint32 GetInterruptMask() const { return m_interruptMask; }
    void AddInterruptMask(uint32 mask) { m_interruptMask |= mask; }
    void UpdateInterruptMask();

    virtual float GetNativeObjectScale() const { return 1.0f; }
    virtual void RecalculateObjectScale();
    [[nodiscard]] uint32 GetDisplayId() const { return GetUInt32Value(UNIT_FIELD_DISPLAYID); }
    virtual void SetDisplayId(uint32 modelId);
    [[nodiscard]] uint32 GetNativeDisplayId() const { return GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID); }
    void RestoreDisplayId();
    void SetNativeDisplayId(uint32 modelId) { SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, modelId); }
    void setTransForm(uint32 spellid) { m_transform = spellid;}
    [[nodiscard]] uint32 getTransForm() const { return m_transform;}

    // DynamicObject management
    void _RegisterDynObject(DynamicObject* dynObj);
    void _UnregisterDynObject(DynamicObject* dynObj);
    DynamicObject* GetDynObject(uint32 spellId);
    bool RemoveDynObject(uint32 spellId);
    void RemoveAllDynObjects();

    [[nodiscard]] GameObject* GetGameObject(uint32 spellId) const;
    void AddGameObject(GameObject* gameObj);
    void RemoveGameObject(GameObject* gameObj, bool del);
    void RemoveGameObject(uint32 spellid, bool del);
    void RemoveAllGameObjects();

    void ModifyAuraState(AuraStateType flag, bool apply);
    uint32 BuildAuraStateUpdateForTarget(Unit* target) const;
    bool HasAuraState(AuraStateType flag, SpellInfo const* spellProto = nullptr, Unit const* Caster = nullptr) const;
    void UnsummonAllTotems(bool onDeath = false);
    Unit* GetMagicHitRedirectTarget(Unit* victim, SpellInfo const* spellInfo);
    Unit* GetMeleeHitRedirectTarget(Unit* victim, SpellInfo const* spellInfo = nullptr);

    int32 SpellBaseDamageBonusDone(SpellSchoolMask schoolMask);
    int32 SpellBaseDamageBonusTaken(SpellSchoolMask schoolMask, bool isDoT = false);
    float SpellPctDamageModsDone(Unit* victim, SpellInfo const* spellProto, DamageEffectType damagetype);
    uint32 SpellDamageBonusDone(Unit* victim, SpellInfo const* spellProto, uint32 pdamage, DamageEffectType damagetype, uint8 effIndex, float TotalMod = 0.0f, uint32 stack = 1);
    uint32 SpellDamageBonusTaken(Unit* caster, SpellInfo const* spellProto, uint32 pdamage, DamageEffectType damagetype, uint32 stack = 1);
    int32 SpellBaseHealingBonusDone(SpellSchoolMask schoolMask);
    int32 SpellBaseHealingBonusTaken(SpellSchoolMask schoolMask);
    float SpellPctHealingModsDone(Unit* victim, SpellInfo const* spellProto, DamageEffectType damagetype);
    uint32 SpellHealingBonusDone(Unit* victim, SpellInfo const* spellProto, uint32 healamount, DamageEffectType damagetype, uint8 effIndex, float TotalMod = 0.0f, uint32 stack = 1);
    uint32 SpellHealingBonusTaken(Unit* caster, SpellInfo const* spellProto, uint32 healamount, DamageEffectType damagetype, uint32 stack = 1);

    uint32 MeleeDamageBonusDone(Unit* pVictim, uint32 damage, WeaponAttackType attType, SpellInfo const* spellProto = nullptr, SpellSchoolMask damageSchoolMask = SPELL_SCHOOL_MASK_NORMAL);
    uint32 MeleeDamageBonusTaken(Unit* attacker, uint32 pdamage, WeaponAttackType attType, SpellInfo const* spellProto = nullptr, SpellSchoolMask damageSchoolMask = SPELL_SCHOOL_MASK_NORMAL);

    bool   isSpellBlocked(Unit* victim, SpellInfo const* spellProto, WeaponAttackType attackType = BASE_ATTACK);
    bool   isBlockCritical();
    float  SpellDoneCritChance(Unit const* /*victim*/, SpellInfo const* spellProto, SpellSchoolMask schoolMask, WeaponAttackType attackType, bool skipEffectCheck) const;
    float  SpellTakenCritChance(Unit const* caster, SpellInfo const* spellProto, SpellSchoolMask schoolMask, float doneChance, WeaponAttackType attackType, bool skipEffectCheck) const;
    static uint32 SpellCriticalDamageBonus(Unit const* caster, SpellInfo const* spellProto, uint32 damage, Unit const* victim);
    static uint32 SpellCriticalHealingBonus(Unit const* caster, SpellInfo const* spellProto, uint32 damage, Unit const* victim);

    void SetLastManaUse(uint32 spellCastTime) { m_lastManaUse = spellCastTime; }
    [[nodiscard]] bool IsUnderLastManaUseEffect() const;

    void SetContestedPvP(Player* attackedPlayer = nullptr, bool lookForNearContestedGuards = true);

    uint32 GetCastingTimeForBonus(SpellInfo const* spellProto, DamageEffectType damagetype, uint32 CastingTime) const;
    float CalculateDefaultCoefficient(SpellInfo const* spellInfo, DamageEffectType damagetype) const;

    void CastDelayedSpellWithPeriodicAmount(Unit* caster, uint32 spellId, AuraType auraType, int32 addAmount, uint8 effectIndex = 0);

    void ApplySpellImmune(uint32 spellId, uint32 op, uint32 type, bool apply, SpellImmuneBlockType blockType = SPELL_BLOCK_TYPE_ALL);
    void ApplySpellDispelImmunity(SpellInfo const* spellProto, DispelType type, bool apply);
    //npcbot
    /*
    virtual bool IsImmunedToSpell(SpellInfo const* spellInfo, Spell const* spell = nullptr);
    */
    virtual bool IsImmunedToSpell(SpellInfo const* spellInfo, Spell const* spell = nullptr) const;
    //end npcbot
    // redefined in Creature
    [[nodiscard]] bool IsImmunedToDamage(SpellSchoolMask meleeSchoolMask) const;
    [[nodiscard]] bool IsImmunedToDamage(SpellInfo const* spellInfo) const;
    [[nodiscard]] bool IsImmunedToDamage(Spell const* spell) const;
    [[nodiscard]] bool IsImmunedToSchool(SpellSchoolMask meleeSchoolMask) const;
    [[nodiscard]] bool IsImmunedToSchool(SpellInfo const* spellInfo) const;
    [[nodiscard]] bool IsImmunedToSchool(Spell const* spell) const;
    [[nodiscard]] bool IsImmunedToDamageOrSchool(SpellSchoolMask meleeSchoolMask) const;
    bool IsImmunedToDamageOrSchool(SpellInfo const* spellInfo) const;
    virtual bool IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index) const;
    // redefined in Creature
    static bool IsDamageReducedByArmor(SpellSchoolMask damageSchoolMask, SpellInfo const* spellInfo = nullptr, uint8 effIndex = MAX_SPELL_EFFECTS);
    static uint32 CalcArmorReducedDamage(Unit const* attacker, Unit const* victim, const uint32 damage, SpellInfo const* spellInfo, uint8 attackerLevel = 0, WeaponAttackType attackType = MAX_ATTACK);
    static void CalcAbsorbResist(DamageInfo& dmgInfo, bool Splited = false);
    static void CalcHealAbsorb(HealInfo& healInfo);

    void  UpdateSpeed(UnitMoveType mtype, bool forced);
    [[nodiscard]] float GetSpeed(UnitMoveType mtype) const;
    [[nodiscard]] float GetSpeedRate(UnitMoveType mtype) const { return m_speed_rate[mtype]; }
    void SetSpeed(UnitMoveType mtype, float rate, bool forced = false);
    void SetSpeedRate(UnitMoveType mtype, float rate) { m_speed_rate[mtype] = rate; }

    float ApplyEffectModifiers(SpellInfo const* spellProto, uint8 effect_index, float value) const;
    int32 CalculateSpellDamage(Unit const* target, SpellInfo const* spellProto, uint8 effect_index, int32 const* basePoints = nullptr) const;
    int32 CalcSpellDuration(SpellInfo const* spellProto);
    int32 ModSpellDuration(SpellInfo const* spellProto, Unit const* target, int32 duration, bool positive, uint32 effectMask);
    void  ModSpellCastTime(SpellInfo const* spellProto, int32& castTime, Spell* spell = nullptr);
    float CalculateLevelPenalty(SpellInfo const* spellProto) const;

    void addFollower(FollowerReference* pRef) { m_FollowingRefMgr.insertFirst(pRef); }
    void removeFollower(FollowerReference* /*pRef*/) { /* nothing to do yet */ }

    MotionMaster* GetMotionMaster() { return i_motionMaster; }
    [[nodiscard]] const MotionMaster* GetMotionMaster() const { return i_motionMaster; }
    [[nodiscard]] virtual MovementGeneratorType GetDefaultMovementType() const;

    [[nodiscard]] bool IsStopped() const { return !(HasUnitState(UNIT_STATE_MOVING)); }
    void StopMoving();
    void StopMovingOnCurrentPos();
    virtual void PauseMovement(uint32 timer = 0, uint8 slot = 0); // timer in ms
    void ResumeMovement(uint32 timer = 0, uint8 slot = 0);

    void AddUnitMovementFlag(uint32 f) { m_movementInfo.flags |= f; }
    void RemoveUnitMovementFlag(uint32 f) { m_movementInfo.flags &= ~f; }
    [[nodiscard]] bool HasUnitMovementFlag(uint32 f) const { return (m_movementInfo.flags & f) == f; }
    [[nodiscard]] uint32 GetUnitMovementFlags() const { return m_movementInfo.flags; }
    void SetUnitMovementFlags(uint32 f) { m_movementInfo.flags = f; }

    void AddExtraUnitMovementFlag(uint16 f) { m_movementInfo.flags2 |= f; }
    void RemoveExtraUnitMovementFlag(uint16 f) { m_movementInfo.flags2 &= ~f; }
    [[nodiscard]] uint16 HasExtraUnitMovementFlag(uint16 f) const { return m_movementInfo.flags2 & f; }
    [[nodiscard]] uint16 GetExtraUnitMovementFlags() const { return m_movementInfo.flags2; }
    void SetExtraUnitMovementFlags(uint16 f) { m_movementInfo.flags2 = f; }

    void SetControlled(bool apply, UnitState state, Unit* source = nullptr, bool isFear = false);
    void DisableRotate(bool apply);
    void DisableSpline();

    ///-----------Combo point system-------------------
       // This unit having CP on other units
    [[nodiscard]] uint8 GetComboPoints(Unit const* who = nullptr) const { return (who && m_comboTarget != who) ? 0 : m_comboPoints; }
    [[nodiscard]] uint8 GetComboPoints(ObjectGuid const& guid) const { return (m_comboTarget && m_comboTarget->GetGUID() == guid) ? m_comboPoints : 0; }
    [[nodiscard]] Unit* GetComboTarget() const { return m_comboTarget; }
    [[nodiscard]] ObjectGuid const GetComboTargetGUID() const { return m_comboTarget ? m_comboTarget->GetGUID() : ObjectGuid::Empty; }
    void AddComboPoints(Unit* target, int8 count);
    void AddComboPoints(int8 count) { AddComboPoints(nullptr, count); }
    void ClearComboPoints();
    void SendComboPoints();
    // Other units having CP on this unit
    void AddComboPointHolder(Unit* unit) { m_ComboPointHolders.insert(unit); }
    void RemoveComboPointHolder(Unit* unit) { m_ComboPointHolders.erase(unit); }
    void ClearComboPointHolders();

    ///----------Pet responses methods-----------------
    void SendPetActionFeedback (uint8 msg);
    void SendPetTalk (uint32 pettalk);
    void SendPetAIReaction(ObjectGuid guid);
    ///----------End of Pet responses methods----------

    void propagateSpeedChange() { GetMotionMaster()->propagateSpeedChange(); }

    // reactive attacks
    void ClearAllReactives();
    void StartReactiveTimer(ReactiveType reactive) { m_reactiveTimer[reactive] = REACTIVE_TIMER_START;}
    void UpdateReactives(uint32 p_time);

    // group updates
    void UpdateAuraForGroup(uint8 slot);

    // proc trigger system
    bool CanProc() {return !m_procDeep;}
    void SetCantProc(bool apply)
    {
        if (apply)
            ++m_procDeep;
        else
        {
            ASSERT(m_procDeep);
            --m_procDeep;
        }
    }

    // pet auras
    typedef std::set<PetAura const*> PetAuraSet;
    PetAuraSet m_petAuras;
    void AddPetAura(PetAura const* petSpell);
    void RemovePetAura(PetAura const* petSpell);
    void CastPetAura(PetAura const* aura);
    bool IsPetAura(Aura const* aura);

    [[nodiscard]] uint32 GetModelForForm(ShapeshiftForm form, uint32 spellId) const;
    uint32 GetModelForTotem(PlayerTotemType totemType);

    // Redirect Threat
    void SetRedirectThreat(ObjectGuid guid, uint32 pct) { _redirectThreatInfo.Set(guid, pct); }
    void ResetRedirectThreat() { SetRedirectThreat(ObjectGuid::Empty, 0); }
    void ModifyRedirectThreat(int32 amount) { _redirectThreatInfo.ModifyThreatPct(amount); }
    uint32 GetRedirectThreatPercent() { return _redirectThreatInfo.GetThreatPct(); }
    [[nodiscard]] Unit* GetRedirectThreatTarget() const;

    bool IsAIEnabled, NeedChangeAI;
    bool CreateVehicleKit(uint32 id, uint32 creatureEntry);
    void RemoveVehicleKit();
    [[nodiscard]] Vehicle* GetVehicleKit()const { return m_vehicleKit; }
    [[nodiscard]] Vehicle* GetVehicle()   const { return m_vehicle; }
    bool IsOnVehicle(Unit const* vehicle) const { return m_vehicle && m_vehicle == vehicle->GetVehicleKit(); }
    [[nodiscard]] Unit* GetVehicleBase()  const;
    [[nodiscard]] Creature* GetVehicleCreatureBase() const;
    [[nodiscard]] ObjectGuid GetTransGUID() const override;
    /// Returns the transport this unit is on directly (if on vehicle and transport, return vehicle)
    [[nodiscard]] TransportBase* GetDirectTransport() const;

    bool m_ControlledByPlayer;
    bool m_CreatedByPlayer;

    bool HandleSpellClick(Unit* clicker, int8 seatId = -1);
    void EnterVehicle(Unit* base, int8 seatId = -1);
    void EnterVehicleUnattackable(Unit* base, int8 seatId = -1);
    void ExitVehicle(Position const* exitPosition = nullptr);
    void ChangeSeat(int8 seatId, bool next = true);

    // Should only be called by AuraEffect::HandleAuraControlVehicle(AuraApplication const* auraApp, uint8 mode, bool apply) const;
    void _ExitVehicle(Position const* exitPosition = nullptr);
    void _EnterVehicle(Vehicle* vehicle, int8 seatId, AuraApplication const* aurApp = nullptr);

    void BuildMovementPacket(ByteBuffer* data) const;

    [[nodiscard]] virtual bool CanSwim() const;
    [[nodiscard]] bool IsLevitating() const { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY); }
    [[nodiscard]] bool IsWalking() const { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_WALKING); }
    [[nodiscard]] bool isMoving() const   { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_MASK_MOVING); }
    [[nodiscard]] bool isTurning() const  { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_MASK_TURNING); }
    [[nodiscard]] bool IsHovering() const { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_HOVER); }
    [[nodiscard]] bool isSwimming() const { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_SWIMMING); }
    [[nodiscard]] virtual bool CanFly() const = 0;
    [[nodiscard]] bool IsFlying() const { return m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FLYING | MOVEMENTFLAG_DISABLE_GRAVITY); }
    [[nodiscard]] bool IsFalling() const;
    [[nodiscard]] float GetHoverHeight() const { return IsHovering() ? GetFloatValue(UNIT_FIELD_HOVERHEIGHT) : 0.0f; }
    [[nodiscard]] virtual bool CanEnterWater() const = 0;

    void RewardRage(uint32 damage, uint32 weaponSpeedHitFactor, bool attacker);

    [[nodiscard]] virtual float GetFollowAngle() const { return static_cast<float>(M_PI / 2); }

    void OutDebugInfo() const;
    [[nodiscard]] virtual bool isBeingLoaded() const { return false;}
    [[nodiscard]] bool IsDuringRemoveFromWorld() const {return m_duringRemoveFromWorld;}

    Pet* ToPet() { if (IsPet()) return reinterpret_cast<Pet*>(this); else return nullptr; }
    Totem* ToTotem() { if (IsTotem()) return reinterpret_cast<Totem*>(this); else return nullptr; }
    TempSummon* ToTempSummon() { if (IsSummon()) return reinterpret_cast<TempSummon*>(this); else return nullptr; }
    [[nodiscard]] const TempSummon* ToTempSummon() const { if (IsSummon()) return reinterpret_cast<const TempSummon*>(this); else return nullptr; }

    // Safe mover
    std::set<SafeUnitPointer*> SafeUnitPointerSet;
    void AddPointedBy(SafeUnitPointer* sup) { SafeUnitPointerSet.insert(sup); }
    void RemovePointedBy(SafeUnitPointer* sup) { SafeUnitPointerSet.erase(sup); }
    static void HandleSafeUnitPointersOnDelete(Unit* thisUnit);
    // Relocation Nofier optimization
    Position m_last_notify_position;
    uint32 m_last_notify_mstime;
    uint16 m_delayed_unit_relocation_timer;
    uint16 m_delayed_unit_ai_notify_timer;
    bool bRequestForcedVisibilityUpdate;
    void ExecuteDelayedUnitRelocationEvent();
    void ExecuteDelayedUnitAINotifyEvent();

    // cooldowns
    [[nodiscard]] virtual bool HasSpellCooldown(uint32 /*spell_id*/) const { return false; }
    [[nodiscard]] virtual bool HasSpellItemCooldown(uint32 /*spell_id*/, uint32 /*itemid*/) const { return false; }
    virtual void AddSpellCooldown(uint32 /*spell_id*/, uint32 /*itemid*/, uint32 /*end_time*/, bool needSendToClient = false, bool forceSendToSpectator = false)
    {
        // workaround for unused parameters
        (void)needSendToClient;
        (void)forceSendToSpectator;
    }

    //npcbot
    /*
    //end npcbot
    [[nodiscard]] bool CanApplyResilience() const { return m_applyResilience; }
    //npcbot
    */
    [[nodiscard]] bool CanApplyResilience() const;
    //end npcbot

    void PetSpellFail(SpellInfo const* spellInfo, Unit* target, uint32 result);

    int32 CalculateAOEDamageReduction(int32 damage, uint32 schoolMask, Unit* caster) const;

    [[nodiscard]] ObjectGuid GetTarget() const { return GetGuidValue(UNIT_FIELD_TARGET); }
    virtual void SetTarget(ObjectGuid /*guid*/ = ObjectGuid::Empty) = 0;

    void SetInstantCast(bool set) { _instantCast = set; }
    [[nodiscard]] bool CanInstantCast() const { return _instantCast; }

    // Movement info
    Movement::MoveSpline* movespline;

    //npcbot: TC method transfer
    bool IsHighestExclusiveAuraEffect(SpellInfo const* spellInfo, AuraType auraType, int32 effectAmount, uint8 auraEffectMask, bool removeOtherAuraApplications = false);
    //end npcbot

    virtual void Talk(std::string_view text, ChatMsg msgType, Language language, float textRange, WorldObject const* target);
    virtual void Say(std::string_view text, Language language, WorldObject const* target = nullptr);
    virtual void Yell(std::string_view text, Language language, WorldObject const* target = nullptr);
    virtual void TextEmote(std::string_view text, WorldObject const* target = nullptr, bool isBossEmote = false);
    virtual void Whisper(std::string_view text, Language language, Player* target, bool isBossWhisper = false);
    virtual void Talk(uint32 textId, ChatMsg msgType, float textRange, WorldObject const* target);
    virtual void Say(uint32 textId, WorldObject const* target = nullptr);
    virtual void Yell(uint32 textId, WorldObject const* target = nullptr);
    virtual void TextEmote(uint32 textId, WorldObject const* target = nullptr, bool isBossEmote = false);
    virtual void Whisper(uint32 textId, Player* target, bool isBossWhisper = false);

    [[nodiscard]] float GetCollisionHeight() const override;
    [[nodiscard]] float GetCollisionWidth() const override;
    [[nodiscard]] float GetCollisionRadius() const override;

    void ProcessPositionDataChanged(PositionFullTerrainStatus const& data) override;
    virtual void ProcessTerrainStatusUpdate();

    [[nodiscard]] bool CanRestoreMana(SpellInfo const* spellInfo) const;

    std::string GetDebugInfo() const override;

    //npcbot
    bool HasReactive(ReactiveType reactive) const { return m_reactiveTimer[reactive] > 0; }
    void ClearReactive(ReactiveType reactive);
    //end npcbot

    [[nodiscard]] uint32 GetOldFactionId() const { return _oldFactionId; }

protected:
    explicit Unit (bool isWorldObject);

    void BuildValuesUpdate(uint8 updateType, ByteBuffer* data, Player* target) override;

    UnitAI* i_AI, *i_disabledAI;

    void _UpdateSpells(uint32 time);
    void _DeleteRemovedAuras();

    void _UpdateAutoRepeatSpell();

    uint8 m_realRace;
    uint8 m_race;

    bool m_AutoRepeatFirstCast;

    int32 m_attackTimer[MAX_ATTACK];

    float m_createStats[MAX_STATS];

    AttackerSet m_attackers;
    Unit* m_attacking;

    DeathState m_deathState;

    int32 m_procDeep;

    typedef std::list<DynamicObject*> DynObjectList;
    DynObjectList m_dynObj;

    typedef GuidList GameObjectList;
    GameObjectList m_gameObj;
    uint32 m_transform;

    Spell* m_currentSpells[CURRENT_MAX_SPELL];

    AuraMap m_ownedAuras;
    AuraApplicationMap m_appliedAuras;
    AuraList m_removedAuras;
    AuraMap::iterator m_auraUpdateIterator;
    uint32 m_removedAurasCount;

    AuraEffectList m_modAuras[TOTAL_AURAS];
    AuraList m_scAuras;                        // casted singlecast auras
    AuraApplicationList m_interruptableAuras;             // auras which have interrupt mask applied on unit
    AuraStateAurasMap m_auraStateAuras;        // Used for improve performance of aura state checks on aura apply/remove
    uint32 m_interruptMask;

    float m_auraModifiersGroup[UNIT_MOD_END][MODIFIER_TYPE_END];
    float m_weaponDamage[MAX_ATTACK][MAX_WEAPON_DAMAGE_RANGE][MAX_ITEM_PROTO_DAMAGES];
    bool m_canModifyStats;
    VisibleAuraMap m_visibleAuras;

    float m_speed_rate[MAX_MOVE_TYPE];

    CharmInfo* m_charmInfo;
    SharedVisionList m_sharedVision;

    MotionMaster* i_motionMaster;

    uint32 m_reactiveTimer[MAX_REACTIVE];
    int32 m_regenTimer;

    ThreatMgr m_ThreatMgr;
    typedef std::map<ObjectGuid, float> CharmThreatMap;
    CharmThreatMap _charmThreatInfo;

    Vehicle* m_vehicle;
    Vehicle* m_vehicleKit;

    uint32 m_unitTypeMask;
    LiquidTypeEntry const* _lastLiquid;

    // xinef: apply resilience
    bool m_applyResilience;
    bool IsAlwaysVisibleFor(WorldObject const* seer) const override;
    bool IsAlwaysDetectableFor(WorldObject const* seer) const override;
    bool _instantCast;

private:
    bool IsTriggeredAtSpellProcEvent(Unit* victim, Aura* aura, WeaponAttackType attType, bool isVictim, bool active, SpellProcEventEntry const*& spellProcEvent, ProcEventInfo const& eventInfo);
    bool HandleDummyAuraProc(Unit* victim, uint32 damage, AuraEffect* triggeredByAura, SpellInfo const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown, ProcEventInfo const& eventInfo);
    bool HandleAuraProc(Unit* victim, uint32 damage, Aura* triggeredByAura, SpellInfo const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown, bool* handled);
    bool HandleProcTriggerSpell(Unit* victim, uint32 damage, AuraEffect* triggeredByAura, SpellInfo const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown, uint32 procPhase, ProcEventInfo& eventInfo);
    bool HandleOverrideClassScriptAuraProc(Unit* victim, uint32 damage, AuraEffect* triggeredByAura, SpellInfo const* procSpell, uint32 cooldown);
    bool HandleAuraRaidProcFromChargeWithValue(AuraEffect* triggeredByAura);
    bool HandleAuraRaidProcFromCharge(AuraEffect* triggeredByAura);

    void UpdateSplineMovement(uint32 t_diff);
    void UpdateSplinePosition();

    // player or player's pet
    [[nodiscard]] float GetCombatRatingReduction(CombatRating cr) const;
    [[nodiscard]] uint32 GetCombatRatingDamageReduction(CombatRating cr, float rate, float cap, uint32 damage) const;

    void PatchValuesUpdate(ByteBuffer& valuesUpdateBuf, BuildValuesCachePosPointers& posPointers, Player* target);
    void InvalidateValuesUpdateCache() { _valuesUpdateCache.clear(); }

protected:
    void SetFeared(bool apply, Unit* fearedBy = nullptr, bool isFear = false);
    void SetConfused(bool apply);
    void SetStunned(bool apply);
    void SetRooted(bool apply, bool isStun = false);

    uint32 m_rootTimes;

private:
    uint32 m_state;                                     // Even derived shouldn't modify
    uint32 m_CombatTimer;
    uint32 m_lastManaUse;                               // msecs
    //TimeTrackerSmall m_movesplineTimer;

    Diminishing m_Diminishing;
    // Manage all Units that are threatened by us
    HostileRefMgr m_HostileRefMgr;

    FollowerRefMgr m_FollowingRefMgr;

    Unit* m_comboTarget;
    int8 m_comboPoints;
    std::unordered_set<Unit*> m_ComboPointHolders;

    RedirectThreatInfo _redirectThreatInfo;

    bool m_cleanupDone; // lock made to not add stuff after cleanup before delete
    bool m_duringRemoveFromWorld; // lock made to not add stuff after begining removing from world

    uint32 _oldFactionId;           ///< faction before charm
    bool _isWalkingBeforeCharm;     ///< Are we walking before we were charmed?

    [[nodiscard]] float processDummyAuras(float TakenTotalMod) const;

    uint32 _lastExtraAttackSpell;
    std::unordered_map<ObjectGuid /*guid*/, uint32 /*count*/> extraAttacksTargets;
    ObjectGuid _lastDamagedTargetGuid;

    typedef std::unordered_map<uint64 /*visibleFlag(uint32) + updateType(uint8)*/, BuildValuesCachedBuffer>  ValuesUpdateCache;
    ValuesUpdateCache _valuesUpdateCache;
};

namespace Acore
{
    // Binary predicate for sorting Units based on percent value of a power
    class PowerPctOrderPred
    {
    public:
        PowerPctOrderPred(Powers power, bool ascending = true) : _power(power), _ascending(ascending) { }

        bool operator()(WorldObject const* objA, WorldObject const* objB) const
        {
            Unit const* a = objA->ToUnit();
            Unit const* b = objB->ToUnit();
            float rA = (a && a->GetMaxPower(_power)) ? float(a->GetPower(_power)) / float(a->GetMaxPower(_power)) : 0.0f;
            float rB = (b && b->GetMaxPower(_power)) ? float(b->GetPower(_power)) / float(b->GetMaxPower(_power)) : 0.0f;
            return _ascending ? rA < rB : rA > rB;
        }

        bool operator()(Unit const* a, Unit const* b) const
        {
            float rA = a->GetMaxPower(_power) ? float(a->GetPower(_power)) / float(a->GetMaxPower(_power)) : 0.0f;
            float rB = b->GetMaxPower(_power) ? float(b->GetPower(_power)) / float(b->GetMaxPower(_power)) : 0.0f;
            return _ascending ? rA < rB : rA > rB;
        }

    private:
        Powers const _power;
        bool const _ascending;
    };

    // Binary predicate for sorting Units based on percent value of health
    class HealthPctOrderPred
    {
    public:
        HealthPctOrderPred(bool ascending = true) : _ascending(ascending) { }

        bool operator()(WorldObject const* objA, WorldObject const* objB) const
        {
            Unit const* a = objA->ToUnit();
            Unit const* b = objB->ToUnit();
            float rA = (a && a->GetMaxHealth()) ? float(a->GetHealth()) / float(a->GetMaxHealth()) : 0.0f;
            float rB = (b && b->GetMaxHealth()) ? float(b->GetHealth()) / float(b->GetMaxHealth()) : 0.0f;
            return _ascending ? rA < rB : rA > rB;
        }

        bool operator() (Unit const* a, Unit const* b) const
        {
            float rA = a->GetMaxHealth() ? float(a->GetHealth()) / float(a->GetMaxHealth()) : 0.0f;
            float rB = b->GetMaxHealth() ? float(b->GetHealth()) / float(b->GetMaxHealth()) : 0.0f;
            return _ascending ? rA < rB : rA > rB;
        }

    private:
        bool const _ascending;
    };
}

class RedirectSpellEvent : public BasicEvent
{
public:
    RedirectSpellEvent(Unit& self, ObjectGuid auraOwnerGUID, AuraEffect* auraEffect) : _self(self), _auraOwnerGUID(auraOwnerGUID), _auraEffect(auraEffect) { }
    bool Execute(uint64 e_time, uint32 p_time) override;

protected:
    Unit& _self;
    ObjectGuid _auraOwnerGUID;
    AuraEffect* _auraEffect;
};

class VehicleDespawnEvent : public BasicEvent
{
public:
    VehicleDespawnEvent(Unit& self, uint32 duration) : _self(self), _duration(duration) { }
    bool Execute(uint64 e_time, uint32 p_time) override;

protected:
    Unit& _self;
    uint32 _duration;
};

#endif

#include "bot_ai.h"
#include "Chat.h"
#include "botmgr.h"
#include "botspell.h"
#include "bottext.h"
#include "bottraits.h"
#include "Group.h"
#include "Map.h"
#include "MotionMaster.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "TemporarySummon.h"
/*
Priest NpcBot (reworked by Trickerer onlysuffering@gmail.com)
Complete - Around 90%
TODO: Mana Burn, Binding Heal, Lightwell
*/

enum PriestBaseSpells
{
    DISPEL_MAGIC_1                      = 527,
    MASS_DISPEL_1                       = 32375,
    CURE_DISEASE_1                      = 528,
    ABOLISH_DISEASE_1                   = 552,
    FEAR_WARD_1                         = 6346,
    PAIN_SUPPRESSION_1                  = 33206,
    PSYCHIC_SCREAM_1                    = 8122,
    FADE_1                              = 586,
    PSYCHIC_HORROR_1                    = 64044,
    SILENCE_1                           = 15487,
    PENANCE_1                           = 47540,
    VAMPIRIC_EMBRACE_1                  = 15286,
    DISPERSION_1                        = 47585,
    MIND_SEAR_1                         = 48045,
    GUARDIAN_SPIRIT_1                   = 47788,
    SHACKLE_UNDEAD_1                    = 9484,
    LESSER_HEAL_1                       = 2050,
    NORMAL_HEAL_1                       = 2054,
    GREATER_HEAL_1                      = 2060,
    RENEW_1                             = 139,
    FLASH_HEAL_1                        = 2061,
    PRAYER_OF_HEALING_1                 = 596,
    CIRCLE_OF_HEALING_1                 = 34861,
    DIVINE_HYMN_1                       = 64843,
    PRAYER_OF_MENDING_1                 = 33076,
    RESURRECTION_1                      = 2006,
    PW_SHIELD_1                         = 17,
    INNER_FIRE_1                        = 588,
    PW_FORTITUDE_1                      = 1243,
    SHADOW_PROTECTION_1                 = 976,
    DIVINE_SPIRIT_1                     = 14752,
    HOLY_FIRE_1                         = 14914,
    SMITE_1                             = 585,
    SW_PAIN_1                           = 589,
    MIND_BLAST_1                        = 8092,
    SW_DEATH_1                          = 32379,
    DEVOURING_PLAGUE_1                  = 2944,
    MIND_FLAY_1                         = 15407,
    VAMPIRIC_TOUCH_1                    = 34914,
    SHADOWFORM_1                        = 15473,
    INNER_FOCUS_1                       = 14751,
    DESPERATE_PRAYER_1                  = 19236,
    POWER_INFUSION_1                    = 10060,
    HYMN_OF_HOPE_1                      = 64901,

    LEVITATE_1                          = 1706,
    SPELL_ID_THORIUM_GRENADE            = 19769
};
enum PriestPassives
{
//Talents
    UNBREAKABLE_WILL                = 14791,//rank 5
    SPIRIT_TAP                      = 15336,//rank 3
    IMPROVED_SPIRIT_TAP             = 15338,//rank 2
    MEDITATION                      = 14777,//rank 3
    INSPIRATION1                    = 14892,
    INSPIRATION2                    = 15362,
    INSPIRATION3                    = 15363,
    SHADOW_WEAVING1                 = 15257,
    SHADOW_WEAVING2                 = 15331,
    SHADOW_WEAVING3                 = 15332,
    SURGE_OF_LIGHT                  = 33154,//rank 2
    IMPROVED_DEVOURING_PLAGUE       = 63627,//rank 3
    HOLY_CONCENTRATION              = 34860,//rank 3
    RENEWED_HOPE                    = 57472,//rank 3
    RAPTURE                         = 47537,//rank 3
    BODY_AND_SOUL1                  = 64127,
    SERENDIPITY                     = 63737,//rank 3
    IMPROVED_SHADOWFORM             = 47570,//rank 2
    MISERY1                         = 33191,
    MISERY2                         = 33192,
    MISERY3                         = 33193,
    DIVINE_AEGIS                    = 47515,//rank 3
    GRACE                           = 47517,//rank 2
    EMPOWERED_RENEW1                = 63534,
    EMPOWERED_RENEW2                = 63542,
    EMPOWERED_RENEW3                = 63543,
    BORROWED_TIME                   = 52800,//rank 5
//Glyphs
    //GLYPH_SW_PAIN                   = 55681,
    GLYPH_PW_SHIELD                 = 55672,
    GLYPH_DISPEL_MAGIC              = 55677,
    GLYPH_PRAYER_OF_HEALING         = 55680,
    GLYPH_SHADOW                    = 55689,
//other
    PRIEST_T10_2P_BONUS             = 70770 //33% renew
};
enum PriestSpecial
{
    SHADOW_WEAVING_BUFF             = 15258,
    MIND_FLAY_DAMAGE                = 58381,
    MIND_SEAR_DAMAGE_1              = 49821,
    SW_DEATH_BACKLASH               = 32409,
    WEAKENED_SOUL_DEBUFF            = 6788,
    SURGE_OF_LIGHT_BUFF             = 33151,
    SERENDIPITY_BUFF                = 63734,
    DIVINE_HYMN_HEAL                = 64844,
    PRAYER_OF_MENDING_AURA_1        = 41635,
    PRAYER_OF_MENDING_HEAL          = 33110,
    PENANCE_HEAL_1                  = 47750,
    IMPROVED_MIND_BLAST_DEBUFF      = 48301,//Mind Trauma
    HYMN_OF_HOPE_BUFF               = 64904,

    SHADOWFIEND_1                   = 34433
};

const uint32 THORIUM_GRENADE_SPELL_ID = 19769;

static const uint32 Priest_spells_damage_arr[] =
{ DEVOURING_PLAGUE_1, HOLY_FIRE_1, MIND_BLAST_1, MIND_FLAY_1, MIND_SEAR_1, PENANCE_1, SMITE_1, SW_PAIN_1, SW_DEATH_1,
VAMPIRIC_TOUCH_1 };

static const uint32 Priest_spells_cc_arr[] =
{ PSYCHIC_HORROR_1, PSYCHIC_SCREAM_1, SHACKLE_UNDEAD_1, SILENCE_1 };

static const uint32 Priest_spells_heal_arr[] =
{ RENEW_1, FLASH_HEAL_1, LESSER_HEAL_1, NORMAL_HEAL_1, GREATER_HEAL_1, PRAYER_OF_HEALING_1, PRAYER_OF_MENDING_1,
GUARDIAN_SPIRIT_1, PENANCE_1, DIVINE_HYMN_1, CIRCLE_OF_HEALING_1, DESPERATE_PRAYER_1 };

static const uint32 Priest_spells_support_arr[] =
{ PW_FORTITUDE_1, DIVINE_SPIRIT_1, SHADOW_PROTECTION_1, ABOLISH_DISEASE_1, CURE_DISEASE_1,
DISPEL_MAGIC_1, MASS_DISPEL_1, DISPERSION_1, FADE_1, FEAR_WARD_1, HYMN_OF_HOPE_1, INNER_FIRE_1, INNER_FOCUS_1,
LEVITATE_1, PAIN_SUPPRESSION_1, POWER_INFUSION_1, PW_SHIELD_1, RESURRECTION_1, SHADOWFORM_1, VAMPIRIC_EMBRACE_1 };

static const std::vector<uint32> Priest_spells_damage(FROM_ARRAY(Priest_spells_damage_arr));
static const std::vector<uint32> Priest_spells_cc(FROM_ARRAY(Priest_spells_cc_arr));
static const std::vector<uint32> Priest_spells_heal(FROM_ARRAY(Priest_spells_heal_arr));
static const std::vector<uint32> Priest_spells_support(FROM_ARRAY(Priest_spells_support_arr));

//Dinkle
const char* priesthealingMessages[] = {
"|cFFFFFFFFHelp! I've fallen and I can't get up... oh, wait, I'm up! But I still need healing!|r",
"|cFFFFFFFFI'm not a potato, but my health is mashed! Heal me, please!|r",
"|cFFFFFFFFIf I had a gold coin for every hit point I'm missing, I could buy a mount!|r",
"|cFFFFFFFFSomeone call a priest! Oh wait, I am one! Help, my health is plummeting!|r",
"|cFFFFFFFFI'm about as sturdy as a gnome on a pogo stick right now. Send healing, please!|r",
"|cFFFFFFFFI'm not a tank, but I'm taking a beating! Heal me, please!|r",
"|cFFFFFFFFNeed heals!|r",
"|cFFFFFFFFHeal please!|r"
"|cFFFFFFFFThis is quite the test of faith... A little healing would be divine!|r",
"|cFFFFFFFFMy spirit is willing, but the flesh is weak and wounded!|r",
};

const char* priestmanaMessages[] = {
    "|cFFFFFFFFI'm not just thirsty, I'm mana-starved! Please, a sip of mana would be great!|r",
    "|cFFFFFFFFMy mana is running on empty! Time to refuel the arcane tank!|r",
    "|cFFFFFFFFI need to stop and drink after this!|r",
    "|cFFFFFFFFI need an innervate please!|r",
    "|cFFFFFFFFMy mana bar looks like a flat tire - definitely needs pumping!|r",
    "|cFFFFFFFFCan someone lend me some mana? I promise I'll give it back... maybe!|r",
    "|cFFFFFFFFLow on mana - mana break please!|r",
    "|cFFFFFFFFMy mana is like a rollercoaster - it's on a downward spiral!|r"
};

const char* psychicScreamMessages[] = {
    "|cFFFFFFFFI'm casting Psychic Scream, somebody stop me!|r",
    "|cFFFFFFFFPrepare for some mind-bending fear!|r",
    "|cFFFFFFFFFear not,  Psychic Scream is here!|r",
    "|cFFFFFFFFGet ready to scream in terror!|r",
    "|cFFFFFFFFLEEEEEEEEEEEEROOOOOOOOOOY!!!|r"
};
//end Dinkle

class priest_bot : public CreatureScript
{
public:
    priest_bot() : CreatureScript("priest_bot") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new priest_botAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        const char* greetings[] = {
        "|cFFFFFFFFHello there!|r",
        "|cFFFFFFFFHey good looking, wanna chat?|r",
        "|cFFFFFFFFHey there!|r",
        "|cFFFFFFFFYo!|r",
        "|cFFFFFFFFWhat's on the agenda for today?|r",
        "|cFFFFFFFFWe swapping out gear?|r",
        "|cFFFFFFFFWhat's up?|r",
        "|cFFFFFFFFWhy hello there!|r",
        "|cFFFFFFFFThink I can heal better than QT Blue?|r",
        "|cFFFFFFFFIf you need healing or damage, I've got you covered!|r",
        "|cFFFFFFFFWant me to swap roles?|r",
        "|cFFFFFFFFHow's it going?|r",
        "|cFFFFFFFFHello! Been up to much lately?|r",
        "|cFFFFFFFFHey there! Ready to kick some butt?|r",
        "|cFFFFFFFFHey! Looking for something fun to do?|r",
        "|cFFFFFFFFWhat's happening? Ready to dive in?|r",
        "|cFFFFFFFFSup? Got any adventures on the horizon?|r",
        "|cFFFFFFFFHowdy! How's the questing going?|r",
        "|cFFFFFFFFGreetings! Any epic loot drops lately?|r",
        "|cFFFFFFFFWhat's good? Keeping those enemies at bay?|r",
        "|cFFFFFFFFHeya! How's the world treating you?|r",
        "|cFFFFFFFFHi! Any tales of valor to share?|r",
        "|cFFFFFFFFAhoy! Ready to set sail on a new quest?|r",
        "|cFFFFFFFFSalutations! What mischief are we getting into today?|r",
        "|cFFFFFFFFGood to see you! Ready for some action?|r",
        "|cFFFFFFFFHey! How about we go make some history?|r",
        "|cFFFFFFFFDid someone order a hero? Because I just found one!|r",
        "|cFFFFFFFFI tried to catch some fog earlier. I mist.|r",
        "|cFFFFFFFFEver tried to eat a clock? It's time-consuming.|r",
        "|cFFFFFFFFI'd tell you a joke about the Void, but it's... endless.|r",
        "|cFFFFFFFFI was going to tell you a joke about undead, but I decided to drop it. It was dead weight.|r",
        "|cFFFFFFFFDo you know why I never play hide and seek with mountains? Because they always peak.|r",
        "|cFFFFFFFFHave you heard about the rogue who stole a calendar? He got twelve months.|r",
        "|cFFFFFFFFI'm reading a book about anti-gravity. It's impossible to put down.|r",
        "|cFFFFFFFFWhy don't adventurers like to party in Dalaran? Because it's always up in the air.|r",
        "|cFFFFFFFFWhy do druids never get lost? They always find the path of leaf resistance.|r",
        "|cFFFFFFFFI bought some shoes from a drug dealer. I don't know what he laced them with, but I've been tripping all day.|r",
        "|cFFFFFFFFIs it just the campfire, or is it getting hot in here when you're around?|r",
        "|cFFFFFFFFYou're not just a sight for sore eyes, you're a sight for fully healed, buffed, and potion-enhanced eyes!|r",
        "|cFFFFFFFFIs that a Divine Hymn? Because being with you feels utterly heavenly.|r",
        "|cFFFFFFFFDo you specialize in Holy? Because being near you feels like a blessing.|r",
        "|cFFFFFFFFYou must have cast Power Word: Shield, because nothing can hurt me when I'm with you.|r",
        "|cFFFFFFFFDid you just cast Mind Control? Because you've taken over my thoughts completely.|r",
        "|cFFFFFFFFWell, well, well, look who's here! What mischief are we up to today?|r",
        "|cFFFFFFFFHey there! Do you have time to talk about our savior, Pimpgarth the Wise?|r",
        };

        int randomIndex = urand(0, sizeof(greetings) / sizeof(greetings[0]) - 1);
        creature->Say(greetings[randomIndex], LANG_UNIVERSAL, creature->ToUnit());

        return creature->GetBotAI()->OnGossipHello(player, 0);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        if (bot_ai* ai = creature->GetBotAI())
            return ai->OnGossipSelect(player, creature, sender, action);
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, char const* code) override
    {
        if (bot_ai* ai = creature->GetBotAI())
            return ai->OnGossipSelectCode(player, creature, sender, action, code);
        return true;
    }

    struct priest_botAI : public bot_ai
    {
        priest_botAI(Creature* creature) : bot_ai(creature)
        {
            _botclass = BOT_CLASS_PRIEST;

            InitUnitFlags();
        }

        bool doCast(Unit* victim, uint32 spellId)
        {
            if (CheckBotCast(victim, spellId) != SPELL_CAST_OK)
                return false;
            return bot_ai::doCast(victim, spellId);
        }

        void CheckHymnOfHope(uint32 diff)
        {
            if (!IsSpellReady(HYMN_OF_HOPE_1, diff) || Rand() > 45 || IsCasting() || IsTank())
                return;

            Group const* gr = !IAmFree() ? master->GetGroup() : GetGroup();
            if (!gr)
                return;

            uint8 LMPcount = 0;
            for (Unit const* member : BotMgr::GetAllGroupMembers(gr))
            {
                if (me->GetMap() != member->FindMap() || !member->IsAlive() || !member->IsInCombat() ||
                    me->GetDistance(member) > 40 || GetManaPCT(member) > (HasRole(BOT_ROLE_HEAL) ? 10 : 50) ||
                    (member->IsNPCBot() && member->ToCreature()->IsTempBot()) ||
                    member->GetAuraEffect(SPELL_AURA_MOD_INCREASE_ENERGY, SPELLFAMILY_PRIEST, 0x0, 0x0, 0x10))
                    continue;
                if (++LMPcount > 2)
                    break;
            }

            if (LMPcount > 2 && doCast(me, GetSpell(HYMN_OF_HOPE_1)))
                return;
        }

        bool MassGroupHeal(uint32 diff)
        {
            if (!HasRole(BOT_ROLE_HEAL) || IsCasting() || Rand() > (65 + 40 * me->GetMap()->IsRaid()))
                return false;

            Group const* gr = !IAmFree() ? master->GetGroup() : GetGroup();
            if (!gr)
                return false;

            bool canHymn = IsSpellReady(DIVINE_HYMN_1, diff, false);
            bool canPray = !!GetSpell(PRAYER_OF_HEALING_1);
            bool canCirc = IsSpellReady(CIRCLE_OF_HEALING_1, diff, false);

            uint8 LHPcount1, LHPcount2, LHPcount3;
            LHPcount1 = LHPcount2 = LHPcount3 = 0;
            uint8 lowestPCT = 100;
            Unit* castTarget = nullptr;

            for (Unit* member : BotMgr::GetAllGroupMembers(gr))
            {
                if (me->GetMap() != member->FindMap() || !member->IsAlive() || !member->IsInCombat() ||
                    member->isPossessed() || member->IsCharmed() || (member->IsNPCBot() && member->ToCreature()->IsTempBot()) ||
                    member->GetAuraEffect(SPELL_AURA_MOD_INCREASE_ENERGY, SPELLFAMILY_PRIEST, 0x0, 0x0, 0x10))
                    continue;

                float dist = me->GetDistance(member);
                uint8 pct = GetHealthPCT(member);
                if (canHymn && pct < std::min<uint32>(80, 50 + member->getAttackers().size()*10) && GetLostHP(member) > 4000 && dist < 40)
                {
                    if (++LHPcount1 > 2)
                        break;
                }
                if (canPray && pct < 65 && dist < 36)
                {
                    if (++LHPcount2 > 3)
                        break;
                }
                if (canCirc && pct < 85 && dist < 40 && (!castTarget || castTarget->GetDistance(member) < 18))
                {
                    if (++LHPcount3 > 1)
                        break;
                    if (pct < lowestPCT)
                    {
                        lowestPCT = pct;
                        castTarget = member;
                    }
                }
            }

            if (LHPcount1 > 2 && doCast(me, GetSpell(DIVINE_HYMN_1)))
                return true;
            if (LHPcount2 > 3)
            {
                if (me->IsInCombat() && IsSpellReady(INNER_FOCUS_1, diff) && GetManaPCT(me) < 70 &&
                    doCast(me, GetSpell(INNER_FOCUS_1)))
                {}
                if (doCast(me, GetSpell(PRAYER_OF_HEALING_1)))
                    return true;
            }
            if (LHPcount3 > 1 && castTarget && doCast(castTarget, GetSpell(CIRCLE_OF_HEALING_1)))
                return true;

            return false;
        }

        bool ShieldGroup(uint32 diff)
        {
            if (!IsSpellReady(PW_SHIELD_1, false, diff) || IsCasting() || Rand() > 10 + 20 * (me->GetMap()->IsRaid()))
                return false;
            if (!IAmFree() && !(me->GetLevel() >= 30 && _spec == BOT_SPEC_PRIEST_DISCIPLINE))
                return false;

            Group const* gr = !IAmFree() ? master->GetGroup() : GetGroup();
            if (!gr)
            {
                Unit* u = master;
                if (u->IsAlive() && !u->getAttackers().empty() && (IsTank(u) || GetHealthPCT(u) < 80) && me->GetDistance(u) < 40 &&
                    ShieldTarget(u, diff))
                    return true;
                if (!IAmFree())
                {
                    BotMap const* map = master->GetBotMgr()->GetBotMap();
                    for (BotMap::const_iterator itr = map->begin(); itr != map->end(); ++itr)
                    {
                        u = itr->second;
                        if (u->IsAlive() && !u->getAttackers().empty() && !u->ToCreature()->IsTempBot() &&
                            (IsTank(u) || GetHealthPCT(u) < 75) && me->GetDistance(u) < 40 &&
                            ShieldTarget(u, diff))
                            return true;
                    }
                    for (Unit::ControlSet::const_iterator itr = master->m_Controlled.begin(); itr != master->m_Controlled.end(); ++itr)
                    {
                        u = *itr;
                        if (!u || !u->IsPet() || me->GetMap() != u->FindMap())
                            continue;
                        if (u->IsAlive() && !u->getAttackers().empty() && (IsTank(u) || GetHealthPCT(u) < 75) && me->GetDistance(u) < 40 &&
                            ShieldTarget(u, diff))
                            return true;
                    }
                }
            }
            else
            {
                std::vector<Unit*> members = BotMgr::GetAllGroupMembers(gr);
                for (uint8 i = 0; i < 2; ++i)
                {
                    for (Unit* member : members)
                    {
                        if (!(i == 0 ? member->IsPlayer() : member->IsNPCBot()) || me->GetMap() != member->FindMap() ||
                            !member->IsAlive() || me->GetDistance(member) > 40 || member->isPossessed() || member->IsCharmed() || 
                            member->getAttackers().empty() || (!IsTank(member) && GetHealthPCT(member) > 75) ||
                            (member->IsNPCBot() && member->ToCreature()->IsTempBot()) ||
                            member->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_PALADIN, 0x0, 0x80000, 0x0))
                            continue;
                        if (ShieldTarget(member, diff))
                            return true;
                    }
                }
            }
            return false;
        }

        bool ShieldTarget(Unit* target, uint32 diff)
        {
            if (!IsSpellReady(PW_SHIELD_1, diff) || IsCasting())
                return false;
            if (target->HasAuraTypeWithFamilyFlags(SPELL_AURA_MECHANIC_IMMUNITY, SPELLFAMILY_PRIEST, 0x20000000) ||
                target->HasAura(WEAKENED_SOUL_DEBUFF))
                return false;

            if (doCast(target, GetSpell(PW_SHIELD_1)))
                return true;

            return false;
        }

        void StartAttack(Unit* u, bool force = false)
        {
            if (!bot_ai::StartAttack(u, force))
                return;
            GetInPosition(force, u);
        }

        void JustEngagedWith(Unit* u) override { bot_ai::JustEngagedWith(u); }
        void KilledUnit(Unit* u) override { bot_ai::KilledUnit(u); }
        void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override { bot_ai::EnterEvadeMode(why); }
        void MoveInLineOfSight(Unit* u) override { bot_ai::MoveInLineOfSight(u); }
        void JustDied(Unit* u) override { UnsummonAll(); bot_ai::JustDied(u); }

        bool removeShapeshiftForm() override
        {
            ShapeshiftForm form = me->GetShapeshiftForm();
            if (form != FORM_NONE)
            {
                switch (form)
                {
                    case FORM_SHADOW:
                        me->RemoveAurasDueToSpell(SHADOWFORM_1);
                        break;
                    default:
                        break;
                }
            }

            return true;
        }

        void BreakCC(uint32 diff) override
        {
            //Improved Shadowform: Fade
            if (IsSpellReady(FADE_1, diff) && me->GetShapeshiftForm() == FORM_SHADOW && me->GetLevel() >= 45 &&
                Rand() < 35 && me->HasAuraWithMechanic((1<<MECHANIC_SNARE)|(1<<MECHANIC_ROOT)))
            {
                if (doCast(me, GetSpell(FADE_1)))
                    return;
            }
            bot_ai::BreakCC(diff);
        }

        void UpdateAI(uint32 diff) override
        {
            // Dinkle
        if (!IsWanderer())
            {
            if (me->IsInCombat()) {
                // Health check
                float healthPercentage = (float)me->GetHealth() / (float)me->GetMaxHealth();
                if (healthPercentage <= 0.30f && !needHealingFlag) {
                    int randomIndex = urand(0, sizeof(priesthealingMessages) / sizeof(priesthealingMessages[0]) - 1);
                    me->Say(priesthealingMessages[randomIndex], LANG_UNIVERSAL, me->ToUnit());
                    needHealingFlag = true;
                }
                else if (healthPercentage >= 0.50f && needHealingFlag) {
                    needHealingFlag = false;
                }

                // Mana check
                float manaPercentage = (float)me->GetPower(POWER_MANA) / (float)me->GetMaxPower(POWER_MANA);
                if (manaPercentage <= 0.25f && !needManaFlag) {
                    int randomIndex = urand(0, sizeof(priestmanaMessages) / sizeof(priestmanaMessages[0]) - 1);
                    me->Say(priestmanaMessages[randomIndex], LANG_UNIVERSAL, me->ToUnit());
                    needManaFlag = true;
                }
                else if (manaPercentage >= 0.50f && needManaFlag) {
                    needManaFlag = false;
                }
            }
        }
            //end Dinkle
            if (!GlobalUpdate(diff))
                return;

            if (IsSpellReady(THORIUM_GRENADE_SPELL_ID, diff))
            {
                std::list<Creature*> targets;
                me->GetCreaturesWithEntryInRange(targets, 35.0f, 15555);

                for (Creature* target : targets)
                {
                    if (!target->IsAlive() || me->IsFriendlyTo(target))
                        continue;

                    if (me->IsWithinDistInMap(target, 35.0f))
                    {
                        me->CastSpell(target, THORIUM_GRENADE_SPELL_ID, true);
                        SetSpellCooldown(THORIUM_GRENADE_SPELL_ID, 3000);
                        break;
                    }
                }
            }

            DoVehicleActions(diff);
            if (!CanBotAttackOnVehicle())
                return;

            Disperse(diff);

            DoDevCheck(diff);
            DoShackCheck(diff);

            if (IsPotionReady())
            {
                if (GetManaPCT(me) < 33)
                    DrinkPotion(true);
                else if (GetHealthPCT(me) < 50 && (!HasRole(BOT_ROLE_HEAL) || me->HasAuraType(SPELL_AURA_MOD_SILENCE)))
                    DrinkPotion(false);
            }

            CheckRacials(diff);

            doDefend(diff);

            if (me->GetMap()->IsRaid())
            {
                CureGroup(GetSpell(DISPEL_MAGIC_1), diff);
                CureGroup(GetSpell(ABOLISH_DISEASE_1) ? GetSpell(ABOLISH_DISEASE_1) : GetSpell(CURE_DISEASE_1), diff);
                MassGroupHeal(diff);
                ShieldGroup(diff);
                CheckMending(diff);
                BuffAndHealGroup(diff);
            }
            else
            {
                MassGroupHeal(diff);
                ShieldGroup(diff);
                CheckMending(diff);
                BuffAndHealGroup(diff);
                CureGroup(GetSpell(DISPEL_MAGIC_1), diff);
                CureGroup(GetSpell(CURE_DISEASE_1), diff);
            }

            if (master->IsInCombat() || me->IsInCombat())
            {
                CheckSilence(diff);
                CheckDispel(diff);
                CheckHymnOfHope(diff);
            }

            Counter(diff);

            if (me->IsInCombat())
            {
                CheckShackles(diff);
                CheckPowerInfusion(diff);
            }
            else
                DoNonCombatActions(diff);

            if (IsCasting())
                return;

            if (IsSpellReady(SHADOWFORM_1, diff) && HasRole(BOT_ROLE_DPS) && !HasRole(BOT_ROLE_HEAL))
            {
                if (doCast(me, SHADOWFORM_1))
                    return;
            }

            if (ProcessImmediateNonAttackTarget())
                return;

            if (!CheckAttackTarget())
                return;

            CheckUsableItems(diff);

            Attack(diff);
        }

        void Attack(uint32 diff)
        {
            Unit* mytar = opponent ? opponent : disttarget ? disttarget : nullptr;
            if (!mytar)
                return;

            StartAttack(mytar, IsMelee());

            CheckAttackState();
            if (!me->IsAlive() || !mytar->IsAlive())
                return;

            MoveBehind(mytar);

            if (GC_Timer > diff)
                return;
            
            //shadow skills range
            if (me->GetDistance(mytar) > CalcSpellMaxRange(MIND_FLAY_1))
                return;

            auto [can_do_shadow, can_do_holy] = CanAffectVictimBools(mytar, SPELL_SCHOOL_SHADOW, SPELL_SCHOOL_HOLY);

            if (IsSpellReady(PSYCHIC_HORROR_1, diff) && can_do_shadow && Rand() < 20 &&
                mytar->GetHealth() > me->GetMaxHealth()/8 && !CCed(mytar) &&
                !mytar->HasAuraType(SPELL_AURA_MOD_DISARM) &&
                (mytar->GetTypeId() == TYPEID_PLAYER ?
                mytar->ToPlayer()->GetWeaponForAttack(BASE_ATTACK) && mytar->ToPlayer()->GetWeaponForAttack(WeaponAttackType(BASE_ATTACK), true) :
                mytar->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID) && mytar->CanUseAttackType(BASE_ATTACK)))
            {
                if (doCast(mytar, GetSpell(PSYCHIC_HORROR_1)))
                    return;
            }

            //spell reflections
            if (IsSpellReady(SW_PAIN_1, diff) && can_do_shadow && CanRemoveReflectSpells(mytar, SW_PAIN_1) &&
                doCast(mytar, SW_PAIN_1)) //yes, using rank 1
                return;

            if (!HasRole(BOT_ROLE_DPS))
                return;

            if (IsSpellReady(SHADOWFIEND_1, diff) && GetManaPCT(me) < 50)
            {
                SummonBotPet(mytar);
                SetSpellCooldown(SHADOWFIEND_1, 180000); // (5 - 2) min with Veiled Shadows
                return;
            }

            if (!HasRole(BOT_ROLE_HEAL) || GetManaPCT(me) > 35 || botPet)
            {
                if (IsSpellReady(SW_DEATH_1, diff) && can_do_shadow && Rand() < 90 && GetHealthPCT(me) > 50 &&
                    (me->GetMap()->IsRaid() || GetHealthPCT(mytar) < 15 || mytar->GetHealth() < me->GetMaxHealth() / 8) &&
                    doCast(mytar, GetSpell(SW_DEATH_1)))
                    return;
                if (IsSpellReady(VAMPIRIC_TOUCH_1, diff) && can_do_shadow && Rand() < 80 &&
                    mytar->GetHealth() > me->GetMaxHealth() / 4 * (1 + mytar->getAttackers().size()) &&
                    !mytar->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, 0x0, 0x400, 0x0, me->GetGUID()) &&
                    doCast(mytar, GetSpell(VAMPIRIC_TOUCH_1)))
                    return;
                if (IsSpellReady(SW_PAIN_1, diff) && can_do_shadow && Rand() < 60 &&
                    mytar->GetHealth() > me->GetMaxHealth() / 2 * (1 + mytar->getAttackers().size()) &&
                    !mytar->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, 0x8000, 0x0, 0x0, me->GetGUID()))
                {
                    AuraEffect const* weav = me->GetAuraEffect(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, SPELLFAMILY_PRIEST, 0x0, 0x8, 0x0);
                    if (me->GetLevel() < 60 || (weav && weav->GetBase()->GetStackAmount() >= 4))
                        if (doCast(mytar, GetSpell(SW_PAIN_1)))
                            return;
                }
                if (IsSpellReady(DEVOURING_PLAGUE_1, diff) && can_do_shadow && !Devcheck && Rand() < 80 &&
                    (GetSpec() == BOT_SPEC_PRIEST_SHADOW || mytar->IsControlledByPlayer()) &&
                    mytar->GetHealth() > me->GetMaxHealth() / 2 * (1 + mytar->getAttackers().size()) &&
                    !(mytar->GetTypeId() == TYPEID_UNIT && (mytar->ToCreature()->GetCreatureTemplate()->MechanicImmuneMask & (1 << (MECHANIC_INFECTED - 1)))) &&
                    !mytar->GetAuraEffect(SPELL_AURA_PERIODIC_LEECH, SPELLFAMILY_PRIEST, 0x02000000, 0x0, 0x0, me->GetGUID()) &&
                    doCast(mytar, GetSpell(DEVOURING_PLAGUE_1)))
                    return;
                if (IsSpellReady(MIND_BLAST_1, diff) && can_do_shadow &&
                    doCast(mytar, GetSpell(MIND_BLAST_1)))
                    return;
                if (IsSpellReady(MIND_SEAR_1, diff) && can_do_shadow && (!me->isMoving() || Rand() < 80) &&
                    mytar->GetVictim() && mytar->GetVictim()->getAttackers().size() > 3)
                {
                    if (Unit* u = FindSplashTarget(CalcSpellMaxRange(MIND_SEAR_1), mytar, 14.f, 3)) //glyphed, cluster of 4
                        if (doCast(u, GetSpell(MIND_SEAR_1)))
                            return;
                }
                if (IsSpellReady(HOLY_FIRE_1, diff) && can_do_holy &&
                    (HasRole(BOT_ROLE_HEAL) || me->GetShapeshiftForm() != FORM_SHADOW) &&
                    doCast(mytar, GetSpell(HOLY_FIRE_1)))
                    return;
                if (IsSpellReady(MIND_FLAY_1, diff) && can_do_shadow &&
                    (!HasRole(BOT_ROLE_HEAL) || mytar->GetHealth() < me->GetMaxHealth() / 2) &&
                    doCast(mytar, GetSpell(MIND_FLAY_1)))
                    return;
                if (IsSpellReady(SMITE_1, diff) && can_do_holy && me->GetLevel() < 20 &&//MF is lvl 20, MB is lvl 10
                    doCast(mytar, GetSpell(SMITE_1)))
                    return;
            }

            if (Spell const* shot = me->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL))
            {
                if (shot->GetSpellInfo()->Id == SHOOT_WAND && shot->m_targets.GetUnitTarget() != mytar)
                    me->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
            }
            else if (IsSpellReady(SHOOT_WAND, diff) && me->GetDistance(mytar) < 30 && GetEquips(BOT_SLOT_RANGED) &&
                doCast(mytar, SHOOT_WAND))
                return;
        }

        bool HealTarget(Unit* target, uint32 diff) override
        {
            if (!target || !target->IsAlive() || target->GetShapeshiftForm() == FORM_SPIRITOFREDEMPTION || me->GetDistance(target) > 45)
                return false;

            uint8 hp = GetHealthPCT(target);
            if (hp > GetHealHpPctThreshold())
                return false;
            bool pointed = IsPointedHealTarget(target);
            if (hp > 90 && !(pointed && me->GetMap()->IsRaid()) &&
                (!target->IsInCombat() || target->getAttackers().empty() || !IsTank(target) || !me->GetMap()->IsRaid()))
                return false;

            int32 hps = GetHPS(target);
            int32 xphp = target->GetHealth() + hps * (me->GetLevel() < 60 ? 2.5f : 2.0f);
            int32 hppctps = int32(hps * 100.f / float(target->GetMaxHealth()));
            int32 xphploss = xphp > int32(target->GetMaxHealth()) ? 0 : abs(int32(xphp - target->GetMaxHealth()));
            int32 xppct = hp + hppctps * (me->GetLevel() < 60 ? 2.5f : 2.0f);
            //TC_LOG_ERROR("entities.player", "priest_bot:HealTarget(): %s's pct %u, hppctps %i, epct %i",
            //    target->GetName().c_str(), uint32(hp), int32(hppctps), int32(xppct));
            if (xppct >= 95 && hp >= 25 && !pointed)
                return false;

            //GUARDIAN SPIRIT no GCD
            if (IsSpellReady(GUARDIAN_SPIRIT_1, diff, false) && !IAmFree() && target->IsInCombat() && !target->getAttackers().empty() &&
                (xppct <= 0 || (hp <= 50 && hppctps <= -15) ||
                (me->GetMap()->Instanceable() && target->GetMaxHealth() > me->GetMaxHealth() << 5)) &&
                !target->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_HEALING_PCT, SPELLFAMILY_PRIEST, 0x40000000))
            {
                me->InterruptNonMeleeSpells(false);
                if (doCast(target, GetSpell(GUARDIAN_SPIRIT_1)))
                {
                    if (target->GetTypeId() == TYPEID_PLAYER)
                        ReportSpellCast(GUARDIAN_SPIRIT_1, LocalizedNpcText(target->ToPlayer(), BOT_TEXT__ON_YOU), target->ToPlayer());

                    if (!IAmFree() && target != master)
                    {
                        std::string msg = target == me ? LocalizedNpcText(master, BOT_TEXT__ON_MYSELF) : (LocalizedNpcText(master, BOT_TEXT__ON_) + target->GetName() + '!');
                        ReportSpellCast(GUARDIAN_SPIRIT_1, msg, master);
                    }
                    //return true;
                }
            }

            //PAIN SUPPRESSION
            if (IsSpellReady(PAIN_SUPPRESSION_1, diff, false) && xppct >= 5 && hp >= 25 && hp <= 55 && hppctps <= -10 &&
                Rand() < 80 && !target->getAttackers().empty() &&
                !target->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_DISPEL_RESIST, SPELLFAMILY_PRIEST, 0x80000000) &&
                !target->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_HEALING_PCT, SPELLFAMILY_PRIEST, 0x40000000))
            {
                me->InterruptNonMeleeSpells(false);
                if (doCast(target, GetSpell(PAIN_SUPPRESSION_1)))
                {
                    if (target->GetTypeId() == TYPEID_PLAYER)
                        ReportSpellCast(PAIN_SUPPRESSION_1, LocalizedNpcText(target->ToPlayer(), BOT_TEXT__ON_YOU), target->ToPlayer());

                    if (!IAmFree() && target != master)
                    {
                        std::string msg = target == me ? LocalizedNpcText(master, BOT_TEXT__ON_MYSELF) : (LocalizedNpcText(master, BOT_TEXT__ON_) + target->GetName() + '!');
                        ReportSpellCast(PAIN_SUPPRESSION_1, msg, master);
                    }
                    return true;
                }
            }

            if (target == me && IsSpellReady(DESPERATE_PRAYER_1, diff) && hp <= 50 && Rand() < 45 &&
                int32(GetLostHP(me)) > _heals[DESPERATE_PRAYER_1])
            {
                me->InterruptNonMeleeSpells(false);
                if (doCast(target, GetSpell(DESPERATE_PRAYER_1)))
                    return true;
            }

            if (IsCasting())
                return false;

            Unit const* u = target->GetVictim();
            bool tanking = u && IsTank(target) && u->GetTypeId() == TYPEID_UNIT && u->ToCreature()->isWorldBoss();
            //Power Word: Shield
            if (me->getLevel() >= 30 && _spec == BOT_SPEC_PRIEST_DISCIPLINE) // Check if the priest bot is at least level 30 and is Discipline spec
            {
                uint32 PW_SHIELD_SPELL = GetSpell(PW_SHIELD_1); // Get the appropriate spell rank
                if (PW_SHIELD_SPELL && IsSpellReady(PW_SHIELD_SPELL, diff) && // Check if PW_SHIELD is off cooldown
                    !target->HasAura(WEAKENED_SOUL_DEBUFF)) // Check if the target doesn't have Weakened Soul
                {
                    if (doCast(target, PW_SHIELD_SPELL))
                    {
                        return true; // Spell was successfully cast
                    }
                }
            }
            //  Holy Priest Specific Renew Logic
            if (_spec == BOT_SPEC_PRIEST_HOLY && IsSpellReady(RENEW_1, diff) &&
                (hppctps < 10 || hp < 80) &&
                !target->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_PRIEST, 0x40, 0x0, 0x0, me->GetGUID()) &&
                (target->IsInCombat() || !target->getAttackers().empty() || me->GetMap()->IsDungeon())) {
                if (doCast(target, GetSpell(RENEW_1)))
                    return true;
            }

            // Additional conditions to dynamically use Renew based on anticipated incoming damage
            if (_spec == BOT_SPEC_PRIEST_HOLY && hppctps < 0 &&
                !target->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_PRIEST, 0x40, 0x0, 0x0, me->GetGUID())) {
                if (doCast(target, GetSpell(RENEW_1)))
                    return true;
            }
            // Renew for Discipline Priest, applied only to the tank
            if (_spec == BOT_SPEC_PRIEST_DISCIPLINE && IsSpellReady(RENEW_1, diff) && IsTank(target) &&
                !target->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_PRIEST, 0x40, 0x0, 0x0, me->GetGUID()))
            {
                if (doCast(target, GetSpell(RENEW_1)))
                {
                    return true; 
                }
            }
            //Penance
            if (IsSpellReady(PENANCE_1, diff) && !target->IsCharmed() && !target->isPossessed() && hp <= 90 && 
                xphploss > (_heals[PENANCE_1] * 0.75)) 
            {
                if (doCast(target, GetSpell(PENANCE_1)))
                    return true;
            }

            //Big Heal
            if (IsSpellReady(HEAL, diff) && (xppct > 15 || !GetSpell(FLASH_HEAL_1)) && (tanking || xphploss > _heals[HEAL]))
            {
                if (me->IsInCombat() && IsSpellReady(INNER_FOCUS_1, diff) && GetManaPCT(me) < 70 &&
                    doCast(me, GetSpell(INNER_FOCUS_1)))
                {}
                if (doCast(target, GetSpell(HEAL)))
                    return true;
            }
            // More Aggressive Flash Heal
            if (IsSpellReady(FLASH_HEAL_1, diff) &&
                (hp <= 85 || xphploss > (_heals[FLASH_HEAL_1] * 0.75))) // Cast if health is below 85% or predicted healing is only slightly less than Flash Heal's capability
            {
                if (doCast(target, GetSpell(FLASH_HEAL_1)))
                    return true;
            }
            //Greater Heal
            if (IsSpellReady(GREATER_HEAL_1, diff) && hp <= 40 && !IsCasting() && !target->getAttackers().empty() &&
                xphploss > _heals[GREATER_HEAL_1])
            {
                if (doCast(target, GetSpell(GREATER_HEAL_1)))
                {
                    return true; 
                }
            }

            return false;
        }

        bool BuffTarget(Unit* target, uint32 diff) override
        {
            // Dinkle: Periodic thoughtful dialogue
            if (!IsWanderer())
            {
            if (urand(0, 1999) < 1)
            {
                if (!me->IsInCombat())
                {
                const char* thoughtfulMessages[] = {
                    "|cFFFFFFFFSometimes, the heaviest burdens we carry are not our gear, but our thoughts.|r",
                    "|cFFFFFFFFIn the calmest waters, the deepest reflections are found.|r",
                    "|cFFFFFFFFHow do you make a tauren cry? Tell them a 'moo-ving' story!|r",
                    "|cFFFFFFFFWhat do you call a tauren who can play the drums? A moo-sician!|r",
                    "|cFFFFFFFFWhy did the Forsaken become a chef? Because they wanted to serve up some killer dishes!|r",
                    "|cFFFFFFFFStrength is not always measured in battles won, but in the resilience of one's spirit.|r",
                    "|cFFFFFFFFA moment of patience in a moment of anger saves a thousand moments of regret.|r",
                    "|cFFFFFFFFWhat do you call a gnome priest? A mini-healer!|r",
                    "|cFFFFFFFFHow does a druid make coffee in the morning? They use a bear-ista to brew it!|r",
                    "|cFFFFFFFFWhy don't mages trust rogues? Because they always vanish!|r",
                    "|cFFFFFFFFWhat's a demon hunter's favorite dessert? Eye scream!|r",
                    "|cFFFFFFFFI told my friend she was drawing her eyebrows too high. She looked surprised.|r",
                    "|cFFFFFFFFWhy do warriors never play cards? Too many rage quits!|r",
                    "|cFFFFFFFFThe light does not abandon its champions; we merely forget to invoke it in our darkest times.|r",
                    "|cFFFFFFFFIn the dance of battle, the most graceful steps are often those of retreat, a strategy well-known to the Silver Covenant.|r",
                    "|cFFFFFFFFAn adventurer's best tool is not the weapon they wield, but the hope they hold, a sentiment echoed by every hero of Azeroth.|r",
                    "|cFFFFFFFFLike the phoenix rises from ashes, so too can heroes rise from defeat.|r",
                    "|cFFFFFFFFThe mightiest of mountains begin as mere stones. Every legend in Azeroth was once a simple tale.|r",
                    "|cFFFFFFFFBooty Bay's defacto ruler is Baron Revilgaz. 'Revilgaz' is 'Zagliver' spelled backwards!|r",
                    "|cFFFFFFFFEver noticed the critters in Dalaran? They're actually spies for the Kirin Tor, keeping an eye on things.|r",
                    "|cFFFFFFFFAzeroth is home to many mysteries. Every stone turned and every leaf overturned brings us closer to understanding the true nature of this world.|r",
                    "|cFFFFFFFFThe echo of the Titans' footsteps can still be heard if we listen closely to the stones of Ulduar.|r",
                    "|cFFFFFFFFThe journey through the Emerald Dream is not for the faint of heart. It's a realm where reality and fantasy merge.|r",
                    "|cFFFFFFFFDid you know the murlocs have a rich oral tradition, passed down through generations? If only we could understand their language.|r",
                    "|cFFFFFFFFThe journey of a thousand miles begins with a single step, and every step brings us closer to our destination.|r",
                    "|cFFFFFFFFThe true journey is not in seeking new landscapes, but in having new eyes.|r"
                };
                int randomIndex = urand(0, sizeof(thoughtfulMessages) / sizeof(thoughtfulMessages[0]) - 1);
                me->Say(thoughtfulMessages[randomIndex], LANG_UNIVERSAL, target->ToUnit());
                }
            }
            }
            
            if (IsSpellReady(FEAR_WARD_1, diff) && (!IAmFree() || target == me) &&
                !target->HasAuraTypeWithMiscvalue(SPELL_AURA_MECHANIC_IMMUNITY, MECHANIC_FEAR) &&
                doCast(target, GetSpell(FEAR_WARD_1)))
                return true;

            if (target == me)
            {
                if (GetSpell(INNER_FIRE_1) &&
                    !me->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_RESISTANCE, SPELLFAMILY_PRIEST, 0x2) &&
                    doCast(me, GetSpell(INNER_FIRE_1)))
                    return true;
                if (HasRole(BOT_ROLE_DPS) && GetSpell(VAMPIRIC_EMBRACE_1) &&
                    !me->HasAuraTypeWithFamilyFlags(SPELL_AURA_DUMMY, SPELLFAMILY_PRIEST, 0x4) &&
                    doCast(me, GetSpell(VAMPIRIC_EMBRACE_1)))
                    return true;
            }
            // Casting Power Word: Shield by Discipline Priest bot
            if (me->getLevel() >= 30 && _spec == BOT_SPEC_PRIEST_DISCIPLINE) // Check if the priest bot is at least level 30 and is Discipline spec
            {
                if (target->GetTypeId() == TYPEID_PLAYER) // Check if the target is a player to prevent shield spam out of combat
                {
                    uint32 PW_SHIELD_SPELL = GetSpell(PW_SHIELD_1); // Get the appropriate spell rank
                    if (PW_SHIELD_SPELL && IsSpellReady(PW_SHIELD_SPELL, diff) && // Check if PW_SHIELD is off cooldown
                        !target->HasAura(WEAKENED_SOUL_DEBUFF)) // Check if the target doesn't have Weakened Soul
                    {
                        if (doCast(target, PW_SHIELD_SPELL))
                        {
                            return true; // Spell was successfully cast
                        }
                    }
                }
            }
            if (me->IsInCombat() && !master->GetMap()->IsRaid())
                return false;

            if (uint32 PW_FORTITUDE = GetSpell(PW_FORTITUDE_1))
            {
                if (!target->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_STAT, SPELLFAMILY_PRIEST, 0x8) &&
                    doCast(target, PW_FORTITUDE))
                {
                    if (!IsWanderer() && urand(0, 100) < 10)
                    {
                        const char* fortitudeMessages[] = {
                            "|cFFFFFFFFFortitude isn't just a buff, it's a state of mind.|r",
                            "|cFFFFFFFFWrapped in fortitude, ready for what's to come.|r",
                            "|cFFFFFFFFStrength flows from within to without.|r",
                            "|cFFFFFFFFWho needs AoE buffs when you have personal affirmations?|r",
                            "|cFFFFFFFFEmbracing the essence of resilience.|r"
                        };
                        int randomIndex = urand(0, sizeof(fortitudeMessages) / sizeof(fortitudeMessages[0]) - 1);
                        me->Say(fortitudeMessages[randomIndex], LANG_UNIVERSAL, target->ToUnit());
                    }
                    return true;
                }
            }
            if (uint32 SHADOW_PROTECTION = GetSpell(SHADOW_PROTECTION_1))
            {
                if (!target->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE, SPELLFAMILY_PRIEST, 0x100) &&
                    doCast(target, SHADOW_PROTECTION))
                {
                    if (!IsWanderer() && urand(0, 100) < 15)
                    {
                        const char* shadowProtectionMessages[] = {
                             "|cFFFFFFFFShadows offer not just concealment, but protection.|r",
                             "|cFFFFFFFFEnveloped in a cloak of shadows.|r",
                             "|cFFFFFFFFDarkness, my old friend, shield me now.|r",
                             "|cFFFFFFFFLet the shadows be a shield, not a shroud.|r"
                        };
                        int randomIndex = urand(0, sizeof(shadowProtectionMessages) / sizeof(shadowProtectionMessages[0]) - 1);
                        me->Say(shadowProtectionMessages[randomIndex], LANG_UNIVERSAL, target->ToUnit());
                    }
                    return true;
                }
            }
            if (uint32 DIVINE_SPIRIT = GetSpell(DIVINE_SPIRIT_1))
            {
                if ((target->GetMaxPower(POWER_MANA) > 1) &&
                    !target->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_STAT, SPELLFAMILY_PRIEST, 0x20) &&
                    doCast(target, DIVINE_SPIRIT))
                {
                    if (!IsWanderer() && urand(0, 100) < 15)
                    {
                        const char* divineSpiritMessages[] = {
                            "|cFFFFFFFFSpirit uplifted, ready to face the world.|r",
                            "|cFFFFFFFFInfused with divine essence, we stand stronger.|r",
                            "|cFFFFFFFFDivinity touches the soul, lightening the burden.|r",
                            "|cFFFFFFFFGuided by a divine spirit, moving forward.|r"
                        };
                        int randomIndex = urand(0, sizeof(divineSpiritMessages) / sizeof(divineSpiritMessages[0]) - 1);
                        me->Say(divineSpiritMessages[randomIndex], LANG_UNIVERSAL, target->ToUnit());
                    }
                    return true;
                }
            }

            return false;
        }

        void DoNonCombatActions(uint32 diff)
        {
            if (GC_Timer > diff || me->IsMounted() || IsCasting())
                return;

            ResurrectGroup(GetSpell(RESURRECTION_1));

            if (GetSpell(LEVITATE_1) && !IAmFree() && Rand() < 30)
            {
                Group const* gr = master->GetGroup();
                if (gr)
                {
                    for (GroupReference const* ref = gr->GetFirstMember(); ref != nullptr; ref = ref->next())
                    {
                        Player* pl = ref->GetSource();
                        if (pl && pl->IsAlive() && pl->FindMap() == me->GetMap() && pl->GetDistance(me) < 30 &&
                            pl->IsFalling() && pl->m_movementInfo.fallTime > 1000 &&
                            !pl->HasAuraType(SPELL_AURA_HOVER))
                        {
                            if (doCast(pl, GetSpell(LEVITATE_1)))
                            {
                                // Dinkle: Add a random chat message for Levitate
                                const char* levitateMessages[] = {
                                    "|cFFFFFFFFLevitating for safety!|r",
                                    "|cFFFFFFFFUp, up, and away with Levitate!|r",
                                    "|cFFFFFFFFEnjoy the flight with Levitate!|r",
                                    "|cFFFFFFFFLevitating for a smooth landing!|r",
                                    "|cFFFFFFFFFloating gracefully with Levitate!|r",
                                    "|cFFFFFFFFEmbrace the airborne adventure!!|r",
                                    "|cFFFFFFFFLevitate activated for a safe landing!|r",
                                    "|cFFFFFFFFSoaring to new heights with Levitate!|r",
                                };

                                int randomIndex = urand(0, sizeof(levitateMessages) / sizeof(levitateMessages[0]) - 1);
                                me->Say(levitateMessages[randomIndex], LANG_UNIVERSAL, me->ToUnit());

                                return;
                            }
                        }
                    }
                }
                else if (master->IsAlive() && master->GetDistance(me) < 30 && master->IsFalling() &&
                    master->m_movementInfo.fallTime > 1000 && !master->HasAuraType(SPELL_AURA_HOVER))
                {
                    if (doCast(master, GetSpell(LEVITATE_1)))
                    {
                        // Dinkle: Add a random chat message for Levitate
                        const char* levitateMessages[] = {
                                    "|cFFFFFFFFLevitating for safety!|r",
                                    "|cFFFFFFFFUp, up, and away with Levitate!|r",
                                    "|cFFFFFFFFEnjoy the flight with Levitate!|r",
                                    "|cFFFFFFFFLevitating for a smooth landing!|r",
                                    "|cFFFFFFFFFloating gracefully with Levitate!|r",
                                    "|cFFFFFFFFEmbrace the airborne adventure!!|r",
                                    "|cFFFFFFFFLevitate activated for a safe landing!|r",
                                    "|cFFFFFFFFSoaring to new heights with Levitate!|r",
                        };

                        int randomIndex = urand(0, sizeof(levitateMessages) / sizeof(levitateMessages[0]) - 1);
                        me->Say(levitateMessages[randomIndex], LANG_UNIVERSAL, me->ToUnit());

                        return;
                    }
                }
            }
        }

        void Counter(uint32 diff)
        {
            if (ShackcheckTimer > diff || !IsSpellReady(SHACKLE_UNDEAD_1, diff) || Shackcheck || Rand() > 65 ||
                (HasRole(BOT_ROLE_HEAL) && (IsCasting() || GetManaPCT(me) < 20)))
                return;

            //always glyphed so <= 0.5 sec cast time
            if (Unit* target = FindCastingTarget(CalcSpellMaxRange(SHACKLE_UNDEAD_1), 0, SHACKLE_UNDEAD_1))
            {
                me->InterruptNonMeleeSpells(false);
                if (doCast(target, GetSpell(SHACKLE_UNDEAD_1)))
                    return;
            }
        }

        void CheckDispel(uint32 diff)
        {
            if (HasRole(BOT_ROLE_HEAL) && !HasRole(BOT_ROLE_DPS))
                return;

            if (DispelcheckTimer > diff || IsCasting() || Rand() > 35)
                return;

            DispelcheckTimer = urand(750, 1000);

            uint32 DM = GetSpell(DISPEL_MAGIC_1);
            uint32 MD = (GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) ? GetSpell(MASS_DISPEL_1) : 0;

            if (!DM && !MD)
                return;

            if (Unit* target = FindHostileDispelTarget(CalcSpellMaxRange(DISPEL_MAGIC_1)))
            {
                uint32 dm = DM && !target->HasAuraWithMechanic(1<<MECHANIC_IMMUNE_SHIELD) ? DM : MD;
                if (dm && doCast(target, dm))
                    return;
            }
        }

        void CheckMending(uint32 diff)
        {
            if (Mend_Timer > diff || !IsSpellReady(PRAYER_OF_MENDING_1, diff) || !HasRole(BOT_ROLE_HEAL) || IsCasting() || Rand() > 75)
                return;

            Mend_Timer = urand(1000, 3000);

            Group const* gr = !IAmFree() ? master->GetGroup() : GetGroup();
            if (!gr)
                return;

            uint32 MENDING_AURA = InitSpell(me, PRAYER_OF_MENDING_AURA_1);
            if (FindAffectedTarget(MENDING_AURA, me->GetGUID(), 70, 4))
                return;

            for (Unit* member : BotMgr::GetAllGroupMembers(gr))
            {
                if (me->GetMap() == member->FindMap() && member->IsAlive() && !member->getAttackers().empty() &&
                    (IsTank(member) || GetBG()) && GetHealthPCT(member) < 85 && me->IsWithinDistInMap(member, 40) &&
                    !member->HasAuraType(SPELL_AURA_RAID_PROC_FROM_CHARGE_WITH_VALUE))
                {
                    if (doCast(member, GetSpell(PRAYER_OF_MENDING_1)))
                        return;
                }
            }
        }
        //Dinkle
        void CheckShackles(uint32 diff)
        {
            if (Shackle_Timer > diff || !IsSpellReady(SHACKLE_UNDEAD_1, diff) || IsCasting() || Rand() > 50)
                return;

            Shackle_Timer = 500;

            if (FindAffectedTarget(GetSpell(SHACKLE_UNDEAD_1), me->GetGUID(), 60, 255))
                return;
            Unit* target = FindUndeadCCTarget(CalcSpellMaxRange(SHACKLE_UNDEAD_1), SHACKLE_UNDEAD_1);
            if (target && doCast(target, GetSpell(SHACKLE_UNDEAD_1)))
            {
                if (!IsWanderer()) // Check if not a wanderer before speaking
                {
                    const char* shackleMessages[] = {
                        "|cFFFFFFFFHolding %s in place. Let's focus on the others!|r",
                        "|cFFFFFFFFGot %s shacked up! Let's keep the pressure on the rest.|r",
                        "|cFFFFFFFF%s won't bother us for a while. Shackled!|r",
                        "|cFFFFFFFFOne less to worry about, I've got %s shackled.|r",
                        "|cFFFFFFFFEnforced a time-out on %s. It's shackled!|r",
                    };

                    int randomIndex = urand(0, sizeof(shackleMessages) / sizeof(char*) - 1);
                    const char* selectedMessage = shackleMessages[randomIndex];

                    char messageBuffer[256];
                    snprintf(messageBuffer, sizeof(messageBuffer), selectedMessage, target->GetName().c_str());

                    me->Say(messageBuffer, LANG_UNIVERSAL, me->ToUnit());
                }
            }
        }
        //Dinkle
        void CheckSilence(uint32 diff)
        {
            if (IsCasting() || Rand() > 40)
                return;

            if (IsSpellReady(SILENCE_1, diff, false))
            {
                if (Unit* target = FindCastingTarget(CalcSpellMaxRange(SILENCE_1), 0, SILENCE_1))
                {
                    if (doCast(target, GetSpell(SILENCE_1)))
                    {
                        if (!IsWanderer()) 
                        {
                            const char* silenceMessages[] = {
                                "|cFFFFFFFF%s has been silenced! No spells for a bit.|r",
                                "|cFFFFFFFFCutting %s's casting short with a swift Silence!|r",
                                "|cFFFFFFFF%s, you shall not cast!|r",
                                "|cFFFFFFFFSilenced %s! Perfect timing.|r",
                                "|cFFFFFFFF%s's spellcasting is temporarily on hold.|r",
                            };

                            int randomIndex = urand(0, sizeof(silenceMessages) / sizeof(char*) - 1);
                            const char* selectedMessage = silenceMessages[randomIndex];

                            char messageBuffer[256];
                            snprintf(messageBuffer, sizeof(messageBuffer), selectedMessage, target->GetName().c_str());

                            me->Say(messageBuffer, LANG_UNIVERSAL, me->ToUnit());
                        }
                        return;
                    }
                }
            }

            if (IsSpellReady(PSYCHIC_HORROR_1, diff))
            {
                if (Unit* target = FindCastingTarget(CalcSpellMaxRange(PSYCHIC_HORROR_1), 0, PSYCHIC_HORROR_1))
                {
                    if (doCast(target, GetSpell(PSYCHIC_HORROR_1)))
                    {
                        if (!IsWanderer()) 
                        {
                            const char* horrorMessages[] = {
                                "|cFFFFFFFF%s is gripped by Psychic Horror! No casting for them.|r",
                                "|cFFFFFFFF%s's mind is terrorized, halting their spells!|r",
                                "|cFFFFFFFFInstilling a Psychic Horror in %s, they're locked down!|r",
                                "|cFFFFFFFF%s is stunned with fear, spells interrupted!|r",
                                "|cFFFFFFFFPsychic Horror paralyzes %s, stopping their cast.|r",
                            };

                            int randomIndex = urand(0, sizeof(horrorMessages) / sizeof(char*) - 1);
                            const char* selectedMessage = horrorMessages[randomIndex];

                            char messageBuffer[256];
                            snprintf(messageBuffer, sizeof(messageBuffer), selectedMessage, target->GetName().c_str());

                            me->Say(messageBuffer, LANG_UNIVERSAL, me->ToUnit());
                        }
                        return;
                    }
                }
            }
        }
        void CheckPowerInfusion(uint32 diff)
        {
            if (!IsSpellReady(POWER_INFUSION_1, diff, false) || IsCasting() || Rand() > 25)
                return;

            const char* powerInfusionSelfMessages[] = {
                "|cFFFFFFFFFeeling the need for speed!|r",
                "|cFFFFFFFFLet's kick things up a notch!|r",
                "|cFFFFFFFFPower Infusion, don't fail me now!|r",
                "|cFFFFFFFFSupercharge incomming!|r"
            };

            const char* powerInfusionOtherMessages[] = {
                "|cFFFFFFFFYou're about to feel a power surge!|r",
                "|cFFFFFFFFLet's get you moving faster!|r",
                "|cFFFFFFFFInjecting a bit of extra oomph!|r",
                "|cFFFFFFFFPower infusion coming right up!|r"
            };

            if (IAmFree())
            {
                if (me->GetVictim() && GetManaPCT(me) < 95)
                {
                    if (!IsWanderer()) 
                    {
                        int randomIndex = urand(0, sizeof(powerInfusionSelfMessages) / sizeof(powerInfusionSelfMessages[0]) - 1);
                        me->Say(powerInfusionSelfMessages[randomIndex], LANG_UNIVERSAL, me->ToUnit());
                    }
                    doCast(me, GetSpell(POWER_INFUSION_1));
                    return;
                }

                return;
            }

            Group const* gr = master->GetGroup();
            BotMap const* map;
            Unit* u = nullptr;
            if (!gr)
            {
                u = master;
                if (u->IsAlive() && u->IsInWorld() && u->GetPowerType() == POWER_MANA && u->GetVictim() && !IsTank(u) &&
                    GetManaPCT(u) < 70 && me->IsWithinDistInMap(u, 30) &&
                    !u->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, SPELLFAMILY_PRIEST, 0x80000000) &&
                    doCast(u, GetSpell(POWER_INFUSION_1)))
                    return;

                map = master->GetBotMgr()->GetBotMap();
                for (BotMap::const_iterator itr = map->begin(); itr != map->end(); ++itr)
                {
                    u = itr->second;
                    if (u->IsAlive() && u->IsInWorld() && u->ToCreature()->GetBotAI()->HasRole(BOT_ROLE_HEAL) &&
                        u->ToCreature()->GetBotClass() < BOT_CLASS_EX_START &&
                        GetManaPCT(u) < 70 && me->IsWithinDistInMap(u, 30) &&
                        !u->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, SPELLFAMILY_PRIEST, 0x80000000) &&
                        doCast(u, GetSpell(POWER_INFUSION_1)))
                        return;
                }
                for (BotMap::const_iterator itr = map->begin(); itr != map->end(); ++itr)
                {
                    u = itr->second;
                    if (u->IsAlive() && u->IsInWorld() && u->GetPowerType() == POWER_MANA && u->GetVictim() && !IsTank(u) &&
                        u->ToCreature()->GetBotClass() < BOT_CLASS_EX_START &&
                        GetManaPCT(u) < 70 && me->IsWithinDistInMap(u, 30) &&
                        !u->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, SPELLFAMILY_PRIEST, 0x80000000) &&
                        doCast(u, GetSpell(POWER_INFUSION_1)))
                        return;
                }

                return;
            }

            for (GroupReference const* itr = gr->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                u = itr->GetSource();
                if (u && u->IsAlive() && u->IsInWorld() && u->GetPowerType() == POWER_MANA && u->GetVictim() && !IsTank(u) &&
                    GetManaPCT(u) < 70 && me->IsWithinDistInMap(u, 30) &&
                    !u->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, SPELLFAMILY_PRIEST, 0x80000000))
                {
                    if (!IsWanderer()) 
                    {
                        int randomIndex = urand(0, sizeof(powerInfusionOtherMessages) / sizeof(powerInfusionOtherMessages[0]) - 1);
                        me->Say(powerInfusionOtherMessages[randomIndex], LANG_UNIVERSAL, u->ToUnit());
                    }
                    doCast(u, GetSpell(POWER_INFUSION_1));
                    return;
                }
            }
            for (GroupReference const* itr = gr->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player const* player = itr->GetSource();
                if (!player || !player->IsInWorld() || me->GetMap() != player->FindMap() || !player->HaveBot())
                    continue;
                map = master->GetBotMgr()->GetBotMap();
                for (BotMap::const_iterator bitr = map->begin(); bitr != map->end(); ++bitr)
                {
                    u = bitr->second;
                    if (u->IsAlive() && u->IsInWorld() && u->ToCreature()->GetBotAI()->HasRole(BOT_ROLE_HEAL) &&
                        !IsHeroExClass(u->ToCreature()->GetBotClass()) &&
                        GetManaPCT(u) < 70 && me->IsWithinDistInMap(u, 30) &&
                        !u->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, SPELLFAMILY_PRIEST, 0x80000000) &&
                        doCast(u, GetSpell(POWER_INFUSION_1)))
                        return;
                }
            }
            for (GroupReference const* itr = gr->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player const* player = itr->GetSource();
                if (!player || !player->IsInWorld() || me->GetMap() != player->FindMap() || !player->HaveBot())
                    continue;
                map = master->GetBotMgr()->GetBotMap();
                for (BotMap::const_iterator bitr = map->begin(); bitr != map->end(); ++bitr)
                {
                    u = bitr->second;
                    if (u->IsAlive() && u->IsInWorld() && u->GetPowerType() == POWER_MANA && u->GetVictim() && !IsTank(u) &&
                        u->ToCreature()->GetBotClass() < BOT_CLASS_EX_START &&
                        GetManaPCT(u) < 70 && me->IsWithinDistInMap(u, 30) &&
                        !u->HasAuraTypeWithFamilyFlags(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, SPELLFAMILY_PRIEST, 0x80000000) &&
                        doCast(u, GetSpell(POWER_INFUSION_1)))
                        return;
                }
            }

            SetSpellCooldown(POWER_INFUSION_1, 1500); //fail
        }

        void doDefend(uint32 diff)
        {
            if (Rand() > 50) return;

            Unit::AttackerSet const& m_attackers = master->getAttackers();
            Unit::AttackerSet const& b_attackers = me->getAttackers();

            //fear master's attackers
            if (IsSpellReady(PSYCHIC_SCREAM_1, diff))
            {
                if (!m_attackers.empty() && (!IsTank(master) || GetHealthPCT(master) < 75))
                {
                    uint8 tCount = 0;
                    for (Unit::AttackerSet::const_iterator iter = m_attackers.begin(); iter != m_attackers.end(); ++iter)
                    {
                        if (!(*iter)) continue;
                        if ((*iter)->ToCreature() && (*iter)->GetCreatureType() == CREATURE_TYPE_UNDEAD) continue;
                        if (me->GetExactDist((*iter)) > 7) continue;
                        if (CCed(*iter) && me->GetExactDist((*iter)) > 5) continue;
                        if (me->IsValidAttackTarget(*iter))
                            ++tCount;
                    }
                    if (tCount > 1 && doCast(me, GetSpell(PSYCHIC_SCREAM_1)))
                    {
                        if (!IsWanderer()) 
                        {
                            int randomIndex = urand(0, sizeof(psychicScreamMessages) / sizeof(psychicScreamMessages[0]) - 1);
                            me->Say(psychicScreamMessages[randomIndex], LANG_UNIVERSAL, me->ToUnit());
                        }
                        return;
                    }
                }

                // Defend myself (psychic horror)
                if (!b_attackers.empty())
                {
                    uint8 tCount = 0;
                    for (Unit::AttackerSet::const_iterator iter = b_attackers.begin(); iter != b_attackers.end(); ++iter)
                    {
                        if (!(*iter)) continue;
                        if ((*iter)->ToCreature() && (*iter)->GetCreatureType() == CREATURE_TYPE_UNDEAD) continue;
                        if (me->GetExactDist((*iter)) > 7) continue;
                        if (CCed(*iter) && me->GetExactDist((*iter)) > 5) continue;
                        if (me->IsValidAttackTarget(*iter))
                            ++tCount;
                    }
                    if (tCount > 0 && doCast(me, GetSpell(PSYCHIC_SCREAM_1)))
                        return;
                }
            }
            // Heal myself
            if ((GetHealthPCT(me) < 95 && !b_attackers.empty()) || (IsWanderer() && IsFlagCarrier(me)))
            {
                if (ShieldTarget(me, diff)) return;

                if (IsSpellReady(FADE_1, diff) && me->IsInCombat())
                {
                    if (b_attackers.empty()) return;
                    uint8 Tattackers = 0;
                    for (Unit::AttackerSet::const_iterator iter = b_attackers.begin(); iter != b_attackers.end(); ++iter)
                    {
                        if (!(*iter)) continue;
                        if (!(*iter)->IsAlive()) continue;
                        if (!(*iter)->CanHaveThreatList()) continue;
                        if (me->GetDistance((*iter)) < 15)
                            Tattackers++;
                    }
                    if (Tattackers > 0)
                    {
                        if (doCast(me, GetSpell(FADE_1)))
                            return;
                    }
                }
            }
        }

        void DoDevCheck(uint32 diff)
        {
            if (DevcheckTimer <= diff)
            {
                DevcheckTimer = 1000;
                Devcheck = GetSpell(DEVOURING_PLAGUE_1) && FindAffectedTarget(GetSpell(DEVOURING_PLAGUE_1), me->GetGUID(), 70);
            }
        }

        void DoShackCheck(uint32 diff)
        {
            if (ShackcheckTimer <= diff)
            {
                ShackcheckTimer = 1000;
                Shackcheck = GetSpell(SHACKLE_UNDEAD_1) && FindAffectedTarget(GetSpell(SHACKLE_UNDEAD_1), me->GetGUID(), 70);
            }
        }

        void Disperse(uint32 diff)
        {
            if (me->GetVehicle())
                return;
            if (!IsSpellReady(DISPERSION_1, diff) || !me->IsInCombat() || HasRole(BOT_ROLE_HEAL) || IsCasting() || Rand() > 60)
                return;
            if ((me->getAttackers().size() > 3 && !IsSpellReady(FADE_1, diff, false) && GetHealthPCT(me) < 90) ||
                (GetHealthPCT(me) < 20 && (me->HasAuraType(SPELL_AURA_PERIODIC_DAMAGE) || !me->getAttackers().empty())) ||
                (GetManaPCT(me) < 35 && !IsPotionReady()) ||
                (me->getAttackers().size() > 1 && (CCed(me, true) || me->HasAuraWithMechanic(1<<MECHANIC_SNARE))))
            {
                if (doCast(me, GetSpell(DISPERSION_1)))
                    return;
            }

            SetSpellCooldown(DISPERSION_1, 500); //fail
        }

        void ApplyClassSpellCritMultiplierAll(Unit const* victim, float& crit_chance, SpellInfo const* spellInfo, SpellSchoolMask schoolMask, WeaponAttackType /*attackType*/) const override
        {
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            uint8 lvl = me->GetLevel();

            //Inner Focus
            if (AuraEffect const* focu = me->GetAuraEffect(INNER_FOCUS_1, 0))
                if (focu->IsAffectedOnSpell(spellInfo))
                    crit_chance += 25.f;

            //Benediction (23236)
            if (lvl >= 60 && (schoolMask & SPELL_SCHOOL_MASK_HOLY))
                crit_chance += 2.f;
            //Increased Prayer of Healing Criticals (23550): 25% additional critical chance for Prayer of Healing
            if (lvl >= 60 && baseId == PRAYER_OF_HEALING_1)
                crit_chance += 25.f;
            //Item - Priest T9 Shadow 4P Bonus (67198)
            if (lvl >= 80 && baseId == MIND_FLAY_DAMAGE)
                crit_chance += 5.f;

            //Holy Specialization: 5% additional critical chance for Holy spells
            if (lvl >= 10 && (schoolMask & SPELL_SCHOOL_MASK_HOLY))
                crit_chance += 5.f;
            //Mind Melt (part 1): 4% additional critical chance for Mind Blast, Mind Flay and Mind Sear
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) &&
                lvl >= 35 && ((spellInfo->SpellFamilyFlags[0] & 0x802000) || (spellInfo->SpellFamilyFlags[1] & 0x80000)))
                crit_chance += 4.f;
            //Mind Melt (part 2): 6% additional critical chance for Vampiric Touch, Devouring Plague and SW: Pain
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) &&
                lvl >= 35 && ((spellInfo->SpellFamilyFlags[0] & 0x2008000) || (spellInfo->SpellFamilyFlags[1] & 0x400)))
                crit_chance += 6.f;
            //Improved Flash Heal (part 2): 10% additional critical chance on targets at or below 50% hp for Flash Heal
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) && lvl >= 40 && baseId == FLASH_HEAL_1 && GetHealthPCT(victim) <= 50)
                crit_chance += 10.f;
            //Renewed Hope part 1: 4% additional critical chance on targets affected by Weakened Soul for Flash Heal, Greater Heal and Penance (Heal)
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) &&
                lvl >= 45 && (baseId == FLASH_HEAL_1 || baseId == HEAL || baseId == PENANCE_HEAL_1) &&
                victim->HasAuraTypeWithFamilyFlags(SPELL_AURA_MECHANIC_IMMUNITY, SPELLFAMILY_PRIEST, 0x20000000))
                crit_chance += 4.f;
        }

        void ApplyClassDamageMultiplierSpell(int32& damage, SpellNonMeleeDamage& damageinfo, SpellInfo const* spellInfo, WeaponAttackType /*attackType*/, bool iscrit) const override
        {
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            uint8 lvl = me->GetLevel();
            float fdamage = float(damage);

            //apply bonus damage mods
            float pctbonus = 0.0f;
            if (iscrit)
            {
                //!!!spell damage is not yet critical and will be multiplied by 1.5
                //so we should put here bonus damage mult /1.5
                //Shadow Power: 50% additional crit damage bonus for Mind Blast, Mind Flay and SW:Death
                if (lvl >= 40 &&
                    (baseId == MIND_BLAST_1 || baseId == MIND_FLAY_DAMAGE || baseId == SW_DEATH_1))
                    pctbonus += 0.333f;
                //Shadowform crit damage increase
                if (me->GetShapeshiftForm() == FORM_SHADOW &&
                    (baseId == SW_PAIN_1 || baseId == DEVOURING_PLAGUE_1 || baseId == VAMPIRIC_TOUCH_1))
                    pctbonus += 0.333f;
            }
            //Improved Mind Flay and Smite (37571)
            if (lvl >= 10 && (baseId == MIND_FLAY_DAMAGE || baseId == SMITE_1))
                pctbonus += 0.05f;
            //Item - Priest T8 Shadow 2P Bonus (64906)
            if (lvl >= 80 && ((baseId == DEVOURING_PLAGUE_1) || (spellInfo->SpellFamilyFlags[2] & 0x8)))
                pctbonus += 0.15f;

            //Twin Disciplines (damage part): 5% bonus damage for instant spells
            if (lvl >= 10 && !spellInfo->CastTimeEntry)
                pctbonus += 0.05f;
            //Darkness: 10% bonus damage for shadow spells
            if (lvl >= 10 && (spellInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_SHADOW))
                pctbonus += 0.1f;
            //Improved Shadow Word: Pain: 6% bonus damage for Shadow Word: Pain
            if (lvl >= 15 && baseId == SW_PAIN_1)
                pctbonus += 0.06f;
            //Focused Power part 1: 4% bonus damage for all spells
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) && lvl >= 35)
                pctbonus += 0.04f;
            //Improved Devouring Plague part 1: 15% bonus damage Devouring Plague
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) && lvl >= 35 && baseId == DEVOURING_PLAGUE_1)
                pctbonus += 0.15f;
            //Shadowform: 15% bonus damage for shadow spells (handled)
            //if (lvl >= 40 && (spellInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_SHADOW) && me->GetShapeshiftForm() == FORM_SHADOW)
            //    pctbonus += 0.15f;
            //Misery part 3: 15% bonus damage (from spellpower) for Mind Blast, Mind Flay and Mind Sear
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) && lvl >= 45)
            {
                if (baseId == MIND_BLAST_1 || baseId == MIND_FLAY_DAMAGE || baseId == MIND_SEAR_DAMAGE_1)
                    fdamage += me->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_MAGIC) * 0.15f * me->CalculateDefaultCoefficient(spellInfo, DIRECT_DAMAGE) * me->CalculateLevelPenalty(spellInfo);
            }

            //If target is affected BY SW: Pain
            if (lvl >= 20 && (baseId == MIND_BLAST_1 || baseId == MIND_FLAY_DAMAGE) && damageinfo.target &&
                damageinfo.target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, 0x8000, 0x0, 0x0, me->GetGUID()))
            {
                //Glyph of Mind Flay: 10% damage bonus for Mind Flay
                if (baseId == MIND_FLAY_DAMAGE)
                    pctbonus += 0.1f;
                //Twisted Faith (part 1): 10% bonus damage for Mind Blast and Mind Flay
                if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) && lvl >= 55)
                    pctbonus += 0.1f;
            }

            //Glyph of Shadow Word: Death: 10% bonus damage for Shadow Word: Death on targets below 35% health
            if (lvl >= 62 && baseId == SW_DEATH_1 && damageinfo.target->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT))
                pctbonus += 0.1f;

            //other
            if (baseId == SW_DEATH_BACKLASH)
            {
                //not affected by +%talents
                pctbonus = 1.f;
                ////T13 Shadow 2P Bonus (Shadow Word: Death), part 2
                //if (lvl >= 60) //buffed
                //    pctbonus -= 0.95f;
                //Pain and Suffering (part 2): 30% reduced backlash damage
                if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) && lvl >= 50)
                    pctbonus -= 0.3f;
            }

            damage = int32(fdamage * (1.0f + pctbonus));
        }

        void ApplyClassDamageMultiplierHeal(Unit const* victim, float& heal, SpellInfo const* spellInfo, DamageEffectType damagetype, uint32 stack) const override
        {
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            uint8 lvl = me->GetLevel();
            float pctbonus = 0.0f;
            float flat_mod = 0.0f;

            //pct mods
            //Improved Greater Heal (38411): 5% bonus healing for Greater Heal
            if (lvl >= 60 && baseId == HEAL)
                pctbonus += 0.05f;
            //Priest T9 Healing 2P: 20% bonus healing for Prayer of Mending
            if (lvl >= 80 && baseId == PRAYER_OF_MENDING_HEAL)
                pctbonus += 0.2f;

            //Twin Disciplines (healing part): 5% bonus healing for instant spells
            if (lvl >= 10 && !spellInfo->CastTimeEntry)
                pctbonus += 0.05f;
            //Improved Renew: 15% bonus healing for Renew
            if (lvl >= 10 && baseId == RENEW_1)
                pctbonus += 0.15f;
            //Focused Power part 2: 4% bonus heal for all spells
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) && lvl >= 35)
                pctbonus += 0.04f;
            //Spiritual Healing: 10% bonus healing for all spells
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) && lvl >= 35)
                pctbonus += 0.15f;
            //Blessend Resilience part 1: 3% bonus healing for all spells
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) && lvl >= 40)
                pctbonus += 0.03f;
            //Empowered Healing: 40% bonus (from spellpower) for Greater Heal and 20% bonus (from spellpower) for Flash Heal and Binding Heal
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) && lvl >= 45)
            {
                if (baseId == HEAL)
                    flat_mod += me->SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_MAGIC) * 0.4f * me->CalculateDefaultCoefficient(spellInfo, damagetype) * 1.88f * me->CalculateLevelPenalty(spellInfo) * stack;
                else if (baseId == FLASH_HEAL_1/* || baseId == BINDING_HEAL_1*/)
                    flat_mod += me->SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_MAGIC) * 0.2f * me->CalculateDefaultCoefficient(spellInfo, damagetype) * 1.88f * me->CalculateLevelPenalty(spellInfo) * stack;
            }
            //Empowered Renew (heal bonus part): 15% bonus healing (from spellpower) for Renew
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) && lvl >= 50 && baseId == RENEW_1)
                flat_mod += me->SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_MAGIC) * 0.15f * me->CalculateDefaultCoefficient(spellInfo, damagetype) * 1.88f * me->CalculateLevelPenalty(spellInfo) * stack;
            //Test of Faith: 12% bonus healing on targets at or below 50% health
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) && lvl >= 50 && GetHealthPCT(victim) <= 50)
                pctbonus += 0.12f;
            //Divine Providence: 10% bonus healing for Circle of Healing, Binding Heal, Holy Nova, Prayer of Healing, Divine Hymn and Prayer of Mending
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) && lvl >= 55 &&
                ((spellInfo->SpellFamilyFlags[0] & 0x18000200) ||
                (spellInfo->SpellFamilyFlags[1] & 0x4) ||
                (spellInfo->SpellFamilyFlags[2] & 0x4)))
                pctbonus += 0.1f;

            //flat mods
            //Improved Prayer of Mending: 100 additional heal for Prayer of Mending
            if (baseId == PRAYER_OF_MENDING_HEAL)
                flat_mod += 100;

            heal = heal * (1.0f + pctbonus) + flat_mod;
        }

        void ApplyClassSpellCostMods(SpellInfo const* spellInfo, int32& cost) const override
        {
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            //SpellSchool school = GetFirstSchoolInMask(spellInfo->GetSchoolMask());
            uint8 lvl = me->GetLevel();
            float fcost = float(cost);
            int32 flatbonus = 0;
            float pctbonus = 0.0f;

            //percent mods
            //Inner Focus
            if (AuraEffect const* focu = me->GetAuraEffect(INNER_FOCUS_1, 0))
                if (focu->IsAffectedOnSpell(spellInfo))
                    pctbonus += 1.f;
            //Surge of Light
            if (AuraEffect const* surg = me->GetAuraEffect(SURGE_OF_LIGHT_BUFF, 1))
                if (surg->IsAffectedOnSpell(spellInfo))
                    pctbonus += 1.f;

            //Reduced Prayer of Healing Cost (38410):
            if (lvl >= 60 && baseId == PRAYER_OF_HEALING_1)
                pctbonus += 0.1f;
            //Greater Heal Cost Reduction (60155):
            if (lvl >= 60 && baseId == HEAL)
                pctbonus += 0.05f;

            //Shadow Focus part 2
            if (lvl >= 15 && (spellInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_SHADOW))
                pctbonus += 0.06f;
            //Absolution:
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) && lvl >= 25 && (spellInfo->SpellFamilyFlags[1] & 0x81))
                pctbonus += 0.15f;
            //Mental Agility:
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) && lvl >= 25 && !spellInfo->CastTimeEntry)
                pctbonus += 0.1f;
            //Improved Healing:
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) &&
                lvl >= 25 && (baseId == HEAL || baseId == DIVINE_HYMN_1 || baseId == PENANCE_HEAL_1))
                pctbonus += 0.15f;
            //Soul Warding part 2
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) && lvl >= 30 && baseId == PW_SHIELD_1)
                pctbonus += 0.15f;
            //Healing Prayers:
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) &&
                lvl >= 30 && (baseId == PRAYER_OF_HEALING_1 || baseId == PRAYER_OF_MENDING_1))
                pctbonus += 0.2f;
            //Focused Mind
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) &&
                lvl >= 30 && (baseId == MIND_BLAST_1 || baseId == MIND_FLAY_1 ||
                baseId == MIND_SEAR_1/* || baseId == MIND_CONTROL_1*/))
                pctbonus += 0.15f;
            //Improved Flash Heal part 1
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) && lvl >= 40 && baseId == FLASH_HEAL_1)
                pctbonus += 0.15f;

            //Glyph of Fading
            if (lvl >= 15 && baseId == FADE_1)
                pctbonus += 0.3f;
            //Glyph of Fortitude
            if (lvl >= 15 && baseId == PW_FORTITUDE_1)
                pctbonus += 0.5f;
            //Glyph of Flash Heal
            if (lvl >= 20 && baseId == FLASH_HEAL_1)
                pctbonus += 0.1f;
            //Glyph of Mass Dispel
            if (lvl >= 70 && baseId == MASS_DISPEL_1)
                pctbonus += 0.35f;

            //flat mods
            //Cleanse Cost Reduced (id: 27847): -25 mana cost for Cleanse
            //if (lvl >= 40 && spellId == GetSpell(CLEANSE_1))
            //    flatbonus += 25;

            //cost can be < 0
            cost = int32(fcost * (1.0f - pctbonus)) - flatbonus;
        }

        void ApplyClassSpellCastTimeMods(SpellInfo const* spellInfo, int32& casttime) const override
        {
            //casttime is in milliseconds
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            //SpellSchool school = GetFirstSchoolInMask(spellInfo->GetSchoolMask());
            uint8 lvl = me->GetLevel();
            int32 timebonus = 0;
            float pctbonus = 0.0f;

            //100% mods
            //Surge of Light
            if (AuraEffect const* surg = me->GetAuraEffect(SURGE_OF_LIGHT_BUFF, 1))
                if (surg->IsAffectedOnSpell(spellInfo))
                    pctbonus += 1.f;

            //pct mods
            //Serendipity: -12% per stack cast time for Prayer of Healing or Greater Heal
            if (baseId == GREATER_HEAL_1 || baseId == PRAYER_OF_HEALING_1)
            {
                if (AuraEffect const* sere = me->GetAuraEffect(SERENDIPITY_BUFF, 0))
                    if (sere->IsAffectedOnSpell(spellInfo))
                        pctbonus += 0.12f * sere->GetBase()->GetStackAmount();
            }

            //flat mods
            //Improved Prayer of Healing (21339)
            if (lvl >= 60 && baseId == PRAYER_OF_HEALING_1)
                timebonus += 100;
            //Master Healer (15027) rank 5
            if (lvl >= 60 && baseId == HEAL)
                timebonus += 500;
            //Prophesy Flash Heal Bonus (21973) part 1
            if (lvl >= 60 && baseId == FLASH_HEAL_1)
                timebonus += 100;

            //Divine Fury
            if (lvl >= 15 && (baseId == HEAL || baseId == SMITE_1 || baseId == HOLY_FIRE_1))
                timebonus += 500;
            //Focused Power part 3
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) && lvl >= 35 && baseId == MASS_DISPEL_1)
                timebonus += 1000;
            //Improved Mana Burn
            //if (lvl >= 35 && baseId == MANA_BURN_1)
            //    timebonus += 1000;

            //Glyph of Scourge Imprisonment
            if (lvl >= 20 && baseId == SHACKLE_UNDEAD_1)
                timebonus += 1000;

            casttime = std::max<int32>((float(casttime) * (1.0f - pctbonus)) - timebonus, 0);
        }

        void ApplyClassSpellCooldownMods(SpellInfo const* spellInfo, uint32& cooldown) const override
        {
            //cooldown is in milliseconds
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            //SpellSchool school = GetFirstSchoolInMask(spellInfo->GetSchoolMask());
            uint8 lvl = me->GetLevel();
            int32 timebonus = 0;
            float pctbonus = 0.0f;

            //pct mods
            //Aspiration
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) &&
                lvl >= 45 && (baseId == INNER_FOCUS_1 || baseId == POWER_INFUSION_1 || baseId == PAIN_SUPPRESSION_1))
                pctbonus += 0.2f;

            //flat mods
            //Veiled Shadows part 2
            //if (lvl >= 25 && baseId == SHADOWFIEND_1)
            //    timebonus += 120000;
            //Glyph of Dispersion:
            if (lvl >= 60 && baseId == DISPERSION_1)
                timebonus += 45000;
            //Glyph of Penance:
            if (lvl >= 60 && baseId == PENANCE_1)
                timebonus += 2000;

            cooldown = std::max<int32>((float(cooldown) * (1.0f - pctbonus)) - timebonus, 0);
        }

        void ApplyClassSpellCategoryCooldownMods(SpellInfo const* spellInfo, uint32& cooldown) const override
        {
            //cooldown is in milliseconds
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            //SpellSchool school = GetFirstSchoolInMask(spellInfo->GetSchoolMask());
            uint8 lvl = me->GetLevel();
            int32 timebonus = 0;
            float pctbonus = 0.0f;

            //pct mods
            //Aspiration
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) && lvl >= 45 && baseId == PENANCE_1)
                pctbonus += 0.2f;
            //Divine Providence:
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) && lvl >= 55 && baseId == PRAYER_OF_MENDING_1)
                pctbonus += 0.3f;

            //flat mods
            //Quick Fade (18388)
            if (lvl >= 40 && baseId == FADE_1)
                timebonus += 2000;

            //Improved Psychic Scream
            if (lvl >= 20 && baseId == PSYCHIC_SCREAM_1)
                timebonus += 4000;
            //Improved Mind Blast
            if (lvl >= 20 && baseId == MIND_BLAST_1)
                timebonus += 2500;
            //Veiled Shadows part 1
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) && lvl >= 25 && baseId == FADE_1)
                timebonus += 6000;
            //Soul Warding part 1
            if ((GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE) && lvl >= 30 && baseId == PW_SHIELD_1)
                timebonus += 4000;

            //Glyph of Fade
            if (lvl >= 15 && baseId == FADE_1)
                timebonus += 9000;

            cooldown = std::max<int32>((float(cooldown) * (1.0f - pctbonus)) - timebonus, 0);
        }

        void ApplyClassSpellGlobalCooldownMods(SpellInfo const* spellInfo, float& cooldown) const override
        {
            //cooldown is in milliseconds
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            //SpellSchool school = GetFirstSchoolInMask(spellInfo->GetSchoolMask());
            uint8 lvl = me->GetLevel();
            float timebonus = 0.0f;
            float pctbonus = 0.0f;

            //Prophesy Flash Heal Bonus (21973) part 2
            if (lvl >= 60 && baseId == FLASH_HEAL_1)
                timebonus += 100;

            cooldown = (cooldown * (1.0f - pctbonus)) - timebonus;
        }

        void ApplyClassSpellRadiusMods(SpellInfo const* spellInfo, float& radius) const override
        {
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            //SpellSchool school = GetFirstSchoolInMask(spellInfo->GetSchoolMask());
            uint8 lvl = me->GetLevel();
            float flatbonus = 0.0f;
            float pctbonus = 0.0f;

            //pct mods
            //Holy Reach
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) &&
                lvl >= 25 && ((spellInfo->SpellFamilyFlags[0] & 0x18400200) || (spellInfo->SpellFamilyFlags[2] & 0x4)))
                pctbonus += 0.2f;

            //flat mods
            //Glyph of Mind Sear
            if (lvl >= 75 && baseId == MIND_SEAR_DAMAGE_1)
                flatbonus += 5.f;

            radius = radius * (1.0f + pctbonus) + flatbonus;
        }

        void ApplyClassSpellRangeMods(SpellInfo const* spellInfo, float& maxrange) const override
        {
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            //SpellSchool school = GetFirstSchoolInMask(spellInfo->GetSchoolMask());
            uint8 lvl = me->GetLevel();
            float flatbonus = 0.0f;
            float pctbonus = 0.0f;

            //pct mods
            //Shadow Reach: +20% range for Shadow Spells
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) && lvl >= 25 &&
                ((spellInfo->SpellFamilyFlags[0] & 0x682A004) ||
                (spellInfo->SpellFamilyFlags[1] & 0x300502) ||
                (spellInfo->SpellFamilyFlags[2] & 0x2040)))
                pctbonus += 0.2f;
            //Holy Reach: +20% range for Holy Spells
            if ((GetSpec() == BOT_SPEC_PRIEST_HOLY) && lvl >= 25 && (spellInfo->SpellFamilyFlags[0] & 0x100080))
                pctbonus += 0.2f;

            //flat mods
            //Glyph of Shackle Undead: +5 yd range for Shackle Undead
            if (lvl >= 20 && baseId == SHACKLE_UNDEAD_1)
                flatbonus += 5.f;

            maxrange = maxrange * (1.0f + pctbonus) + flatbonus;
        }

        void ApplyClassSpellMaxTargetsMods(SpellInfo const* spellInfo, uint32& targets) const override
        {
            uint32 bonusTargets = 0;

            //Glyph of Circle of Healing: + 1 target
            if (spellInfo->SpellFamilyFlags[0] & 0x10000000)
                bonusTargets += 1;

            targets = targets + bonusTargets;
        }

        void ApplyClassEffectMods(SpellInfo const* spellInfo, uint8 effIndex, float& value) const override
        {
            uint32 baseId = spellInfo->GetFirstRankSpell()->Id;
            uint8 lvl = me->GetLevel();
            float pctbonus = 1.0f;

            //Improved Power Word: Fortitude
            if (lvl >= 15 && baseId == PW_FORTITUDE_1 && effIndex == EFFECT_0)
                pctbonus *= 1.3f;
            if (lvl >= 20 && baseId == PW_SHIELD_1 && effIndex == EFFECT_0)
            {
                //Improved PWSH: +15% effect
                pctbonus *= 1.15f;
                //Borrowed Time: +40% of spellpower
                if (GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE && lvl >= 55)
                    value += me->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_MAGIC) * 0.4f;
            }

            value = value * pctbonus;
        }

        void OnClassSpellGo(SpellInfo const* spellInfo) override
        {
            //Surge of Light
            //Inner Focus
            AuraEffect const* surg = me->GetAuraEffect(SURGE_OF_LIGHT_BUFF, 1);
            AuraEffect const* focu = me->GetAuraEffect(INNER_FOCUS_1, 0);
            if (surg && surg->IsAffectedOnSpell(spellInfo))
                me->RemoveAurasDueToSpell(SURGE_OF_LIGHT_BUFF);
            else if (focu && focu->IsAffectedOnSpell(spellInfo))
                me->RemoveAurasDueToSpell(INNER_FOCUS_1);

            //Serendipity
            if (AuraEffect const* sere = me->GetAuraEffect(SERENDIPITY_BUFF, 0))
                if (sere->IsAffectedOnSpell(spellInfo))
                    me->RemoveAurasDueToSpell(SERENDIPITY_BUFF);
        }

        void SpellHitTarget(Unit* wtarget, SpellInfo const* spell) override
        {
            Unit* target = wtarget->ToUnit();
            if (!target)
                return;

            uint32 spellId = spell->Id;
            uint32 baseId = spell->GetFirstRankSpell()->Id;
            uint8 lvl = me->GetLevel();

            //Glyph of Hymn of Hope: +2 sec duration
            if (lvl >= 60 && (baseId == HYMN_OF_HOPE_1 || baseId == HYMN_OF_HOPE_BUFF))
            {
                if (Aura* hymn = target->GetAura(spellId))
                {
                    hymn->SetDuration(hymn->GetDuration() + 2000);
                    hymn->SetDuration(hymn->GetMaxDuration() + 2000);
                }
            }
            //Priest T9 Shadow 2P Bonus (67193)
            if (lvl >= 80 && baseId == VAMPIRIC_TOUCH_1)
            {
                if (Aura* touc = target->GetAura(spellId))
                {
                    uint32 dur = touc->GetMaxDuration() + 6000;
                    touc->SetDuration(dur);
                    touc->SetMaxDuration(dur);
                }
            }

            //Improved Mind Blast part 2
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) && lvl >= 20 && baseId == MIND_BLAST_1)
                me->CastSpell(target, IMPROVED_MIND_BLAST_DEBUFF, true);

            //Weakened Soul Reduction (id: 33333): -2 sec to Weakened Soul duration
            if (lvl >= 51 && baseId == WEAKENED_SOUL_DEBUFF)
            {
                if (Aura* soul = target->GetAura(spellId))
                {
                    uint32 dur = soul->GetMaxDuration() - 2000;
                    soul->SetDuration(dur);
                    soul->SetMaxDuration(dur);
                }
            }
            //Pain and Suffering (part 1): 100% to refresh Shadow Word: Pain on target hit by Mind Flay
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) && lvl >= 50 && baseId == MIND_FLAY_1 && GetSpell(SW_PAIN_1))
                if (Aura* pain = target->GetAura(GetSpell(SW_PAIN_1), me->GetGUID()))
                    pain->RefreshDuration();
            if (baseId == FEAR_WARD_1)
            {
                //2 minutes bonus duration for Fear Ward
                if (Aura* ward = target->GetAura(spellId, me->GetGUID()))
                {
                    uint32 dur = ward->GetDuration() + 120000;
                    ward->SetDuration(dur);
                    ward->SetMaxDuration(dur);
                }
            }
            //buffs duration
            if (baseId == INNER_FIRE_1 || baseId == VAMPIRIC_EMBRACE_1 || baseId == PW_FORTITUDE_1 ||
                baseId == SHADOW_PROTECTION_1 || baseId == DIVINE_SPIRIT_1)
            {
                //1 hour duration for all buffs
                if (Aura* buff = target->GetAura(spellId, me->GetGUID()))
                {
                    uint32 dur = HOUR * IN_MILLISECONDS;
                    buff->SetDuration(dur);
                    buff->SetMaxDuration(dur);
                }
            }

            OnSpellHitTarget(target, spell);
        }

        void SpellHit(Unit* wcaster, SpellInfo const* spell) override
        {
            Unit* caster = wcaster->ToUnit();
            if (!caster)
                return;

            uint32 spellId = spell->Id;
            uint32 baseId = spell->GetFirstRankSpell()->Id;
            uint8 lvl = me->GetLevel();

            //Glyph of Inner Fire + Improved Inner Fire:
            if (lvl >= 15 && baseId == INNER_FIRE_1)
            {
                if (Aura* fire = me->GetAura(spellId))
                {
                    fire->SetCharges(fire->GetCharges() + 12);
                    for (uint8 i = 0; i != MAX_SPELL_EFFECTS; ++i)
                        if (AuraEffect* eff = fire->GetEffect(i))
                            eff->ChangeAmount(int32(eff->GetAmount() * (i == 0 ? 1.45f*1.5f : 1.45f)));
                }
            }
            //Improved Vampiric Embrace
            if ((GetSpec() == BOT_SPEC_PRIEST_SHADOW) && lvl >= 30 && baseId == VAMPIRIC_EMBRACE_1)
            {
                if (AuraEffect* vamp = me->GetAuraEffect(spellId, 0))
                    vamp->ChangeAmount(vamp->GetAmount() + 10); //67% is essentially this
            }

            OnSpellHit(caster, spell);
        }

        void DamageDealt(Unit* victim, uint32& damage, DamageEffectType damageType) override
        {
            bot_ai::DamageDealt(victim, damage, damageType);
        }

        void DamageTaken(Unit* u, uint32& /*damage*/, DamageEffectType /*damageType*/, SpellSchoolMask /*schoolMask*/) override
        {
            if (!u)
                return;
            if (!u->IsInCombat() && !me->IsInCombat())
                return;
            OnOwnerDamagedBy(u);
        }

        void OwnerAttackedBy(Unit* u) override
        {
            if (!u)
                return;
            OnOwnerDamagedBy(u);
        }

        float GetSpellAttackRange(bool longRange) const override
        {
            return longRange ? CalcSpellMaxRange(MIND_FLAY_1) : 20.f;
        }

        void SummonBotPet(Unit* target)
        {
            if (botPet)
                UnsummonAll();

            uint32 entry = BOT_PET_SHADOWFIEND;

            //Position pos;

            //15 sec duration
            Creature* myPet = me->SummonCreature(entry, *me, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
            //me->GetNearPoint(myPet, pos.m_positionX, pos.m_positionY, pos.m_positionZ, 0, 2, me->GetOrientation());
            //myPet->GetMotionMaster()->MovePoint(me->GetMapId(), pos);
            myPet->SetCreator(master);
            myPet->SetOwnerGUID(me->GetGUID());
            myPet->SetFaction(master->GetFaction());
            myPet->SetControlledByPlayer(!IAmFree());
            myPet->SetPvP(me->IsPvP());
            myPet->SetByteValue(UNIT_FIELD_BYTES_2, 1, master->GetByteValue(UNIT_FIELD_BYTES_2, 1));
            myPet->SetUInt32Value(UNIT_CREATED_BY_SPELL, SHADOWFIEND_1);

            botPet = myPet;

            myPet->Attack(target, true);
            if (!HasBotCommandState(BOT_COMMAND_MASK_UNCHASE))
                myPet->GetMotionMaster()->MoveChase(target);
        }

        void UnsummonAll() override
        {
            if (botPet)
                botPet->ToTempSummon()->UnSummon();
        }

        void SummonedCreatureDies(Creature* /*summon*/, Unit* /*killer*/) override
        {
        }

        void SummonedCreatureDespawn(Creature* summon) override
        {
            //TC_LOG_ERROR("entities.unit", "SummonedCreatureDespawn: %s's %s", me->GetName().c_str(), summon->GetName().c_str());
            if (summon == botPet)
                botPet = nullptr;
        }

        uint32 GetAIMiscValue(uint32 data) const override
        {
            switch (data)
            {
                case BOTAI_MISC_PET_TYPE:
                    return BOT_PET_SHADOWFIEND;
                default:
                    return 0;
            }
        }

        void Reset() override
        {
            UnsummonAll();

            Shackle_Timer = 0;
            Mend_Timer = 0;

            DispelcheckTimer = 0;
            DevcheckTimer = 0;
            ShackcheckTimer = 0;

            Devcheck = false;
            Shackcheck = false;

            DefaultInit();
        }

        void ReduceCD(uint32 diff) override
        {
            if (Shackle_Timer > diff)               Shackle_Timer -= diff;
            if (Mend_Timer > diff)                  Mend_Timer -= diff;

            if (DispelcheckTimer > diff)            DispelcheckTimer -= diff;
            if (DevcheckTimer > diff)               DevcheckTimer -= diff;
            if (ShackcheckTimer > diff)             ShackcheckTimer -= diff;
        }

        void InitPowers() override
        {
            me->SetPowerType(POWER_MANA);
        }

        void InitSpells() override
        {
            uint8 lvl = me->GetLevel();
            bool isDisc = GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE;
            bool isHoly = GetSpec() == BOT_SPEC_PRIEST_HOLY;
            bool isShad = GetSpec() == BOT_SPEC_PRIEST_SHADOW;

            InitSpellMap(DISPEL_MAGIC_1);
            InitSpellMap(MASS_DISPEL_1);
            InitSpellMap(CURE_DISEASE_1);
            InitSpellMap(ABOLISH_DISEASE_1);
            InitSpellMap(FEAR_WARD_1);
            InitSpellMap(PSYCHIC_SCREAM_1);
            InitSpellMap(FADE_1);
            InitSpellMap(MIND_SEAR_1);
            InitSpellMap(SHACKLE_UNDEAD_1);
            InitSpellMap(GREATER_HEAL_1);
            InitSpellMap(NORMAL_HEAL_1);
            InitSpellMap(LESSER_HEAL_1);
            InitSpellMap(RENEW_1);
            InitSpellMap(FLASH_HEAL_1);
            InitSpellMap(PRAYER_OF_HEALING_1);
            InitSpellMap(DIVINE_HYMN_1);
            InitSpellMap(PRAYER_OF_MENDING_1);
            InitSpellMap(RESURRECTION_1);
            InitSpellMap(PW_SHIELD_1);
            InitSpellMap(INNER_FIRE_1);
            InitSpellMap(PW_FORTITUDE_1);
            InitSpellMap(SHADOW_PROTECTION_1);
            InitSpellMap(DIVINE_SPIRIT_1);
            InitSpellMap(HOLY_FIRE_1);
            InitSpellMap(SMITE_1);
            InitSpellMap(SW_PAIN_1);
            InitSpellMap(MIND_BLAST_1);
            InitSpellMap(SW_DEATH_1);
            InitSpellMap(DEVOURING_PLAGUE_1);
            InitSpellMap(HYMN_OF_HOPE_1);
            InitSpellMap(LEVITATE_1);
            InitSpellMap(SHADOWFIEND_1); //not casted

  /*Talent*/lvl >= 20 && isDisc ? InitSpellMap(INNER_FOCUS_1) : RemoveSpell(INNER_FOCUS_1);
  /*Talent*/lvl >= 40 && isDisc ? InitSpellMap(POWER_INFUSION_1) : RemoveSpell(POWER_INFUSION_1);
  /*Talent*/lvl >= 50 && isDisc ? InitSpellMap(PAIN_SUPPRESSION_1) : RemoveSpell(PAIN_SUPPRESSION_1);
  /*Talent*/lvl >= 60 && isDisc ? InitSpellMap(PENANCE_1) : RemoveSpell(PENANCE_1);

  /*Talent*/lvl >= 20 && isHoly ? InitSpellMap(DESPERATE_PRAYER_1) : RemoveSpell(DESPERATE_PRAYER_1);
  /*Talent*/lvl >= 50 && isHoly ? InitSpellMap(CIRCLE_OF_HEALING_1) : RemoveSpell(CIRCLE_OF_HEALING_1);
  /*Talent*/lvl >= 60 && isHoly ? InitSpellMap(GUARDIAN_SPIRIT_1) : RemoveSpell(GUARDIAN_SPIRIT_1);

  /*Talent*/lvl >= 20 && isShad ? InitSpellMap(MIND_FLAY_1) : RemoveSpell(MIND_FLAY_1);
  /*Talent*/lvl >= 30 && isShad ? InitSpellMap(SILENCE_1) : RemoveSpell(SILENCE_1);
  /*Talent*/lvl >= 30 && isShad ? InitSpellMap(VAMPIRIC_EMBRACE_1) : RemoveSpell(VAMPIRIC_EMBRACE_1);
  /*Talent*/lvl >= 40 && isShad ? InitSpellMap(SHADOWFORM_1) : RemoveSpell(SHADOWFORM_1);
  /*Talent*/lvl >= 50 && isShad ? InitSpellMap(VAMPIRIC_TOUCH_1) : RemoveSpell(VAMPIRIC_TOUCH_1);
  /*Talent*/lvl >= 50 && isShad ? InitSpellMap(PSYCHIC_HORROR_1) : RemoveSpell(PSYCHIC_HORROR_1);
  /*Talent*/lvl >= 60 && isShad ? InitSpellMap(DISPERSION_1) : RemoveSpell(DISPERSION_1);

            HEAL = GetSpell(GREATER_HEAL_1) ? GREATER_HEAL_1 :
                GetSpell(NORMAL_HEAL_1) ? NORMAL_HEAL_1 :
                GetSpell(LESSER_HEAL_1) ? LESSER_HEAL_1 : 0;
        }

        void ApplyClassPassives() const override
        {
            uint8 level = master->GetLevel();
            bool isDisc = GetSpec() == BOT_SPEC_PRIEST_DISCIPLINE;
            bool isHoly = GetSpec() == BOT_SPEC_PRIEST_HOLY;
            bool isShad = GetSpec() == BOT_SPEC_PRIEST_SHADOW;

            RefreshAura(UNBREAKABLE_WILL, level >= 10 ? 1 : 0);
            RefreshAura(MEDITATION, level >= 20 ? 1 : 0);
            RefreshAura(RENEWED_HOPE, isDisc && level >= 45 ? 1 : 0);
            RefreshAura(RAPTURE, isDisc && level >= 45 ? 1 : 0);
            RefreshAura(DIVINE_AEGIS, isDisc && level >= 50 ? 1 : 0);
            RefreshAura(GRACE, isDisc && level >= 50 ? 1 : 0);
            RefreshAura(BORROWED_TIME, isDisc && level >= 55 ? 1 : 0);

            RefreshAura(INSPIRATION3, isHoly && level >= 25 ? 1 : 0);
            RefreshAura(INSPIRATION2, isHoly && level >= 23 && level < 25 ? 1 : 0);
            RefreshAura(INSPIRATION1, isHoly && level >= 20 && level < 23 ? 1 : 0);
            RefreshAura(SURGE_OF_LIGHT, isHoly && level >= 35 ? 1 : 0);
            RefreshAura(HOLY_CONCENTRATION, isHoly && level >= 40 ? 1 : 0);
            RefreshAura(BODY_AND_SOUL1, isHoly && level >= 45 ? 1 : 0);
            RefreshAura(SERENDIPITY, isHoly && level >= 45 ? 1 : 0);
            RefreshAura(EMPOWERED_RENEW3, isHoly && level >= 55 ? 1 : 0);
            RefreshAura(EMPOWERED_RENEW2, isHoly && level >= 53 && level < 55 ? 1 : 0);
            RefreshAura(EMPOWERED_RENEW1, isHoly && level >= 50 && level < 53 ? 1 : 0);

            RefreshAura(SPIRIT_TAP, isShad && level >= 10 ? 1 : 0);
            RefreshAura(IMPROVED_SPIRIT_TAP, isShad && level >= 10 ? 1 : 0);
            RefreshAura(SHADOW_WEAVING3, isShad && level >= 30 ? 1 : 0);
            RefreshAura(SHADOW_WEAVING2, isShad && level >= 28 && level < 30 ? 1 : 0);
            RefreshAura(SHADOW_WEAVING1, isShad && level >= 25 && level < 28 ? 1 : 0);
            RefreshAura(IMPROVED_DEVOURING_PLAGUE, isShad && level >= 35 ? 1 : 0);
            RefreshAura(IMPROVED_SHADOWFORM, isShad && level >= 45 ? 1 : 0);
            RefreshAura(MISERY3, isShad && level >= 50 ? 1 : 0);
            RefreshAura(MISERY2, isShad && level >= 48 && level < 50 ? 1 : 0);
            RefreshAura(MISERY1, isShad && level >= 45 && level < 48 ? 1 : 0);

            //RefreshAura(GLYPH_SW_PAIN, level >= 15 ? 1 : 0);
            RefreshAura(GLYPH_PW_SHIELD, level >= 15 ? 1 : 0);
            RefreshAura(GLYPH_DISPEL_MAGIC, level >= 18 ? 1 : 0);
            RefreshAura(GLYPH_PRAYER_OF_HEALING, level >= 30 ? 1 : 0);
            RefreshAura(GLYPH_SHADOW, level >= 30 ? 1 : 0);
            RefreshAura(PRIEST_T10_2P_BONUS, level >= 70 ? 1 : 0);
        }

        bool CanUseManually(uint32 basespell) const override
        {
            switch (basespell)
            {
                case MASS_DISPEL_1:
                case ABOLISH_DISEASE_1:
                case PAIN_SUPPRESSION_1:
                case FADE_1:
                case PENANCE_1:
                case VAMPIRIC_EMBRACE_1:
                case DISPERSION_1:
                case GUARDIAN_SPIRIT_1:
                case RENEW_1:
                case PRAYER_OF_HEALING_1:
                case CIRCLE_OF_HEALING_1:
                case DIVINE_HYMN_1:
                case PRAYER_OF_MENDING_1:
                case PW_SHIELD_1:
                case INNER_FIRE_1:
                case PW_FORTITUDE_1:
                case SHADOW_PROTECTION_1:
                case DIVINE_SPIRIT_1:
                case FEAR_WARD_1:
                case FLASH_HEAL_1:
                case GREATER_HEAL_1:
                case LEVITATE_1:
                    return true;
                case NORMAL_HEAL_1:
                    return !GetSpell(GREATER_HEAL_1);
                case LESSER_HEAL_1:
                    return !GetSpell(NORMAL_HEAL_1) && !GetSpell(GREATER_HEAL_1);
                case SHADOWFORM_1:
                    return me->GetShapeshiftForm() != FORM_SHADOW;
                default:
                    return false;
            }
        }

        std::vector<uint32> const* GetDamagingSpellsList() const override
        {
            return &Priest_spells_damage;
        }
        std::vector<uint32> const* GetCCSpellsList() const override
        {
            return &Priest_spells_cc;
        }
        std::vector<uint32> const* GetHealingSpellsList() const override
        {
            return &Priest_spells_heal;
        }
        std::vector<uint32> const* GetSupportSpellsList() const override
        {
            return &Priest_spells_support;
        }

        void InitHeals() override
        {
            SpellInfo const* spellInfo;
            if (InitSpell(me, HEAL))
            {
                spellInfo = sSpellMgr->GetSpellInfo(InitSpell(me, HEAL));
                _heals[HEAL] = me->SpellHealingBonusDone(me, spellInfo, spellInfo->Effects[0].CalcValue(me), DamageEffectType::HEAL, 0);
            }
            else
                _heals[HEAL] = 0;

            if (InitSpell(me, FLASH_HEAL_1))
            {
                spellInfo = sSpellMgr->GetSpellInfo(InitSpell(me, FLASH_HEAL_1));
                _heals[FLASH_HEAL_1] = me->SpellHealingBonusDone(me, spellInfo, spellInfo->Effects[0].CalcValue(me), DamageEffectType::HEAL, 0);
            }
            else
                _heals[FLASH_HEAL_1] = 0;

            if (InitSpell(me, PENANCE_HEAL_1))
            {
                spellInfo = sSpellMgr->GetSpellInfo(InitSpell(me, PENANCE_HEAL_1));
                _heals[PENANCE_1] = me->SpellHealingBonusDone(me, spellInfo, spellInfo->Effects[0].CalcValue(me), DamageEffectType::HEAL, 0);
            }
            else
                _heals[PENANCE_1] = 0;

            if (InitSpell(me, DESPERATE_PRAYER_1))
            {
                spellInfo = sSpellMgr->GetSpellInfo(InitSpell(me, DESPERATE_PRAYER_1));
                _heals[DESPERATE_PRAYER_1] = me->SpellHealingBonusDone(me, spellInfo, spellInfo->Effects[0].CalcValue(me), DamageEffectType::HEAL, 0);
            }
            else
                _heals[DESPERATE_PRAYER_1] = 0;
        }

    private:
        //Dinkle
        bool needHealingFlag;
        bool needManaFlag;
        //end Dinkle
        uint32 HEAL;
        uint32 Shackle_Timer, Mend_Timer, DispelcheckTimer, DevcheckTimer, ShackcheckTimer;
/*Misc*/bool Devcheck, Shackcheck;

        typedef std::unordered_map<uint32 /*baseId*/, int32 /*amount*/> HealMap;
        HealMap _heals;
    };
};

void AddSC_priest_bot()
{
    new priest_bot();
}

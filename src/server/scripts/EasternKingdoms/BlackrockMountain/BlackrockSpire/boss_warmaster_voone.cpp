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

#include "CreatureScript.h"
#include "ScriptedCreature.h"
#include "blackrock_spire.h"
#include "TemporarySummon.h"
#include "../scripts/Custom/Timewalking/10Man.h"

enum Spells
{
    SPELL_THRASH = 3391,
    SPELL_SNAPKICK = 15618,
    SPELL_CLEAVE = 15284,
    SPELL_UPPERCUT = 10966,
    SPELL_MORTALSTRIKE = 16856,
    SPELL_PUMMEL = 15615,
    SPELL_THROWAXE = 16075,
    SPELL_RAGE_BURST = 826663, 
    SPELL_DISARM = 6713,  
    SPELL_WHIRLWIND = 1680  
};

enum Events
{
    EVENT_SNAP_KICK = 1,
    EVENT_CLEAVE,
    EVENT_UPPERCUT,
    EVENT_MORTAL_STRIKE,
    EVENT_PUMMEL,
    EVENT_THROW_AXE,
    EVENT_THRASH,
    EVENT_BERSERK,
    EVENT_RAGE_BURST,
    EVENT_DISARM,
    EVENT_WHIRLWIND,
    EVENT_CALL_REINFORCEMENTS
};

enum Phases
{
    PHASE_THRASHER = 1,
    PHASE_BRAWLER,
    PHASE_WARMASTER,
    PHASE_BERSERK
};

enum EventGroups
{
    GROUP_THRASHER = 1,
    GROUP_BRAWLER,
    GROUP_WARMASTER,
    GROUP_BERSERK
};

class boss_warmaster_voone : public CreatureScript
{
public:
    boss_warmaster_voone() : CreatureScript("boss_warmaster_voone") { }

    struct boss_warmastervooneAI : public BossAI
    {
        boss_warmastervooneAI(Creature* creature) : BossAI(creature, DATA_WARMASTER_VOONE) { }

        void JustEngagedWith(Unit* /*who*/) override
        {
            _JustEngagedWith();

            events.SetPhase(PHASE_BRAWLER);
            events.ScheduleEvent(EVENT_THRASH, 3s, GROUP_BRAWLER, PHASE_BRAWLER);
            events.ScheduleEvent(EVENT_THROW_AXE, 1s, GROUP_BRAWLER, PHASE_BRAWLER);
            events.ScheduleEvent(EVENT_RAGE_BURST, 5s); // Initial rage burst
        }

        void Reset() override
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*type*/, SpellSchoolMask /*school*/) override
        {
            if (me->HealthBelowPctDamaged(65, damage) && events.IsInPhase(PHASE_BRAWLER))
            {
                events.SetPhase(PHASE_THRASHER);
                events.CancelEventGroup(GROUP_BRAWLER);
                events.ScheduleEvent(EVENT_CLEAVE, 14s, GROUP_THRASHER, PHASE_THRASHER);
                events.ScheduleEvent(EVENT_MORTAL_STRIKE, 12s, GROUP_THRASHER, PHASE_THRASHER);
                events.ScheduleEvent(EVENT_CALL_REINFORCEMENTS, 3s, GROUP_THRASHER, PHASE_THRASHER); // First call for reinforcements
            }
            else if (me->HealthBelowPctDamaged(40, damage) && events.IsInPhase(PHASE_THRASHER))
            {
                events.SetPhase(PHASE_WARMASTER);
                events.CancelEventGroup(GROUP_THRASHER);
                events.ScheduleEvent(EVENT_SNAP_KICK, 8s, GROUP_WARMASTER, PHASE_WARMASTER);
                events.ScheduleEvent(EVENT_UPPERCUT, 20s, GROUP_WARMASTER, PHASE_WARMASTER);
                events.ScheduleEvent(EVENT_PUMMEL, 32s, GROUP_WARMASTER, PHASE_WARMASTER);
                events.ScheduleEvent(EVENT_DISARM, 25s, GROUP_WARMASTER, PHASE_WARMASTER); // Disarm effect
            }
            else if (me->HealthBelowPctDamaged(20, damage) && !events.IsInPhase(PHASE_BERSERK))
            {
                events.SetPhase(PHASE_BERSERK);
                events.ScheduleEvent(EVENT_WHIRLWIND, 5s, GROUP_BERSERK, PHASE_BERSERK); // Whirlwind attack
                events.ScheduleEvent(EVENT_SNAP_KICK, 4s, GROUP_BERSERK, PHASE_BERSERK);
                events.ScheduleEvent(EVENT_PUMMEL, 10s, GROUP_BERSERK, PHASE_BERSERK);
                events.ScheduleEvent(EVENT_THROW_AXE, 15s, GROUP_BERSERK, PHASE_BERSERK);
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            Map::PlayerList const& players = me->GetMap()->GetPlayers();
            if (players.begin() != players.end())
            {
                uint32 baseRewardLevel = 1;
                bool isDungeon = me->GetMap()->IsDungeon();

                Player* player = players.begin()->GetSource();
                if (player)
                {
                    DistributeChallengeRewards(player, me, baseRewardLevel, isDungeon);
                }
            }
        }

        void ExecuteEvent(uint32 eventId) override
        {
            switch (eventId)
            {
            case EVENT_SNAP_KICK:
                DoCastVictim(SPELL_SNAPKICK);
                events.RepeatEvent(6 * IN_MILLISECONDS);
                break;
            case EVENT_CLEAVE:
                DoCastVictim(SPELL_CLEAVE);
                events.RepeatEvent(12 * IN_MILLISECONDS);
                break;
            case EVENT_UPPERCUT:
                DoCastVictim(SPELL_UPPERCUT);
                events.RepeatEvent(14 * IN_MILLISECONDS);
                break;
            case EVENT_MORTAL_STRIKE:
                DoCastVictim(SPELL_MORTALSTRIKE);
                events.RepeatEvent(10 * IN_MILLISECONDS);
                break;
            case EVENT_PUMMEL:
                DoCastVictim(SPELL_PUMMEL);
                events.RepeatEvent(16 * IN_MILLISECONDS);
                break;
            case EVENT_THROW_AXE:
                DoCastRandomTarget(SPELL_THROWAXE);
                events.RepeatEvent(8 * IN_MILLISECONDS);
                break;
            case EVENT_THRASH:
                DoCastSelf(SPELL_THRASH);
                events.RepeatEvent(10 * IN_MILLISECONDS);
                break;
            case EVENT_RAGE_BURST:
                me->Yell("Behold da fury of a true warrior!", LANG_UNIVERSAL);
                DoCastSelf(SPELL_RAGE_BURST);
                events.RepeatEvent(20 * IN_MILLISECONDS);
                break;
            case EVENT_DISARM:
                DoCastVictim(SPELL_DISARM);
                events.RepeatEvent(30 * IN_MILLISECONDS);
                break;
            case EVENT_WHIRLWIND:
                DoCastVictim(SPELL_WHIRLWIND);
                events.RepeatEvent(15 * IN_MILLISECONDS);
                break;
            case EVENT_CALL_REINFORCEMENTS:
            {
                me->Yell("Brothers of da Smolderthorn, to me aid! Show dese invaders da strength of da Smolderthorns!", LANG_UNIVERSAL);

                Position summonPos1 = me->GetNearPosition(5.0f, 0.0f);
                if (TempSummon* summon1 = me->SummonCreature(9268, summonPos1))
                {
                    summon1->AI()->DoZoneInCombat();
                }

                Position summonPos2 = me->GetNearPosition(5.0f, M_PI / 4); 
                if (TempSummon* summon2 = me->SummonCreature(9268, summonPos2))
                {
                    summon2->AI()->DoZoneInCombat();
                }

                events.RepeatEvent(35 * IN_MILLISECONDS); // Adjust timing as needed for balance.
                break;
            }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetBlackrockSpireAI<boss_warmastervooneAI>(creature);
    }
};

void AddSC_boss_warmastervoone()
{
    new boss_warmaster_voone();
}

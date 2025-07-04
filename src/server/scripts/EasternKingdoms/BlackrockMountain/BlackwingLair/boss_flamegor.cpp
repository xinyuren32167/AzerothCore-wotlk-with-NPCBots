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
#include "blackwing_lair.h"

enum Emotes
{
    EMOTE_FRENZY            = 0,
};

enum Spells
{
    SPELL_SHADOWFLAME        = 22539,
    SPELL_WINGBUFFET         = 23339,
    SPELL_FRENZY             = 23342  //This spell periodically triggers fire nova
};

enum Events
{
    EVENT_SHADOWFLAME = 1,
    EVENT_WINGBUFFET = 2,
    EVENT_FRENZY = 3,
    EVENT_SPAWN_SHADES = 4
};

const uint32 NPC_SHADE_OF_FLAMEGOR = 400149;

class boss_flamegor : public CreatureScript
{
public:
    boss_flamegor() : CreatureScript("boss_flamegor") { }

    struct boss_flamegorAI : public BossAI
    {
        boss_flamegorAI(Creature* creature) : BossAI(creature, DATA_FLAMEGOR) { }

        void JustEngagedWith(Unit* who) override
        {
            BossAI::JustEngagedWith(who);

            events.ScheduleEvent(EVENT_SHADOWFLAME, 18s);
            events.ScheduleEvent(EVENT_WINGBUFFET, 30s);
            events.ScheduleEvent(EVENT_FRENZY, 10s);
            events.ScheduleEvent(EVENT_SPAWN_SHADES, 20s);
        }

        void Reset() override
        {
            BossAI::Reset();
            DespawnShades();
        }

        void JustDied(Unit* /*killer*/) override
        {
            BossAI::JustDied(nullptr);
            DespawnShades();
            DoCastSelf(875167, true);
            Map::PlayerList const& players = me->GetMap()->GetPlayers();
            for (auto const& playerPair : players)
            {
                Player* player = playerPair.GetSource();
                if (player)
                {
                    DistributeChallengeRewards(player, me, 1, false);
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SHADOWFLAME:
                        DoCastVictim(SPELL_SHADOWFLAME);
                        events.ScheduleEvent(EVENT_SHADOWFLAME, 15s, 25s);
                        break;
                    case EVENT_WINGBUFFET:
                        DoCastVictim(SPELL_WINGBUFFET);
                        if (DoGetThreat(me->GetVictim()))
                            DoModifyThreatByPercent(me->GetVictim(), -75);
                        events.ScheduleEvent(EVENT_WINGBUFFET, 30s);
                        break;
                    case EVENT_FRENZY:
                        Talk(EMOTE_FRENZY);
                        DoCast(me, SPELL_FRENZY);
                        events.ScheduleEvent(EVENT_FRENZY, 8s, 10s);
                        break;
                    case EVENT_SPAWN_SHADES:
                        SpawnShades();
                        events.ScheduleEvent(EVENT_SPAWN_SHADES, 50s); 
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }

            DoMeleeAttackIfReady();
        }

        void SpawnShades()
        {
            for (int i = 0; i < 2; ++i)  
            {
                Creature* shade = DoSummon(NPC_SHADE_OF_FLAMEGOR, me, 10.0f, 180000, TEMPSUMMON_CORPSE_DESPAWN);
                if (shade)
                    shade->SetInCombatWithZone();
            }
        }

        void DespawnShades()
        {
            std::list<Creature*> shades;
            GetCreatureListWithEntryInGrid(shades, me, NPC_SHADE_OF_FLAMEGOR, 150.0f);
            for (Creature* shade : shades)
            {
                if (shade)
                    shade->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetBlackwingLairAI<boss_flamegorAI>(creature);
    }
};

void AddSC_boss_flamegor()
{
    new boss_flamegor();
}

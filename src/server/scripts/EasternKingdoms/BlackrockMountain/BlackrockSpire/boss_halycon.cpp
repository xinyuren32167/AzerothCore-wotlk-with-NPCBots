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

enum Spells
{
    SPELL_REND = 13738,
    SPELL_THRASH = 3391,
    SPELL_HEROIC_LEAP = 52174,
    SPELL_ENRAGE = 8269
};

enum Events
{
    EVENT_REND = 1,
    EVENT_THRASH = 2,
    EVENT_HEROIC_LEAP = 3,
    EVENT_ENRAGE = 4
};


enum Says
{
    EMOTE_DEATH                     = 0
};

const Position SummonLocation = { -167.9561f, -411.7844f, 76.23057f, 1.53589f };

class boss_halycon : public CreatureScript
{
public:
    boss_halycon() : CreatureScript("boss_halycon") { }

    struct boss_halyconAI : public BossAI
    {
        boss_halyconAI(Creature* creature) : BossAI(creature, DATA_HALYCON) { }

        void Reset() override
        {
            _Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            _JustEngagedWith();
            events.ScheduleEvent(EVENT_REND, 17s, 20s);
            events.ScheduleEvent(EVENT_THRASH, 10s, 12s);
            events.ScheduleEvent(EVENT_HEROIC_LEAP, 8s, 15s);
            events.ScheduleEvent(EVENT_ENRAGE, 2min);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            me->SummonCreature(NPC_GIZRUL_THE_SLAVENER, SummonLocation, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);
            Talk(EMOTE_DEATH);
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
                    case EVENT_REND:
                        DoCastVictim(SPELL_REND);
                        events.ScheduleEvent(EVENT_REND, 8s, 10s);
                        break;
                    case EVENT_THRASH:
                        DoCast(me, SPELL_THRASH);
                        break;
                    case EVENT_HEROIC_LEAP:
                        DoCastRandomTarget(SPELL_HEROIC_LEAP, 0, 35.0f, false, true); 
                        events.ScheduleEvent(EVENT_HEROIC_LEAP, 20s, 30s); 
                        break;
                    case EVENT_ENRAGE:
                        DoCast(me, SPELL_ENRAGE, true);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetBlackrockSpireAI<boss_halyconAI>(creature);
    }
};

void AddSC_boss_halycon()
{
    new boss_halycon();
}

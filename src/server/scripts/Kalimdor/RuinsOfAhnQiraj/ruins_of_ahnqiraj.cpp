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
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "ruins_of_ahnqiraj.h"

enum Spells
{
    // Hive'Zara Stinger
    SPELL_HIVEZARA_CATALYST             = 25187,
    SPELL_STINGER_CHARGE_NORMAL         = 25190,
    SPELL_STINGER_CHARGE_BUFFED         = 25191,

    // Obsidian Destroyer
    SPELL_PURGE                         = 25756,
    SPELL_DRAIN_MANA                    = 25755,
    SPELL_DRAIN_MANA_VISUAL             = 26639,
    SPELL_SUMMON_SMALL_OBSIDIAN_CHUNK   = 27627, // Server-side
};

struct npc_hivezara_stinger : public ScriptedAI
{
    npc_hivezara_stinger(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        scheduler.CancelAll();
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(who ,who->HasAura(SPELL_HIVEZARA_CATALYST) ? SPELL_STINGER_CHARGE_BUFFED : SPELL_STINGER_CHARGE_NORMAL, true);

        scheduler.Schedule(5s, [this](TaskContext context)
        {
            Unit* target = SelectTarget(SelectTargetMethod::Random, 1, [&](Unit* u)
            {
                return u && !u->IsPet() && u->IsWithinDist2d(me, 20.f) && u->HasAura(SPELL_HIVEZARA_CATALYST);
            });
            if (!target)
            {
                target = SelectTarget(SelectTargetMethod::Random, 1, [&](Unit* u)
                {
                    return u && !u->IsPet() && u->IsWithinDist2d(me, 20.f);
                });
            }

            if (target)
            {
                DoCast(target, target->HasAura(SPELL_HIVEZARA_CATALYST) ? SPELL_STINGER_CHARGE_BUFFED : SPELL_STINGER_CHARGE_NORMAL, true);
            }

            context.Repeat(4500ms, 6500ms);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
        {
            return;
        }

        scheduler.Update(diff,
            std::bind(&ScriptedAI::DoMeleeAttackIfReady, this));
    }
};

struct npc_obsidian_destroyer : public ScriptedAI
{
    npc_obsidian_destroyer(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        scheduler.CancelAll();
        me->SetPower(POWER_MANA, 0);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        scheduler.Schedule(6s, [this](TaskContext context)
        {
            std::list<Unit*> targets;
            SelectTargetList(targets, 6, SelectTargetMethod::Random, 1, [&](Unit* target)
            {
                return target && target->IsPlayer() && target->GetPower(POWER_MANA) > 0;
            });

            for (Unit* target : targets)
            {
                DoCast(target, SPELL_DRAIN_MANA, true);
            }

            if (me->GetPowerPct(POWER_MANA) >= 100.f)
            {
                DoCastAOE(SPELL_PURGE, true);
            }

            context.Repeat(6s);
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSelf(SPELL_SUMMON_SMALL_OBSIDIAN_CHUNK, true);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
        {
            return;
        }

        scheduler.Update(diff,
            std::bind(&ScriptedAI::DoMeleeAttackIfReady, this));
    }
};

enum FleshHunterSpells
{
    SPELL_TRASH = 3391,
    SPELL_CONSUME = 25371,
    SPELL_CONSUME_HEAL = 25378,
    SPELL_POISON_BOLT = 25424,
    SPELL_CONSUME_DMG = 25373,
    SPELL_SPLIT = 25383,
};

class mob_flesh_hunter : public CreatureScript
{
public:
    mob_flesh_hunter() : CreatureScript("mob_flesh_hunter") { }

    struct mob_flesh_hunterAI : public ScriptedAI
    {
        mob_flesh_hunterAI(Creature* creature) : ScriptedAI(creature) { }

        ObjectGuid m_uiConsumeVictim;

        uint32 m_uiPoisonBolt_Timer;
        uint32 m_uiTrash_Timer;
        uint32 m_uiConsume_Timer;
        uint32 m_uiConsumeDamage_Timer;

        bool m_bPlayerConsumed;
        bool m_bPlayerConsumedCharged;

        void Reset() override
        {
            m_uiPoisonBolt_Timer = 3000;
            m_uiTrash_Timer = 5000;
            m_uiConsume_Timer = 3000;
            m_uiConsumeDamage_Timer = 1000;

            m_uiConsumeVictim.Clear();
            m_bPlayerConsumed = false;
            m_bPlayerConsumedCharged = false;
        }

        void JustEngagedWith(Unit* who) override
        {
            me->SetInCombatWithZone();
        }

        void KilledUnit(Unit* victim) override
        {
            if (victim->GetGUID() == m_uiConsumeVictim)
                DoCast(me, SPELL_CONSUME_HEAL);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (m_uiPoisonBolt_Timer <= diff)
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
                {
                    DoCast(target, SPELL_POISON_BOLT);
                    m_uiPoisonBolt_Timer = 3000;
                }
            }
            else
                m_uiPoisonBolt_Timer -= diff;

            if (m_uiConsume_Timer <= diff)
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 0.0f, true))
                {
                    DoCast(target, SPELL_CONSUME);
                    m_uiConsumeVictim = target->GetGUID();
                    m_bPlayerConsumed = true;
                    m_uiConsume_Timer = 30000;
                }
            }
            else
                m_uiConsume_Timer -= diff;

            if (Unit* pConsumeTarget = ObjectAccessor::GetUnit(*me, m_uiConsumeVictim))
            {
                if (pConsumeTarget->HasAura(SPELL_CONSUME))
                {
                    if (m_uiConsumeDamage_Timer <= diff)
                    {
                        DoCast(pConsumeTarget, SPELL_CONSUME_DMG);
                        me->GetMotionMaster()->Clear();
                        me->StopMoving();
                        me->GetThreatMgr().ResetThreat(pConsumeTarget);
                        m_uiConsumeDamage_Timer = 1000;
                        m_bPlayerConsumedCharged = true;
                        pConsumeTarget->ModifyHealth(-int32(pConsumeTarget->GetMaxHealth() / 10));
                    }
                    else
                        m_uiConsumeDamage_Timer -= diff;

                    if (!pConsumeTarget->IsAlive())
                    {
                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
                            me->GetMotionMaster()->MoveChase(target);
                        me->SetHealth(me->GetMaxHealth());
                    }
                }
                else
                {
                    if (pConsumeTarget->IsAlive() && m_bPlayerConsumedCharged)
                    {
                        DoCast(pConsumeTarget, SPELL_SPLIT);
                        m_bPlayerConsumedCharged = false;
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                    }
                }
            }

            if (m_uiTrash_Timer <= diff)
            {
                DoCastVictim(SPELL_TRASH);
                m_uiTrash_Timer = 5000 + urand(0, 2000);
            }
            else
                m_uiTrash_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_flesh_hunterAI(creature);
    }
};

class spell_drain_mana : public SpellScript
{
    PrepareSpellScript(spell_drain_mana);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                target->CastSpell(caster, SPELL_DRAIN_MANA_VISUAL, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_drain_mana::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

void AddSC_ruins_of_ahnqiraj()
{
    RegisterRuinsOfAhnQirajCreatureAI(npc_hivezara_stinger);
    RegisterRuinsOfAhnQirajCreatureAI(npc_obsidian_destroyer);
    new mob_flesh_hunter();
    RegisterSpellScript(spell_drain_mana);
}


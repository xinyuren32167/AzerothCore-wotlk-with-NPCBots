#include "bot_ai.h"
#include "botcommon.h"
#include "botdatamgr.h"
#include "botgossip.h"
#include "botspell.h"
#include "bottext.h"
#include "botmgr.h"
#include "Creature.h"
#include "Log.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "botdatamgr.h"

using namespace lfg;
/*
NPCbot giver NPC by Trickerer (<https://github.com/trickerer/> <onlysuffering@gmail.com>)
Complete - 100%
*/

#define HIRE GOSSIP_SENDER_BOTGIVER_HIRE
#define HIRE_CLASS GOSSIP_SENDER_BOTGIVER_HIRE_CLASS
#define HIRE_ENTRY GOSSIP_SENDER_BOTGIVER_HIRE_ENTRY
#define HIRE_RAID_GROUP GOSSIP_SENDER_BOTGIVER_HIRE_RAID_GROUP
#define HIRE_RAID_GROUP_10 GOSSIP_SENDER_BOTGIVER_HIRE_RAID_GROUP_10
#define HIRE_RAID_GROUP_25 GOSSIP_SENDER_BOTGIVER_HIRE_RAID_GROUP_25
#define BOTS_PER_PAGE 30
#define RACE_GOBLIN 9
#define RACE_VOID_ELF 12
#define RACE_VULPERA 13
#define RACE_HIGH_ELF 14
#define RACE_PANDAREN 15
#define RACE_WORGEN 16
#define RACE_EREDAR 17
#define RACE_ZADALARI 18
#define RACE_LIGHTFORGED_DRAENEI 19
#define RACE_DEMONHUNTER_A 20
#define RACE_DEMONHUNTER_H 21
#define RACE_TUSKARR 28
#define BOT_TEXT_RACE_GOBLIN               75624 
#define BOT_TEXT_RACE_VOID_ELF             75625 
#define BOT_TEXT_RACE_VULPERA              75626 
#define BOT_TEXT_RACE_HIGH_ELF             75627 
#define BOT_TEXT_RACE_PANDAREN             75628 
#define BOT_TEXT_RACE_WORGEN               75629 
#define BOT_TEXT_RACE_EREDAR               75630 
#define BOT_TEXT_RACE_ZADALARI             75631 
#define BOT_TEXT_RACE_LIGHTFORGED_DRAENEI  75632 
#define BOT_TEXT_RACE_DEMONHUNTER_A        75633 
#define BOT_TEXT_RACE_DEMONHUNTER_H        75634
#define BOT_TEXT_RACE_TUSKARR              75636 



class script_bot_giver : public CreatureScript
{
private:
    NpcBotRegistry _existingBots; // Declare _existingBots as a member variable

public:
    script_bot_giver() : CreatureScript("script_bot_giver") { }

    //struct bot_giver_AI : public CreatureAI
    //{
    //    bot_giver_AI(Creature* creature) : CreatureAI(creature) {}

    //    void UpdateAI(uint32 /*diff*/) override {}

        bool OnGossipHello(Player* player, Creature* me) override
        {
            if (!BotMgr::IsNpcBotModEnabled())
            {
                player->PlayerTalkClass->SendCloseGossip();
                return true;
            }

            if (me->isMoving())
                me->BotStopMovement();

            AddGossipItemFor(player, GOSSIP_ICON_TALK, bot_ai::LocalizedNpcText(player, BOT_TEXT_BOTGIVER_SERVICE), HIRE, GOSSIP_ACTION_INFO_DEF + 1);

        //    if (!player->HaveBot() && player->GetLevel() >= 60)
        //        AddGossipItemFor(player, GOSSIP_ICON_TALK, bot_ai::LocalizedNpcText(player, BOT_TEXT_BOTGIVER_HIRE_RAID), HIRE_RAID_GROUP, GOSSIP_ACTION_INFO_DEF + 1);

            AddGossipItemFor(player, GOSSIP_ICON_CHAT, bot_ai::LocalizedNpcText(player, BOT_TEXT_NEVERMIND), 0, GOSSIP_ACTION_INFO_DEF + 2);

            player->PlayerTalkClass->SendGossipMenu(GOSSIP_BOTGIVER_GREET, me->GetGUID());
            return true;
        }

        //bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
        bool OnGossipSelect(Player* player, Creature* me, uint32 sender, uint32 action) override
        {
            if (!BotMgr::IsNpcBotModEnabled())
            {
                player->PlayerTalkClass->SendCloseGossip();
                return true;
            }

            //uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            //uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);

            player->PlayerTalkClass->ClearMenus();
            bool subMenu = false;

            uint32 gossipTextId = GOSSIP_BOTGIVER_GREET;

            switch (sender)
            {
                case 0: //exit
                    break;
                case 1: //BACK: return to main menu
                    return OnGossipHello(player, me);
                case HIRE:
                {
                    gossipTextId = GOSSIP_BOTGIVER_HIRE;

                    if (player->GetNpcBotsCount() >= BotMgr::GetMaxNpcBots(player->GetLevel()))
                    {
                        WhisperTo(player, me, bot_ai::LocalizedNpcText(player, BOT_TEXT_BOTGIVER_TOO_MANY_BOTS).c_str());
                        break;
                    }

                    subMenu = true;

                    uint8 availCount = 0;
                    std::array<uint32, BOT_CLASS_END> npcbot_count_per_class{ 0 };

                    {
                        std::unique_lock<std::shared_mutex> lock(*BotDataMgr::GetLock());
                        for (Creature const* bot : BotDataMgr::GetExistingNPCBots())
                        {
                            if (!bot->IsAlive() || bot->IsTempBot() || bot->IsWandererBot() || bot->GetBotAI()->GetBotOwnerGuid() || bot->HasAura(BERSERK))
                                continue;
                            if (BotMgr::FilterRaces() && bot->GetBotClass() < BOT_CLASS_EX_START && (bot->GetRaceMask() & RACEMASK_ALL_PLAYABLE) &&
                                !(bot->GetRaceMask() & ((player->GetRaceMask() & RACEMASK_ALLIANCE) ? RACEMASK_ALLIANCE : RACEMASK_HORDE)))
                                continue;

                            ++npcbot_count_per_class[bot->GetBotClass()];
                        }
                    }

                    for (uint8 botclass = BOT_CLASS_WARRIOR; botclass < BOT_CLASS_END; ++botclass)
                    {
                        if (!BotMgr::IsClassEnabled(botclass))
                            continue;

                        if (player->HaveBot() && BotMgr::GetMaxClassBots())
                        {
                            uint8 count = 0;
                            BotMap const* map = player->GetBotMgr()->GetBotMap();
                            for (BotMap::const_iterator itr = map->begin(); itr != map->end(); ++itr)
                                if (itr->second->GetBotClass() == botclass)
                                    ++count;
                            if (count >= BotMgr::GetMaxClassBots())
                                continue;
                        }

                        uint32 textId;
                        switch (botclass)
                        {
                            case BOT_CLASS_WARRIOR:     textId = BOT_TEXT_CLASS_WARRIOR_PLU;        break;
                            case BOT_CLASS_PALADIN:     textId = BOT_TEXT_CLASS_PALADIN_PLU;        break;
                            case BOT_CLASS_MAGE:        textId = BOT_TEXT_CLASS_MAGE_PLU;           break;
                            case BOT_CLASS_PRIEST:      textId = BOT_TEXT_CLASS_PRIEST_PLU;         break;
                            case BOT_CLASS_WARLOCK:     textId = BOT_TEXT_CLASS_WARLOCK_PLU;        break;
                            case BOT_CLASS_DRUID:       textId = BOT_TEXT_CLASS_DRUID_PLU;          break;
                            case BOT_CLASS_DEATH_KNIGHT:textId = BOT_TEXT_CLASS_DEATH_KNIGHT_PLU;   break;
                            case BOT_CLASS_ROGUE:       textId = BOT_TEXT_CLASS_ROGUE_PLU;          break;
                            case BOT_CLASS_SHAMAN:      textId = BOT_TEXT_CLASS_SHAMAN_PLU;         break;
                            case BOT_CLASS_HUNTER:      textId = BOT_TEXT_CLASS_HUNTER_PLU;         break;
                            case BOT_CLASS_BM:          textId = BOT_TEXT_CLASS_BM_PLU;             break;
                            case BOT_CLASS_SPHYNX:      textId = BOT_TEXT_CLASS_SPHYNX_PLU;         break;
                            case BOT_CLASS_ARCHMAGE:    textId = BOT_TEXT_CLASS_ARCHMAGE_PLU;       break;
                            case BOT_CLASS_DREADLORD:   textId = BOT_TEXT_CLASS_DREADLORD_PLU;      break;
                            case BOT_CLASS_SPELLBREAKER:textId = BOT_TEXT_CLASS_SPELLBREAKER_PLU;   break;
                            case BOT_CLASS_DARK_RANGER: textId = BOT_TEXT_CLASS_DARK_RANGER_PLU;    break;
                            case BOT_CLASS_NECROMANCER: textId = BOT_TEXT_CLASS_NECROMANCER_PLU;    break;
                            case BOT_CLASS_SEA_WITCH:   textId = BOT_TEXT_CLASS_SEAWITCH_PLU;       break;
                            case BOT_CLASS_CRYPT_LORD:  textId = BOT_TEXT_CLASS_CRYPT_LORD_PLU;     break;
                            default:                    textId = 0;                                 break;
                        }

                        if (!textId)
                            continue;

                        std::ostringstream bclass;
                        bclass << npcbot_count_per_class[botclass] << " " << bot_ai::LocalizedNpcText(player, textId) << " (" << BotMgr::GetNpcBotCostStr(player->GetLevel(), botclass) << ")";

                        AddGossipItemFor(player, GOSSIP_ICON_TALK, bclass.str(), HIRE_CLASS, GOSSIP_ACTION_INFO_DEF + botclass);

                        if (++availCount >= BOT_GOSSIP_MAX_ITEMS - 1) //back
                            break;
                    }

                    if (availCount == 0)
                        gossipTextId = GOSSIP_BOTGIVER_HIRE_EMPTY;

                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, bot_ai::LocalizedNpcText(player, BOT_TEXT_NEVERMIND), 0, GOSSIP_ACTION_INFO_DEF + 1);

                    break;
                }
                case HIRE_CLASS:
                {
                    gossipTextId = GOSSIP_BOTGIVER_HIRE_CLASS;

                    // Calculate the page number and bot class
                    uint8 pageNumber = action / (GOSSIP_ACTION_INFO_DEF * BOTS_PER_PAGE);
                    uint8 botclass = action % GOSSIP_ACTION_INFO_DEF;

                    uint32 cost = BotMgr::GetNpcBotCost(player->GetLevel(), botclass);
                    if (!player->HasEnoughMoney(cost))
                    {
                        WhisperTo(player, me, bot_ai::LocalizedNpcText(player, BOT_TEXT_HIREFAIL_COST).c_str());
                        break;
                    }

                    subMenu = true;

                    uint8 availCount = 0;

                    // Dinkle: Limit Dark rangers
                    if (botclass == BOT_CLASS_DARK_RANGER)
                    {
                        uint8 darkRangerCount = 0;
                        BotMap const* map = player->GetBotMgr()->GetBotMap();
                        for (BotMap::const_iterator itr = map->begin(); itr != map->end(); ++itr)
                            if (itr->second->GetBotClass() == BOT_CLASS_DARK_RANGER)
                                ++darkRangerCount;

                        if (darkRangerCount >= BotMgr::GetMaxDarkRangerBots())
                        {
                            WhisperTo(player, me, "You cannot hire more Dark Rangers due to the limit set in your configuration.");
                            break; // Stop processing and close the gossip window
                        }
                    }

                    // Calculate the start and end indices for the bots on this page
                    uint8 startIndex = pageNumber * BOTS_PER_PAGE;
                    uint8 endIndex = startIndex + BOTS_PER_PAGE;

                    // Go through bots map to find what bots are available
                    std::unique_lock<std::shared_mutex> lock(*BotDataMgr::GetLock());
                    NpcBotRegistry const& allBots = BotDataMgr::GetExistingNPCBots();

                    // Iterate over the bots and add them as gossip items
                    uint8 botIndex = 0;
                    for (NpcBotRegistry::const_iterator ci = allBots.begin(); ci != allBots.end(); ++ci)
                    {
                        Creature const* bot = *ci;
                        bot_ai const* ai = bot->GetBotAI();
                        if (bot->GetBotClass() != botclass || !bot->IsAlive() || ai->IsTempBot() || bot->IsWandererBot() || ai->GetBotOwnerGuid() || bot->HasAura(BERSERK))
                            continue;
                        if (BotMgr::FilterRaces() && botclass < BOT_CLASS_EX_START && (bot->GetRaceMask() & RACEMASK_ALL_PLAYABLE) &&
                            !(bot->GetRaceMask() & ((player->GetRaceMask() & RACEMASK_ALLIANCE) ? RACEMASK_ALLIANCE : RACEMASK_HORDE)))
                            continue;

                        // Dinkle test for specific quest status complete on custom bots.
                        if (bot->GetName() == "Sylvanas" && !player->HasAchieved(762))
                        {
                            // Player has not earned the required achievement to hire "Sylvanas"
                            // WhisperTo(player, me, "You must earn the required achievement to hire Sylvanas.");
                            continue; // Skip adding "Sylvanas" as an option and continue with the next bot
                        }

                        // Only add the bot if it is on this page
                        if (botIndex >= startIndex && botIndex < endIndex)
                        {
                            std::ostringstream message1;
                            message1 << bot_ai::LocalizedNpcText(player, BOT_TEXT_BOTGIVER_WISH_TO_HIRE_) << bot->GetName() << '?';

                            std::ostringstream info_ostr;
                            uint32 raceTextId;
                            switch (bot->GetRace())
                            {
                            case RACE_HUMAN:        raceTextId = BOT_TEXT_RACE_HUMAN;   break;
                            case RACE_ORC:          raceTextId = BOT_TEXT_RACE_ORC;     break;
                            case RACE_DWARF:        raceTextId = BOT_TEXT_RACE_DWARF;   break;
                            case RACE_NIGHTELF:     raceTextId = BOT_TEXT_RACE_NELF;    break;
                            case RACE_UNDEAD_PLAYER:raceTextId = BOT_TEXT_RACE_UNDEAD;  break;
                            case RACE_TAUREN:       raceTextId = BOT_TEXT_RACE_TAUREN;  break;
                            case RACE_GNOME:        raceTextId = BOT_TEXT_RACE_GNOME;   break;
                            case RACE_TROLL:        raceTextId = BOT_TEXT_RACE_TROLL;   break;
                            case RACE_BLOODELF:     raceTextId = BOT_TEXT_RACE_BELF;    break;
                            case RACE_DRAENEI:      raceTextId = BOT_TEXT_RACE_DRAENEI; break;
                            case RACE_GOBLIN:               raceTextId = BOT_TEXT_RACE_GOBLIN;  break;
                            case RACE_VOID_ELF:             raceTextId = BOT_TEXT_RACE_VOID_ELF; break;
                            case RACE_VULPERA:              raceTextId = BOT_TEXT_RACE_VULPERA;  break;
                            case RACE_HIGH_ELF:             raceTextId = BOT_TEXT_RACE_HIGH_ELF; break;
                            case RACE_PANDAREN:             raceTextId = BOT_TEXT_RACE_PANDAREN; break;
                            case RACE_WORGEN:               raceTextId = BOT_TEXT_RACE_WORGEN;   break;
                            case RACE_EREDAR:               raceTextId = BOT_TEXT_RACE_EREDAR;   break;
                            case RACE_ZADALARI:             raceTextId = BOT_TEXT_RACE_ZADALARI; break;
                            case RACE_LIGHTFORGED_DRAENEI:  raceTextId = BOT_TEXT_RACE_LIGHTFORGED_DRAENEI; break;
                            case RACE_DEMONHUNTER_A:        raceTextId = BOT_TEXT_RACE_DEMONHUNTER_A; break;
                            case RACE_DEMONHUNTER_H:        raceTextId = BOT_TEXT_RACE_DEMONHUNTER_H; break;
                            case RACE_TUSKARR:              raceTextId = BOT_TEXT_RACE_TUSKARR; break;
                            default:                raceTextId = BOT_TEXT_RACE_UNKNOWN; break;
                            }
                            info_ostr << bot->GetName() << " (" << (
                                bot->GetGender() == GENDER_MALE ? bot_ai::LocalizedNpcText(player, BOT_TEXT_GENDER_MALE) + ' ' :
                                bot->GetGender() == GENDER_FEMALE ? bot_ai::LocalizedNpcText(player, BOT_TEXT_GENDER_FEMALE) + ' ' :
                                "") << bot_ai::LocalizedNpcText(player, raceTextId) << ')';

                            player->PlayerTalkClass->GetGossipMenu().AddMenuItem(-1, GOSSIP_ICON_TALK, info_ostr.str(),
                                HIRE_ENTRY, GOSSIP_ACTION_INFO_DEF + bot->GetEntry(), message1.str(), cost, false);
                            availCount++;
                        }

                        botIndex++;

                        if (botIndex >= endIndex)
                        {
                            break;
                        }
                    }

                    // If there are more bots available, add an option to go to the next page
                    if (botIndex < allBots.size())
                    {
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Next page", HIRE_CLASS, (pageNumber + 1) * GOSSIP_ACTION_INFO_DEF * BOTS_PER_PAGE + botclass);
                    }

                    // If this isn't the first page, add an option to go back to the previous page
                    if (pageNumber > 0)
                    {
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Previous page", HIRE_CLASS, (pageNumber - 1) * GOSSIP_ACTION_INFO_DEF * BOTS_PER_PAGE + botclass);
                    }

                    if (availCount == 0)
                        gossipTextId = GOSSIP_BOTGIVER_HIRE_EMPTY;

                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, bot_ai::LocalizedNpcText(player, BOT_TEXT_BACK), HIRE, GOSSIP_ACTION_INFO_DEF + 1);

                    break;
                }
                case HIRE_ENTRY:
                {
                    uint32 entry = action - GOSSIP_ACTION_INFO_DEF;
                    Creature const* bot = BotDataMgr::FindBot(entry);
                    if (!bot)
                    {
                        //possible but still
                        LOG_ERROR("entities.unit", "HIRE_NBOT_ENTRY: bot {} not found!", entry);
                        break;
                    }

                    bot_ai const* ai = bot->GetBotAI();
                    if (bot->IsInCombat() || !bot->IsAlive() || bot_ai::CCed(bot) ||
                        bot->HasUnitState(UNIT_STATE_CASTING) || ai->GetBotOwnerGuid() || bot->HasAura(BERSERK))
                    {
                        //TC_LOG_ERROR("entities.unit", "HIRE_NBOT_ENTRY: bot %u (%s) is unavailable all of the sudden!", entry);
                        std::ostringstream failMsg;
                        failMsg << bot->GetName() << bot_ai::LocalizedNpcText(player, BOT_TEXT_BOTGIVER__BOT_BUSY);
                        WhisperTo(player, me, failMsg.str().c_str());
                        break;
                    }

                    //laways returns true
                    bot->GetBotAI()->OnGossipSelect(player, me, GOSSIP_SENDER_HIRE, GOSSIP_ACTION_INFO_DEF);

                    if (player->HaveBot() && player->GetBotMgr()->GetBot(bot->GetGUID()))
                        WhisperTo(player, me, bot_ai::LocalizedNpcText(player, BOT_TEXT_BOTGIVER_HIRESUCCESS).c_str());

                    break;
                }
            }

            if (subMenu)
                player->PlayerTalkClass->SendGossipMenu(gossipTextId, me->GetGUID());
            else
                player->PlayerTalkClass->SendCloseGossip();

            return true;
        }

        void WhisperTo(Player* player, Creature* me, char const* message)
        {
            me->Whisper(message, LANG_UNIVERSAL, player);
        }
        //};

        //CreatureAI* GetAI(Creature* creature) const override
        //{
        //    return new bot_giver_AI(creature);
        //}
};

void AddSC_script_bot_giver()
{
    new script_bot_giver();
}

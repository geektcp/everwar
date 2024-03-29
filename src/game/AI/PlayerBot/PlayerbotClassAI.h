#ifndef _PLAYERBOTCLASSAI_H
#define _PLAYERBOTCLASSAI_H

#include "Common.h"
#include "World.h"
#include "SpellMgr.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "Unit.h"
#include "SharedDefines.h"
#include "PlayerbotAI.h"

class Player;
class PlayerbotAI;

enum JOB_TYPE
{
    JOB_HEAL     = 0x01,
    JOB_TANK     = 0x02,
    JOB_MASTER   = 0x04, // Not a fan of this distinction but user (or rather, admin) choice
    JOB_DPS      = 0x08,
    JOB_ALL      = 0x0F, // all of the above
    JOB_MANAONLY = 0x10  // for buff checking (NOTE: this means any with powertype mana AND druids (who may be shifted but still have mana)
};

struct heal_priority
{
    Player* p;
    uint8 hp;
    JOB_TYPE type;
    heal_priority(Player* pin, uint8 hpin, JOB_TYPE t) : p(pin), hp(hpin), type(t) {}
    // overriding the operator like this is not recommended for general use - however we won't use this struct for anything else
    bool operator<(const heal_priority& a) const { return type < a.type; }
};

class MANGOS_DLL_SPEC PlayerbotClassAI
{
public:
    PlayerbotClassAI(Player * const master, Player * const bot, PlayerbotAI * const ai);
    virtual ~PlayerbotClassAI();

    // all combat actions go here
    virtual CombatManeuverReturns DoFirstCombatManeuver(Unit*);
    virtual CombatManeuverReturns DoNextCombatManeuver(Unit*);
    bool Pull() { DEBUG_LOG("[PlayerbotAI]: Warning: Using PlayerbotClassAI::Pull() rather than class specific function"); return false; }
    bool Neutralize() { DEBUG_LOG("[PlayerbotAI]: Warning: Using PlayerbotClassAI::Neutralize() rather than class specific function"); return false; }

    // all non combat actions go here, ex buffs, heals, rezzes
    virtual void DoNonCombatActions();
    bool EatDrinkBandage(bool bMana = true, unsigned char foodPercent = 50, unsigned char drinkPercent = 50, unsigned char bandagePercent = 70);

    // Utilities
    Player* GetMaster () { return m_master; }
    Player* GetPlayerBot() { return m_bot; }
    PlayerbotAI* GetAI() { return m_ai; }
    bool CanPull();
    bool CastHoTOnTank();
    time_t GetWaitUntil() { return m_WaitUntil; }
    void SetWait(uint8 t) { m_WaitUntil = m_ai->CurrentTime() + t; }
    void ClearWait() { m_WaitUntil = 0; }
    //void SetWaitUntil(time_t t) { m_WaitUntil = t; }

protected:
    virtual CombatManeuverReturns DoFirstCombatManeuverPVE(Unit*);
    virtual CombatManeuverReturns DoNextCombatManeuverPVE(Unit*);
    virtual CombatManeuverReturns DoFirstCombatManeuverPVP(Unit*);
    virtual CombatManeuverReturns DoNextCombatManeuverPVP(Unit*);

    CombatManeuverReturns CastSpellNoRanged(uint32 nextAction, Unit *pTarget);
    CombatManeuverReturns CastSpellWand(uint32 nextAction, Unit *pTarget, uint32 SHOOT);
    virtual CombatManeuverReturns HealPlayer(Player* target);
    CombatManeuverReturns Buff(bool (*BuffHelper)(PlayerbotAI*, uint32, Unit*), uint32 spellId, uint32 type = JOB_ALL, bool bMustBeOOC = true);
    bool NeedGroupBuff(uint32 groupBuffSpellId, uint32 singleBuffSpellId);
    Player* GetHealTarget(JOB_TYPE type = JOB_ALL);
    Player* GetDispelTarget(DispelType dispelType, JOB_TYPE type = JOB_ALL, bool bMustBeOOC = false);
    Player* GetResurrectionTarget(JOB_TYPE type = JOB_ALL, bool bMustBeOOC = true);
    JOB_TYPE GetTargetJob(Player* target);

    // These values are used in GetHealTarget and can be overridden per class (to accomodate healing spell health checks)
    uint8 m_MinHealthPercentTank;
    uint8 m_MinHealthPercentHealer;
    uint8 m_MinHealthPercentDPS;
    uint8 m_MinHealthPercentMaster;

    time_t m_WaitUntil;

    Player* m_master;
    Player* m_bot;
    PlayerbotAI* m_ai;

    // first aid
    uint32 RECENTLY_BANDAGED;
};

#endif

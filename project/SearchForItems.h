#pragma once
#include "ConditionsAndActions.h"

struct WorldInfo;
class Character;

class SearchForItems final :public Action
{
public:
	SearchForItems();
	SearchForItems(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pBlackboard) const override;
	virtual std::string GetName() const override { return "Search for items"; };
private:
	void RotateAgent(Character* pCharacter);
	
	const float m_Range{ 10.f };
	Elite::Vector2 m_CurrentTarget{};

	float m_Timer{};
	float m_WasAttackedTimer{};
	float m_Patience{ 30.f };
	float m_WasAttackTimerMax{ 2.5f };

	void GenerateNewTarget(const WorldInfo& worldInfo);
};

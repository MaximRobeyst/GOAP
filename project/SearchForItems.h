#pragma once
#include "ConditionsAndActions.h"

class Character;

class SearchForItems final :public Action
{
public:
	SearchForItems();
	SearchForItems(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pBlackboard) const override;
	virtual std::string GetName() const override { return "Search for items"; };
private:
	float m_WanderAngle{};
};

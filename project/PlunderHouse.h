#pragma once
#include "ConditionsAndActions.h"

class Character;

class PlunderHouse : public Action
{
public:
	PlunderHouse();
	PlunderHouse(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pBlackboard) const override;
	std::string GetName() const override;
};


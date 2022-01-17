#pragma once
#include "ConditionsAndActions.h"


class FleeFromEnemy final : public Action
{
public:
	FleeFromEnemy();
	FleeFromEnemy(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	virtual std::string GetName() const override { return "Flee From Enemy"; };
};

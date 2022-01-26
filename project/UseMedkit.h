#pragma once
#include "ConditionsAndActions.h"

class UseMedkit final : public Action
{
public:
	UseMedkit();
	UseMedkit(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	
	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	std::string GetName() const override;
};


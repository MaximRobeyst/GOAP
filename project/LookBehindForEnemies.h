#pragma once
#include "ConditionsAndActions.h"

class LookBehindForEnemies final : public Action
{
public:
	LookBehindForEnemies();
	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	std::string GetName() const override;

private:
	
};


#pragma once
#include "ConditionsAndActions.h"

class EscapePurgeZone final : public Action
{
public:
	EscapePurgeZone();

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	std::string GetName() const override;
};


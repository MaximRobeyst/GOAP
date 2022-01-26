#pragma once
#include "ConditionsAndActions.h"

class MoveBackInBoundaries final : public Action
{
public:
	MoveBackInBoundaries();
	MoveBackInBoundaries(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	std::string GetName() const override;
	Elite::Vector2 GetTarget(Character* pCharacter) override;
};


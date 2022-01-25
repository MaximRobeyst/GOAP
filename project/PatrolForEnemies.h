#pragma once
#include "ConditionsAndActions.h"

// This is a wander action until he find an enemy where he will

class PatrolForEnemies final : public Action
{
public:
	PatrolForEnemies();
	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	std::string GetName() const override;
private:
	float m_WanderAngle{};
};


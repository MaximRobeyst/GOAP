#pragma once
#include "ConditionsAndActions.h"


class EvadeEnemy final : public Action
{
public:
	EvadeEnemy();
	EvadeEnemy(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	virtual std::string GetName() const override { return "Flee From Enemy"; };
private:
	Elite::Vector2 m_LastKnownLocation;
};

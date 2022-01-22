#pragma once
#include "ConditionsAndActions.h"

class Character;

class ShootAtEnemy final :public Action
{
public:
	ShootAtEnemy();
	ShootAtEnemy(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt,Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	virtual std::string GetName() const override { return "Shoot at enemy"; };
private:
	float m_ShootCooldown{};
	float m_Timer{};
};


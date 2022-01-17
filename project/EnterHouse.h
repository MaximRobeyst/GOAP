#pragma once
#include "ConditionsAndActions.h"
class Character;

class EnterHouse final : public Action
{
public:
	EnterHouse();
	EnterHouse(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	Elite::Vector2 GetTarget(Character* pCharacter) override;
	
	virtual std::string GetName() const override { return "Enter house"; };
private:
	float m_WanderAngle{};
	float m_Range{ 5.f };
	Elite::Vector2 m_Target{};
};

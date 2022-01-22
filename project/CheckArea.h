#pragma once
#include "ConditionsAndActions.h"

class CheckArea final : public Action
{
public:
	CheckArea();


	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	std::string GetName() const override;
private:
	const float m_TimeToCheck{ 3.f };
	float m_Timer{ 0.f };
};


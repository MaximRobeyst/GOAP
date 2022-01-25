#pragma once
#include "ConditionsAndActions.h"

class NeedsFood final : public Action
{
public:
	NeedsFood();
	
	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	std::string GetName() const override;

public:
	float m_TooCloseRange{ 1.f };
	bool m_FoodAdded{ false };
};


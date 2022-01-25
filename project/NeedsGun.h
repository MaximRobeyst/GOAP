#pragma once
#include "ConditionsAndActions.h"

class NeedsGun final : public Action
{
public:
	NeedsGun();
	
	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	std::string GetName() const override;

private:
	float m_TooCloseRange{ 1.f };
	bool m_GunAdded{false};
};


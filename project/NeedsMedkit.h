#pragma once
#include "ConditionsAndActions.h"

class NeedsMedkit final : public Action
{
public:
	NeedsMedkit();

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	std::string GetName() const override;
	Elite::Vector2 GetTarget(Character* pCharacter) override;

private:
	float m_TooCloseRange{ 1.f };
	bool m_MedkitAdded{ false };
};

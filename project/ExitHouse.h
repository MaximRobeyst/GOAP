#pragma once
#include "ConditionsAndActions.h"

class ExitHouse final :public Action
{
public:
	ExitHouse();
	ExitHouse(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(float dt, Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	virtual std::string GetName() const override { return "Exit House"; };
	Elite::Vector2 GetTarget(Character* pCharacter) override;
private:
	float m_WanderAngle{};
	
	float m_WanderWeight{ 0.4f };
	float m_SeekWeight{ 0.6f };
	
	bool m_Done{ false };
};


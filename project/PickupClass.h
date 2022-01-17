#pragma once
#include <Exam_HelperStructs.h>

#include "ConditionsAndActions.h"

class Character;

class PickupItem : public Action
{
public:
	PickupItem();
	PickupItem(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Character* pCharacter) const override;
	bool ExecuteAction(Character* pCharacter) override;
	bool IsDone(Character* pCharacter) override;
	bool RequiresInRange() const override;
	bool IsInRange(Character* pCharacter) const override;
	Elite::Vector2 GetTarget(Character* pCharacter) override;
	std::string GetName() const override;
private:
	UINT GetEmptySlot(Character* pCharacter);
	void GetItemTarget(Character* pCharacter);

	float m_Range{ 5.f };
	ItemInfo m_Info{};
};

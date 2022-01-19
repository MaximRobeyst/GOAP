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
	EntityInfo GetItemTarget(Character* pCharacter) const;

	float m_Range{ 1.5f };
	float m_TooCloseRange{ 1.f };
	bool m_ItemAdded{ false };
	Elite::Vector2 m_ItemLocation;
};

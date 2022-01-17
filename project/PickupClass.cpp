#include "stdafx.h"
#include "PickupClass.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>


PickupItem::PickupItem()
{
	m_Preconditions["EnemiesInFov"] = true;
	m_Preconditions["HouseInFov"] = true;
	m_Preconditions["ItemInFov"] = true;
	m_Preconditions["InventoryFull"] = false;
	m_Preconditions["InHouse"] = true;

	m_Effects["Survive"] = true;

	m_Cost = 0.5f;
}

bool PickupItem::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool PickupItem::ExecuteAction(Character* pCharacter)
{
	if (m_Info.ItemHash = 0)
		GetItemTarget(pCharacter);

	pCharacter->GetInterface()->Inventory_AddItem(0, m_Info);
	return true;
}

bool PickupItem::IsDone(Character* pCharacter)
{
	return false;
}

bool PickupItem::RequiresInRange() const
{
	return true;
}

bool PickupItem::IsInRange(Character* pCharacter) const
{
	return Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, m_Info.Location) < (pCharacter->GetAgentInfo().GrabRange * pCharacter->GetAgentInfo().GrabRange);
}

Elite::Vector2 PickupItem::GetTarget(Character* pCharacter)
{
	if (m_Info.ItemHash == 0)
		GetItemTarget(pCharacter);

	return m_Info.Location;
}

std::string PickupItem::GetName() const
{
	return "Item In Sight";
}

UINT PickupItem::GetEmptySlot(Character* pCharacter)
{
	ItemInfo itemInfo;
	for (auto i = 0; i < pCharacter->GetInterface()->Inventory_GetCapacity(); ++i)
	{
		if (pCharacter->GetInventory()[i].Type == eItemType::_LAST)
			return i;
	}

	return pCharacter->GetInterface()->Inventory_GetCapacity();
}

void PickupItem::GetItemTarget(Character* pCharacter)
{
	ItemInfo itemInfo{};
	auto entityInFov = pCharacter->GetEntitiesInFOV();

	auto iter = std::find_if(entityInFov.begin(), entityInFov.end(), [](EntityInfo info)
		{
			return info.Type == eEntityType::ITEM;
		});

	if (iter == pCharacter->GetEntitiesInFOV().end())
		return;
	
	pCharacter->GetInterface()->Item_GetInfo(*iter, itemInfo);

	m_Info = itemInfo;

}

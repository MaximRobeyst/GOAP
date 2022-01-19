#include "stdafx.h"
#include "PickupClass.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>


PickupItem::PickupItem()
{
	m_Preconditions["EnemiesInFov"] = true;
	m_Preconditions["ItemInFov"] = true;
	m_Preconditions["InventoryFull"] = false;

	m_Effects["Survive"] = true;

	m_Cost = 0.5f;
}

bool PickupItem::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool PickupItem::ExecuteAction(Character* pCharacter)
{
	auto info = GetItemTarget(pCharacter);
	ItemInfo itemInfo;
	
	if (IsInRange(pCharacter))
	{
		if(pCharacter->GetInterface()->Item_Grab(info, itemInfo))
		{
			m_ItemAdded = pCharacter->GetInterface()->Inventory_AddItem(0, itemInfo);
		}
	}
	return true;
}

bool PickupItem::IsDone(Character* pCharacter)
{
	return m_ItemAdded;
}

bool PickupItem::RequiresInRange() const
{
	return true;
}

bool PickupItem::IsInRange(Character* pCharacter) const
{
	float distanceSquared = Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, GetItemTarget(pCharacter).Location);
	
	if(distanceSquared < m_TooCloseRange * m_TooCloseRange)
	{
		pCharacter->ChangeCharacterState("ItemInFov", false);
	}

	
	return distanceSquared < (pCharacter->GetAgentInfo().GrabRange * pCharacter->GetAgentInfo().GrabRange);
}

Elite::Vector2 PickupItem::GetTarget(Character* pCharacter)
{
	m_ItemLocation = GetItemTarget(pCharacter).Location;
	return m_ItemLocation;
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

EntityInfo PickupItem::GetItemTarget(Character* pCharacter) const
{
	EntityInfo itemInfo{};
	auto entityInFov = pCharacter->GetEntitiesInFOV();

	auto iter = std::find_if(entityInFov.begin(), entityInFov.end(), [](EntityInfo info)
		{
			return info.Type == eEntityType::ITEM;
		});
	if (iter == entityInFov.end())
		return itemInfo;
	

	return *iter;

}

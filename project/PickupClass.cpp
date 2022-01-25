#include "stdafx.h"
#include "PickupClass.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

#include "Helpers.h"


PickupItem::PickupItem()
{
	//m_Preconditions["ItemInFov"] = true;
	m_Preconditions["InventoryFull"] = false;
	m_Preconditions["HasItemTarget"] = true;
	m_Preconditions["EnergyLow"] = false;
	m_Preconditions["InPurgeZone"] = false;

	m_Effects["Survive"] = true;

	m_Cost = 0.5f;
}

bool PickupItem::CheckProceduralPreconditions(Character* pCharacter) const
{
	if (pCharacter->GetConditions()["HealthLow"] && pCharacter->GetConditions()["HasMedkit"])
		return false;
	if (pCharacter->GetConditions()["EnergyLow"] && pCharacter->GetConditions()["HasFood"])
		return false;
	
	return true;
}

bool PickupItem::ExecuteAction(float dt, Character* pCharacter)
{
	auto itemMemory = pCharacter->GetItemMemory();
	const auto entityInFov = pCharacter->GetEntitiesInFOV();

	const auto info = pCharacter->GetCurrentItemTarget();
	ItemInfo itemInfo;

	auto steering = pCharacter->GetSteeringOutput();
	steering.AutoOrient = true;

	const auto iter = std::find_if(entityInFov.begin(), entityInFov.end(), SameLocation{info});
	
	if (IsInRange(pCharacter) && iter != entityInFov.end())
	{
		if(Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, pCharacter->GetCurrentItemTarget()) < (m_TooCloseRange * m_TooCloseRange))
		{
			steering.LinearVelocity = { iter->Location - pCharacter->GetAgentInfo().Position };
			steering.LinearVelocity.Normalize();
			steering.LinearVelocity *= pCharacter->GetAgentInfo().MaxLinearSpeed;
		}
		else
		{
			if (pCharacter->GetInterface()->Item_Grab(*iter, itemInfo))
			{
				m_ItemAdded = pCharacter->GetInterface()->Inventory_AddItem(pCharacter->GetSlot(), itemInfo);
				if (m_ItemAdded)
				{
					pCharacter->PopItemFromMemory();
					switch (itemInfo.Type)
					{
					case eItemType::MEDKIT:
						pCharacter->ChangeCharacterState("HasMedkit", true);
						break;
					case eItemType::PISTOL:
						pCharacter->ChangeCharacterState("HasWeapon", true);
						break;
					case eItemType::FOOD:
						pCharacter->ChangeCharacterState("HasFood", true);
						break;
					case eItemType::GARBAGE:
						pCharacter->RemoveItemFromInventory(pCharacter->GetSlot());
						break;
					}
				}

				pCharacter->SetSlot(pCharacter->GetSlot() + 1);
			}
		}
	}
	pCharacter->SetSteeringOutput(steering);
	
	return true;
}

bool PickupItem::IsDone(Character* pCharacter)
{
	return pCharacter->GetItemMemory().empty() && m_ItemAdded;
}

bool PickupItem::RequiresInRange() const
{
	return true;
}

bool PickupItem::IsInRange(Character* pCharacter) const
{
	const float distanceSquared = Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, pCharacter->GetCurrentItemTarget());
	
	return distanceSquared < (pCharacter->GetAgentInfo().GrabRange * pCharacter->GetAgentInfo().GrabRange);
}

Elite::Vector2 PickupItem::GetTarget(Character* pCharacter)
{
	return pCharacter->GetCurrentItemTarget();
}

std::string PickupItem::GetName() const
{
	return "Item In Sight";
}

//void PickupItem::GetItemTargetLocations(Character* pCharacter)
//{
//	auto entityInFov = pCharacter->GetEntitiesInFOV();
//	const auto itemMemory = pCharacter->GetItemMemory();
//
//	for(auto i = entityInFov.begin(); i != entityInFov.end(); ++i)
//	{
//		if (i->Type == eEntityType::ITEM &&
//			std::find_if(itemMemory.begin(), itemMemory.end(), [i](const Elite::Vector2& info) { return info == i->Location; }) == itemMemory.end())
//			pCharacter->AddItemToMemory(*i);
//	}
//
//	//std::sort(entityInFov.begin(), entityInFov.end(), IsFurther<EntityInfo>{ pCharacter->GetAgentInfo().Position });
//	
//	m_ItemLocation = m_ItemLocations[m_ItemLocations.size() - 1];
//}
//
//EntityInfo PickupItem::GetCurrentItemTarget(Character* pCharacter) const
//{
//	auto entityInFov = pCharacter->GetEntitiesInFOV();
//	std::vector<EntityInfo> items{};
//
//	for (auto i = entityInFov.begin(); i != entityInFov.end(); ++i)
//	{
//		if (i->Type == eEntityType::ITEM)
//			items.push_back(*i);
//	}
//
//	std::sort(entityInFov.begin(), entityInFov.end(), IsFurther<EntityInfo>{ pCharacter->GetAgentInfo().Position });
//
//	return items[items.size() - 1];
//}
//
//std::vector<EntityInfo> PickupItem::GetCurrentItemTargets(Character* pCharacter) const
//{
//	auto entityInFov = pCharacter->GetEntitiesInFOV();
//	std::vector<EntityInfo> items{};
//
//	for (auto i = entityInFov.begin(); i != entityInFov.end(); ++i)
//	{
//		if (i->Type == eEntityType::ITEM)
//			items.push_back(*i);
//	}
//
//	std::sort(entityInFov.begin(), entityInFov.end(), IsFurther<EntityInfo>{ pCharacter->GetAgentInfo().Position });
//
//	return items;
//}

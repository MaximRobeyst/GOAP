#include "stdafx.h"
#include "PickupClass.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>


PickupItem::PickupItem()
{
	m_Preconditions["ItemInFov"] = true;
	m_Preconditions["InventoryFull"] = false;

	m_Effects["Survive"] = true;

	m_Cost = 0.5f;
}

bool PickupItem::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool PickupItem::ExecuteAction(float dt, Character* pCharacter)
{
	GetItemTargetLocations(pCharacter);

	if (!m_ItemLocations.empty())
		pCharacter->GetConditions()["ItemInFov"] = true;

	std::for_each(m_ItemLocations.begin(), m_ItemLocations.end(), [pCharacter](const Elite::Vector2& v)
		{
			pCharacter->GetInterface()->Draw_Point(v, 6.f, Elite::Vector3{ 1,0,0 }, 0.4f);
		});
	std::cout << m_ItemLocations.size() << std::endl;
	
	const auto info = GetCurrentItemTarget(pCharacter);
	ItemInfo itemInfo;

	auto steering = pCharacter->GetSteeringOutput();
	steering.AutoOrient = true;
	pCharacter->SetSteeringOutput(steering);
	
	if (IsInRange(pCharacter))
	{
		
		if(Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, info.Location) < m_TooCloseRange * m_TooCloseRange)
		{
			auto agentInfo = pCharacter->GetAgentInfo();
			agentInfo.LinearVelocity = itemInfo.Location - agentInfo.Position;
			agentInfo.LinearVelocity.Normalize();
			agentInfo.LinearVelocity *= -agentInfo.MaxLinearSpeed;
			
			pCharacter->SetAgentInfo(agentInfo);
		}
		else
		{
			if (pCharacter->GetInterface()->Item_Grab(info, itemInfo))
			{
				m_ItemAdded = pCharacter->GetInterface()->Inventory_AddItem(pCharacter->GetSlot(), itemInfo);
				m_ItemLocations.pop_back();
				if(m_ItemAdded)
				{
					
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
	
	return true;
}

bool PickupItem::IsDone(Character* pCharacter)
{
	bool done = m_ItemLocations.empty() && m_ItemAdded;
	
	return done;
}

bool PickupItem::RequiresInRange() const
{
	return true;
}

bool PickupItem::IsInRange(Character* pCharacter) const
{
	const float distanceSquared = Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, m_ItemLocation);
	
	return distanceSquared < (pCharacter->GetAgentInfo().GrabRange * pCharacter->GetAgentInfo().GrabRange);
}

Elite::Vector2 PickupItem::GetTarget(Character* pCharacter)
{
	return m_ItemLocation;
}

std::string PickupItem::GetName() const
{
	return "Item In Sight";
}

UINT PickupItem::GetEmptySlot(Character* pCharacter)
{
	for (UINT i = 0; i < pCharacter->GetInterface()->Inventory_GetCapacity(); ++i)
	{
		if (pCharacter->GetInventory()[i].Type == eItemType::_LAST)
			return i;
	}

	return pCharacter->GetInterface()->Inventory_GetCapacity();
}

void PickupItem::GetItemTargetLocations(Character* pCharacter)
{
	auto entityInFov = pCharacter->GetEntitiesInFOV();

	for(auto i = entityInFov.begin(); i != entityInFov.end(); ++i)
	{
		if (i->Type == eEntityType::ITEM && 
			std::find_if(m_ItemLocations.begin(), m_ItemLocations.end(), [i](const Elite::Vector2& info){ return info == i->Location;}) == m_ItemLocations.end())
			m_ItemLocations.push_back(i->Location);
	}

	std::sort(m_ItemLocations.begin(), m_ItemLocations.end(), [pCharacter](const Elite::Vector2& lhs, const Elite::Vector2& rhs)
		{
			return Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, lhs) > Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, rhs);
		});
	
	m_ItemLocation = m_ItemLocations[m_ItemLocations.size() - 1];
	
	//while(iter !=  entityInFov.end())
	//{
	//	iter = std::find_if(iter, entityInFov.end(), [](EntityInfo info)
	//		{
	//			return info.Type == eEntityType::ITEM;
	//		});
	//
	//	items.push_back(*iter);
	//	++iter;
	//}

	//if (items.empty())
	//	return itemInfo;
	//
	//
	//return entityInFov[0];

}

EntityInfo PickupItem::GetCurrentItemTarget(Character* pCharacter) const
{
	auto entityInFov = pCharacter->GetEntitiesInFOV();
	std::vector<EntityInfo> items{};

	for (auto i = entityInFov.begin(); i != entityInFov.end(); ++i)
	{
		if (i->Type == eEntityType::ITEM)
			items.push_back(*i);
	}

	std::sort(entityInFov.begin(), entityInFov.end(), [pCharacter](const EntityInfo& lhs, const EntityInfo& rhs)
		{
			return Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, lhs.Location) > Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, rhs.Location);
		});

	
	return items[items.size() - 1];
}

std::vector<EntityInfo> PickupItem::GetCurrentItemTargets(Character* pCharacter) const
{
	auto entityInFov = pCharacter->GetEntitiesInFOV();
	std::vector<EntityInfo> items{};

	for (auto i = entityInFov.begin(); i != entityInFov.end(); ++i)
	{
		if (i->Type == eEntityType::ITEM)
			items.push_back(*i);
	}

	std::sort(entityInFov.begin(), entityInFov.end(), [pCharacter](const EntityInfo& lhs, const EntityInfo& rhs)
		{
			return Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, lhs.Location) > Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, rhs.Location);
		});


	return items;
}

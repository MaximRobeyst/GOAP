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
	//m_Preconditions["EnergyLow"] = false;
	m_Preconditions["InPurgeZone"] = false;

	m_Effects["Survive"] = true;

	m_Cost = 0.5f;
}

PickupItem::PickupItem(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
	: Action(preConditions, effects, cost)
{
}

bool PickupItem::CheckProceduralPreconditions(Character* pCharacter) const
{
	if (pCharacter->GetConditions()["HealthLow"] && pCharacter->GetConditions()["HasMedkit"])
		return false;
	if (pCharacter->GetConditions()["EnergyLow"] && pCharacter->GetConditions()["HasFood"])
		return false;
	if (pCharacter->GetConditions()["EnemyInFov"] && pCharacter->GetConditions()["HasWeapon"])
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

	if(m_KeepItemTargetTimer >= m_KeepItemTargetTimerMax)
	{
		m_KeepItemTargetTimer = 0.f;
		pCharacter->PopItemFromMemory();
	}

	const auto iter = std::find_if(entityInFov.begin(), entityInFov.end(), SameLocation{info});
	
	if (IsInRange(pCharacter) && iter != entityInFov.end())
	{
		if(Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, pCharacter->GetCurrentItemTarget()) < (m_TooCloseRange * m_TooCloseRange))
		{
			steering.LinearVelocity = { iter->Location - pCharacter->GetAgentInfo().Position };
			steering.LinearVelocity.Normalize();
			steering.LinearVelocity *= pCharacter->GetAgentInfo().MaxLinearSpeed;
			m_KeepItemTargetTimer += dt;
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

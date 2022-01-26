#include "stdafx.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include "IExamInterface.h"

#include "Planner.h"
#include "Blackboard.h"
#include "FiniteStateMachine.h"

//Actions
#include "CheckArea.h"
#include "EnterHouse.h"
#include "ExitHouse.h"
#include "EvadeEnemy.h"
#include "EscapeHouse.h"
#include "EscapePurgeZone.h"
#include "Helpers.h"
#include "PickupClass.h"
#include "SearchForItems.h"
#include "ShootAtEnemy.h"
#include "LookBehindForEnemies.h"
#include "MoveBackInBoundaries.h"
#include "NeedsFood.h"
#include "NeedsGun.h"
#include "PatrolForEnemies.h"
#include "UseFood.h"
#include "UseMedkit.h"

Character::Character(IExamInterface* pInterface)
	: m_pPlanner{new Planner{}}
	, m_pInterface{pInterface}
{
	m_Inventory.resize(m_pInterface->Inventory_GetCapacity());
	
	m_pActions.push_back(new EvadeEnemy());
	m_pActions.push_back(new ShootAtEnemy());
	m_pActions.push_back(new SearchForItems());
	m_pActions.push_back(new EnterHouse());
	m_pActions.push_back(new PickupItem());
	m_pActions.push_back(new ExitHouse());
	m_pActions.push_back(new UseMedkit());
	m_pActions.push_back(new UseFood());
	//m_pActions.push_back(new EscapeHouse());
	m_pActions.push_back(new LookBehindForEnemies());
	m_pActions.push_back(new MoveBackInBoundaries());
	m_pActions.push_back(new EscapePurgeZone);
	m_pActions.push_back(new PatrolForEnemies());
	m_pActions.push_back(new NeedsFood());
	m_pActions.push_back(new NeedsGun());

	// When Our food is low we want a custom type of search for items
	m_pActions.push_back(new SearchForItems(
		std::map<std::string, bool> 
		{
			{"EnergyLow", true},
			{"HasFood", false},
		},
		std::map<std::string, bool> 
		{
			{"EnergyLow", false},
			{"HasFood", true},
			{"Survive", true}
		},
		15.f
		));

	// When we dont have a weapon we want a custom type of search for items
	//m_pActions.push_back(new SearchForItems(
	//	std::map<std::string, bool>
	//	{
	//		{"HasWeapon", false },
	//		{"HasFood", true },
	//		{"HasMedkit", true}
	//	},
	//	std::map<std::string, bool>
	//	{
	//		{"HasWeapon", true},
	//		{ "HasFood", true },
	//		{ "Survive", true }
	//	},
	//		15.f
	//			));
	//m_pActions.push_back(new CheckArea());

	
	// Set World Conditions
	m_WorldConditions["EnemyChasing"] = true;
	m_WorldConditions["EnemyInFov"] = true;
	m_WorldConditions["HouseInFov"] = false;
	m_WorldConditions["ItemInFov"] = false;
	m_WorldConditions["HasWeapon"] = false;
	m_WorldConditions["Survive"] = true;
	m_WorldConditions["InHouse"] = false;
	m_WorldConditions["HasMedkit"] = false;
	m_WorldConditions["HealthLow"] = false;
	m_WorldConditions["HasHouseTarget"] = false;
	m_WorldConditions["WasBitten"] = false;
	m_WorldConditions["OutsideBoundries"] = false;
	m_WorldConditions["HasItemTarget"] = false;
	m_WorldConditions["PurgeZoneInFov"] = false;
	m_WorldConditions["InPurgeZone"] = false;
	m_WorldConditions["HasFood"] = false;
	m_WorldConditions["EnergyLow"] = false;
	
	// Set Character goals
	m_Goals["Survive"] = true;

	//m_AgentInfo.Orientation = Elite::randomFloat(static_cast<float>(-M_PI), static_cast<float>(M_PI));
	
	MakeFSM();
}

Character::~Character()
{
	for (Action* action : m_pActions)
		delete action;
	
	delete m_pPlanner;
}

void Character::Update(float dt)
{
	m_WorldConditions["InventoryFull"] = m_CurrentSlot == 5;
	if(m_WorldConditions["InventoryFull"])

	if (m_EnteredHouseClearTimer >= m_EnteredHouseClearTime)
		m_EnteredHouses.clear();
	else
		m_EnteredHouseClearTimer += dt;

	//if(m_AgentInfo.Bitten)
	//{
	//}


	std::for_each(m_EnteredHouses.begin(), m_EnteredHouses.end(), [this](Elite::Vector2 v)
		{
			m_pInterface->Draw_Circle(v, 10.f, Elite::Vector3{ 1,0,1 }, 0.4f);
		});

	auto dimensions = m_pInterface->World_GetInfo().Dimensions;
	m_WorldConditions["OutsideBoundries"] = !(m_AgentInfo.Position.x > -dimensions.x / 2 &&
		m_AgentInfo.Position.x < (dimensions.x / 2) &&
		m_AgentInfo.Position.y > (-dimensions.y / 2) &&
		m_AgentInfo.Position.y < (dimensions.y / 2));

	Elite::Vector2 worldBoundries[4] = {
		Elite::Vector2{dimensions.x / 2, dimensions.y / 2},
		Elite::Vector2{dimensions.x / 2, -dimensions.y / 2},
		Elite::Vector2{-dimensions.x / 2, -dimensions.y / 2},
		Elite::Vector2{-dimensions.x / 2, dimensions.y / 2} };

	Elite::Vector2 backInBoundries[4] = {
		Elite::Vector2{dimensions.x / 3, dimensions.y / 3},
		Elite::Vector2{dimensions.x / 3, -dimensions.y / 3},
		Elite::Vector2{-dimensions.x / 3, -dimensions.y / 3},
		Elite::Vector2{-dimensions.x / 3, dimensions.y / 3} };
	
	m_pInterface->Draw_Polygon(
		worldBoundries,
		4,
		Elite::Vector3{1,0,0},
		0.4f
	);
	m_pInterface->Draw_Polygon(
		backInBoundries,
		4,
		Elite::Vector3{ 0,1,0 },
		0.4f
	);

	m_WorldConditions["HealthLow"] = m_AgentInfo.Health < 8.f;
	m_WorldConditions["EnergyLow"] = m_AgentInfo.Energy < 2.f;

	m_WorldConditions["WasBitten"] = m_AgentInfo.WasBitten;
	
	if (!m_HousesInFOV.empty())
	{
		if (m_EnteredHouses.empty() || std::find_if(m_EnteredHouses.begin(), m_EnteredHouses.end(), [this](const Elite::Vector2& v)
			{
				return v == m_HousesInFOV[0].Center;
			}) == m_EnteredHouses.end())
		{
			m_CurrentHouseTarget = *std::min_element(m_HousesInFOV.begin(), m_HousesInFOV.end(), IsCloser(m_AgentInfo.Position));
			m_WorldConditions["HasHouseTarget"] = true;
		}
		m_WorldConditions["HouseInFov"] = true;
	}
	else
	{
		m_WorldConditions["HouseInFov"] = false;
	}


	m_WorldConditions["InHouse"] = m_AgentInfo.IsInHouse;

	const auto itemTarget = std::find_if(m_EntitiesInFOV.begin(), m_EntitiesInFOV.end(), IsType<EntityInfo, eEntityType>(eEntityType::ITEM));
	m_WorldConditions["ItemInFov"] = itemTarget != m_EntitiesInFOV.end();
	if(m_WorldConditions["ItemInFov"])
	{
		for(const auto& entityInfo : m_EntitiesInFOV)
		{
			if(entityInfo.Type == eEntityType::ITEM)
			{
				AddItemToMemory(entityInfo);
				if (IsCloser(m_AgentInfo.Position)(itemTarget->Location, m_CurrentItemTarget))
				{
					m_CurrentItemTarget = itemTarget->Location;
				}
			}
		}
	}
	m_WorldConditions["HasItemTarget"] = m_CurrentItemTarget != Elite::Vector2{};
	if (m_WorldConditions["HasFood"] && m_WorldConditions["HasWeapon"] && m_WorldConditions["HasMedkit"])
		m_WorldConditions["HasItemTarget"] = false;

	std::for_each(m_ItemMemory.begin(), m_ItemMemory.end(), [this](const Elite::Vector2& v)
		{
			m_pInterface->Draw_Circle(v, 1.f, Elite::Vector3{ 0.5f,1,0.f }, 0.9f);
		});

	m_pInterface->Draw_Circle(m_CurrentItemTarget, 2.f, Elite::Vector3{ 0,1,0.5f }, 0.9f);
	

	m_WorldConditions["EnemyInFov"] = 
		std::find_if(m_EntitiesInFOV.begin(), m_EntitiesInFOV.end(), IsType<EntityInfo, eEntityType>(eEntityType::ENEMY)) != m_EntitiesInFOV.end();

	auto purgeZone = std::find_if(m_EntitiesInFOV.begin(), m_EntitiesInFOV.end(), IsType<EntityInfo, eEntityType>(eEntityType::PURGEZONE));
	
	m_WorldConditions["PurgeZoneInFov"] = (purgeZone != m_EntitiesInFOV.end());

	if(m_WorldConditions["PurgeZoneInFov"])
	{
		auto purgeZoneInfo = PurgeZoneInfo{};
		m_pInterface->PurgeZone_GetInfo(*purgeZone, purgeZoneInfo);
		
		m_WorldConditions["InPurgeZone"] = Elite::DistanceSquared(m_AgentInfo.Position, purgeZone->Location) < (purgeZoneInfo.Radius * purgeZoneInfo.Radius);
	}

	//m_WorldConditions["HasWeapon"] = HasItemOfType(eItemType::PISTOL);
		
	
	m_pFSM->Update(dt, this);
}

std::vector<Action*> Character::GetPlan() const
{
	return m_pActionState->GetCurrentPlan();
}

std::vector<Action*> Character::GetActions() const
{
	return m_pActions;
}

Action* Character::GetCurrentAction() const
{
	return m_pActionState->GetCurrentAction();
}

Planner* Character::GetPlanner() const
{
	return m_pActionState->GetPlanner();
}

void Character::ChangeCharacterState(const std::string& string, bool b)
{
//#ifdef _DEBUG
//	if (m_WorldConditions[string] != b)
//		std::cout << string << " changed to " << (b ? "true" : "false") << std::endl;
//#endif
	
	m_WorldConditions[string] = b;
}

std::map<std::string, bool> Character::GetConditions() const
{
	return m_WorldConditions;
}

std::map<std::string, bool> Character::GetGoals() const
{
	return m_Goals;
}

void Character::SetAgentInfo(const AgentInfo& agentInfo)
{
	m_AgentInfo = agentInfo;
}

AgentInfo Character::GetAgentInfo() const
{
	return m_AgentInfo;
}

void Character::SetSteeringOutput(const SteeringPlugin_Output& steeringOutput)
{
	m_SteeringOutput = steeringOutput;
}

SteeringPlugin_Output Character::GetSteeringOutput() const
{
	return m_SteeringOutput;
}

void Character::SetHouseInFOV(const std::vector<HouseInfo>& housesInFOV)
{
	m_HousesInFOV = housesInFOV;
}

void Character::SetEntitiesInFOV(const std::vector<EntityInfo>& entitiesInFOV)
{
	m_EntitiesInFOV = entitiesInFOV;
}

std::vector<HouseInfo> Character::GetHousesInFOV() const
{
	return m_HousesInFOV;
}

std::vector<EntityInfo> Character::GetEntitiesInFOV() const
{
	return m_EntitiesInFOV;
}

std::vector<ItemInfo> Character::GetInventory() const
{
	return m_Inventory;
}

std::vector<Elite::Vector2> Character::GetItemMemory() const
{
	return m_ItemMemory;
}

void Character::AddItemToMemory(const EntityInfo& entity)
{
	ItemInfo itemInfo;
	m_pInterface->Item_GetInfo(entity, itemInfo);

	if (std::find_if(m_ItemMemory.begin(), m_ItemMemory.end(), SameLocation(m_CurrentItemTarget)) == m_ItemMemory.end())
		//[this](const ItemInfo& itemInfo){return itemInfo.Location == m_CurrentItemTarget;}) == m_ItemMemory.end())
	{
		m_ItemMemory.push_back(itemInfo.Location);
	}

	std::sort(m_ItemMemory.begin(), m_ItemMemory.end(), IsFurther(m_AgentInfo.Position));


	if (m_CurrentItemTarget == Elite::Vector2{})
	{
		m_CurrentItemTarget = entity.Location;
		m_WorldConditions["HasItemTarget"] = true;
	}
}

void Character::PopItemFromMemory()
{
	m_ItemMemory.pop_back();
	if (m_ItemMemory.empty())
	{
		m_CurrentItemTarget = Elite::Vector2{};
		m_WorldConditions["HasItemTarget"] = false;
	}
	else
		m_CurrentItemTarget = m_ItemMemory[m_ItemMemory.size() - 1];
}

Elite::Vector2 Character::GetCurrentItemTarget() const
{
	return m_CurrentItemTarget;
}

bool Character::HasItemOfType(const eItemType& itemType) const
{
	ItemInfo itemInfo{};

	for(UINT slotId{}; slotId < m_pInterface->Inventory_GetCapacity(); ++slotId)
	{
		m_pInterface->Inventory_GetItem(slotId, itemInfo);
		if (itemInfo.Type == itemType && itemInfo.ItemHash != 0)
			return true;
	}

	return false;
}

int Character::GetIndexForType(const eItemType& itemType) const
{
	ItemInfo itemInfo{};

	for (UINT slotId{}; slotId < m_pInterface->Inventory_GetCapacity(); ++slotId)
	{
		m_pInterface->Inventory_GetItem(slotId, itemInfo);
		if (itemInfo.Type == itemType && itemInfo.ItemHash != 0)
			return slotId;
	}
	
	return -1;
}

int Character::HasAmountOfType(const eItemType& itemType) const
{
	int amount{};
	ItemInfo itemInfo{};
	
	for (UINT slotId{}; slotId < m_pInterface->Inventory_GetCapacity(); ++slotId)
	{
		m_pInterface->Inventory_GetItem(slotId, itemInfo);
		if (itemInfo.Type == itemType)
			++amount;
	}

	return amount;
}

void Character::RemoveItemFromInventory(const int index)
{
	m_pInterface->Inventory_RemoveItem(index);
	if(m_CurrentSlot > index - 1) m_CurrentSlot = index - 1;
}

void Character::SetSlot(const int index)
{
	m_CurrentSlot = index;
}

int Character::GetSlot() const
{
	return m_CurrentSlot;
}

std::vector<Elite::Vector2> Character::GetEnteredHouses() const
{
	return m_EnteredHouses;
}

void Character::AddEnteredHouse(const Elite::Vector2& houseCenter)
{
	if(std::find_if(m_EnteredHouses.begin(), m_EnteredHouses.end(), [houseCenter](const Elite::Vector2& v)
		{
			return v == houseCenter;
		}) != m_EnteredHouses.end())
		return;
	
	m_EnteredHouses.push_back(houseCenter);
	m_EnteredHouseClearTimer = 0.f;
}

HouseInfo Character::GetCurrentHouseTarget() const
{
	return m_CurrentHouseTarget;
}

void Character::SetCurrentHouseTarget(const HouseInfo& houseCenter)
{
	m_CurrentHouseTarget = houseCenter;
}

IExamInterface* Character::GetInterface() const
{
	return m_pInterface;
}

void Character::PrintWorldStates() const
{
	std::cout << "==== World Conditions ====" << std::endl;
	std::for_each(m_WorldConditions.begin(), m_WorldConditions.end(), [](std::pair<std::string, bool> a)
		{
			std::cout << a.first << " = " << a.second << std::endl;
		});
	std::cout << std::endl;
}

void Character::MakeFSM()
{
	m_pActionState = new ActionState(this);
	m_pMoveState = new MoveState();
	const auto pMoveTransition = new ToMoveTransition();
	const auto pActionTransition = new ToActionTransition();

	m_pFSM = new FiniteStateMachine(m_pActionState);
	
	m_pFSM->AddTransition(m_pActionState, m_pMoveState, pMoveTransition);
	m_pFSM->AddTransition(m_pMoveState, m_pActionState, pActionTransition);
}

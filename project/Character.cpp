#include "stdafx.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include "IExamInterface.h"

#include "Planner.h"
#include "Blackboard.h"
#include "FiniteStateMachine.h"

//Actions
#include "EnterHouse.h"
#include "ExitHouse.h"
#include "FleeFromEnemy.h"
#include "PickupClass.h"
#include "SearchForItems.h"
#include "ShootAtEnemy.h"

Character::Character(IExamInterface* pInterface)
	: m_pPlanner{new Planner{}}
	, m_pInterface{pInterface}
{
	m_Inventory.resize(m_pInterface->Inventory_GetCapacity());
	
	m_pActions.push_back(new FleeFromEnemy());
	m_pActions.push_back(new ShootAtEnemy());
	m_pActions.push_back(new SearchForItems());
	m_pActions.push_back(new EnterHouse());
	m_pActions.push_back(new PickupItem());
	m_pActions.push_back(new ExitHouse());

	
	// Set World Conditions
	m_WorldConditions["EnemiesInFov"] = false;
	m_WorldConditions["HouseInFov"] = false;
	m_WorldConditions["ItemInFov"] = false;
	m_WorldConditions["HasWeapon"] = false;
	m_WorldConditions["Survive"] = true;
	m_WorldConditions["InHouse"] = false;
	
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
	m_WorldConditions["InventoryFull"] = false;

	std::for_each(m_EnteredHouses.begin(), m_EnteredHouses.end(), [this](Elite::Vector2 v)
		{
			m_pInterface->Draw_Circle(v, 10.f, Elite::Vector3{ 1,0,1 }, 0.4f);
		});
	
	if (m_HousesInFOV.size() > 0)
	{
		m_CurrentHouseTarget = m_HousesInFOV[0];
		
		m_WorldConditions["InHouse"] = (m_AgentInfo.Position.x > m_CurrentHouseTarget.Center.x - (m_CurrentHouseTarget.Size.x / 2) &&
			m_AgentInfo.Position.x < m_CurrentHouseTarget.Center.x + (m_CurrentHouseTarget.Size.x / 2) &&
			m_AgentInfo.Position.y > m_CurrentHouseTarget.Center.y - (m_CurrentHouseTarget.Size.y / 2) &&
			m_AgentInfo.Position.y < m_CurrentHouseTarget.Center.y + (m_CurrentHouseTarget.Size.y / 2));

		m_WorldConditions["HouseInFov"] = true;
		
		//if(m_EnteredHouses.size() <= 0 || std::find_if(m_EnteredHouses.begin(), m_EnteredHouses.end(), [this](Elite::Vector2 v)
		//{
		//		return v == m_CurrentHouseTarget.Center;
		//}) != m_EnteredHouses.end())
		//{
		//	m_WorldConditions["HouseInFov"] = true;
		//}
		//else
		//{
		//	m_WorldConditions["HouseInFov"] = false;
		//}
	}
	else
	{
		m_WorldConditions["InHouse"] = false;
		m_WorldConditions["HouseInFov"] = false;
	}
	
	m_WorldConditions["ItemInFov"] = std::find_if(m_EntitiesInFOV.begin(), m_EntitiesInFOV.end(), [](EntityInfo info)
		{
			return info.Type == eEntityType::ITEM;
		}) != m_EntitiesInFOV.end();
	
	m_pFSM->Update(dt, this);

	std::cout << "==== Current State ====" << std::endl;
	auto currentPlan = m_pActionState->GetCurrentPlan();
	std::for_each(currentPlan.begin(), currentPlan.end(), [](Action* a)
		{
			std::cout << " -> " << a->GetName();
		});
	std::cout << std::endl;
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

void Character::ChangeCharacterState(std::string string, bool b)
{
#ifdef _DEBUG
	if (m_WorldConditions[string] != b)
		std::cout << string << " changed to " << (b ? "true" : "false") << std::endl;
#endif
	
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

void Character::SetAgentInfo(AgentInfo agentInfo)
{
	m_AgentInfo = agentInfo;
}

AgentInfo Character::GetAgentInfo() const
{
	return m_AgentInfo;
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

std::vector<Elite::Vector2> Character::GetEnteredHouses() const
{
	return m_EnteredHouses;
}

void Character::AddEnteredHouse(const Elite::Vector2& houseCenter)
{
	m_EnteredHouses.push_back(houseCenter);
}

HouseInfo Character::GetCurrentHouseTarget() const
{
	return m_CurrentHouseTarget;
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
	ToMoveTransition* pMoveTransition = new ToMoveTransition();
	ToActionTransition* pActionTransition = new ToActionTransition();

	m_pFSM = new FiniteStateMachine(m_pActionState);
	
	m_pFSM->AddTransition(m_pActionState, m_pMoveState, pMoveTransition);
	m_pFSM->AddTransition(m_pMoveState, m_pActionState, pActionTransition);
}

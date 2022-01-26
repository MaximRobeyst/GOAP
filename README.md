# GOAP
## I. Introduction
My own implementation of the Goal Oriented Action Planning(GOAP) in a zombie survival game framework. GOAP is an AI system used in games such as F.E.A.R., S.T.A.L.K.E.R. and Fallout 3. It can plan actions dynamically based on world conditions. These actions are then executed if any action or world condition is changed during the execution, the AI will replan and execute a different plan. This also removes the need for a overly complex predefined graphs such as Finite State Machines and Behaviour Trees. 

## II. Design and implementation
the design and implementation has 4 important components the planner, actions, the FSM and the character. The planner is the most integral part of the whole system it tells which actions are viable and should be performed in what situation. The actions themselves all inherit from an abstract class with some abstract functions that are all. the FSM is a normal finite state machine with only 2 states an action state and a move state, and then finally the character this one holds al the information for the agent and updates the worldconditions.

### Planner
The planner is the most integral part of the GOAP system. It will make your agent preform it's actions in the right place and right time with the information its given. The planner uses a path finding algorithm to find the best suitable plan in the agents current situation based on the worldstate and the actions it's been given. It will generate a graph of connected nodes that. The node is a struct that includes multiple variabes.

```cpp
	struct PlanNode
	{
		PlanNode() = default;
		PlanNode(PlanNode* pParent, float runningCost, std::map<std::string, bool> state, Action* action); // Contructor
		~PlanNode();

		bool operator< (const PlanNode& plannode) const; // Compare running costs

		PlanNode* pParent; // The parent node
		float runningCost{}; // The cost of running this action
		std::map<std::string, bool> state; // The effect the action might have on the worldstate
		Action* action{}; // The action that needs to be performed
	};
```

The main function of the planner is of course the ```GetPlan``` function this function takes 4 parameters a vector of ```Action*``` that represent the availableActions, a map with a ```string & bool``` that represents the world states, another map of ```string & bool``` that represent the goal, and a ```Character*``` that has al the agents info needed

the action will first loop over all available actions and check if there procedural precondtions return true. If this is the case the action gets added to a vector of type ```Action*``` called usable actions only these actions will be used in the planning stage. The planning stage creates a graph with plannodes. the ```BuildGraph ``` function is recursive so it can branch, it takes 4 parameters a ```PlanNode*``` that represents the starting node that the other nodes will connect to, a reference vector of type ```PlanNode*``` these are all the leaves for the graph. the vector of type ```Action*``` usableActions that we made before and the map with ```strings & bool``` that represents the goal we want to go to.
the function goes over all actions and tries to find an instance where the conditions of our startnode and our action precondtions allign with each other if this is the case we create a temporary new conditions if these match with the goal we have found a plan otherwise we try call BuildGraph again but the current node as a starting node

after building the graph we than look for the cheapest node and loop over its parents. The parents get added to a vector called results, this vector than gets reversed and this will then be returned as the path. congratulations you now have a generated plan! 

### FSM
The FSM is a simple Finite state machine with only 2 states and transitions. The action state is responsible for performing the action and the move state is responsible for moving the character to the position it needs to go to

![FSM](https://user-images.githubusercontent.com/51368164/151240193-08e264dc-0fab-41fa-941b-54cd723a5b28.jpg)

#### Action State
The action state has a very simple update function that first check if the it's current action is empty or if the plan is valid if one of these conditions is false a new plan needs to be generated otherwise we execute our action. if our action is completed and our plan is only of size 1 we generate a new plan otherwise we go to the next plan in the list and repeat the process.
```cpp
void ActionState::Update(float deltaTime, Character* pCharacter)
{
	if (m_pCurrentAction == nullptr || !m_pPlanner->IsPlanValid(m_pCurrentAction, m_pCharacter->GetConditions(), pCharacter))
		GenerateNewPlan(pCharacter);

	m_pCurrentAction->ExecuteAction(deltaTime, pCharacter);

	if (m_pCurrentAction->IsDone(pCharacter))
	{
		++m_ActionPointer;
		if (m_pCurrentPlan.size() >= static_cast<size_t>(m_ActionPointer))
		{
			m_ActionPointer = 0;
			GenerateNewPlan(pCharacter);
		}
		
		m_pCurrentAction = m_pCurrentPlan[m_ActionPointer];
	}
}
```

the transition to the move states checks if our action needs to be in range and if it is already in range.
```cpp
bool ToMoveTransition::ToTransition(Character* pCharacter) const
{
	if (pCharacter->GetCurrentAction() == nullptr)
		return false;
	
	return pCharacter->GetCurrentAction()->RequiresInRange() && !pCharacter->GetCurrentAction()->IsInRange(pCharacter);

}
```

#### Move State
the move state looks for the next navmesh point in the direction we need to go and performs a seek behaviour to get there.
```cpp
void MoveState::Update(float deltaTime, Character* pCharacter)
{
	IExamInterface* pInterface = pCharacter->GetInterface();

	auto agentInfo = pCharacter->GetAgentInfo();
	auto steering = pCharacter->GetSteeringOutput();

	auto target = pCharacter->GetCurrentAction()->GetTarget(pCharacter);
	pCharacter->GetInterface()->Draw_Point(target, 4.f, Elite::Vector3{ 1.f, 1.f, 1.f }, 0.4f);

	steering.AutoOrient = true;
	steering.LinearVelocity = pInterface->NavMesh_GetClosestPathPoint(target) - agentInfo.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed;

	pCharacter->SetSteeringOutput(steering);
}
```
to transition back into the action state we only check if we are in range, the action is done or if our plan is still valid
```cpp
bool ToActionTransition::ToTransition(Character* pCharacter) const
{
	if (pCharacter->GetCurrentAction() == nullptr)
		return false;

	return pCharacter->GetCurrentAction()->IsInRange(pCharacter) || pCharacter->GetCurrentAction()->IsDone(pCharacter) ||
		!pCharacter->GetPlanner()->IsPlanValid(pCharacter->GetCurrentAction(), pCharacter->GetConditions(), pCharacter);
}
```
### Action
actions are an integral part of the GOAP system. they are responsible for what the agent does. The action needed to be an abstract class so that multiple different action can be made with differering behaviours each the base Action class looks like this
```cpp
class Action
{
public:
	Action() = default; // Empty Contructor
	Action(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost); // If your action needs to be used with custom Precondition, effects and cost
	virtual ~Action() = default;  // Destructor
	
	virtual bool CheckProceduralPreconditions(Character* pCharacter) const = 0;	// Procedural conditions the character has to suffice for the action to execute
	virtual bool ExecuteAction(float dt,Character* pCharacter) = 0; // The action that is executing
	
	virtual bool IsDone(Character* pCharacter) = 0; // is the action done executing
	virtual bool RequiresInRange() const = 0; // does the action need to be in range if so the agent will go the to moveto state first and then execute
	virtual bool IsInRange(Character* pCharacter) const = 0;
	virtual Elite::Vector2 GetTarget(Character* pCharacter);
	
	std::map<std::string, bool> GetPreconditions();
	std::map<std::string, bool> GetEffects();
	float GetCost();
	
protected:
	std::map<std::string, bool> m_Preconditions; // conditions the world has to be in for the action to be executed
	std::map<std::string, bool> m_Effects; // the effect this action will have on the world
	float m_Cost{};	// cost of the action
};
```

1. CheckProceduralPreconditions(Character* pCharacter)
   - This function checks some procedural preconditions needed to set up the action. the action performs if this function succeeds, if the procedural preconditions fail the action cannot be performed and will not be considered in planning. The procedural preconditions can be set to a simple ```return true;``` if the action does not need any procedural preconditions, otherwise you can check whatever values needed for the function.

2. IsDone(Character* pCharacter)
   - This function will return true if the action is succesfully performed by the agent without any failures. If you action needs to be performed infinitly until it doens't fit the worldconditions anymore you just need it to ```return false```. if this function is set to a ```return true``` you might run in to the problem that you will go to the same action every frame, which leads the planner to replan every frame and waste resources so it is best not to do this unless you have a specific state that needs to be performed only one frame

3. RequiresInRange() const
   - This function tells if the action needs to be performed within a certain range of another object.If RequiresInRange is true the FSM will switch to a move state and move to the target given by the actions GetTarget() function.

4. IsInRange(Character* character)
   - this function simply returns if the character is in a suitable position.

5. GetTarget(Character* character)
   - this function returns the target position of where the action needs to be performed 

6. Preconditions
   - The precondition are certain states the world has to be in for this action to be able to be performed for instance if you'd want to pick up an item the precondition should be that that item is in your FOV ("ItemInFov") and that your inventory has a space left for that item ("InventoryFull") if these 2 conditions don't apply the action will not be considered during planning.

7. Effects
   - The effect are the results our action could have when being executed this can be used by the planner to create a sequence of actions that will be executed after each other until they are finished or they have become invalid. the effects that an action can have on the world are not always guaranteed.

8. Cost
   - The cost of this action is the representation of how desirable this action will be. 
### Character
the character is a very simple class but it holds all information about the world, inventory and so on. This is just our representation of the agent itself

## III. Result
with al the architecture i made these 14 states some of them have some similar behaviour while others are very different from each other. I will go over a few examples and explain how they work
![Flowchart](https://user-images.githubusercontent.com/51368164/151229766-bd22852a-16f9-4221-ad73-8de975d51339.jpg)

### Shoot at enemy
The shoot at enemy function is fairly simple but pretty efficient in killing enemies the play comes across. The action does not have any procedural preconditions and  starts with getting the information from the character class such as its steering, agentinfo and entities in fov. To know if we are looking at the enemy we need to know what the angle difference is this gets calculated like this: ```const float offsetWithEnemy = Elite::GetOrientationFromVelocity(info.Location - agentInfo.Position) - agentInfo.Orientation;```. Than after we do this we set our auto orientation to false and set our angular velocity according to the angleoffset now to know when get to shoot our enemy we have to know how far he. if the enemy is farther away our chance of missing is way bigger that's why the maximum angle offset it `0.07f` the other radiuses have bigger angles because the enemy is closer they are represented by the circles in the gif, if we can shoot check if we still have ammo if not we remove our item so we don't waste any space in our inventory.
![ezgif-5-01d696cd04](https://user-images.githubusercontent.com/51368164/151234725-63c1fe71-9e28-4ff9-88f7-aac666580fb5.gif)

<details><summary><i>Code for this action</i></summary>
	
<br>
	
```cpp
#include "stdafx.h"
#include "ShootAtEnemy.h"
#include "Character.h"

#include <IExamInterface.h>

#include "Helpers.h"

ShootAtEnemy::ShootAtEnemy()
{
	m_Preconditions["HasWeapon"] = true;
	m_Preconditions["EnemyInFov"] = true;

	m_Effects["Survive"] = true;

	m_Cost = 0.25f;
}

ShootAtEnemy::ShootAtEnemy(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
	:Action(preConditions, effects, cost)
{
}

bool ShootAtEnemy::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool ShootAtEnemy::ExecuteAction(float dt, Character* pCharacter)
{
	auto entityList = pCharacter->GetEntitiesInFOV();

	const auto agentInfo = pCharacter->GetAgentInfo();
	auto steering = pCharacter->GetSteeringOutput();

	// Sort from closest to furthest
	std::sort(entityList.begin(), entityList.end(), IsCloser(agentInfo.Position));
	// Get the first enemy you find because that will be the closest one
	EntityInfo info = *std::find_if(entityList.begin(), entityList.end(), IsType<EntityInfo, eEntityType>(eEntityType::ENEMY));

	// Get certain radiuses because if the enemy is closer the max angle can be bigger
	const float personalSpace = pCharacter->GetAgentInfo().FOV_Range * 0.05f;
	const float smallAngle = pCharacter->GetAgentInfo().FOV_Range * 0.333333f;
	const float mediumAngle = pCharacter->GetAgentInfo().FOV_Range * 0.6666666f;
	// The big anlge is just the fov range

	const float offsetWithEnemy = Elite::GetOrientationFromVelocity(info.Location - agentInfo.Position) - agentInfo.Orientation;

	steering.AutoOrient = false;
	
	steering.AngularVelocity = offsetWithEnemy < 0 ? -pCharacter->GetAgentInfo().MaxAngularSpeed : pCharacter->GetAgentInfo().MaxAngularSpeed;

	float maxAngleOffset;
	if (Elite::DistanceSquared(agentInfo.Position, info.Location) < smallAngle * smallAngle)
		maxAngleOffset = 0.2f;
	else if (Elite::DistanceSquared(agentInfo.Position, info.Location) < mediumAngle * mediumAngle)
		maxAngleOffset = 0.1f;
	else
		maxAngleOffset = 0.069f;


	std::for_each(entityList.begin(), entityList.end(), [agentInfo, pCharacter](const EntityInfo& info)
		{
			Elite::Vector2 dir{ Elite::Vector2{info.Location - pCharacter->GetAgentInfo().Position} };
			pCharacter->GetInterface()->Draw_Direction(
				pCharacter->GetAgentInfo().Position, dir, dir.Normalize(), Elite::Vector3{ 0,0,1 }, 0.9f);
		});
	Elite::Vector2 dir{ Elite::Vector2{info.Location - pCharacter->GetAgentInfo().Position} };
	
	pCharacter->GetInterface()->Draw_Direction(
		pCharacter->GetAgentInfo().Position, dir, dir.Normalize(), Elite::Vector3{ 0,1,0 }, 0.9f);

	pCharacter->GetInterface()->Draw_Circle(pCharacter->GetAgentInfo().Position, personalSpace, Elite::Vector3{ 1.f, 0.05f, 0.f });
	pCharacter->GetInterface()->Draw_Circle(pCharacter->GetAgentInfo().Position, smallAngle, Elite::Vector3{ 1.f, 0.333f, 0.f });
	pCharacter->GetInterface()->Draw_Circle(pCharacter->GetAgentInfo().Position, mediumAngle, Elite::Vector3{ 1.f, 0.666f, 0.f });
	pCharacter->GetInterface()->Draw_Circle(pCharacter->GetAgentInfo().Position, pCharacter->GetAgentInfo().FOV_Range, Elite::Vector3{ 1.f, 1.f, 0.f });

	pCharacter->GetInterface()->Draw_Direction(
		pCharacter->GetAgentInfo().Position,
		pCharacter->GetAgentInfo().LinearVelocity + Elite::Vector2(sinf(maxAngleOffset), cosf(maxAngleOffset)),
		pCharacter->GetAgentInfo().FOV_Range,
		Elite::Vector3{ 0,0,1 },
		0.9f
	);
	
	if(abs(offsetWithEnemy) < maxAngleOffset)
	{
		m_CurrentWeaponIndex = pCharacter->GetIndexForType(eItemType::PISTOL);
		ItemInfo itemInfo{};
		
		pCharacter->GetInterface()->Inventory_UseItem(m_CurrentWeaponIndex);
		
		pCharacter->GetInterface()->Inventory_GetItem(m_CurrentWeaponIndex, itemInfo);
		if(pCharacter->GetInterface()->Weapon_GetAmmo(itemInfo) <= 0)
		{
			pCharacter->RemoveItemFromInventory(m_CurrentWeaponIndex);
			if (pCharacter->HasItemOfType(eItemType::PISTOL))
				m_CurrentWeaponIndex = pCharacter->GetIndexForType(eItemType::PISTOL);
			
			pCharacter->ChangeCharacterState("HasWeapon", pCharacter->HasItemOfType(eItemType::PISTOL));
			return false;
		}
	}

	pCharacter->SetSteeringOutput(steering);
	pCharacter->SetAgentInfo(agentInfo);
	
	return true;
}

bool ShootAtEnemy::IsDone(Character* pCharacter)
{
	return false;
}

bool ShootAtEnemy::RequiresInRange() const
{
	return false;
}

bool ShootAtEnemy::IsInRange(Character* pCharacter) const
{
	return true;
}
```
</details>

### Pick up item
The Pickup item action picks up items by getting the current item target in the player when pick up it will pop it from the itemlocations list and set another item target. It also sets some new conditions when an item is picked up, for example when you pickup a pistol the condition "HasWeapon" will be set to true. 
When the item is pick up the action is done and the planner will generate a new plan.
![GPP_TEST_RELEASE_KRfvC7HCLa](https://user-images.githubusercontent.com/51368164/151248908-125c197e-d9ec-488a-946b-afe66db0944a.gif)

<details><summary><i>Code for this action</i></summary>
	
<br>
```
#include "stdafx.h"
#include "PickupClass.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

#include "Helpers.h"


PickupItem::PickupItem()
{
	m_Preconditions["InventoryFull"] = false;
	m_Preconditions["HasItemTarget"] = true;
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
```
	
</details>
	
### EnterHouse
The enter house function is very self explanatory, It will go to the center of the current house target and adds it to the visited houses when entered. it also has a procedural precondition that checks if we have visisted this house before, if we did we don't enter it because we probably looted it already. It also the agent to be in range which means the agents FSM will set its state to the move state and move to its designated location, until it's at it's location or something happens.
![GPP_TEST_RELEASE_G5F1SJKRBj](https://user-images.githubusercontent.com/51368164/151246813-5df989b7-9327-494d-a46f-f90fea18bc7f.gif)
	
<details><summary><i>Code for this action</i></summary>
	
<br>
	
```cpp
#include "stdafx.h"
#include "EnterHouse.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

#include "Helpers.h"

EnterHouse::EnterHouse()
{
	m_Preconditions["EnemyChasing"] = true;
	m_Preconditions["HasHouseTarget"] = true;
	m_Preconditions["ItemInFov"] = false;
	m_Preconditions["InPurgeZone"] = false;
	m_Preconditions["InventoryFull"] = false;

	m_Effects["InHouse"] = true;
	m_Effects["EnemyChasing"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 2.f;
}

EnterHouse::EnterHouse(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
	: Action(preConditions, effects, cost)
{
}

bool EnterHouse::CheckProceduralPreconditions(Character* pCharacter) const
{
	if (pCharacter->GetConditions()["HasWeapon"] && pCharacter->GetConditions()["EnemyInFov"])
		return false;
	
	auto houseInfo = pCharacter->GetCurrentHouseTarget();

	// if house has been entered recently don't enter it again
	if (!pCharacter->GetEnteredHouses().empty())
	{
		auto enteredHouses = pCharacter->GetEnteredHouses();

		if (find_if(enteredHouses.begin(), enteredHouses.end(), SameLocation(pCharacter->GetAgentInfo().Position)) != enteredHouses.end())
		{
			return false;
		}
	}
	return true;
}

bool EnterHouse::ExecuteAction(float dt, Character* pCharacter)
{
	GetTarget(pCharacter);

	const auto agentInfo = pCharacter->GetAgentInfo();
	auto steering = pCharacter->GetSteeringOutput();

	steering.LinearVelocity = pCharacter->GetCurrentHouseTarget().Center - agentInfo.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -agentInfo.MaxLinearSpeed;
	pCharacter->SetSteeringOutput(steering);
	return true;
}

bool EnterHouse::IsDone(Character* pCharacter)
{
	const auto entitiesInFov = pCharacter->GetEntitiesInFOV();
	const auto item = std::find_if(entitiesInFov.begin(), entitiesInFov.end(), IsType<EntityInfo, eEntityType>(eEntityType::ITEM));
	if(item != entitiesInFov.end())
	{
		pCharacter->AddEnteredHouse(pCharacter->GetCurrentHouseTarget().Center);
		pCharacter->ChangeCharacterState("HasHouseTarget", false);
		pCharacter->SetCurrentHouseTarget(HouseInfo{});
		pCharacter->ChangeCharacterState("InHouse", true);
	}

	return IsInRange(pCharacter) || item != entitiesInFov.end();
}

bool EnterHouse::RequiresInRange() const
{
	return true;
}

bool EnterHouse::IsInRange(Character* pCharacter) const
{
	if (pCharacter->GetHousesInFOV().empty())
		return false;
	
	HouseInfo houseInfo = pCharacter->GetHousesInFOV()[0];


	if (pCharacter->GetAgentInfo().Position.x > houseInfo.Center.x - (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.x < houseInfo.Center.x + (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.y > houseInfo.Center.y - (houseInfo.Size.y / 2) &&
		pCharacter->GetAgentInfo().Position.y < houseInfo.Center.y + (houseInfo.Size.y / 2))
	{
		pCharacter->ChangeCharacterState("InHouse", true);
	}

	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x - (houseInfo.Size.x / 2) ,  houseInfo.Center.y - (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);
	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x + (houseInfo.Size.x / 2) ,  houseInfo.Center.y - (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);
	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x - (houseInfo.Size.x / 2) ,  houseInfo.Center.y + (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);
	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x + (houseInfo.Size.x / 2) ,  houseInfo.Center.y + (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);

	auto distance = Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, houseInfo.Center);

	if(distance < (m_Range * m_Range))
	{
		pCharacter->AddEnteredHouse(houseInfo.Center);
		pCharacter->ChangeCharacterState("HasHouseTarget", false);
		pCharacter->SetCurrentHouseTarget(HouseInfo{});
		pCharacter->ChangeCharacterState("InHouse", true);
	}
	
	return distance < (m_Range * m_Range);

}

Elite::Vector2 EnterHouse::GetTarget(Character* pCharacter)
{
	m_Target = pCharacter->GetCurrentHouseTarget().Center;

	return m_Target;
}
```
</details>

## IV. Conclusion/Future work
While not the best implementation of the GOAP system i'm still very proud of it. Some improvements i would like to make would be to create better actions right now when a plan is made they almost every time consist of only 1 action this is because i did not use the effects a class has enough. The other thing i would improve is the planner right now it uses pointer to plan stuff out, this caused a lot of memory allocation and when a new plan needs to be generated every frame (which is not the case luckely) the performance hit would be very bad espacially in a bigger project. i would also love to make the agents a fight each other or make them team up to survive which would be a fun experiment. 

#### Sources used
- https://gamedevelopment.tutsplus.com/tutorials/goal-oriented-action-planning-for-a-smarter-ai--cms-20793
- https://alumni.media.mit.edu/~jorkin/goap.html


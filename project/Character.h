#pragma once
#include <Exam_HelperStructs.h>

#include "ConditionsAndActions.h"
class IExamInterface;
class Planner;
class FiniteStateMachine;
class Blackboard;
class ActionState;
class MoveState;

class Character final
{
public:
	Character(IExamInterface* pInterface);
	~Character();

	void Update(float dt);

	void AddAction(Action* action);

	std::vector<Action*> GetPlan() const;
	std::vector<Action*> GetActions() const;
	Action* GetCurrentAction() const;
	Planner* GetPlanner() const;
	
	void ChangeCharacterState(const std::string& string, bool b);
	std::map<std::string, bool> GetConditions() const;
	std::map<std::string, bool> GetGoals() const;
	
	void SetAgentInfo(const AgentInfo& agentInfo);
	AgentInfo GetAgentInfo() const;

	void SetSteeringOutput(const SteeringPlugin_Output& steeringOutput);
	SteeringPlugin_Output GetSteeringOutput() const;

	void SetHouseInFOV(const std::vector<HouseInfo>& housesInFOV);
	void SetEntitiesInFOV(const std::vector<EntityInfo>& entitiesInFOV);

	std::vector<HouseInfo> GetHousesInFOV() const;
	std::vector<EntityInfo> GetEntitiesInFOV() const;
	std::vector<ItemInfo> GetInventory() const;
	
	bool HasItemOfType(const eItemType& itemType) const;
	int GetIndexForType(const eItemType& itemType) const;
	
	void RemoveItemFromInventory(const int index);
	void SetSlot(const int index);
	int GetSlot() const;
	
	std::vector<Elite::Vector2> GetEnteredHouses() const;
	void AddEnteredHouse(const Elite::Vector2& houseCenter);
	HouseInfo GetCurrentHouseTarget() const;
	void SetCurrentHouseTarget(const HouseInfo& houseCenter);

	IExamInterface* GetInterface() const;

	void PrintWorldStates() const;
	
private:
	void MakeFSM();

	bool InventoryFull();

	AgentInfo m_AgentInfo;
	IExamInterface* m_pInterface;
	SteeringPlugin_Output m_SteeringOutput;

	std::vector<HouseInfo> m_HousesInFOV{};
	std::vector<EntityInfo> m_EntitiesInFOV{};
	std::vector<ItemInfo> m_Inventory{};

	std::vector<EnemyInfo> m_EnemyTrack{};

	Elite::Vector2 m_Target{};
	
	FiniteStateMachine* m_pFSM;				// the finite state machine that will make the character move and perform actions its states will be (Plan, Move, Perform)
	ActionState* m_pActionState;			// state in the FSM that performs an action
	MoveState* m_pMoveState;				// state in the FSM that moves to the target the agent is supposed to go
	
	Planner* m_pPlanner;					// Planner that will find plans for our character

	std::vector<Action*> m_pActions;		// Al the actions the character has
	std::vector<Elite::Vector2> m_EnteredHouses{};
	HouseInfo m_CurrentHouseTarget{};

	std::map<std::string, bool> m_WorldConditions{};	// All the condition the world has
	std::map<std::string, bool> m_Goals{};				// the goals the player has

	int m_CurrentSlot{ 0 };

	float m_EnteredHouseClearTimer{ 0.f };
	const float m_EnteredHouseClearTime{ 30.0f };
};


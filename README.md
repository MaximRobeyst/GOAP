# GOAP
## Introduction
My own implementation of the Goal Oriented Action Planning(GOAP) in a zombie survival game. GOAP is an AI system used in games such as F.E.A.R., S.T.A.L.K.E.R. and Fallout 3. GOAP can plan actions dynamically based on world conditions. These actions are then executed if any action or world condition is changed during the execution, the AI then will replan and execute a different plan. This also removes the need for a overly complex predefined graphs such as Finite State Machines and Behaviour Trees. 
## Actions
actions are an integral part of the GOAP system. they are responsible for what the agent does. The action needed to be an abstract class so that multiple different action can be made with differering behaviours each the base Action class looks like this
```
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
	
	virtual std::string GetName() const = 0;
	
	std::map<std::string, bool> GetPreconditions();
	std::map<std::string, bool> GetEffects();
	float GetCost();
	
protected:
	std::map<std::string, bool> m_Preconditions; // conditions the world has to be in for the action to be executed
	std::map<std::string, bool> m_Effects; // the effect this action will have on the world
	float m_Cost{};	// cost of the action
};
```

### CheckProceduralPreconditions(Character* pCharacter)
this function check some procedural precondition set up to make the action perform if this function fails the action cannot be performed and will not be considered in planning. the procedural preconditions can be set to a simple ```return true;``` if the action does not need any procedural functions otherwise you can check whatever values needed for the function.

### IsDone(Character* pCharacter)
this function is pretty similar to the CheckProceduralPreconditions function only you'd generally not want to set this value to true all the time otherwise the planner would need to generate a new plan every frame. that's why you'd best set it to a standard false, except if the action is finishable. Like when you pick up and item the Action would be done because you picked up the item.

### RequiresInRange() const
this function tells if the function needs to be performed within a range of something else. if RequiresInRange is true the finite state machine will  switch to a move state and move to the target given by the GetTarget() function 

## Planner
The planner uses a path finding alorithm to find it's new course of action the planner has multiple function for this and a struct name PlanNode.
The plannode consists of a pointer to its parent, a float containing the runningcost, a map to indicate the state it should be in and the action that needs to be performed the plan will create these Plannodes while generating a plan. while this has some memory Allocation it doesn't run every frame unless you tell it to do that so the effect on performance isn't that bad.



## Finite State machine

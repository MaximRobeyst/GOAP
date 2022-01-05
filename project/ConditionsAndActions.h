#pragma once
#include <vector>
#include <string>

struct Condition
{
	std::string conditionName;
	bool state;
};

class Action
{
public:
	Action();
	virtual ~Action() = default;
	
	virtual bool CheckProceduralPreconditions() const = 0;	// Procedural conditions the character has to suffice for the action to execute
	virtual bool ExecuteAction() const = 0;					// The action that is executing
	virtual bool IsDone() const = 0;						// is the action done execting

private:
	std::vector<Condition*> m_Preconditions;	// conditions the world has to be in for the action to be executed
	std::vector<Condition*> m_Effects;			// the effect this action will have on the world
	float m_Cost{};								// cost of the action
};
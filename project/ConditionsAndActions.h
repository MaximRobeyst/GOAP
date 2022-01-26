#pragma once
#include <map>
#include <vector>
#include <string>

class Character;

//TODO: maybe change RequiresRange to a var
class Action
{
public:
	Action() = default;
	Action(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);
	virtual ~Action() = default;
	
	virtual bool CheckProceduralPreconditions(Character* pCharacter) const = 0;	// Procedural conditions the character has to suffice for the action to execute
	virtual bool ExecuteAction(float dt,Character* pCharacter) = 0;						// The action that is executing
	
	virtual bool IsDone(Character* pCharacter) = 0;								// is the action done executing
	virtual bool RequiresInRange() const = 0;				// does the action need to be in range if so the agent will go the to moveto state first and then execute
	virtual bool IsInRange(Character* pCharacter) const = 0;
	virtual Elite::Vector2 GetTarget(Character* pCharacter);
	
	virtual std::string GetName() const = 0;
	
	std::map<std::string, bool> GetPreconditions();
	std::map<std::string, bool> GetEffects();
	float GetCost();
	
protected:
	std::map<std::string, bool> m_Preconditions;	// conditions the world has to be in for the action to be executed
	std::map<std::string, bool> m_Effects;			// the effect this action will have on the world
	float m_Cost{};								// cost of the action
};


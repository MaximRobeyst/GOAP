#pragma once
#include <map>
#include <vector>
#include <string>

class Action
{
public:
	Action() = default;
	Action(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);
	virtual ~Action() = default;
	
	virtual bool CheckProceduralPreconditions() const = 0;	// Procedural conditions the character has to suffice for the action to execute
	virtual bool ExecuteAction() = 0;						// The action that is executing
	virtual bool IsDone() = 0;								// is the action done executing
	virtual bool RequiresInRange() const = 0;				// does the action need to be in range if so the agent will go the to moveto state first and then execute

	std::map<std::string, bool> GetPreconditions();
	std::map<std::string, bool> GetEffects();
	float GetCost();

	virtual std::string GetName() const = 0;
	
protected:
	std::map<std::string, bool> m_Preconditions;	// conditions the world has to be in for the action to be executed
	std::map<std::string, bool> m_Effects;			// the effect this action will have on the world
	float m_Cost{};								// cost of the action
	//bool m_InRange{};							// is the character in range for this action to be performed (COMMENTED: for now because i dont know if i'll need this)
};

class GetAxe final: public Action	// Preconditions {Haschips, isHungry} => Effects {Hungry = false, haschips = false}
{
public:
	GetAxe(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);
	
	bool CheckProceduralPreconditions() const override;
	bool ExecuteAction() override;
	bool IsDone() override;
	bool RequiresInRange() const override;
	std::string GetName() const override;
};

class ChopLogs final : public Action	// Preconditions {HasIngredients = false} => Effects{HasIngredients}
{
public:
	ChopLogs(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);
	
	bool CheckProceduralPreconditions() const override;
	bool ExecuteAction() override;
	bool IsDone() override;
	bool RequiresInRange() const override;
	std::string GetName() const override;
};

class CollectBraches final : public Action // Preconditions {HasIngredients = true} => Effects{hungry = false, hasIngredients = false}
{
public:
	CollectBraches(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);
	
	bool CheckProceduralPreconditions() const override;
	bool ExecuteAction() override;
	bool IsDone() override;
	bool RequiresInRange() const override;
	std::string GetName() const override;
};
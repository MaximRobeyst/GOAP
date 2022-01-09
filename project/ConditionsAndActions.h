#pragma once
#include <map>
#include <vector>
#include <string>

class Blackboard;

class Action
{
public:
	Action() = default;
	Action(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);
	virtual ~Action() = default;
	
	virtual bool CheckProceduralPreconditions(Blackboard* pBlackboard) const = 0;	// Procedural conditions the character has to suffice for the action to execute
	virtual bool ExecuteAction(Blackboard* pBlackboard) = 0;						// The action that is executing
	virtual bool IsDone() = 0;								// is the action done executing
	virtual bool RequiresInRange() const = 0;				// does the action need to be in range if so the agent will go the to moveto state first and then execute

	std::map<std::string, bool> GetPreconditions();
	std::map<std::string, bool> GetEffects();
	float GetCost();
	
protected:
	std::map<std::string, bool> m_Preconditions;	// conditions the world has to be in for the action to be executed
	std::map<std::string, bool> m_Effects;			// the effect this action will have on the world
	float m_Cost{};								// cost of the action
	//bool m_InRange{};							// is the character in range for this action to be performed (COMMENTED: for now because i dont know if i'll need this)
};

class FleeFromEnemy final : public Action
{
public:
	FleeFromEnemy();
	FleeFromEnemy(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Blackboard* pBlackboard) const override;
	bool ExecuteAction(Blackboard* pBlackboard) override;
	bool IsDone() override;
	bool RequiresInRange() const override;
};

class ShootAtEnemy final :public Action
{
public:
	ShootAtEnemy();
	ShootAtEnemy(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Blackboard* pBlackboard) const override;
	bool ExecuteAction(Blackboard* pBlackboard) override;
	bool IsDone() override;
	bool RequiresInRange() const override;
};

class SearchForItems final :public Action
{
public:
	SearchForItems();
	SearchForItems(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Blackboard* pBlackboard) const override;
	bool ExecuteAction(Blackboard* pBlackboard) override;
	bool IsDone() override;
	bool RequiresInRange() const override;

private:
	float m_WanderAngle{};
};

class EnterHouse final : public Action
{
public:
	EnterHouse();
	EnterHouse(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost);

	bool CheckProceduralPreconditions(Blackboard* pBlackboard) const override;
	bool ExecuteAction(Blackboard* pBlackboard) override;
	bool IsDone() override;
	bool RequiresInRange() const override;

private:
	float m_WanderAngle{};
};
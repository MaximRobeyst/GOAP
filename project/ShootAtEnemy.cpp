#include "stdafx.h"
#include "ShootAtEnemy.h"
#include "Character.h"

#include <IExamInterface.h>

ShootAtEnemy::ShootAtEnemy()
{
	m_Preconditions["HasWeapon"] = true;
	//m_Preconditions["EnemyChasing"] = true;
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
	
	auto agentInfo = pCharacter->GetAgentInfo();
	auto steering = pCharacter->GetSteeringOutput();
	
	std::sort(entityList.begin(), entityList.end(), [agentInfo](const EntityInfo& lhs, const EntityInfo& rhs)
		{
			return Elite::DistanceSquared(agentInfo.Position, lhs.Location) < Elite::DistanceSquared(agentInfo.Position, rhs.Location);
		});
	
	EntityInfo info = *std::find_if(entityList.begin(), entityList.end(), [](const EntityInfo& info)
	{
			return info.Type == eEntityType::ENEMY;
	});

#ifdef _DEBUG

	std::for_each(entityList.begin(), entityList.end(), [agentInfo, pCharacter](const EntityInfo& info)
		{
			Elite::Vector2 dir{ Elite::Vector2{info.Location - pCharacter->GetAgentInfo().Position} };
			pCharacter->GetInterface()->Draw_Direction(
				pCharacter->GetAgentInfo().Position, dir, dir.Normalize(), Elite::Vector3{ 1,1,0 }, 0.9f);
		});
	Elite::Vector2 dir{ Elite::Vector2{info.Location - pCharacter->GetAgentInfo().Position} };
	
	pCharacter->GetInterface()->Draw_Direction(
		pCharacter->GetAgentInfo().Position, dir, dir.Normalize(), Elite::Vector3{ 1,0,0 }, 0.9f);
#endif
	
	steering.AutoOrient = false;
	steering.AngularVelocity = (Elite::GetOrientationFromVelocity(info.Location - agentInfo.Position) - agentInfo.Orientation);
	std::cout << "Angular Velocity" << steering.AngularVelocity << std::endl;

	const float offsetWithEnemy = Elite::GetOrientationFromVelocity(info.Location - agentInfo.Position) -agentInfo.Orientation;
	std::cout << "Offset with enemy" << abs(offsetWithEnemy)
				<< "Angle: " << Elite::GetOrientationFromVelocity(info.Location - agentInfo.Position)
				<< "Player Rotation: " << agentInfo.Orientation << std::endl;
	if(abs(offsetWithEnemy) < 0.08f)
	{
		const auto index = pCharacter->GetIndexForType(eItemType::PISTOL);
		ItemInfo itemInfo{};
		
		pCharacter->GetInterface()->Inventory_UseItem(index);
		std::cout << "SHOT!";
		
		pCharacter->GetInterface()->Inventory_GetItem(index, itemInfo);
		if(pCharacter->GetInterface()->Weapon_GetAmmo(itemInfo) <= 0)
		{
			pCharacter->RemoveItemFromInventory(index);
			pCharacter->ChangeCharacterState("HasWeapon", false);
			return false;
		}
	}

	pCharacter->SetSteeringOutput(steering);
	pCharacter->SetAgentInfo(agentInfo);
	
	return true;
}

bool ShootAtEnemy::IsDone(Character* pCharacter)
{
	return true;
}

bool ShootAtEnemy::RequiresInRange() const
{
	return false;
}

bool ShootAtEnemy::IsInRange(Character* pCharacter) const
{
	return true;
}

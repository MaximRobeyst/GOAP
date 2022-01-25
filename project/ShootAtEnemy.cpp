#include "stdafx.h"
#include "ShootAtEnemy.h"
#include "Character.h"

#include <IExamInterface.h>

#include "Helpers.h"

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
	
	std::sort(entityList.begin(), entityList.end(), IsCloser<EntityInfo>(agentInfo.Position));
	
	EntityInfo info = *std::find_if(entityList.begin(), entityList.end(), IsType<EntityInfo, eEntityType>(eEntityType::ENEMY));


	float personalSpace = pCharacter->GetAgentInfo().FOV_Range * 0.05f;
	float smallAngle = pCharacter->GetAgentInfo().FOV_Range * 0.333333f;
	float mediumAngle = pCharacter->GetAgentInfo().FOV_Range * 0.6666666f;
	// The big anlge is just the fov range
	
	steering.AutoOrient = false;
	steering.AngularVelocity = pCharacter->GetAgentInfo().MaxAngularSpeed;
	std::cout << "Angular Velocity" << steering.AngularVelocity << std::endl;

	const float offsetWithEnemy = Elite::GetOrientationFromVelocity(info.Location - agentInfo.Position) - agentInfo.Orientation;
	std::cout << "Offset with enemy" << abs(offsetWithEnemy)
		<< "Angle: " << Elite::GetOrientationFromVelocity(info.Location - agentInfo.Position)
		<< "Player Rotation: " << agentInfo.Orientation << std::endl;

	float maxAngleOffset;
	if (Elite::DistanceSquared(agentInfo.Position, info.Location) < smallAngle * smallAngle)
		maxAngleOffset = 0.2f;
	else if (Elite::DistanceSquared(agentInfo.Position, info.Location) < mediumAngle * mediumAngle)
		maxAngleOffset = 0.1f;
	else
		maxAngleOffset = 0.069f;

	
#ifdef _DEBUG

	std::for_each(entityList.begin(), entityList.end(), [agentInfo, pCharacter](const EntityInfo& info)
		{
			Elite::Vector2 dir{ Elite::Vector2{info.Location - pCharacter->GetAgentInfo().Position} };
			pCharacter->GetInterface()->Draw_Direction(
				pCharacter->GetAgentInfo().Position, dir, dir.Normalize(), Elite::Vector3{ 0,0,1 }, 0.9f);
		});
	Elite::Vector2 dir{ Elite::Vector2{info.Location - pCharacter->GetAgentInfo().Position} };
	
	pCharacter->GetInterface()->Draw_Direction(
		pCharacter->GetAgentInfo().Position, dir, dir.Normalize(), Elite::Vector3{ 0,1,0 }, 0.9f);

	pCharacter->GetInterface()->Draw_Circle(pCharacter->GetAgentInfo().Position, personalSpace, Elite::Vector3{ 1.f, 0.f, 0.f });
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
	
#endif
	if(abs(offsetWithEnemy) < maxAngleOffset)
	{
		const auto index = pCharacter->GetIndexForType(eItemType::PISTOL);
		ItemInfo itemInfo{};
		
		pCharacter->GetInterface()->Inventory_UseItem(index);
		std::cout << "SHOT!";
		
		pCharacter->GetInterface()->Inventory_GetItem(index, itemInfo);
		if(pCharacter->GetInterface()->Weapon_GetAmmo(itemInfo) <= 0)
		{
			pCharacter->RemoveItemFromInventory(index);
			
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

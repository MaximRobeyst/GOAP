#include "stdafx.h"
#include "ExitHouse.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

ExitHouse::ExitHouse()
{
	m_Preconditions["EnemyChasing"] = true;
	//m_Preconditions["HouseInFov"] = true;
	m_Preconditions["ItemInFov"] = false;
	m_Preconditions["InHouse"] = true;
	m_Preconditions["WasBitten"] = false;
	m_Preconditions["EnemyInFov"] = false;

	m_Effects["Survive"] = true;
	m_Effects["InHouse"] = false;
	m_Effects["HouseInFov"] = true;

	m_Cost = 10.0f;
}

bool ExitHouse::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool ExitHouse::ExecuteAction(float dt, Character* pCharacter)
{
	auto steering = pCharacter->GetSteeringOutput();

	if (steering.LinearVelocity.SqrtMagnitude() < 0.25f)
		m_WanderAngle += static_cast<float>(M_PI);
	
	m_WanderAngle += Elite::randomFloat(-Elite::ToRadians(30.f), Elite::ToRadians(30.f));
	const Elite::Vector2 target = pCharacter->GetAgentInfo().Position + pCharacter->GetAgentInfo().LinearVelocity.GetNormalized() * 6.f + (Elite::Vector2(cosf(m_WanderAngle) * 4.f, sinf(m_WanderAngle) * 4.f));

	Elite::Vector2 velocityWander{target - pCharacter->GetAgentInfo().Position};
	velocityWander.Normalize();
	velocityWander *= pCharacter->GetAgentInfo().MaxLinearSpeed;
	
	Elite::Vector2 velocitySeek{pCharacter->GetInterface()->NavMesh_GetClosestPathPoint(GetTarget(pCharacter)) - pCharacter->GetAgentInfo().Position};
	velocitySeek.Normalize();
	velocitySeek *= pCharacter->GetAgentInfo().MaxLinearSpeed;

	steering.AutoOrient = true;
	steering.LinearVelocity = (velocityWander * m_WanderWeight) + (velocitySeek * m_SeekWeight);
	pCharacter->SetSteeringOutput(steering);

	
	return true;
}

bool ExitHouse::IsDone(Character* pCharacter)
{
	return false;
}

bool ExitHouse::RequiresInRange() const
{
	return false;
}

bool ExitHouse::IsInRange(Character* pCharacter) const
{
	if (pCharacter->GetHousesInFOV().empty())
		return true;

	HouseInfo houseInfo = pCharacter->GetHousesInFOV()[0];


	if (pCharacter->GetAgentInfo().Position.x > houseInfo.Center.x - (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.x < houseInfo.Center.x + (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.y > houseInfo.Center.y - (houseInfo.Size.y / 2) &&
		pCharacter->GetAgentInfo().Position.y < houseInfo.Center.y + (houseInfo.Size.y / 2))
	{
		return false;
	}
	pCharacter->ChangeCharacterState("InHouse", false);
	return true;

}

Elite::Vector2 ExitHouse::GetTarget(Character* pCharacter)
{
	return Elite::Vector2{};
}

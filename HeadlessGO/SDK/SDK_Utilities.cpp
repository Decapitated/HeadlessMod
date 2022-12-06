#include "SDK_Utilities.h"
#include "math.h"

using namespace SDK_Utilities;

#define PI 3.1415926535897932384626433832795

const char* boneArray[22] = {
		"ValveBiped.Bip01_Pelvis",
			"ValveBiped.Bip01_Spine",
				"ValveBiped.Bip01_Spine1",
					"ValveBiped.Bip01_Spine2",
						"ValveBiped.Bip01_L_Clavicle",
							"ValveBiped.Bip01_L_UpperArm",
								"ValveBiped.Bip01_L_Forearm",
									"ValveBiped.Bip01_L_Hand",
						"ValveBiped.Bip01_R_Clavicle",
							"ValveBiped.Bip01_R_UpperArm",
								"ValveBiped.Bip01_R_Forearm",
									"ValveBiped.Bip01_R_Hand",
						"ValveBiped.Bip01_Neck1",
							"ValveBiped.Bip01_Head",
		"ValveBiped.Bip01_L_Thigh",
			"ValveBiped.Bip01_L_Calf",
				"ValveBiped.Bip01_L_Foot",
					"ValveBiped.Bip01_L_Toe0",
		"ValveBiped.Bip01_R_Thigh",
			"ValveBiped.Bip01_R_Calf",
				"ValveBiped.Bip01_R_Foot",
				"ValveBiped.Bip01_R_Toe0"
};

const char** SDK_Utilities::getBoneArray()
{
	return boneArray;
}

IClientEntity* SDK_Utilities::GetLocalPlayer() {
	return (IClientEntity*)SourceInterfaces::pEntityList->
		GetClientEntity(SourceInterfaces::pEngine->GetLocalPlayer());
}

bool SDK_Utilities::IsValid(IClientEntity* entity, int* classID, bool npc)
{
	if (entity == nullptr)
		return false;
	int _classID = entity->GetClientClass()->m_ClassID;
	if (_classID != ClassIDs::CGMOD_Player && _classID != ClassIDs::CAI_BaseNPC)
		return false;
	if (!npc && _classID == ClassIDs::CAI_BaseNPC)
		return false;
	if (entity->GetHealth() <= 0 || entity->GetHealth() >= 999999999)
		return false;
	if(classID != nullptr)
		*classID = _classID;
	return true;
}

IClientEntity* SDK_Utilities::GrabClosestEnt()
{
	float oldDifference = 99999999.0f;
	float currDiff = 0.0f;
	short int bestEnt = 0;

	for (int i = 0; i < SourceInterfaces::pEntityList->GetHighestEntityIndex(); i++)
	{
		IClientEntity* pCurrentEnt = (IClientEntity*)SourceInterfaces::pEntityList->GetClientEntity(i);

		if (!IsValid(pCurrentEnt))
			continue;

		currDiff = SDKMath::GetDistanceBetween(GetLocalPlayer()->GetOrigin(), pCurrentEnt->GetOrigin());

		if (currDiff < oldDifference)
		{
			bestEnt = i;
			oldDifference = currDiff;
		}
	}
	return (IClientEntity*)SourceInterfaces::pEntityList->GetClientEntity(bestEnt);
}

IClientEntity* SDK_Utilities::GrabClosestEntToCrosshair(Vector &outTargetPos)
{

	RECT ScreenSize = SDKMath::GetViewport();
	Vector crosshair(ScreenSize.right / 2.f, ScreenSize.bottom / 2.f, 0);

	IClientEntity* target{};
	float targetDist = 999999.0;

	int maxEntities = SourceInterfaces::pEntityList->GetHighestEntityIndex();
	for (int i = 0; i <= maxEntities; i++)
	{
		IClientEntity* currentEntity = (IClientEntity*)SourceInterfaces::pEntityList->GetClientEntity(i);
		if (i == SourceInterfaces::pEngine->GetLocalPlayer() || !IsValid(currentEntity, NULL, true))
			continue;

		int targetHead = GrabBone(currentEntity, boneArray[13]);
		Vector targetPos = (targetHead == -1 ? currentEntity->GetOrigin() : GetBonePos(currentEntity, targetHead));
		Vector targetScreen;
		if (SDKMath::WorldToScreen(targetPos, targetScreen))
		{
			float distToCross = crosshair.Dist(targetScreen);
			if (distToCross < targetDist)
			{
				target = currentEntity;
				targetDist = distToCross;
				outTargetPos = targetPos;
			}
		}
	}
	return target;
}

IClientEntity* SDK_Utilities::GrabClosestEntToCrosshair(CUserCmd* cmd, QAngle& outAngle)
{
	IClientEntity* target{};
	float targetDist = 9999.0;

	int maxEntities = SourceInterfaces::pEntityList->GetHighestEntityIndex();
	for (int i = 0; i <= maxEntities; i++)
	{
		IClientEntity* currentEntity = (IClientEntity*)SourceInterfaces::pEntityList->GetClientEntity(i);
		if (i == SourceInterfaces::pEngine->GetLocalPlayer() || !IsValid(currentEntity, NULL, true))
			continue;

		int targetHead = GrabBone(currentEntity, boneArray[13]);
		Vector targetPos = (targetHead == -1 ? currentEntity->GetOrigin() : GetBonePos(currentEntity, targetHead));

		Vector temp;
		if (SDKMath::WorldToScreen(targetPos, temp))
		{
			IClientEntity* localPlayer = GetLocalPlayer();
			int localHead = GrabBone(localPlayer, boneArray[13]);
			Vector localPos = (localHead == -1 ? localPlayer->GetOrigin() : GetBonePos(localPlayer, localHead));

			QAngle tempAngle = Math::GetAngle(localPos, targetPos);
			Vector aimAngle(tempAngle.x, tempAngle.y, tempAngle.z);
			if (GetVisible(currentEntity))
			{
				
				Vector viewAngle = Vector(
					cmd->m_viewangles.x,
					cmd->m_viewangles.y, 
					0);
				float distToCross = viewAngle.Dist(aimAngle);
				if (distToCross < targetDist)
				{
					target = currentEntity;
					targetDist = distToCross;
					outAngle = tempAngle;
				}
			}
		}
	}
	return target;
}

int SDK_Utilities::GrabBone(IClientEntity* ent, const char* boneName)
{
	int aimbone = 6;
	matrix3x4_t pMatrix[128];

	ent->SetupBones(pMatrix, 128, 0x00000100, 0.0f);

	auto pStudioModel = SourceInterfaces::pModelInfo->GetStudioModel(ent->GetModel());

	if (!pStudioModel)
		return -1;

	auto set = pStudioModel->pHitboxSet(0);

	if (!set)
		return -1;

	for (int c = 0; c < set->numhitboxes; c++)
	{
		auto pHitBox = set->pHitbox(c);

		if (!pHitBox)
			return -1;

		auto cHitBoxName = pStudioModel->pBone(pHitBox->bone)->pszName();

		// use strstr because its fastest algorithim for comparing strings !!!!!!!
		if (boneName && strstr(cHitBoxName, boneName))
		{
			aimbone = pHitBox->bone;
		}
	}

	return aimbone;
}

Vector SDK_Utilities::GetBonePos(IClientEntity* ent, int boneIndex)
{
	matrix3x4_t bonePos[128];
	Vector vHeadPos;
	ent->SetupBones(bonePos, 128, 0x00000100, 0.0f);

	matrix3x4_t Hitbox = bonePos[boneIndex];

	vHeadPos.x = Hitbox[0][3];
	vHeadPos.y = Hitbox[1][3];
	vHeadPos.z = Hitbox[2][3];

	return vHeadPos;
}

void SDK_Utilities::TraceRay(Vector start, Vector end, trace_t &trace, CTraceFilter* filter)
{
	Ray_t ray;
	ray.Init(start, end);

	SourceInterfaces::pEngineTrace->TraceRay(ray, MASK_SOLID, filter, &trace);
}

bool SDK_Utilities::GetVisible(IClientEntity* ent)
{
	IClientEntity* localPlayer = GetLocalPlayer();
	int localHead = GrabBone(localPlayer, getBoneArray()[13]);
	Vector localPos = (localHead == -1 ? localPlayer->GetOrigin() : GetBonePos(localPlayer, localHead));

	int targetHead = SDK_Utilities::GrabBone(ent, getBoneArray()[13]);
	Vector targetPos = (targetHead == -1 ? ent->GetOrigin() : GetBonePos(ent, targetHead));

	trace_t trace;
	CTraceFilter filter(localPlayer);

	TraceRay(localPos, targetPos, trace, &filter);

	return (trace.m_pEnt == ent);
}

namespace SDK_Utilities
{
	namespace Math
	{

		bool WorldToScreen(Vector in, Vector& out)
		{
			const matrix3x4_t w2sm = SourceInterfaces::pEngine->WorldToScreenMatrix();
			float w = w2sm[3][0] * in.x + w2sm[3][1] * in.y + w2sm[3][2] * in.z + w2sm[3][3];

			if (w > 0.001f)
			{
				int width, height; SourceInterfaces::pEngine->GetScreenSize(width, height);

				float fl1DBw = 1 / w;
				out.x = (width / 2) + (0.5f * ((w2sm[0][0] * in.x + w2sm[0][1] * in.y + w2sm[0][2] * in.z + w2sm[0][3]) * fl1DBw) * width + 0.5f);
				out.y = (height / 2) - (0.5f * ((w2sm[1][0] * in.x + w2sm[1][1] * in.y + w2sm[1][2] * in.z + w2sm[1][3]) * fl1DBw) * height + 0.5f);
				return true;
			}
			return false;
		}

		float AngleNormalize(float angle)
		{
			angle = fmodf(angle, 360.0f);
			if (angle > 180)
			{
				angle -= 360;
			}
			if (angle < -180)
			{
				angle += 360;
			}
			return angle;
		}
		float AngleNormalizePositive(float angle)
		{
			angle = fmodf(angle, 360.0f);

			if (angle < 0.0f)
			{
				angle += 360.0f;
			}

			return angle;
		}
		float AngleDifference(float destAngle, float srcAngle)
		{
			float delta;

			delta = fmodf(destAngle - srcAngle, 360.0f);
			if (destAngle > srcAngle)
			{
				if (delta >= 180)
					delta -= 360;
			}
			else
			{
				if (delta <= -180)
					delta += 360;
			}
			return delta;
		}
		QAngle FixAngles(QAngle angles)
		{
			angles.x = AngleNormalize(angles.x);
			angles.y = AngleNormalize(angles.y);
			if (angles.x > 89.f)
				angles.x = 89.f;
			else if (angles.x < -89.f)
				angles.x = -89.f;
			return angles;
		}
		QAngle FixAnglePositive(QAngle angles)
		{
			angles.x = AngleNormalizePositive(angles.x);
			angles.y = AngleNormalizePositive(angles.y);
			angles.z = AngleNormalizePositive(angles.z);
			return angles;
		}
		QAngle GetAngle(Vector source, Vector target)
		{
			Vector normalized = target - source;
			QAngle angle;
			angle.x = (-atan2f(normalized.z, sqrtf(powf(normalized.y, 2) + powf(normalized.x, 2))) * (180.0f / PI)); // pitch
			angle.y = (atan2f(normalized.y, normalized.x)) * (180.0f / PI); // yaw
			return angle;
		}
		float DEG2RAD(float degrees)
		{
			return degrees * 4.0 * atan(1.0) / 180.0;
		}
		Vector Forward(QAngle ang)
		{
			ang = FixAngles(ang);
			float radx = DEG2RAD(ang.x);
			float rady = DEG2RAD(ang.y);

			return Vector(cos(radx) * cos(rady), cos(radx) * sin(rady), sin(radx));
		}
		float NormalizeValue(float min, float max, float value)
		{
			float normalized = (value - min) / (max - min);
			if (normalized < 0) return 0;
			if (normalized > 1) return 1;
			return normalized;
		}
	}
}
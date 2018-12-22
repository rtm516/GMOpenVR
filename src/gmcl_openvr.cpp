#include <stdio.h>
#include <math.h>
#include <float.h>
#include "GarrysMod/Lua/Interface.h"
#include "openvr.h"

int version = 1; //every release this will be incremented, in lua, the user will be warned to update the dll if they have a lua version ahead of the module.

using namespace GarrysMod::Lua;
using namespace vr;

IVRSystem *vr_pointer;
HmdMatrix34_t HmdMatrix;

uint32_t m_nRenderWidth;
uint32_t m_nRenderHeight;

void luaPrint(GarrysMod::Lua::ILuaBase *LUA, const char *printStr)
{
	std::string text = "[OpenVR] ";
	text += printStr;

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "print");
		LUA->PushString(text.c_str());
		LUA->Call(1, 0);
	LUA->Pop();
}

LUA_FUNCTION(GetVersion)
{
	LUA->PushNumber(version);
	return 1;
}

LUA_FUNCTION(IsHmdPresent)
{
	LUA->PushBool(VR_IsHmdPresent());
	return 1;
}

LUA_FUNCTION(IsRuntimeInstalled)
{
	LUA->PushBool(VR_IsRuntimeInstalled());
	return 1;
}

LUA_FUNCTION(InitVR)
{
	luaPrint(LUA, "Initialising VR");

	EVRInitError eError = VRInitError_None;
	vr_pointer = VR_Init(&eError, VRApplication_Scene);
	if (eError != VRInitError_None)
	{
		luaPrint(LUA, "Error initialising: " + eError);

		LUA->PushBool(false);
		return 1;
	}
	vr_pointer->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);
	LUA->PushBool(true);
	return 1;
}

void ProcessTracking() {
	for (TrackedDeviceIndex_t unDevice = 0; unDevice < k_unMaxTrackedDeviceCount; unDevice++)
	{
		TrackedDevicePose_t trackedDevicePose;
		TrackedDevicePose_t *devicePose = &trackedDevicePose;

		ETrackedDeviceClass trackedDeviceClass = VRSystem()->GetTrackedDeviceClass(unDevice);
		if (trackedDeviceClass == ETrackedDeviceClass::TrackedDeviceClass_HMD)
		{
			VRSystem()->GetDeviceToAbsoluteTrackingPose(TrackingUniverseStanding, 0, &trackedDevicePose, 1);
			HmdMatrix = devicePose->mDeviceToAbsoluteTracking;
		}
	}
}

LUA_FUNCTION(UpdateTracking)
{
	//luaPrint(LUA, "Updating tracking positions");
	if (!vr_pointer)
	{
		luaPrint(LUA, "Error updating tracking positions: Invalid vr_pointer");
		LUA->PushBool(false);
		return 1;
	}

	ProcessTracking();

	LUA->PushBool(true);
	return 1;
}

LUA_FUNCTION(CountDevices)
{
	LUA->PushNumber(vr::k_unMaxTrackedDeviceCount);
	return 1;
}

LUA_FUNCTION(Render)
{
	LUA->CheckType(1, Type::TEXTURE);
	LUA->CheckType(2, Type::TEXTURE);

	LUA->PushBool(false);
	return 1;
}

int ResolveDeviceType( int deviceId )
{

	if (!vr_pointer)
	{
		return -1;
	}

	ETrackedDeviceClass deviceClass = VRSystem()->GetTrackedDeviceClass(deviceId);

	return static_cast<int>(deviceClass);
}

int ResolveDeviceRole(int deviceId)
{

	if (!vr_pointer)
	{
		return -1;
	}

	int deviceRole = VRSystem()->GetInt32TrackedDeviceProperty(deviceId, ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);
	
	return static_cast<int>(deviceRole);
}


LUA_FUNCTION(GetDevicePose)
{
	(LUA->CheckType(1, Type::NUMBER));
	int deviceId = static_cast<int>(LUA->GetNumber(1));
	LUA->PushBool(false);
	return 1;
}

LUA_FUNCTION(GetDeviceClass)
{
	(LUA->CheckType(1, Type::NUMBER));
	int deviceId = static_cast<int>(LUA->GetNumber(1));
	int type = ResolveDeviceType(deviceId);
	LUA->PushNumber(type);
	return 1;
}

LUA_FUNCTION(GetDeviceRole)
{
	(LUA->CheckType(1, Type::NUMBER));
	int deviceId = static_cast<int>(LUA->GetNumber(1));
	int type = ResolveDeviceRole(deviceId);
	LUA->PushNumber(type);
	return 1;
}

LUA_FUNCTION(GetHeadsetVector)
{
	if (!vr_pointer)
	{
		LUA->PushBool(false);
		return 1;
	}

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "Vector");
		LUA->PushNumber(HmdMatrix.m[0][3]);
		LUA->PushNumber(HmdMatrix.m[1][3]);
		LUA->PushNumber(HmdMatrix.m[2][3]);
	LUA->Call(3, 1);
	LUA->Remove(-2);
	return 1;
}

LUA_FUNCTION(GetHeadsetAngle)
{
	HmdQuaternion_t q;
	if (!vr_pointer)
	{
		LUA->PushBool(false);
		return 1;
	}

	q.x = sqrt(fmax(0, 1 + HmdMatrix.m[0][0] - HmdMatrix.m[1][1] - HmdMatrix.m[2][2])) / 2;
	q.y = sqrt(fmax(0, 1 - HmdMatrix.m[0][0] + HmdMatrix.m[1][1] - HmdMatrix.m[2][2])) / 2;
	q.z = sqrt(fmax(0, 1 - HmdMatrix.m[0][0] - HmdMatrix.m[1][1] + HmdMatrix.m[2][2])) / 2;
	q.x = copysign(q.x, (double)HmdMatrix.m[2][1] - (double)HmdMatrix.m[1][2]);
	q.y = copysign(q.y, (double)HmdMatrix.m[0][2] - (double)HmdMatrix.m[2][0]);
	q.z = copysign(q.z, (double)HmdMatrix.m[1][0] - (double)HmdMatrix.m[0][1]);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "Angle");
		LUA->PushNumber(q.x);
		LUA->PushNumber(q.y);
		LUA->PushNumber(q.z);
	LUA->Call(3, 1);
	LUA->Remove(-2);
	return 1;
}


GMOD_MODULE_OPEN()
{
	luaPrint(LUA, "Loading...");
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			LUA->PushCFunction(GetVersion); LUA->SetField(-2, "GetVersion");

			LUA->PushCFunction(IsHmdPresent); LUA->SetField(-2, "IsHmdPresent");
			LUA->PushCFunction(IsRuntimeInstalled); LUA->SetField(-2, "IsRuntimeInstalled");

			LUA->PushCFunction(InitVR); LUA->SetField(-2, "InitVR");
			LUA->PushCFunction(UpdateTracking); LUA->SetField(-2, "UpdateTracking");
			LUA->PushCFunction(CountDevices); LUA->SetField(-2, "CountDevices");
			LUA->PushCFunction(Render); LUA->SetField(-2, "Render");

			LUA->PushCFunction(GetDeviceClass); LUA->SetField(-2, "GetDeviceClass");
			LUA->PushCFunction(GetDeviceRole); LUA->SetField(-2, "GetDeviceRole");
			LUA->PushCFunction(GetHeadsetVector); LUA->SetField(-2, "GetHeadsetVector");
			LUA->PushCFunction(GetHeadsetAngle); LUA->SetField(-2, "GetHeadsetAngle");
		LUA->SetField(-2, "OpenVR");
	LUA->Pop();
	luaPrint(LUA, "Loaded!");
	return 0;
}

void Shutdown()
{
	if (vr_pointer != NULL)
	{
		VR_Shutdown();
		vr_pointer = NULL;
	}
}
 
GMOD_MODULE_CLOSE()
{
	Shutdown();
    return 0;
}
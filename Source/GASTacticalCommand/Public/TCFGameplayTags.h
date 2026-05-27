//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "NativeGameplayTags.h"

namespace TCFGameplayTags
{
	// Squad State
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_State_Suppressed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_State_Pinned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_State_Fortified);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_State_Retreating);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_State_Capturing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_State_Suppressing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_State_Moving);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_State_Stationary);

	// Squad Role
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_Role_Rifle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_Role_Engineer);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Squad_Role_Heavy);

	// Doctrine
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Doctrine_Offensive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Doctrine_Defensive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Doctrine_Utility);

	// Order Type
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Type_BasicAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Type_SuppressiveFire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Type_Rally);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Type_FortifyPosition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Type_TacticalRetreat);
	
	// Order Cooldown
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Cooldown_BasicAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Cooldown_Rally);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Cooldown_SuppressiveFire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Cooldown_FortifyPosition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Cooldown_TacticalRetreat);

	// Order Blocking
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Blocked_Suppressed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Blocked_Pinned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Blocked_Retreating);

	// Order Requirements
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Requires_Stationary);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Requires_Cohesion);

	// Terrain
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Terrain_Cover_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Terrain_Cover_Heavy);

	// Order Result
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Success);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_InvalidSource);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_NoAuthority);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_MissingDefinition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_MissingRequirements);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_BlockedByTags);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_InvalidTarget);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_OutOfRange);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_MissingAbility);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_ActivationFailed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_Cooldown);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Order_Result_Failed_Cost);
}

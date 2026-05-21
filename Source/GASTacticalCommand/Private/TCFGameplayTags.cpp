//Copyright Kyle Cuss and Cuss Programming 2026.

#include "TCFGameplayTags.h"

namespace TCFGameplayTags
{
	// Squad State
	UE_DEFINE_GAMEPLAY_TAG(Squad_State_Suppressed, "Squad.State.Suppressed");
	UE_DEFINE_GAMEPLAY_TAG(Squad_State_Pinned, "Squad.State.Pinned");
	UE_DEFINE_GAMEPLAY_TAG(Squad_State_Fortified, "Squad.State.Fortified");
	UE_DEFINE_GAMEPLAY_TAG(Squad_State_Retreating, "Squad.State.Retreating");
	UE_DEFINE_GAMEPLAY_TAG(Squad_State_Capturing, "Squad.State.Capturing");
	UE_DEFINE_GAMEPLAY_TAG(Squad_State_Suppressing, "Squad.State.Suppressing");
	UE_DEFINE_GAMEPLAY_TAG(Squad_State_Moving, "Squad.State.Moving");
	UE_DEFINE_GAMEPLAY_TAG(Squad_State_Stationary, "Squad.State.Stationary");

	// Squad Role
	UE_DEFINE_GAMEPLAY_TAG(Squad_Role_Rifle, "Squad.Role.Rifle");
	UE_DEFINE_GAMEPLAY_TAG(Squad_Role_Engineer, "Squad.Role.Engineer");
	UE_DEFINE_GAMEPLAY_TAG(Squad_Role_Heavy, "Squad.Role.Heavy");

	// Doctrine
	UE_DEFINE_GAMEPLAY_TAG(Doctrine_Offensive, "Doctrine.Offensive");
	UE_DEFINE_GAMEPLAY_TAG(Doctrine_Defensive, "Doctrine.Defensive");
	UE_DEFINE_GAMEPLAY_TAG(Doctrine_Utility, "Doctrine.Utility");

	// Order Type
	UE_DEFINE_GAMEPLAY_TAG(Order_Type_SuppressiveFire, "Order.Type.SuppressiveFire");
	UE_DEFINE_GAMEPLAY_TAG(Order_Type_Rally, "Order.Type.Rally");
	UE_DEFINE_GAMEPLAY_TAG(Order_Type_FortifyPosition, "Order.Type.FortifyPosition");
	UE_DEFINE_GAMEPLAY_TAG(Order_Type_TacticalRetreat, "Order.Type.TacticalRetreat");
	
	// Order Cooldown
	UE_DEFINE_GAMEPLAY_TAG(Order_Cooldown_Rally, "Order.Cooldown.Rally");
	UE_DEFINE_GAMEPLAY_TAG(Order_Cooldown_SuppressiveFire, "Order.Cooldown.SuppressiveFire");
	UE_DEFINE_GAMEPLAY_TAG(Order_Cooldown_FortifyPosition, "Order.Cooldown.FortifyPosition");
	UE_DEFINE_GAMEPLAY_TAG(Order_Cooldown_TacticalRetreat, "Order.Cooldown.TacticalRetreat");

	// Order Blocking
	UE_DEFINE_GAMEPLAY_TAG(Order_Blocked_Suppressed, "Order.Blocked.Suppressed");
	UE_DEFINE_GAMEPLAY_TAG(Order_Blocked_Pinned, "Order.Blocked.Pinned");
	UE_DEFINE_GAMEPLAY_TAG(Order_Blocked_Retreating, "Order.Blocked.Retreating");

	// Order Requirements
	UE_DEFINE_GAMEPLAY_TAG(Order_Requires_Stationary, "Order.Requires.Stationary");
	UE_DEFINE_GAMEPLAY_TAG(Order_Requires_Cohesion, "Order.Requires.Cohesion");

	// Terrain
	UE_DEFINE_GAMEPLAY_TAG(Terrain_Cover_Light, "Terrain.Cover.Light");
	UE_DEFINE_GAMEPLAY_TAG(Terrain_Cover_Heavy, "Terrain.Cover.Heavy");

	// Order Result
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Success, "Order.Result.Success");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_InvalidSource, "Order.Result.Failed.InvalidSource");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_NoAuthority, "Order.Result.Failed.NoAuthority");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_MissingDefinition, "Order.Result.Failed.MissingDefinition");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_MissingRequirements, "Order.Result.Failed.MissingRequirements");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_BlockedByTags, "Order.Result.Failed.BlockedByTags");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_InvalidTarget, "Order.Result.Failed.InvalidTarget");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_OutOfRange, "Order.Result.Failed.OutOfRange");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_MissingAbility, "Order.Result.Failed.MissingAbility");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_ActivationFailed, "Order.Result.Failed.ActivationFailed");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_Cooldown, "Order.Result.Failed.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(Order_Result_Failed_Cost, "Order.Result.Failed.Cost");
}
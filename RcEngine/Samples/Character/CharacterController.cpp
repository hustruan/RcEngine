#include "CharacterController.h"
#include <Input/InputSystem.h>

CharacterController::CharacterController(void)
	: mMovingBack(false)
{
}


CharacterController::~CharacterController(void)
{
}

void CharacterController::SetupAnimation()
{

}

void CharacterController::UpdateSmoothedMovementDirection()
{
	InputSystem& inputSys = InputSystem::GetSingleton();
	bool grounded = IsGrounded();

	// Forward vector relative to the camera along the x-z plane	
	float3 forward;
	forward.Y() = 0;
	forward = Normalize(forward);

	// Right vector relative to the camera, always orthogonal to the forward vector
	float3 right = float3(forward.Z(), 0, -forward.X());
	
	float3 keybordDirection(0.0f, 0.0f, 0.0f);
	if (inputSys.KeyDown(KC_W))	keybordDirection.Z() += 1.0f;
	if (inputSys.KeyDown(KC_S))	keybordDirection.Z() -= 1.0f;
	if (inputSys.KeyDown(KC_A))	keybordDirection.X() -= 1.0f;
	if (inputSys.KeyDown(KC_D))	keybordDirection.X() += 1.0f;

	// Are we moving backwards or looking backwards
	mMovingBack = keybordDirection.Z() < 0.1f;

	bool wasMovding = mIsMoving;
	mIsMoving = LengthSquared(keybordDirection) > 0.1f;

	// Target direction relative to the camera
	float3 targetDirection = keybordDirection.X() * right + keybordDirection.Z() * forward;
}

bool CharacterController::IsGrounded() const
{
	return false;
}

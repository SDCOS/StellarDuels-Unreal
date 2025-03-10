// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "PlayerPawn.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class FIGHTINGGAME_API APlayerPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// This is the actual mesh that we choose, so the mannequin
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* PlayerMesh;

	// Spring Arm Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	// Spring Arm will attach to the camera, offsetting the camera and allowing it to "orbit" the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera");
	UCameraComponent* Camera;

	//takes our enhanced input mapppings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> IA_Sprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> IA_Crouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> Forward_Movement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> Backward_Movement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> Left_Movement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> Right_Movement;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Turn;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_LookUp;


	//animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* JumpFromStand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* StandtoCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* CrouchIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* CrouchToStand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* Stand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* WalkForward;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* WalkBackward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* WalkLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* WalkRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* CrouchWalkForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* CrouchWalkBackward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* CrouchWalkLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* CrouchWalkRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* Sprint;

	UPROPERTY(EditAnywhere, Category = "Jumping")
	float JumpForce = 800.0f; // Jump height

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 500.0f; // Default walking speed

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 1000.0f; // Sprinting speed


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartJump();
	void StopJump();
	void MoveLeft();
	void MoveRight();
	void MoveForward();
	void MoveBackward();
	void LookUp(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void StartCrouch();
	void StopCrouch();
	void StartSprint();
	void StopSprint();
	void StopMoving();
	//virtual void Landed(const FHitResult& Hit) override;
	void PlayCrouchIdle();
	bool bIsCrouching = false;
	bool bIsMoving = false;
	float CrouchHoldThreshold = 0.2f; // Time (seconds) to determine tap vs hold
	FTimerHandle CrouchTimerHandle;
	float CrouchStartTime = 0.0f; // Time when crouch button was pressed
	bool bIsJumping = false;
	bool bCanDoubleJump = true;
	bool bIsSprinting = false; // Track if the player is sprinting
};

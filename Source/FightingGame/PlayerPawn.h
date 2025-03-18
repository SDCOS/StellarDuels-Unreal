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
//#include "Components/WidgetComponent.h"
//#include "HealthBarWidget.h"

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

	UPROPERTY(Replicated, EditAnywhere, Category = "Jumping")
	float JumpForce = 800.0f; // Jump height

	UPROPERTY(Replicated, EditAnywhere, Category = "Movement")
	float WalkSpeed = 500.0f; // Default walking speed

	UPROPERTY(Replicated, EditAnywhere, Category = "Movement")
	float SprintSpeed = 1000.0f; // Sprinting speed

	UPROPERTY(Replicated, EditAnywhere, Category = "Movement")
	float MoveSpeed = 200.0f;

	UPROPERTY(Replicated)
	bool bIsCrouching = false;

	UPROPERTY(Replicated)
	bool bIsMoving = false;

	UPROPERTY(Replicated)
	float CrouchHoldThreshold = 0.2f; // Time (seconds) to determine tap vs hold

	FTimerHandle CrouchTimerHandle;

	UPROPERTY(Replicated)
	float CrouchStartTime = 0.0f; // Time when crouch button was pressed

	UPROPERTY(Replicated)
	bool bIsJumping = false;

	UPROPERTY(Replicated)
	bool bCanDoubleJump = true;

	UPROPERTY(Replicated)
	bool bIsSprinting = false; // Track if the player is sprinting

	// // Health values
	// UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Health")
	// float Health = 100.f;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	// float MaxHealth = 100.f;

	// // Health bar widget component (attached to the pawn)
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	// UWidgetComponent* HealthBarComponent;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void StartJump();
	void StartJump_Local();
	void StopJump();
	void MoveLeft();
	void MoveLeft_Local();
	void MoveRight();
	void MoveRight_Local();
	void MoveForward();
	void MoveForward_Local();
	void MoveBackward();
	void MoveBackward_Local();
	void LookUp(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void StartCrouch();
	void StartCrouch_Local();
	void StopCrouch();
	void StopCrouch_Local();
	void StartSprint();
	void StartSprint_Local();
	void StopSprint();
	void StopSprint_Local();
	void StopMoving();
	void StopMoving_Local();
	//virtual void Landed(const FHitResult& Hit) override;
	void PlayCrouchIdle();
	//void TakeDamage(float DamageAmount);

	UFUNCTION(Server, Reliable)
	void Server_StartJump();

	UFUNCTION(Server, Reliable)
	void Server_StopJump();

	UFUNCTION(Server, Reliable)
	void Server_StartSprint();

	UFUNCTION(Server, Reliable)
	void Server_StopSprint();

	UFUNCTION(Server, Reliable)
	void Server_MoveLeft();

	UFUNCTION(Server, Reliable)
	void Server_MoveRight();

	UFUNCTION(Server, Reliable)
	void Server_MoveForward();

	UFUNCTION(Server, Reliable)
	void Server_MoveBackward();

	UFUNCTION(Server, Reliable)
	void Server_StartCrouch();

	UFUNCTION(Server, Reliable)
	void Server_StopCrouch();

	UFUNCTION(Server, Reliable)
	void Server_StopMoving();

};

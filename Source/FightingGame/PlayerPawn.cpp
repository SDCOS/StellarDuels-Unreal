// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"

//NOTE: If you need a visual for this, make the blueprint version of this class in the editor. It will show you what everything (including the capsule) looks like

//FIXME: Make individual input components for WASD instead of combining them. Run the standing animation in BeginPlay.
// Add animation logic in the movement functions (including which one to play when going diagonal)
// Fix the jumping animation

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  Is important for frame-by-frame operations (like calculating fps)
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the Skeletal Mesh Component
	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(RootComponent);

	FRotator InitialRotation(0.f, -90.f, 0.f);
	FVector InitialLocation(0, 0, -60);
	PlayerMesh->SetRelativeLocation(InitialLocation);
	PlayerMesh->SetRelativeRotation(InitialRotation);
	
	GetCharacterMovement()->UpdatedComponent = RootComponent;
	GetCharacterMovement()->GravityScale = 1.5f; // Gravity Control
	GetCharacterMovement()->MaxWalkSpeed = 600.0f; // walking speed
	GetCharacterMovement()->JumpZVelocity = 600.f; // jump velocity
	GetCharacterMovement()->AirControl = 0.2f;     // Control in the air
	PlayerMesh->SetSimulatePhysics(false); // Disable physics simulation for controlled movement
	PlayerMesh->SetEnableGravity(true);   // Enable gravity

	// Set the Skeletal Mesh using its path in the content browser (right click asset then click "copy reference")
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (MeshAsset.Succeeded())
	{
		PlayerMesh->SetSkeletalMesh(MeshAsset.Object);
	}

	// Camera offset from character
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(PlayerMesh); // Attach to the cube
	SpringArmComponent->TargetArmLength = 380.0f; // Distance from the cube
	SpringArmComponent->bUsePawnControlRotation = true; // Rotate arm based on controller input
	SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f));

	// Create a Camera Component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArmComponent); // Attach camera to spring arm

	// Load the Input Mapping Context
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Finder(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Enhanced_Input/IMC_default.IMC_default'"));
	if (IMC_Finder.Succeeded())
	{
		IMC_Default = IMC_Finder.Object;
	}

	// Load the input actions
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Jump_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/IA_Jump.IA_Jump'"));
	if (IA_Jump_Finder.Succeeded())
	{
		IA_Jump = IA_Jump_Finder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> AD_Movement_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/WASD_movement.WASD_movement'"));
	if (AD_Movement_Finder.Succeeded())
	{
		AD_Movement = AD_Movement_Finder.Object;
	}

	//Load JumpFromStand
	JumpFromStand = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Jump_From_Stand.Jump_From_Stand'"));


}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	//allows us to control the skeletal mesh (?) might not need this ****
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->Possess(this);
	}

	//for inputs
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetWorld()->GetFirstLocalPlayerFromController());
	Subsystem->AddMappingContext(IMC_Default, 0);

	//start stand animation here
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to EnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind jumping actions
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &APlayerPawn::StartJump);
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Completed, this, &APlayerPawn::StopJump);
		EnhancedInput->BindAction(AD_Movement, ETriggerEvent::Triggered, this, &APlayerPawn::Move_Sideways);
	}

}

void APlayerPawn::StartJump()
{
	PlayerMesh->PlayAnimation(JumpFromStand, false); // this setup does not work as intended :o - also there should be some logic that chooses if we should play JumpFromStand or some other jumping animation
	Jump();
}

void APlayerPawn::StopJump()
{
	StopJumping(); // Call the built-in StopJumping() function
}

void APlayerPawn::Move_Sideways(const FInputActionValue& Value)
{
	// Ensure Value is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		// Get the character's forward and right vectors
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, ControlRotation.Yaw, 0);

		// Right/Left direction
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

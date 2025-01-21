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

//FIXME: Jump animation is wack, also add running animations

// Sets default values
APlayerPawn::APlayerPawn()
{

	UE_LOG(LogTemp, Warning, TEXT("Pawn Constructor"));

 	// Set this pawn to call Tick() every frame.  Is important for frame-by-frame operations (like calculating fps)
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the Skeletal Mesh Component
	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(RootComponent);

	FRotator InitialRotation(0.f, -90.f, 0.f);
	FVector InitialLocation(0, 0, -82);
	PlayerMesh->SetRelativeLocation(InitialLocation);
	PlayerMesh->SetRelativeRotation(InitialRotation);
	
	GetCharacterMovement()->UpdatedComponent = RootComponent;
	GetCharacterMovement()->GravityScale = 1.5f; // Gravity Control
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f; // walking speed
	GetCharacterMovement()->JumpZVelocity = 1000.f; // jump velocity
	GetCharacterMovement()->AirControl = 0.6f;     // Control in the air
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
	else {
		UE_LOG(LogTemp, Error, TEXT("IMC_default not found"));
	}

	// Load the input actions
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Jump_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/IA_Jump.IA_Jump'"));
	if (IA_Jump_Finder.Succeeded())
	{
		IA_Jump = IA_Jump_Finder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Forward_Movement_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/Move_Forward.Move_Forward'"));
	if (Forward_Movement_Finder.Succeeded())
	{
		Forward_Movement = Forward_Movement_Finder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Backward_Movement_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/Move_Backward.Move_Backward'"));
	if (Backward_Movement_Finder.Succeeded())
	{
		Backward_Movement = Backward_Movement_Finder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Left_Movement_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/Move_Left.Move_Left'"));
	if (Left_Movement_Finder.Succeeded())
	{
		Left_Movement = Left_Movement_Finder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Right_Movement_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/Move_Right.Move_Right'"));
	if (Right_Movement_Finder.Succeeded())
	{
		Right_Movement = Right_Movement_Finder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Turn_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/IA_Turn.IA_Turn'"));
	if (IA_Turn_Finder.Succeeded())
	{
		IA_Turn = IA_Turn_Finder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LookUp_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/IA_Pan_Up.IA_Pan_Up'"));
	if (IA_LookUp_Finder.Succeeded())
	{
		IA_LookUp = IA_LookUp_Finder.Object;
	}

	//Load animations
	JumpFromStand = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Jump_From_Stand.Jump_From_Stand'"));
	Stand = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Equip_Rifle_Standing.Equip_Rifle_Standing'"));

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	//allows us to control the skeletal mesh (?) might not need this ****
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->Possess(this);
		PC->SetInputMode(FInputModeGameOnly()); // this is super important! Allows people to move when they create a server
		PC->bShowMouseCursor = false;
		UE_LOG(LogTemp, Warning, TEXT("Player Possessed"));
	}

	//for inputs
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetWorld()->GetFirstLocalPlayerFromController());
	Subsystem->AddMappingContext(IMC_Default, 0);

	//start stand animation here
	PlayerMesh->PlayAnimation(Stand, false);
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

	UE_LOG(LogTemp, Warning, TEXT("Setup Input Called"));

	// Cast to EnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("Binding Actions"));
		// Bind jumping actions
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &APlayerPawn::StartJump);
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Completed, this, &APlayerPawn::StopJump);
		EnhancedInput->BindAction(Forward_Movement, ETriggerEvent::Triggered, this, &APlayerPawn::MoveForward);
		EnhancedInput->BindAction(Backward_Movement, ETriggerEvent::Triggered, this, &APlayerPawn::MoveBackward);
		EnhancedInput->BindAction(Left_Movement, ETriggerEvent::Triggered, this, &APlayerPawn::MoveLeft);
		EnhancedInput->BindAction(Right_Movement, ETriggerEvent::Triggered, this, &APlayerPawn::MoveRight);
		EnhancedInput->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &APlayerPawn::Turn);
		EnhancedInput->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &APlayerPawn::LookUp);
	}

}

void APlayerPawn::StartJump()
{
	UE_LOG(LogTemp, Warning, TEXT("Jump"));
	PlayerMesh->PlayAnimation(JumpFromStand, false); // this setup does not work as intended :o - also there should be some logic that chooses if we should play JumpFromStand or some other jumping animation
	Jump();
}

void APlayerPawn::StopJump()
{
	StopJumping(); // Call the built-in StopJumping() function
}

void APlayerPawn::MoveForward()
{
	UE_LOG(LogTemp, Warning, TEXT("move forward"));
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, 1); //2nd param is a speed multiplier from -1 to 1
}

void APlayerPawn::MoveBackward()
{
	UE_LOG(LogTemp, Warning, TEXT("move backward"));
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, -1);
}

void APlayerPawn::MoveLeft()
{
	UE_LOG(LogTemp, Warning, TEXT("left"));
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Use the Y-axis for left/right movement
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, -1);
}

void APlayerPawn::MoveRight()
{
	UE_LOG(LogTemp, Warning, TEXT("right"));
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Use the Y-axis for left/right movement
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, 1);
}

void APlayerPawn::Turn(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("mousex"));
	float TurnValue = Value.Get<float>();
	AddControllerYawInput(TurnValue/3);
}

void APlayerPawn::LookUp(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("mousey"));
	float LookUpValue = Value.Get<float>();
	AddControllerPitchInput(-LookUpValue/3);
}

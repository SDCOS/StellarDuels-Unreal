// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Public/Projectile.h"
#include "HUD_Crosshair.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"



//NOTE: If you need a visual for this, make the blueprint version of this class in the editor. It will show you what everything (including the capsule) looks like
//FIXME: Loop up and down animations
//FIXME: replicate shooting
//FIXME: Jump animation is wack, also add running animations
//FIXME: Replication (all functions including animations) - - - Jump animation not replicating for some reasion - - actually it looks like it isnt bc you need to hold spacebar for the full animation to play
//FIXME: In StopMove - make a better transition animation to stand
//FIXME: For situations where, say, a and w are pressed at once: integrate that logic into existing move functions by setting a bools,
//for instance, "is moving right" or "is moving left" to see if we need to go diagonal. also add animations and transition animations for this

// Sets default values
APlayerPawn::APlayerPawn()
{
	bReplicates = true;
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
	GetCharacterMovement()->MaxWalkSpeed = 500.0f; // walking speed
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

	// Assign the C++ class to the TSubclassOf variable
	ProjectileClass = AProjectile::StaticClass();

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
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Sprint_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/IA_Sprint.IA_Sprint'"));
	if (IA_Sprint_Finder.Succeeded())
	{
		IA_Sprint = IA_Sprint_Finder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Crouch_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/IA_Crouch.IA_Crouch'"));
	if (IA_Jump_Finder.Succeeded())
	{
		IA_Crouch = IA_Crouch_Finder.Object;
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

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Shoot_Finder(TEXT("/Script/EnhancedInput.InputAction'/Game/Enhanced_Input/IA_Shoot.IA_Shoot'"));
	if (IA_Shoot_Finder.Succeeded())
	{
		IA_Shoot = IA_Shoot_Finder.Object;
	}

	//Load animations
	JumpFromStand = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Jump_From_Stand.Jump_From_Stand'"));
	Stand = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Equip_Rifle_Standing.Equip_Rifle_Standing'"));
	Idle = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Idle_Rifle_Hip.Idle_Rifle_Hip'"));
	StandtoCrouch = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Stand_to_Crouch_Rifle_Hip.Stand_to_Crouch_Rifle_Hip'"));
	CrouchToStand = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Crouch_to_Stand_Rifle_Hip.Crouch_to_Stand_Rifle_Hip'"));
	CrouchIdle = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Crouch_Idle_Rifle_Hip.Crouch_Idle_Rifle_Hip'"));
	WalkForward = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Walk_Fwd_Rifle_Ironsights.Walk_Fwd_Rifle_Ironsights'"));
	WalkBackward = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Walk_Bwd_Rifle_Ironsights.Walk_Bwd_Rifle_Ironsights'"));
	WalkLeft = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Walk_Lt_Rifle_Ironsights.Walk_Lt_Rifle_Ironsights'"));
	WalkRight = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Walk_Rt_Rifle_Ironsights.Walk_Rt_Rifle_Ironsights'"));
	CrouchWalkForward = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Crouch_Walk_Fwd_Rifle_Ironsights.Crouch_Walk_Fwd_Rifle_Ironsights'"));
	CrouchWalkBackward = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Crouch_Walk_Bwd_Rifle_Ironsights.Crouch_Walk_Bwd_Rifle_Ironsights'"));
	CrouchWalkLeft = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Crouch_Walk_Lt_Rifle_Ironsights.Crouch_Walk_Lt_Rifle_Ironsights'"));
	CrouchWalkRight = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Crouch_Walk_Rt_Rifle_Ironsights.Crouch_Walk_Rt_Rifle_Ironsights'"));
	Sprint = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/AnimStarterPack/Sprint_Fwd_Rifle.Sprint_Fwd_Rifle'"));
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	//allows us to control the skeletal mesh (?) might not need this ****
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (HasAuthority()) {
			PC->Possess(this);
		}
		PC->SetInputMode(FInputModeGameOnly()); // this is super important! Allows people to move when they create a server
		PC->bShowMouseCursor = false;
		UE_LOG(LogTemp, Warning, TEXT("Player Possessed"));
	}

	//for inputs
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetWorld()->GetFirstLocalPlayerFromController());
	Subsystem->AddMappingContext(IMC_Default, 0);

	//start stand animation here
	PlayerMesh->PlayAnimation(Stand, false);

	if (IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("[Host] Pawn is locally controlled"));
	}
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//are we off the map?
	TimeSinceLastCheck += DeltaTime;
	if (TimeSinceLastCheck >= 0.5f)
	{
		FVector Location = GetActorLocation();
		if (Location.Z < -2500.f)
		{
			Respawn();
			Server_ModifyPlayerDeaths();
		}
		TimeSinceLastCheck = 0.f;
	}

}

void APlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerPawn, PlayerKills);
	DOREPLIFETIME(APlayerPawn, PlayerDeaths);
	DOREPLIFETIME(APlayerPawn, Health);
}

void APlayerPawn::OnRep_Health()
{
	//ui
	if (IsLocallyControlled()) {
		SetPlayerHealthHUD(Health);
	}
}

void APlayerPawn::OnRep_PlayerKills()
{
	if (IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Updating HUD with new kill count"));
		SetPlayerKillsHUD(PlayerKills);
	}
}


void APlayerPawn::OnRep_PlayerDeaths()
{
	//ui
	if (IsLocallyControlled())
	{
		SetPlayerDeathsHUD(PlayerDeaths);
	}
}

void APlayerPawn::Respawn() {
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->PlayerState)
	{
		FString PlayerName = PC->PlayerState->GetPlayerName();

		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

		for (AActor* Start : PlayerStarts)
		{

			if ((PlayerName == "PlayerA" && Start->ActorHasTag("PlayerA")) ||
				(PlayerName == "PlayerB" && Start->ActorHasTag("PlayerB")))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Resetting actor location and rotation"));
				SetActorLocation(Start->GetActorLocation());
				SetActorRotation(Start->GetActorRotation());
				break;
			}
		}
	}
}

void APlayerPawn::Server_ModifyHealth_Implementation(float Damage)
{
	if (HasAuthority())
	{
		Health -= Damage;

		if (IsLocallyControlled()) {
			SetPlayerHealthHUD(Health);
		}

		if (Health <= 0.0f)
		{
			Respawn();
		}

		// OnRep_Health will handle syncing the health back to clients
		// ui here as well bc client acts as server
	}
}


void APlayerPawn::Server_ModifyPlayerKills_Implementation(APlayerPawn* EnemyPlayer)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("server player kills"));
	if (HasAuthority()) // Always make sure the server has authority before changing replicated variables.
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("server player kills - has authority"));
		EnemyPlayer->PlayerKills++;
		// Only needed for listen server host:
		if (EnemyPlayer->IsLocallyControlled()) // the listen server's player
		{
			EnemyPlayer->SetPlayerKillsHUD(EnemyPlayer->PlayerKills);
		}
		// This will be replicated to all clients
		// The `OnRep_Health()` function will be triggered on clients.
		// ui here as well bc client acts as server
	}
}

void APlayerPawn::Server_ModifyPlayerDeaths_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("server player deaths"));
	if (HasAuthority()) // Always make sure the server has authority before changing replicated variables.
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("server player deaths - has authority"));
		PlayerDeaths += 1;
		if (IsLocallyControlled()) {
			SetPlayerDeathsHUD(PlayerDeaths);
		}

		// This will be replicated to all clients
		// The `OnRep_Health()` function will be triggered on clients.
		// ui here as well bc client acts as server
	}
}

void APlayerPawn::SetPlayerKillsHUD(int32 kill_num) {
	APlayerController* PC = Cast<APlayerController>(GetController());
	AHUD_Crosshair* HUD = Cast<AHUD_Crosshair>(PC->GetHUD());
	if (HUD)
	{
		HUD->SetKillCount(kill_num);
	}
}

void APlayerPawn::SetPlayerDeathsHUD(int32 death_num) {
	APlayerController* PC = Cast<APlayerController>(GetController());
	AHUD_Crosshair* HUD = Cast<AHUD_Crosshair>(PC->GetHUD());
	if (HUD)
	{
		HUD->SetDeathCount(death_num);
	}
}

void APlayerPawn::SetPlayerHealthHUD(int32 health_num) {
	APlayerController* PC = Cast<APlayerController>(GetController());
	AHUD_Crosshair* HUD = Cast<AHUD_Crosshair>(PC->GetHUD());
	if (HUD)
	{
		HUD->SetHealthCount(health_num);
	}
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
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &APlayerPawn::StartJump);
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Completed, this, &APlayerPawn::StopJump);
		EnhancedInput->BindAction(Forward_Movement, ETriggerEvent::Triggered, this, &APlayerPawn::MoveForward);
		EnhancedInput->BindAction(Forward_Movement, ETriggerEvent::Completed, this, &APlayerPawn::StopMoving);
		EnhancedInput->BindAction(Backward_Movement, ETriggerEvent::Triggered, this, &APlayerPawn::MoveBackward);
		EnhancedInput->BindAction(Backward_Movement, ETriggerEvent::Completed, this, &APlayerPawn::StopMoving);
		EnhancedInput->BindAction(Left_Movement, ETriggerEvent::Triggered, this, &APlayerPawn::MoveLeft);
		EnhancedInput->BindAction(Left_Movement, ETriggerEvent::Completed, this, &APlayerPawn::StopMoving);
		EnhancedInput->BindAction(Right_Movement, ETriggerEvent::Triggered, this, &APlayerPawn::MoveRight);
		EnhancedInput->BindAction(Right_Movement, ETriggerEvent::Completed, this, &APlayerPawn::StopMoving);
		EnhancedInput->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &APlayerPawn::Turn);
		EnhancedInput->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &APlayerPawn::LookUp);
		EnhancedInput->BindAction(IA_Crouch, ETriggerEvent::Triggered, this, &APlayerPawn::StartCrouch);
		EnhancedInput->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &APlayerPawn::StopCrouch);
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Triggered, this, &APlayerPawn::StartSprint);
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &APlayerPawn::StopSprint);
		EnhancedInput->BindAction(IA_Shoot, ETriggerEvent::Started, this, &APlayerPawn::StartShoot);
		EnhancedInput->BindAction(IA_Shoot, ETriggerEvent::Completed, this, &APlayerPawn::StopShoot);
	}

}

void APlayerPawn::Multicast_PlayAnimationLooping_Implementation(UAnimSequence* Anim)
{
	if (!IsLocallyControlled()) // Prevents double-playing on the local client
	{
		PlayerMesh->PlayAnimation(Anim, true);
	}
}

void APlayerPawn::Multicast_PlayAnimationNonLooping_Implementation(UAnimSequence* Anim)
{
	if (!IsLocallyControlled()) // Prevents double-playing on the local client
	{
		PlayerMesh->PlayAnimation(Anim, true);
	}
}

float APlayerPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Call the base class - this will reduce health and apply general damage effects
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	AActor* OwnerActor = DamageCauser->GetOwner();
	APlayerPawn* EnemyPlayer = Cast<APlayerPawn>(OwnerActor);

	if (ActualDamage > 0.f)
	{
		// Apply damage to health
		Server_ModifyHealth(ActualDamage);

		// Check if dead
		if (Health <= 0.f) //RESET HEALTH AND KILLS/DEATH over the server * * * * * * 
		{
			Server_ModifyHealth(-100);
			Server_ModifyPlayerDeaths();
			Server_ModifyPlayerKills(EnemyPlayer);
		}
	}

	return ActualDamage;
}


void APlayerPawn::StartSprint()
{
	if (HasAuthority()) {
		StartSprint_Local();
	}
	else {
		StartSprint_Local();
		Server_StartSprint();
	}
}

void APlayerPawn::StartSprint_Local()
{
	if (!bIsSprinting && bIsMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("Started Sprinting"));
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed; // Increase speed
		bIsSprinting = true;

		if (Sprint)
		{
			PlayerMesh->PlayAnimation(Sprint, true);
			Multicast_PlayAnimationLooping(Sprint);
		}
	}
}

void APlayerPawn::Server_StartSprint_Implementation()
{
	StartSprint_Local();
}

void APlayerPawn::StopSprint()
{
	if (HasAuthority()) {
		StopSprint_Local();
	}
	else {
		StopSprint_Local();
		Server_StopSprint();
	}
}

void APlayerPawn::StopSprint_Local()
{
	if (bIsSprinting)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stopped Sprinting"));
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Reset speed
		bIsSprinting = false;

		if (WalkForward)
		{
			if (bIsMoving) {
				PlayerMesh->PlayAnimation(WalkForward, true);
				Multicast_PlayAnimationLooping(WalkForward);
			}
			else {
				PlayerMesh->PlayAnimation(Idle, true);
				Multicast_PlayAnimationLooping(Idle);
			}
		}
	}
}

void APlayerPawn::Server_StopSprint_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Stopped Sprinting"));
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // Reset speed
	bIsSprinting = false;

	if (WalkForward)
	{
		if (bIsMoving) PlayerMesh->PlayAnimation(WalkForward, true);
		else PlayerMesh->PlayAnimation(Idle, true);
	}
}

void APlayerPawn::StartJump()
{
	//Multicast_PlayAnimationNonLooping(JumpFromStand);
	if (HasAuthority()) {
		StartJump_Local();
		if (!GetCharacterMovement()->IsMovingOnGround()) {
			bCanDoubleJump = false;
		}
		else {
			if (GetCharacterMovement()->IsMovingOnGround() && !bCanDoubleJump) {
				bCanDoubleJump = true;
			}
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("StartJump -> doesn't have authority"));
		StartJump_Local();
		Server_StartJump();
	}
}

void APlayerPawn::StartJump_Local() { //does not check for authority, only will be used locally for the client and will not change variables replicated to the server. This removes lag on the end of the client when they jump
	if (GetCharacterMovement()->IsMovingOnGround()) // First jump - check if on the ground
	{
		UE_LOG(LogTemp, Warning, TEXT("Jumping from ground"));
		LaunchCharacter(FVector(0, 0, JumpForce), false, true);
		//bIsJumping = true;
		//bCanDoubleJump = true; // Enable double jump
		// Play correct animation
		if (JumpFromStand)
		{
			PlayerMesh->PlayAnimation(JumpFromStand, false);
			Multicast_PlayAnimationNonLooping(JumpFromStand);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("JumpFromStand is a nullptr"));
		}
	}
	else if (bCanDoubleJump) // Double Jump
	{
		UE_LOG(LogTemp, Warning, TEXT("Double Jump!"));
		LaunchCharacter(FVector(0, 0, JumpForce * 1.0f), false, true);
		if (JumpFromStand)
		{
			UE_LOG(LogTemp, Warning, TEXT("Action jump perform"));
			PlayerMesh->PlayAnimation(JumpFromStand, false);
			Multicast_PlayAnimationNonLooping(JumpFromStand);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("JumpFromStand is a nullptr"));
		}
		//bCanDoubleJump = false;
	}
}

//void APlayerPawn::Landed(const FHitResult& Hit)
//{
//	//Super::Landed(Hit);
//
//	UE_LOG(LogTemp, Warning, TEXT("Landed! Reset jump."));
//	bIsJumping = false;
//	bCanDoubleJump = true; // Restore double jump ability
//}

void APlayerPawn::StopJump() {
	//bIsJumping = false;
	//bCanDoubleJump = true; // Restore double jump ability
	if (HasAuthority()) {
		if (bIsMoving) {
			PlayerMesh->PlayAnimation(WalkForward, false);
			Multicast_PlayAnimationNonLooping(WalkForward);
		}
		else {
			PlayerMesh->PlayAnimation(Idle, false);
			Multicast_PlayAnimationNonLooping(Idle); //Loop here? check loops and non loops in the future
		}
	}
	else {
		Server_StopJump();
	}
}

void APlayerPawn::Server_StartJump_Implementation()
{
	StartJump();
}

void APlayerPawn::Server_StopJump_Implementation()
{
	StopJump();
}


void APlayerPawn::StartCrouch()
{
	if (HasAuthority()) {
		StartCrouch_Local();
	}
	else {
		StartCrouch_Local();
		Server_StartCrouch();
	}
}

void APlayerPawn::StartCrouch_Local()
{
	CrouchStartTime = GetWorld()->GetTimeSeconds();
	if (!bIsCrouching) {
		UE_LOG(LogTemp, Warning, TEXT("Crouching"));
		GetCharacterMovement()->MaxWalkSpeed = 200.0f; //Reduce the speed
		bIsCrouching = true;
		if (StandtoCrouch)
		{
			PlayerMesh->PlayAnimation(StandtoCrouch, false);
			Multicast_PlayAnimationNonLooping(StandtoCrouch);
			if (bIsMoving) {
				PlayerMesh->PlayAnimation(CrouchWalkForward, true);
				Multicast_PlayAnimationLooping(CrouchWalkForward);
			}
		}

		GetWorld()->GetTimerManager().SetTimer(CrouchTimerHandle, this, &APlayerPawn::PlayCrouchIdle, StandtoCrouch->GetPlayLength(), false);
	}
}

void APlayerPawn::Server_StartCrouch_Implementation()
{
	StartCrouch_Local();
}

void APlayerPawn::PlayCrouchIdle() //////////////////////if statement may screw stuff up
{
	if (bIsCrouching && CrouchIdle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Crouch Idle"));
		if (!bIsMoving) {
			PlayerMesh->PlayAnimation(CrouchIdle, false);
			Multicast_PlayAnimationNonLooping(CrouchIdle);

		}
		else {
			PlayerMesh->PlayAnimation(CrouchWalkForward, true);
			Multicast_PlayAnimationLooping(CrouchWalkForward);
		}
	}
}

void APlayerPawn::StopCrouch()
{
	if (HasAuthority()) {
		StopCrouch_Local();
	}
	else {
		StopCrouch_Local();
		Server_StopCrouch();
	}
}

void APlayerPawn::Server_StopCrouch_Implementation()
{
	float HeldTime = GetWorld()->GetTimeSeconds() - CrouchStartTime;
	GetWorld()->GetTimerManager().ClearTimer(CrouchTimerHandle);
	UE_LOG(LogTemp, Warning, TEXT("Standing up"));
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	bIsCrouching = false;
	PlayerMesh->PlayAnimation(CrouchToStand, false);
	if (bIsMoving) PlayerMesh->PlayAnimation(WalkForward, true);
}

void APlayerPawn::StopCrouch_Local()
{
	float HeldTime = GetWorld()->GetTimeSeconds() - CrouchStartTime;
	GetWorld()->GetTimerManager().ClearTimer(CrouchTimerHandle);
	if (bIsCrouching)
	{
		UE_LOG(LogTemp, Warning, TEXT("Standing up"));
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
		bIsCrouching = false;
		PlayerMesh->PlayAnimation(CrouchToStand, false);
		Multicast_PlayAnimationNonLooping(CrouchToStand);
		if (bIsMoving) {
			PlayerMesh->PlayAnimation(WalkForward, true);
			Multicast_PlayAnimationLooping(WalkForward);
		}
		else {
			PlayerMesh->PlayAnimation(Idle, true);
			Multicast_PlayAnimationLooping(WalkForward);
		}
	}
}

void APlayerPawn::MoveForward()
{
	if (HasAuthority()) {
		MoveForward_Local();
	}
	else {
		MoveForward_Local();
		Server_MoveForward();
	}

}

void APlayerPawn::MoveForward_Local()
{
	UE_LOG(LogTemp, Warning, TEXT("move forward"));
	if (!bIsMoving)
	{
		bIsMoving = true;
		if (WalkForward)
		{
			if (!bIsCrouching) {
				PlayerMesh->PlayAnimation(WalkForward, true);
				Multicast_PlayAnimationLooping(WalkForward);
			}
			else {
				PlayerMesh->PlayAnimation(CrouchWalkForward, true);
				Multicast_PlayAnimationLooping(CrouchWalkForward);
			}
		}
	}
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, 1); //2nd param is a speed multiplier from -1 to 1

}

void APlayerPawn::Server_MoveForward_Implementation()
{
	MoveForward_Local();
}

void APlayerPawn::StopMoving()
{
	if (HasAuthority()) {
		StopMoving_Local();
	}
	else {
		StopMoving_Local();
		Server_StopMoving();
	}
}

void APlayerPawn::StopMoving_Local()
{
	if (bIsMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("stopped moving"));

		bIsMoving = false;

		if (Stand)
		{
			if (!bIsCrouching) {
				PlayerMesh->PlayAnimation(Idle, true); // Loop idle animation
				Multicast_PlayAnimationLooping(Idle);
			}
			else {
				PlayerMesh->PlayAnimation(CrouchIdle, true);
				Multicast_PlayAnimationLooping(CrouchIdle);
			}
		}
	}
}

void APlayerPawn::Server_StopMoving_Implementation()
{
	StopMoving_Local();
}

void APlayerPawn::MoveBackward()
{
	if (HasAuthority()) {
		MoveBackward_Local();
	}
	else {
		MoveBackward_Local();
		Server_MoveBackward();
	}
}

void APlayerPawn::MoveBackward_Local()
{
	UE_LOG(LogTemp, Warning, TEXT("move backward"));
	if (!bIsMoving)
	{
		bIsMoving = true;
		if (WalkBackward)
		{
			if (!bIsCrouching) {
				PlayerMesh->PlayAnimation(WalkBackward, true);
				Multicast_PlayAnimationLooping(WalkBackward);
			}
			else {
				PlayerMesh->PlayAnimation(CrouchWalkBackward, true);
				Multicast_PlayAnimationLooping(CrouchWalkBackward);
			}
		}
	}
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, -1);
}

void APlayerPawn::Server_MoveBackward_Implementation()
{
	MoveBackward_Local();
}

void APlayerPawn::MoveLeft()
{
	if (HasAuthority()) {
		MoveLeft_Local();
	}
	else {
		MoveLeft_Local();
		Server_MoveLeft();
	}
}

void APlayerPawn::MoveLeft_Local()
{
	UE_LOG(LogTemp, Warning, TEXT("left"));

	if (!bIsMoving)
	{
		bIsMoving = true;
		if (WalkForward)
		{
			if (!bIsCrouching) {
				PlayerMesh->PlayAnimation(WalkLeft, true);
				Multicast_PlayAnimationLooping(WalkLeft);
			}
			else {
				PlayerMesh->PlayAnimation(CrouchWalkLeft, true);
				Multicast_PlayAnimationLooping(CrouchWalkLeft);
			}
		}
	}
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Use the Y-axis for left/right movement
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, -1);
}

void APlayerPawn::Server_MoveLeft_Implementation()
{
	MoveLeft_Local();
}

void APlayerPawn::MoveRight()
{
	if (HasAuthority()) {
		MoveRight_Local();
	}
	else {
		MoveRight_Local();
		Server_MoveRight();
	}
}

void APlayerPawn::Server_MoveRight_Implementation()
{
	MoveRight_Local();
}

void APlayerPawn::MoveRight_Local()
{
	UE_LOG(LogTemp, Warning, TEXT("right"));
	if (!bIsMoving)
	{
		bIsMoving = true;
		if (WalkForward)
		{
			if (!bIsCrouching) {
				PlayerMesh->PlayAnimation(WalkRight, true);
				Multicast_PlayAnimationLooping(WalkRight);
			}
			else {
				PlayerMesh->PlayAnimation(CrouchWalkRight, true);
				Multicast_PlayAnimationLooping(CrouchWalkRight);
			}
		}
	}
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
	AddControllerYawInput(TurnValue * MouseSensitivityYaw);
}

void APlayerPawn::LookUp(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("mousey"));
	float LookUpValue = Value.Get<float>();
	AddControllerPitchInput(LookUpValue * MouseSensitivityPitch);
}

FVector APlayerPawn::AimingAt(FVector CameraLocation, FRotator CameraRotation) {
	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * 10000.f); // trace far into the distance

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(FireTrace), true, this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	FVector TargetPoint = bHit ? HitResult.ImpactPoint : TraceEnd;
	return TargetPoint;
}

void APlayerPawn::StartShoot()
{
	// Start shooting immediately, then repeat every FireRate seconds
	if (HasAuthority()) {
		Shoot();
		GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &APlayerPawn::Shoot, FireRate, true);
	}
	else {
		//Shoot locally first
		Shoot();
		GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &APlayerPawn::Shoot, FireRate, true);
		GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &APlayerPawn::Server_Shoot, FireRate, true);
	}
}

void APlayerPawn::Shoot()
{

	if (ProjectileClass)
	{
		//Get the camera location & forward direction
		FVector CameraLocation;
		FRotator CameraRotation;
		GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector TargetPoint = AimingAt(CameraLocation, CameraRotation);

		//Adjust spawn location slightly in front of the camera
		float SpawnDistance = 430.0f; // 1 meter ahead
		FVector MuzzleLocation = CameraLocation + CameraRotation.Vector() * SpawnDistance;
		MuzzleLocation.Z -= 45;

		FVector ShootDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();
		FRotator SpawnRotation = ShootDirection.Rotation();
		//Set up spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Cast<APawn>(this);

		//Spawn the projectile
		APlayerController* PC = Cast<APlayerController>(GetController());

		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, SpawnRotation, SpawnParams);
		
		if (Projectile)
		{
			Projectile->SetOwner(this);

			/* FIX ME:  velocity of projectile += player's velocity */ 

			//Projectile->ProjectileMovementComponent->Velocity = MuzzleLocation;
		}
		//else {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("projectile does NOT exist!"));
		//}

	}
}



void APlayerPawn::StopShoot()
{
	GetWorldTimerManager().ClearTimer(ShootTimerHandle);
}

void APlayerPawn::Server_Shoot_Implementation() {
	Shoot();
}


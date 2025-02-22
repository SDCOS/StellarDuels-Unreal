// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/TutorialTarget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATutorialTarget::ATutorialTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SetActorTickEnabled(true);
    SetActorEnableCollision(true);

    SetLifeSpan(0); // Prevent auto-destruction
    bReplicates = true;  // Ensure it can be spawned in networked games
    bAlwaysRelevant = true;

    //Set it to tick every frame
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    //Create mesh for block
    AgentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AgentMesh"));
    RootComponent = AgentMesh;

    //Set a default mesh for agent: block 
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BlockMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
    if (BlockMesh.Succeeded()) {
        UE_LOG(LogTemp, Warning, TEXT("Found target"));
        AgentMesh->SetStaticMesh(BlockMesh.Object);  // Set the static mesh to the cube shape
        AgentMesh->SetRelativeScale3D(FVector(1.0f));  // Adjust the scale if necessary
    }
}

// Called when the game starts or when spawned
void ATutorialTarget::BeginPlay()
{
	Super::BeginPlay();

    StartLocation = GetActorLocation();
    TargetLocation = StartLocation + FVector(0, 0, MoveDistance);
	
    UE_LOG(LogTemp, Warning, TEXT("ATutorialTarget BeginPlay Called"));

    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("No Authority - Skipping Spawn"));
        return; // Prevent multiple spawns in multiplayer
    }

    if (IsPlacedInLevel())
    {
        UE_LOG(LogTemp, Warning, TEXT("ATutorialTarget was manually placed in the level."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ATutorialTarget was spawned dynamically, spawning another agent..."));
        SpawnTutorialAgent();
    }
}

//Tutorial Target Spawning 
void ATutorialTarget::SpawnTutorialAgent() {
    UE_LOG(LogTemp, Warning, TEXT("SpawnTutorialAgent called"));

    UWorld* World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode BeginPlay: GetWorld() returned nullptr"));
        return;
    }

    FVector SpawnLocation = FVector(20, 20, 500);
    FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ATutorialTarget* SpawnedTarget = World->SpawnActor<ATutorialTarget>(ATutorialTarget::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

    if (SpawnedTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("ATutorialTarget manually spawned from GameMode!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn ATutorialTarget from GameMode."));
    }
}


// Corrected function to check if the actor was placed in the level
bool ATutorialTarget::IsPlacedInLevel()
{
    bool bIsInLevel = !this->GetOwner(); // If there's no owner, it was placed in the level.

    if (bIsInLevel)
    {
        UE_LOG(LogTemp, Warning, TEXT("Target is placed in the level."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Target was spawned dynamically."));
    }

    return bIsInLevel;
}

// Called every frame
void ATutorialTarget::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Ensure StartLocation is set
    if (StartLocation.IsNearlyZero())
    {
        StartLocation = GetActorLocation();
    }

    // Calculate oscillating position
    FVector CurrentLocation = StartLocation;
    CurrentLocation.Y += MoveDistance * FMath::Sin(GetWorld()->GetTimeSeconds() * MoveSpeed);

    // Apply new location
    bool bSuccess = SetActorLocation(CurrentLocation, false);
}

void ATutorialTarget::Destroyed()
{
    Super::Destroyed();
    UE_LOG(LogTemp, Error, TEXT("ATutorialTarget is being destroyed before BeginPlay!"));
}


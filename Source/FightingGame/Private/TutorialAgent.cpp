// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/TutorialAgent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Character.h"

//Sets default values
ATutorialAgent::ATutorialAgent()
{   
    SetActorTickEnabled(true);
    SetActorEnableCollision(true);

    SetLifeSpan(0); // Prevent auto-destruction
    bReplicates = true;  // Ensure it can be spawned in networked games
    bAlwaysRelevant = true;

    //Set it to tick every frame
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // Set Root Component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Create and attach mesh
    AgentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AgentMesh"));
    AgentMesh->SetupAttachment(RootComponent);

    // Load the default Skeletal Mesh
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin"));
    if (MeshAsset.Succeeded()) {
        UE_LOG(LogTemp, Warning, TEXT("Found agent mesh"));
        //Load player mesh
        AgentMesh->SetSkeletalMesh(MeshAsset.Object);

        //Scale
        AgentMesh->SetRelativeScale3D(FVector(1.0f));
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Could not find agent mesh or load Idle Pose! Check asset path."));
    }
}

// Called when the game starts or when spawned
void ATutorialAgent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("ATutorialAgent BeginPlay Called"));

    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("No Authority - Skipping Spawn"));
        return; // Prevent multiple spawns in multiplayer
    }

    if (IsPlacedInLevel())
    {
        UE_LOG(LogTemp, Warning, TEXT("ATutorialAgent was manually placed in the level."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ATutorialAgent was spawned dynamically, spawning another agent..."));
        SpawnTutorialAgent();
    }
}

// Corrected function to check if the actor was placed in the level
bool ATutorialAgent::IsPlacedInLevel()
{
    bool bIsInLevel = !this->GetOwner(); // If there's no owner, it was placed in the level.

    if (bIsInLevel)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor is placed in the level."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor was spawned dynamically."));
    }

    return bIsInLevel;
}

// Called every frame
void ATutorialAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATutorialAgent::SpawnTutorialAgent() {
    UE_LOG(LogTemp, Warning, TEXT("SpawnTutorialAgent called"));

    UWorld* World = GetWorld();
    if (World == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode BeginPlay: GetWorld() returned nullptr"));
        return;
    }

    FVector SpawnLocation = FVector(10, 10, 10);
    FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ATutorialAgent* SpawnedAgent = World->SpawnActor<ATutorialAgent>(ATutorialAgent::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

    if (SpawnedAgent)
    {
        UE_LOG(LogTemp, Warning, TEXT("ATutorialAgent manually spawned from GameMode!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn ATutorialAgent from GameMode."));
    }
}

void ATutorialAgent::Destroyed()
{
    Super::Destroyed();
    UE_LOG(LogTemp, Error, TEXT("ATutorialAgent is being destroyed before BeginPlay!"));
}


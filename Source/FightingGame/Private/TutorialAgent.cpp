// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialAgent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

//Sets default values
ATutorialAgent::ATutorialAgent()
{
    //Set it to tick every frame
    PrimaryActorTick.bCanEverTick = true;

    //Create mesh for block
    AgentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AgentMesh"));
    RootComponent = AgentMesh;  

    //Set a default mesh for agent: block 
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BlockMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
    if (BlockMesh.Succeeded()) {
        UE_LOG(LogTemp, Error, TEXT("Found block"));
        AgentMesh->SetStaticMesh(BlockMesh.Object);  // Set the static mesh to the cube shape
        AgentMesh->SetRelativeScale3D(FVector(1.0f));  // Adjust the scale if necessary
    }
}

// Called when the game starts or when spawned
void ATutorialAgent::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("Begin Play is called"));

    //Check if the level is loaded and then spawn
    if (UGameplayStatics::GetCurrentLevelName(this) == "TutorialMap") {
        UE_LOG(LogTemp, Warning, TEXT("Begin Play checks map good"));
        SpawnTutorialAgent();
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Begin play changes map"));
        //Open the TutorialMap level first
        UGameplayStatics::OpenLevel(GetWorld(), TEXT("TutorialMap"));
        SpawnTutorialAgent();
    }
}

// Called every frame
void ATutorialAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATutorialAgent::SpawnTutorialAgent() {
    UE_LOG(LogTemp, Warning, TEXT("SpawnTutorialAgent called"));

    if (UWorld* World = GetWorld()) {
        UE_LOG(LogTemp, Warning, TEXT("SpawnTutorialAgent entered world, spawning agent"));

        // Spawn the actor at a specific location
        FVector SpawnLocation = FVector(0.0f, 0.0f, 100.0f);
        FRotator SpawnRotation = FRotator::ZeroRotator;
        
        // Spawn the actor into the current world (TutorialMap)
        ATutorialAgent* SpawnedAgent = World->SpawnActor<ATutorialAgent>(ATutorialAgent::StaticClass(), SpawnLocation, SpawnRotation);

        if (SpawnedAgent) {
            UE_LOG(LogTemp, Warning, TEXT("Agent spawned successfully!"));
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn agent."));
        }
    }
}


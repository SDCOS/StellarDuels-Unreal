#include "TutorialGamemode.h"
#include "TutorialAgent.h"
#include "TutorialTarget.h"
#include "Kismet/GameplayStatics.h"
#include "../HUD_Crosshair.h"
#include "../PlayerPawn.h"

ATutorialGameMode::ATutorialGameMode()
{
    UE_LOG(LogTemp, Warning, TEXT("TutorialGameMode Constructor Called"));
    DefaultPawnClass = APlayerPawn::StaticClass();
    HUDClass = AHUD_Crosshair::StaticClass();
}

void ATutorialGameMode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("TutorialGameMode BeginPlay Called"));

    UWorld* World = GetWorld();
    if (World)
    {
        FVector ActorSpawnLocation(20, 20, 10);
        FVector TargetSpawnLocation(100, 100, 10);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ATutorialAgent* SpawnedAgent = World->SpawnActor<ATutorialAgent>(ATutorialAgent::StaticClass(), ActorSpawnLocation, SpawnRotation, SpawnParams);
        ATutorialTarget* SpawnedTarget = World->SpawnActor<ATutorialTarget>(ATutorialTarget::StaticClass(), TargetSpawnLocation, SpawnRotation, SpawnParams);
        
        if (SpawnedAgent && SpawnedTarget)
        {
            UE_LOG(LogTemp, Warning, TEXT("ATutorialAgent and Target successfully spawned from GameMode!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn ATutorialAgent/Target"));
        }
    }
}


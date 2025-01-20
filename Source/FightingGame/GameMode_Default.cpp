// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode_Default.h"

#include "HUD_Crosshair.h"
#include "PlayerPawn.h" //custom classes
#include "GameFramework/PlayerStart.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

//FIXME: The player on the server bugs out whenever someone joins

AGameMode_Default::AGameMode_Default()
{
    // Set default classes (assign your custom ones here if needed)
    DefaultPawnClass = APlayerPawn::StaticClass();  
    HUDClass = AHUD_Crosshair::StaticClass();         
}

void AGameMode_Default::BeginPlay()
{
    Super::BeginPlay();
}

void AGameMode_Default::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    // Example: Custom logic for initializing a new player
    if (NewPlayer && NewPlayer->GetPawn() == nullptr)
    {
        AActor* SpawnLocation = ChoosePlayerStart(NewPlayer);
        if (SpawnLocation)
        {
            APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnLocation->GetActorLocation(), SpawnLocation->GetActorRotation());
            if (NewPawn)
            {
                NewPlayer->Possess(NewPawn);
                UE_LOG(LogTemp, Log, TEXT("New player spawned and possessed a pawn."));
            }
        }
    }
}

AActor* AGameMode_Default::ChoosePlayerStart_Implementation(AController* PlayerController)
{
    // Example: Find a random PlayerStart actor
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    if (PlayerStarts.Num() > 0)
    {
        return PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)];
    }

    return Super::ChoosePlayerStart_Implementation(PlayerController);
}

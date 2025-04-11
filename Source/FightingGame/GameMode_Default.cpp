// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode_Default.h"

#include "HUD_Crosshair.h"
#include "PlayerPawn.h" //custom classes
#include "GameFramework/PlayerStart.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"


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
    APlayerController* InitPC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
    InitPawn = InitPC->GetPawn();
}

void AGameMode_Default::PostLogin(APlayerController* NewPlayer) {
    Super::PostLogin(NewPlayer);

    APlayerController* ServerPC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

    //for debugging *****
    if (ServerPC)
    {
        APawn* ControlledPawn = ServerPC->GetPawn();
        if (ControlledPawn)
        {
            UE_LOG(LogTemp, Warning, TEXT("Server PlayerController is still possessing: %s"), *ControlledPawn->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Server PlayerController has lost possession of its pawn!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Server PlayerController is null!"));
    }
    // *********

    
    if (!ServerPC->GetPawn())
    {
        ServerPC->Possess(InitPawn); // Replace with the reference to your server-side pawn
        UE_LOG(LogTemp, Warning, TEXT("Repossessed server-side pawn"));
    }

    APawn* NewPlayerPawn = NewPlayer->GetPawn();
    if (NewPlayerPawn)
    {
        NewPlayerPawn->SetOwner(NewPlayer);
    }

    APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>();
    int32 PlayerIndex = GameState->PlayerArray.IndexOfByKey(PS);

    if (PS)
    {
        FString AssignedName = (PlayerIndex == 0) ? "PlayerA" : "PlayerB";
        PS->SetPlayerName(AssignedName);
    }

}

AActor* AGameMode_Default::ChoosePlayerStart_Implementation(AController* Player)
{
    // Retrieve all Player Start actors in the world
    TArray<AActor*> PlayerPawns;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerPawn::StaticClass(), PlayerPawns);

    int32 NumPlayerPawns = PlayerPawns.Num();

    FVector SpawnLocation0(0.f, 0.f, 0.f); // X, Y, Z
    FRotator SpawnRotation0(0.f, 90.f, 0.f); // Pitch, Yaw, Roll
    FTransform SpawnTransform0(SpawnRotation0, SpawnLocation0);

    APlayerStart* SpawnPoint0 = GetWorld()->SpawnActor<APlayerStart>(
        APlayerStart::StaticClass(),
        SpawnTransform0
    );

    FVector SpawnLocation1(200.f, 0.f, 0.f); // X, Y, Z
    FRotator SpawnRotation1(0.f, 90.f, 0.f); // Pitch, Yaw, Roll
    FTransform SpawnTransform1(SpawnRotation1, SpawnLocation1);

    APlayerStart* SpawnPoint1 = GetWorld()->SpawnActor<APlayerStart>(
        APlayerStart::StaticClass(),
        SpawnTransform1
    );

    SpawnPoint0->Tags.Add("PlayerA");
    SpawnPoint1->Tags.Add("PlayerB");


    if (APlayerState* PS = Player->GetPlayerState<APlayerState>())
    {
        if (NumPlayerPawns % 2 == 0)
        {
            return SpawnPoint0;
        }
        else
        {
            return SpawnPoint1;
        }
    }

    // Fallback
    return Super::ChoosePlayerStart_Implementation(Player);
}




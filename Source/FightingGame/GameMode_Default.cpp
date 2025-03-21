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
    APlayerController* InitPC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
    InitPawn = InitPC->GetPawn();
}

void AGameMode_Default::PostLogin(APlayerController* NewPlayer) {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("postlogin called"));
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
    } // WORK ON THIS FUTURE FLYNN - -  - - - -  - - -

    APawn* NewPlayerPawn = NewPlayer->GetPawn();
    if (NewPlayerPawn)
    {
        NewPlayerPawn->SetOwner(NewPlayer);
    }

}



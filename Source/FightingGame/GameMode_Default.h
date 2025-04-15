// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameMode_Default.generated.h"

/**
 * Multiplayer GameMode with custom functionality for player spawning and session management.'
 * 
 * This is a GameMode. GameModes decide which playerPawn class, HUD class, replay class etc. that the engine uses when you start the game.
 * 
 * This class also allows new players to spawn at the location of an existing player
 */
UCLASS()
class FIGHTINGGAME_API AGameMode_Default : public AGameMode
{
    GENERATED_BODY()

public:
    AGameMode_Default();

protected:
    APawn* InitPawn;
    // Function to initialize custom game state
    virtual void BeginPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer);
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};

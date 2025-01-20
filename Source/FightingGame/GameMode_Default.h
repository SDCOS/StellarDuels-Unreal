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
    // Override to handle player initialization
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    // Override to control where players spawn
    virtual AActor* ChoosePlayerStart_Implementation(AController* PlayerController) override;

    // Function to initialize custom game state
    virtual void BeginPlay() override;


};

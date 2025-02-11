// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TutorialTarget.generated.h"

UCLASS()
class FIGHTINGGAME_API ATutorialTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the actor is destroyed
	virtual void Destroyed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Mesh for the agent
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AgentMesh;

	//Spawn the agent into the tutorial, called when BeginPlay occurs
	void SpawnTutorialAgent();

	//Check if actor is in the level
	bool IsPlacedInLevel();

	/** Floating Movement Variables */
	UPROPERTY(EditAnywhere, Category = "Floating Movement")
	float MoveDistance = 100.0f; // How high it moves

	UPROPERTY(EditAnywhere, Category = "Floating Movement")
	float MoveSpeed = 2.0f; // Speed of oscillation

	UPROPERTY(VisibleAnywhere, Category = "Floating Movement")
	FVector TargetLocation;

	UPROPERTY(VisibleAnywhere, Category = "Floating Movement")
	bool bMovingToTarget = true; // Track movement direction

	// Store initial location
	FVector StartLocation;

};

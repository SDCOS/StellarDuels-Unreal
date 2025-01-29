// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TutorialAgent.generated.h"


UCLASS(Blueprintable)
class FIGHTINGGAME_API ATutorialAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialAgent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Mesh for the agent
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AgentMesh;

	//Spawn the agent into the tutorial, called when BeginPlay occurs
	void SpawnTutorialAgent();
};

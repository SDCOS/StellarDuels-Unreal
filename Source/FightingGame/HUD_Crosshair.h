// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "HUD_Crosshair.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGGAME_API AHUD_Crosshair : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;

	AHUD_Crosshair();

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	UUserWidget* HUDWidgetInstance;
};
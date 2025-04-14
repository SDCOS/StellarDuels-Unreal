// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Public/CrosshairWidget.h"
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
	UCrosshairWidget* HUDWidgetInstance;

public:
	void SetKillCount(int32 NewKills);

	void SetDeathCount(int32 NewDeaths);

	void SetHealthCount(int32 NewHealth);


};
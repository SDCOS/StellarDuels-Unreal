// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_Crosshair.h"
#include "Engine/Canvas.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"


AHUD_Crosshair::AHUD_Crosshair()
{
    // Use ConstructorHelpers to find the widget blueprint
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/HUD/Crosshair.Crosshair_C'"));
    if (WidgetClassFinder.Succeeded())
    {
        HUDWidgetClass = WidgetClassFinder.Class; // Set the widget class
    }
}

void AHUD_Crosshair::BeginPlay()
{
    Super::BeginPlay();

    if (HUDWidgetClass)
    {
        // Create the widget
        HUDWidgetInstance = CreateWidget<UCrosshairWidget>(GetWorld(), HUDWidgetClass);
        if (HUDWidgetInstance)
        {
            // Add it to the viewport
            HUDWidgetInstance->AddToViewport();
        }
    }
}

void AHUD_Crosshair::SetKillCount(int32 NewKills) {
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->UpdateKills(NewKills);
    }
}

void AHUD_Crosshair::SetDeathCount(int32 NewDeaths) {
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->UpdateDeaths(NewDeaths);
    }
}

void AHUD_Crosshair::SetHealthCount(int32 NewHealth) {
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->UpdateHealth(NewHealth);
    }
}

void AHUD_Crosshair::DrawHUD()
{
    Super::DrawHUD();

    float FPS = 1.0f / GetWorld()->GetDeltaSeconds();
    FString FPS_string = FString::Printf(TEXT("FPS: %.2f"), FPS);
    // shows fps
    DrawText(FPS_string, FColor::White, 50, 50);

}
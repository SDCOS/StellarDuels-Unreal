// Fill out your copyright notice in the Description page of Project Settings.


#include "CrosshairWidget.h"

void UCrosshairWidget::UpdateKills(int32 Kills_num)
{
    if (Kills)
    {
        FString Formatted = FString::Printf(TEXT("Kills: %d"), Kills_num);
        Kills->SetText(FText::FromString(Formatted));
    }
}

void UCrosshairWidget::UpdateDeaths(int32 Deaths_num)
{
    if (Deaths)
    {
        FString Formatted = FString::Printf(TEXT("Deaths: %d"), Deaths_num);
        Deaths->SetText(FText::FromString(Formatted));
    }
}

void UCrosshairWidget::UpdateHealth(int32 Health_num) {
    if (Health && Health_Bar) {
        FString Formatted = FString::Printf(TEXT("Health: %d"), Health_num);
        Health->SetText(FText::FromString(Formatted));
        
        float Percent = FMath::Clamp(static_cast<float>(Health_num) / 100.f, 0.f, 1.f);
        Health_Bar->SetPercent(Percent);
    }
}
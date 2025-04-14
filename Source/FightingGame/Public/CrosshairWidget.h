// CrosshairWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "CrosshairWidget.generated.h"

UCLASS()
class FIGHTINGGAME_API UCrosshairWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Kills;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Deaths;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Health;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* Health_Bar;

    UFUNCTION(BlueprintCallable)
    void UpdateKills(int32 Kills_num);

    UFUNCTION(BlueprintCallable)
    void UpdateDeaths(int32 Deaths_num);

    UFUNCTION(BlueprintCallable)
    void UpdateHealth(int32 Health_num);
};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialGameMode.generated.h"

UCLASS()
class FIGHTINGGAME_API ATutorialGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATutorialGameMode();

protected:
    virtual void BeginPlay() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "StellarGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIGHTINGGAME_API UStellarGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UStellarGameInstance();

protected:
	TSharedPtr<FOnlineSessionSearch> SessionSearch; 
	
	IOnlineSessionPtr SessionInterface;

	virtual void Init() override;
	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void CreateServer();

	UFUNCTION(BlueprintCallable)
	void JoinServer();

	UFUNCTION(BlueprintCallable)
	void EnterTutorial();

	UFUNCTION(BlueprintCallable)
	void LoginWithEOS();
};

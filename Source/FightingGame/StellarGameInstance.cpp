// Fill out your copyright notice in the Description page of Project Settings.


#include "StellarGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UStellarGameInstance::UStellarGameInstance() {

}

void UStellarGameInstance::Init() {

	Super::Init(); //if there is a red line here, it is a vscode bug

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get()) {
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UStellarGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UStellarGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UStellarGameInstance::OnJoinSessionComplete);
		}
	}

}

void UStellarGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded) {
	if (Succeeded) {
		GetWorld()->ServerTravel("/Game/TestMap?listen"); //might need single quotes? idk how that would make any sense
	}
}

void UStellarGameInstance::OnFindSessionComplete(bool Succeeded) {
	if (Succeeded) {
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		if (SearchResults.Num()) { // false if = 0
			SessionInterface->JoinSession(0, FName("Stellar Session"), SearchResults[0]); //hardcoding the result that we want to join FOR TESTING (we know that if there is at least one session, SearchResults[0] will exist)
		}
	}
}

void UStellarGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "") {
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UStellarGameInstance::CreateServer() {
	UE_LOG(LogTemp, Warning, TEXT("CreateServer has been called"));
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL") ? false : true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 5;

	SessionInterface->CreateSession(0, FName("Stellar Session"), SessionSettings); // first comma should be period? There is no way....
}

void UStellarGameInstance::JoinServer() {
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
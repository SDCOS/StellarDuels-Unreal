// Fill out your copyright notice in the Description page of Project Settings.


#include "StellarGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

//FIXME: The player on the server bugs out whenever someone joins. This probaby has to do with how new players are managed in GameMode_Default but I figured I'd add this here as well

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
		GetWorld()->ServerTravel("/Game/TestMap?listen");
	}
}

void UStellarGameInstance::OnFindSessionComplete(bool Succeeded) {
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete has been called"));
	if (Succeeded) {
		UE_LOG(LogTemp, Warning, TEXT("Session found"));
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		if (SearchResults.Num()) { // false if = 0
			SessionInterface->JoinSession(0, FName("Stellar Session"), SearchResults[0]); //hardcoding the result that we want to join FOR TESTING (we know that if there is at least one session, SearchResults[0] will exist)
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No session found"));
	}
}

void UStellarGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete has been called"));
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "") {
			UE_LOG(LogTemp, Warning, TEXT("Join address not \"\""));
			if (JoinAddress.Contains(":0"))
			{
				JoinAddress = JoinAddress.Replace(TEXT(":0"), TEXT(":7777")); //not resolving the port correctly, so we have to manually set it to 7777, which is unreal's default port for session management and the correct port in this case
			}
			UE_LOG(LogTemp, Warning, TEXT("Resolved Join Address: %s"), *JoinAddress);
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Join address is \"\""));
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
	UE_LOG(LogTemp, Warning, TEXT("JoinServer has been called"));
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
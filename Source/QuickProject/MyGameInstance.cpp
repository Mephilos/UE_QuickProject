// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

UMyGameInstance::UMyGameInstance()
{
}

void UMyGameInstance::FindAndJoinSession()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->MaxSearchResults = 10;
			SessionSearch->bIsLanQuery = true;

			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionsComplete);
			SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		}

	}
}
void UMyGameInstance::OnFindSessionsComplete(bool bSuccess)
{
	if (bSuccess && SessionInterface.IsValid() && SessionSearch.IsValid())
	{
		if (SessionSearch->SearchResults.Num() > 0)
		{
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionComplete);
			SessionInterface->JoinSession(0, "MySession", SessionSearch->SearchResults[0]);
		}
		else
		{
			FOnlineSessionSettings SessionSettings;
			SessionSettings.NumPublicConnections = 2;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bIsLANMatch = true;

			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionComplete);
			SessionInterface->CreateSession(0, "MySession", SessionSettings);
		}
	}
}
void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (bSuccess)
	{
		GetWorld()->ServerTravel("/Game/FirstPerson/Maps/FirstPersonMap");
	}
}
void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success && SessionInterface.IsValid())
	{
		FString Address;
		if (SessionInterface->GetResolvedConnectString(SessionName, Address))
		{
			APlayerController* PlayerController = GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

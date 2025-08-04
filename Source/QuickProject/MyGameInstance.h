// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUICKPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();

	UFUNCTION(BlueprintCallable)
	void FindAndJoinSession();

protected:
	IOnlineSessionPtr SessionInterface;

	void OnFindSessionsComplete(bool bSuccess);
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
};

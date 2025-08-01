// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DeathmatchPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class QUICKPROJECT_API ADeathmatchPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	void AddScore(int32 Amount);

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DeathmatchGameMode.generated.h"

/**
 * 
 */
UCLASS()
class QUICKPROJECT_API ADeathmatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	TSubclassOf<ACharacter> DummyCharacterClass;

public:

	void PlayerDead(ACharacter* DeadCharacter, AController* DeadPlayerController, AController* KillerController, bool bHeadshot);
	
	// 더미 리스폰 함수
	void RespawnDummy();
};

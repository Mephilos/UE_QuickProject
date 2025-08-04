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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	int32 MatchTime = 120;

	// 매치 시간 끝났을 때 쓸 함수
	void OnMatchEnd();
	FTimerHandle MatchTimerHandle;

	virtual void BeginPlay() override;

public:
	// 플레이어 죽음 함수
	void PlayerDead(ACharacter* DeadCharacter, AController* DeadPlayerController, AController* KillerController, bool bHeadshot);
	
	// 더미 리스폰 함수
	void RespawnDummy();
	virtual void PostLogin(APlayerController* NewPlayer) override;
};

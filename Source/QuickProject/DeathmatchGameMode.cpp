// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathmatchGameMode.h"
#include "DeathmatchPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void ADeathmatchGameMode::PlayerDead(ACharacter* DeadCharacter, AController* DeadPlayerController, AController* KillerController)
{
	if (KillerController && KillerController != DeadPlayerController)
	{
		if (ADeathmatchPlayerState* KillerPlayerState = KillerController->GetPlayerState<ADeathmatchPlayerState>())
		{
			// TODE: 해드샷 추가 점수 예정
			KillerPlayerState->AddScore(1);
		}
	}

	if (DeadPlayerController) 
	{
		// 죽은 컨트롤러가 플레이어
		if (DeadPlayerController->IsPlayerController())
		{
			// 3초 뒤 리스폰
			FTimerHandle RespawnTimer;
			FTimerDelegate RespawnDelegate;
			RespawnDelegate.BindUFunction(this, FName("RestartPlayer"), DeadPlayerController);
			GetWorld()->GetTimerManager().SetTimer(RespawnTimer, RespawnDelegate, 3.0f, false);
		}
		else // 플레이어 외
		{
			FTimerHandle RespawnTimer;
			GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &ADeathmatchGameMode::RespawnDummy, 3.0f, false);
		}
	}

	// 죽은 케릭터 파괴
	if (DeadCharacter)
	{
		DeadCharacter->Destroy();
	}
}
void ADeathmatchGameMode::RespawnDummy()
{
	// 더미 클래스 지정 확인
	if (!DummyCharacterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("DummyCharacterClass not Set in GameMode"));
		return;
	}
	
	TArray<AActor*> PlayerStartPoint;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartPoint);

	if (PlayerStartPoint.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, PlayerStartPoint.Num() - 1);
		AActor* SpawnPoint = PlayerStartPoint[RandomIndex];

		GetWorld()->SpawnActor<ACharacter>(DummyCharacterClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation());
		UE_LOG(LogTemp, Warning, TEXT("Dummy Spawn"));
	}
}

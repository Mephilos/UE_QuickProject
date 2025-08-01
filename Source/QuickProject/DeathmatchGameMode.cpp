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
			// TODE: �ص弦 �߰� ���� ����
			KillerPlayerState->AddScore(1);
		}
	}

	if (DeadPlayerController) 
	{
		// ���� ��Ʈ�ѷ��� �÷��̾�
		if (DeadPlayerController->IsPlayerController())
		{
			// 3�� �� ������
			FTimerHandle RespawnTimer;
			FTimerDelegate RespawnDelegate;
			RespawnDelegate.BindUFunction(this, FName("RestartPlayer"), DeadPlayerController);
			GetWorld()->GetTimerManager().SetTimer(RespawnTimer, RespawnDelegate, 3.0f, false);
		}
		else // �÷��̾� ��
		{
			FTimerHandle RespawnTimer;
			GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &ADeathmatchGameMode::RespawnDummy, 3.0f, false);
		}
	}

	// ���� �ɸ��� �ı�
	if (DeadCharacter)
	{
		DeadCharacter->Destroy();
	}
}
void ADeathmatchGameMode::RespawnDummy()
{
	// ���� Ŭ���� ���� Ȯ��
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathmatchGameMode.h"
#include "DeathmatchPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"


void ADeathmatchGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(MatchTimerHandle, this, &ADeathmatchGameMode::OnMatchEnd, MatchTime, false);	
	UE_LOG(LogTemp, Warning, TEXT("Match Time: %d sec"), MatchTime);
}

void ADeathmatchGameMode::PlayerDead(ACharacter* DeadCharacter, AController* DeadPlayerController, AController* KillerController, bool bHeadshot)
{
	if (KillerController && KillerController != DeadPlayerController)
	{
		if (ADeathmatchPlayerState* KillerPlayerState = KillerController->GetPlayerState<ADeathmatchPlayerState>())
		{
			// �ص弦�̸�
			if (bHeadshot)
			{
				KillerPlayerState->AddScore(15);
				UE_LOG(LogTemp, Warning, TEXT("Headshot score +15"));
			}
			else
			{
				KillerPlayerState->AddScore(10);
				UE_LOG(LogTemp, Warning, TEXT("GYANG kill +10"));
			}
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
void ADeathmatchGameMode::OnMatchEnd()
{
	ADeathmatchPlayerState* WinnerPlayerState = nullptr;
	int32 MaxScore = -1;

	for (APlayerState* PlayerState : GetWorld()->GetGameState()->PlayerArray)
	{
		if (ADeathmatchPlayerState* DMPlayerState = Cast<ADeathmatchPlayerState>(PlayerState))
		{
			if (DMPlayerState->GetScore() > MaxScore)
			{
				MaxScore = DMPlayerState->GetScore();
				WinnerPlayerState = DMPlayerState;
			}
		}
	}

	if (WinnerPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Winner: %s, Score: %f"), *WinnerPlayerState->GetPlayerName(), WinnerPlayerState->GetScore());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Mathc Draw"));
	}
	// UE_LOG(LogTemp, Warning, TEXT("Match Done"));
}

void ADeathmatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GetNumPlayers() >= 2)
	{
		if (!MatchTimerHandle.IsValid() || !GetWorld()->GetTimerManager().IsTimerActive(MatchTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(MatchTimerHandle, this, &ADeathmatchGameMode::OnMatchEnd, MatchTime, false);
			UE_LOG(LogTemp, Warning, TEXT("Match Start. Player: %d"), GetNumPlayers());
		}
	}
}

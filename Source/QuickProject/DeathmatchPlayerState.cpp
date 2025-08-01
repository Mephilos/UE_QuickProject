// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathmatchPlayerState.h"
#include "Net/UnrealNetwork.h"

void ADeathmatchPlayerState::AddScore(int32 Amount)
{
	SetScore(GetScore() + Amount);
}
void ADeathmatchPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


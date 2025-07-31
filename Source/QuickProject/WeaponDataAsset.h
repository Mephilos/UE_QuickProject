// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class QUICKPROJECT_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 기본 데미지(몸통 기준)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float BaseDamage = 100.0f;
	// 팔다리 배율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float LimbDamageMultiplier = 0.5;
	// 해드샷 배울
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float HeadDamageMultiplier = 2.0f;
	// 발사 쿨
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float FireCooldown = 1.0f;
	// 무기 사거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wwapon Stats")
	float MaxRange = 10000.0f;
};

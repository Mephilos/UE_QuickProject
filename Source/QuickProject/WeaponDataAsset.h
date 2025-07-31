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
	// �⺻ ������(���� ����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float BaseDamage = 100.0f;
	// �ȴٸ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float LimbDamageMultiplier = 0.5;
	// �ص弦 ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float HeadDamageMultiplier = 2.0f;
	// �߻� ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float FireCooldown = 1.0f;
	// ���� ��Ÿ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wwapon Stats")
	float MaxRange = 10000.0f;
};

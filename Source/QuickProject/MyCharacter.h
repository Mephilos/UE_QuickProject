// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class QUICKPROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
	// �뽬 ����
	UPROPERTY(EditAnywhere, Category = "Movement")
	float DashDistance = 5000.0f;
	// �뽬 ������ ��ٿ�
	UPROPERTY(EditAnywhere, Category = "Movement")
	float DashRechargeCooldown = 1.0f;
	// �뽬 ���� Ƚ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	int32 MaxDashCharges = 3;
	// ���� �뽬 ����
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	int32 CurrentDashCharges;
	
	FTimerHandle DashRechargeCooldownHandle;

	//// ���� ����
	//float DodgeDistance = 2000.0f;
	//float DodgeCooldown = 0.2f;
	//float LastForwardPressTime = 0.f;
	//float LastRightPressTime = 0.f;


	// �̵� �Լ�
	void Dash();
	void DashRecharge();
	void MoveForward(float Value);
	void MoveRight(float Value);
	//void Dodge(FVector DodgeDirection);
	//void DodgeChecker(float& LastPressTime, FVector DodgeDirection);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Jump() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

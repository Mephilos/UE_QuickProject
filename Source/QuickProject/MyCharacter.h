// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class QUICKPROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	
	// ���ڽ��� ��ǲ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* DashAction;

	// �뽬 ����
	UPROPERTY(EditAnywhere, Category = "Movement/Dash")
	float DashDistance = 5000.0f;
	// �뽬 ������ ��ٿ�
	UPROPERTY(EditAnywhere, Category = "Movement/Dash")
	float DashRechargeCooldown = 1.0f;
	// �뽬 ���� Ƚ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement/Dash")
	int32 MaxDashCharges = 3;
	// ���� �뽬 ����
	UPROPERTY(BlueprintReadOnly, Category = "Movement/Dash")
	int32 CurrentDashCharges;
	
	FTimerHandle DashRechargeCooldownHandle;

	//// ���� ����
	//float DodgeDistance = 2000.0f;
	//float DodgeCooldown = 0.2f;
	//float LastForwardPressTime = 0.f;
	//float LastRightPressTime = 0.f;


	virtual void BeginPlay() override;

	// �̵� �Լ�
	void Dash();
	void DashRecharge();
	/*void MoveForward(float Value);
	void MoveRight(float Value);*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	//void Dodge(FVector DodgeDirection);
	//void DodgeChecker(float& LastPressTime, FVector DodgeDirection);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Jump() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

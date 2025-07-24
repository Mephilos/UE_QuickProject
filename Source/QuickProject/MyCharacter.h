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
	// 인핸스드 인풋 기본 이동
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* JumpAction;
	
	// 대쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* DashAction;
	
	// 닷지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* DodgeForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* DodgeBackwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* DodgeRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* DodgeLeftAction;
	// 앉기(슬라이딩)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* CrouchAction;



	UPROPERTY(EditAnywhere, Category = "Movement-Dodge")
	float DodgeDistance = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Movement-Dodge")
	float DodgeTapTime = 0.25f;

	float LastForwardTapTime = 0.f;
	float LastBackwardTapTime = 0.f;
	float LastRightTapTime = 0.f;
	float LastLeftTapTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Movement-Slide")
	float MinSlideSpeed = 1500.0f; // 슬라이딩 발동 가능 속도
	
	UPROPERTY(EditAnywhere, Category = "Movement-Slide")
	float SlideFriction = 0.1f;



	// 대쉬
	UPROPERTY(EditAnywhere, Category = "Movement-Dash")
	float DashDistance = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Movement-Dash")
	float DashRechargeCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement-Dash")
	int32 MaxDashCharges = 3;

	UPROPERTY(BlueprintReadOnly, Category = "Movement-Dash")
	int32 CurrentDashCharges; // 대쉬 스택
	
	FTimerHandle DashRechargeCooldownHandle;

	FTimerHandle SlideTimerHandle;

	float DefaultFriction; // 기존 마찰력 보존 하기 위한 변수


	virtual void BeginPlay() override;

	// 대쉬 함수
	void Dash();
	void DashRecharge();
	
	// 닷지 함수
	void DodgeForward();
	void DodgeBackward();
	void DodgeRight();
	void DodgeLeft();
	void PerformDodge(const FVector& Direction);
	
	// 슬라이딩 함수
	void StartSlide();
	void StopSlide();

	// 이동 함수
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
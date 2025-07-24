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
	
	// 이동 설정
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	// 대쉬 설정
	UPROPERTY(EditAnywhere, Category = "Movement");
	float DashDistance = 5000.0f;
	
	UPROPERTY(EditAnywhere, Category = "Movement");
	float DashCooldown = 1.0f;

	bool bCanDash = true;

	FTimerHandle DashCooldownTimerHandle;

	void Dash();

	void ResetDash();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMyCharacter::Dash()
{
	if (bCanDash)
	{
		bCanDash = false;
		// 케릭터의 입력 방향
		FVector DashDirection = GetLastMovementInputVector();
		// 케릭터의 입력이 없으면
		if (DashDirection.IsNearlyZero())
		{	
			// 앞 방향으로
			DashDirection = GetActorForwardVector();
		}
		// 대쉬
		LaunchCharacter(DashDirection.GetSafeNormal() * DashDistance, true, true);
		// 대쉬 쿨다운 뒤 ResetDash 함수를 실행하는 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(DashCooldownTimerHandle, this, &AMyCharacter::ResetDash, DashCooldown, false);
	}	
}
void AMyCharacter::ResetDash()
{
	bCanDash = true;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMyCharacter::Dash);
}


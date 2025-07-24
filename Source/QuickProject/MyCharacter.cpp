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
		// �ɸ����� �Է� ����
		FVector DashDirection = GetLastMovementInputVector();
		// �ɸ����� �Է��� ������
		if (DashDirection.IsNearlyZero())
		{	
			// �� ��������
			DashDirection = GetActorForwardVector();
		}
		// �뽬
		LaunchCharacter(DashDirection.GetSafeNormal() * DashDistance, true, true);
		// �뽬 ��ٿ� �� ResetDash �Լ��� �����ϴ� Ÿ�̸� ����
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


// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CurrentDashCharges = MaxDashCharges; // ���� ���۽� �뽬 ���� ����
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
	if (CurrentDashCharges > 0) // �뽬 ������ ���� ���� �� ����
	{
		CurrentDashCharges--; // �뽬 ���� �Ҹ�

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
		// ���� Ÿ�̸Ӱ� ���� ���� �ƴ� ���� ����
		if (!GetWorld()->GetTimerManager().IsTimerActive(DashRechargeCooldownHandle))
		{
			// DashRechargeCooldown ���� �ݺ� ����ǵ���
			GetWorld()->GetTimerManager().SetTimer(DashRechargeCooldownHandle, this, &AMyCharacter::DashRecharge, DashRechargeCooldown, true);
		}
	}
}
void AMyCharacter::DashRecharge()
{
	CurrentDashCharges++; // �뽬 ���� ++

	if (CurrentDashCharges >= MaxDashCharges) // �뽬 ������ �ִ�ġ�� ���� ���� �� ����
	{
		CurrentDashCharges = MaxDashCharges; // �뽬 ���� ���� Ȯ��
		GetWorld()->GetTimerManager().ClearTimer(DashRechargeCooldownHandle); // �뽬 ���� Ÿ�� �ʱ�ȭ
	}
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


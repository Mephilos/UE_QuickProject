// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// �ɹ� ���� �ʱ�ȭ
	DefaultMappingContext = nullptr;
	MoveAction = nullptr;
	LookAction = nullptr;
	JumpAction = nullptr;
	DashAction = nullptr;
	CrouchAction = nullptr;
	DodgeForwardAction = nullptr;
	DodgeBackwardAction = nullptr;
	DodgeRightAction = nullptr;
	DodgeLeftAction = nullptr;
	DefaultFriction = 0.0f;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CurrentDashCharges = MaxDashCharges; // ���� ���۽� �뽬 ���� ����

	JumpMaxCount = 2;
	// �ɸ��� ������Ʈ ��������
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	// �ִ� �̵��ӵ�
	MoveComp->MaxWalkSpeed = 2000.0f;
	// ���ӵ�
	MoveComp->MaxAcceleration = 4000.0f;
	// ������
	MoveComp->GroundFriction = 8.0f;
	// ������
	MoveComp->BrakingDecelerationWalking = 8000.0f;
	// ���� �����
	MoveComp->AirControl = 0.5f;
	// �߷� ������ ����
	MoveComp->GravityScale = 1.5f;
	// ���� ������
	MoveComp->FallingLateralFriction = 3.0f;
	// ��ũ���� �����ϰ� ����
	MoveComp->NavAgentProps.bCanCrouch = true;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//void AMyCharacter::MoveForward(float Value)
//{
//	if (Value != 0.0f)
//	{
//		// Ű �Է½�
//		FVector Direction = GetActorForwardVector() * FMath::Sign(Value);
//		DodgeChecker(LastForwardPressTime, Direction); // ���� üĿ
//		AddMovementInput(GetActorForwardVector(), Value);
//	}
//}
//
//void AMyCharacter::MoveRight(float Value)
//{
//	if (Value != 0.0f)
//	{
//		// Ű �Է½�
//		FVector Direction = GetActorRightVector() * FMath::Sign(Value);
//		DodgeChecker(LastRightPressTime, Direction); // ��ġ üĿ
//		AddMovementInput(GetActorRightVector(), Value);
//	}
//}
//
//void AMyCharacter::DodgeChecker(float& LastPressTime, FVector DodgeDirection)
//{
//	if (GetWorld()->GetTimeSeconds() - LastPressTime <= DodgeCooldown)
//	{
//		Dodge(DodgeDirection);
//		LastPressTime = 0.f;
//	}
//	else
//	{
//		LastPressTime = GetWorld()->GetTimeSeconds();
//	}
//}
//
//void AMyCharacter::Dodge(FVector DodgeDirection)
//{
//	LaunchCharacter(DodgeDirection.GetSafeNormal() * DodgeDistance, true, false);
//}

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
		LaunchCharacter(DashDirection.GetSafeNormal() * DashDistance, true, false);
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

void AMyCharacter::PerformDodge(const FVector& Direction)
{
	// ������ �浹���� �ʵ��� Z�� �ӵ��� ����
	LaunchCharacter(Direction.GetSafeNormal() * DodgeDistance, true, false);
}

void AMyCharacter::DodgeForward()
{
	if (GetWorld()->GetTimeSeconds() - LastForwardTapTime <= DodgeTapTime)
	{
		PerformDodge(GetActorForwardVector());
		LastForwardTapTime = 0.f; // ���� ���� �� �ʱ�ȭ
	}
	else
	{
		LastForwardTapTime = GetWorld()->GetTimeSeconds();
	}
}

void AMyCharacter::DodgeBackward()
{
	if (GetWorld()->GetTimeSeconds() - LastBackwardTapTime <= DodgeTapTime)
	{
		PerformDodge(-GetActorForwardVector()); // �ڴ� ���� ������ �ݴ�
		LastBackwardTapTime = 0.f;
	}
	else
	{
		LastBackwardTapTime = GetWorld()->GetTimeSeconds();
	}
}

void AMyCharacter::DodgeRight()
{
	if (GetWorld()->GetTimeSeconds() - LastRightTapTime <= DodgeTapTime)
	{
		PerformDodge(GetActorRightVector());
		LastRightTapTime = 0.f;
	}
	else
	{
		LastRightTapTime = GetWorld()->GetTimeSeconds();
	}
}

void AMyCharacter::DodgeLeft()
{
	if (GetWorld()->GetTimeSeconds() - LastLeftTapTime <= DodgeTapTime)
	{
		PerformDodge(-GetActorRightVector()); // ������ ������ ������ �ݴ�
		LastLeftTapTime = 0.f;
	}
	else
	{
		LastLeftTapTime = GetWorld()->GetTimeSeconds();
	}
}

void AMyCharacter::StartSlide()
{
	if (GetCharacterMovement()->Velocity.Size() >= MinSlideSpeed && GetCharacterMovement()->IsMovingOnGround())
	{
		DefaultFriction = GetCharacterMovement()->GroundFriction;
		GetCharacterMovement()->GroundFriction = SlideFriction;

		Crouch();

		GetWorld()->GetTimerManager().SetTimer(SlideTimerHandle, this, &AMyCharacter::StopSlide, 0.75f, false);
	}
}

void AMyCharacter::StopSlide()
{
	UnCrouch();

	GetCharacterMovement()->GroundFriction = DefaultFriction;
	
	GetWorld()->GetTimerManager().ClearTimer(SlideTimerHandle);

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AMyCharacter::Jump()
{
	// ���� ���� ����׿�
	UE_LOG(LogTemp, Warning, TEXT("jump: current jump count %d"), JumpCurrentCount);
	Super::Jump();	
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//����, �뽬
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMyCharacter::Dash);
		//�̵�, ����
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		// ����
		EnhancedInputComponent->BindAction(DodgeForwardAction, ETriggerEvent::Started, this, &AMyCharacter::DodgeForward);
		EnhancedInputComponent->BindAction(DodgeBackwardAction, ETriggerEvent::Started, this, &AMyCharacter::DodgeBackward);
		EnhancedInputComponent->BindAction(DodgeRightAction, ETriggerEvent::Started, this, &AMyCharacter::DodgeRight);
		EnhancedInputComponent->BindAction(DodgeLeftAction, ETriggerEvent::Started, this, &AMyCharacter::DodgeLeft);
		// �ɱ�(�����̵�)
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyCharacter::StartSlide);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMyCharacter::StopSlide);

	}
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMyCharacter::Dash);
	//
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::StopJumping);
}


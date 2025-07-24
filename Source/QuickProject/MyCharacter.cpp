// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// 맴버 변수 초기화
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
	
	CurrentDashCharges = MaxDashCharges; // 게임 시작시 대쉬 스택 만땅

	JumpMaxCount = 2;
	// 케릭터 컴포넌트 가져오기
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	// 최대 이동속도
	MoveComp->MaxWalkSpeed = 2000.0f;
	// 가속도
	MoveComp->MaxAcceleration = 4000.0f;
	// 마찰력
	MoveComp->GroundFriction = 8.0f;
	// 제동력
	MoveComp->BrakingDecelerationWalking = 8000.0f;
	// 공중 제어력
	MoveComp->AirControl = 0.5f;
	// 중력 스케일 증가
	MoveComp->GravityScale = 1.5f;
	// 공중 마찰력
	MoveComp->FallingLateralFriction = 3.0f;
	// 웅크리기 가능하게 설정
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
//		// 키 입력시
//		FVector Direction = GetActorForwardVector() * FMath::Sign(Value);
//		DodgeChecker(LastForwardPressTime, Direction); // 닷지 체커
//		AddMovementInput(GetActorForwardVector(), Value);
//	}
//}
//
//void AMyCharacter::MoveRight(float Value)
//{
//	if (Value != 0.0f)
//	{
//		// 키 입력시
//		FVector Direction = GetActorRightVector() * FMath::Sign(Value);
//		DodgeChecker(LastRightPressTime, Direction); // 닷치 체커
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
	if (CurrentDashCharges > 0) // 대쉬 스택이 남아 있을 때 실행
	{
		CurrentDashCharges--; // 대쉬 스택 소모

		// 케릭터의 입력 방향
		FVector DashDirection = GetLastMovementInputVector();
		// 케릭터의 입력이 없으면
		if (DashDirection.IsNearlyZero())
		{
			// 앞 방향으로
			DashDirection = GetActorForwardVector();
		}
		// 대쉬
		LaunchCharacter(DashDirection.GetSafeNormal() * DashDistance, true, false);
		// 스택 타이머가 실행 중이 아닐 때만 실행
		if (!GetWorld()->GetTimerManager().IsTimerActive(DashRechargeCooldownHandle))
		{
			// DashRechargeCooldown 마다 반복 실행되도록
			GetWorld()->GetTimerManager().SetTimer(DashRechargeCooldownHandle, this, &AMyCharacter::DashRecharge, DashRechargeCooldown, true);
		}
	}
}
void AMyCharacter::DashRecharge()
{
	CurrentDashCharges++; // 대쉬 스택 ++

	if (CurrentDashCharges >= MaxDashCharges) // 대쉬 스택이 최대치에 도달 했을 때 실행
	{
		CurrentDashCharges = MaxDashCharges; // 대쉬 스택 제한 확인
		GetWorld()->GetTimerManager().ClearTimer(DashRechargeCooldownHandle); // 대쉬 스택 타임 초기화
	}
}

void AMyCharacter::PerformDodge(const FVector& Direction)
{
	// 점프와 충돌하지 않도록 Z축 속도는 보존
	LaunchCharacter(Direction.GetSafeNormal() * DodgeDistance, true, false);
}

void AMyCharacter::DodgeForward()
{
	if (GetWorld()->GetTimeSeconds() - LastForwardTapTime <= DodgeTapTime)
	{
		PerformDodge(GetActorForwardVector());
		LastForwardTapTime = 0.f; // 닷지 성공 후 초기화
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
		PerformDodge(-GetActorForwardVector()); // 뒤는 전방 벡터의 반대
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
		PerformDodge(-GetActorRightVector()); // 왼쪽은 오른쪽 벡터의 반대
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
	// 더블 점프 디버그용
	UE_LOG(LogTemp, Warning, TEXT("jump: current jump count %d"), JumpCurrentCount);
	Super::Jump();	
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//점프, 대쉬
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMyCharacter::Dash);
		//이동, 시점
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		// 닷지
		EnhancedInputComponent->BindAction(DodgeForwardAction, ETriggerEvent::Started, this, &AMyCharacter::DodgeForward);
		EnhancedInputComponent->BindAction(DodgeBackwardAction, ETriggerEvent::Started, this, &AMyCharacter::DodgeBackward);
		EnhancedInputComponent->BindAction(DodgeRightAction, ETriggerEvent::Started, this, &AMyCharacter::DodgeRight);
		EnhancedInputComponent->BindAction(DodgeLeftAction, ETriggerEvent::Started, this, &AMyCharacter::DodgeLeft);
		// 앉기(슬라이딩)
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


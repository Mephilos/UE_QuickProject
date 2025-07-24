// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CurrentDashCharges = MaxDashCharges; // 게임 시작시 대쉬 스택 만땅

	JumpMaxCount = 2;
	// 케릭터 컴포넌트 가져오기
	//UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	//// 최대 이동속도
	//MoveComp->MaxWalkSpeed = 1000.0f;
	//// 가속도
	//MoveComp->MaxAcceleration = 8000.0f;
	//// 마찰력
	//MoveComp->GroundFriction = 8.0f;
	//// 제동력
	//MoveComp->BrakingDecelerationWalking = 8000.0f;
	//// 공중 제어력
	//MoveComp->AirControl = 1.0f;
	//// 중력 스케일 증가
	//MoveComp->GravityScale = 1.0f;
	//// 공중 마찰력
	//MoveComp->FallingLateralFriction = 3.0f;

	
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
		// 키 입력시
		//FVector Direction = GetActorForwardVector() * FMath::Sign(Value);
		//DodgeChecker(LastForwardPressTime, Direction); // 닷지 체커
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// 키 입력시
		//FVector Direction = GetActorRightVector() * FMath::Sign(Value);
		//DodgeChecker(LastRightPressTime, Direction); // 닷치 체커
		AddMovementInput(GetActorRightVector(), Value);
	}
}

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
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMyCharacter::Dash);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::StopJumping);
}


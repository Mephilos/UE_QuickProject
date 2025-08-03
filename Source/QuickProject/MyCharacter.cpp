#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/CapsuleComponent.h"
#include "DeathmatchGameMode.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

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
	FireAction = nullptr;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	CurrentDashCharges = MaxDashCharges; // 게임 시작시 대쉬 스택 만땅

	JumpMaxCount = 2;
	// 케릭터 컴포넌트 가져오기
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	// 최대 이동속도
	MoveComp->MaxWalkSpeed = 1500.0f;
	// 가속도
	MoveComp->MaxAcceleration = 2000.0f;
	// 마찰력
	MoveComp->GroundFriction = 100.0f;
	// 제동력
	MoveComp->BrakingDecelerationWalking = 8000.0f;
	// 공중 제어력
	MoveComp->AirControl = 0.5f;
	// 중력 스케일 증가
	MoveComp->GravityScale = 1.0f;
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
	//커브 에셋 바인딩, 업데이트 함수 연결
	if (SlideSpeedCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("UpdateSlide"));
		SlideTimeline.AddInterpFloat(SlideSpeedCurve, ProgressFunction);
	}
	// 체력 초기화(최대로)
	CurrentHealth = MaxHealth;
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// 컨트롤러 회전 값 가져오기
		const FRotator Rotation = Controller->GetControlRotation();
		// 수평 방향만 가져오기 롤, 피치는 0로
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// YawRotation으로 백터 x y 조정
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
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

void AMyCharacter::Dash_Implementation()
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

		// Z축을 강제로 0으로 설정해서 수직이동 제한
		DashDirection.Z = 0.0f;

		// 다시 정규화해서 대쉬 속도에 영향이 없게
		DashDirection = DashDirection.GetSafeNormal();
		// 대쉬
		LaunchCharacter(DashDirection * DashDistance, true, false);
		
		// 쿨다운 타이머는 서버에서만 사용 되더록
		if (HasAuthority())
		{
			// 스택 타이머가 실행 중이 아닐 때만 실행
			if (!GetWorld()->GetTimerManager().IsTimerActive(DashRechargeCooldownHandle))
			{
				// DashRechargeCooldown 마다 반복 실행되도록
				GetWorld()->GetTimerManager().SetTimer(DashRechargeCooldownHandle, this, &AMyCharacter::DashRecharge, DashRechargeCooldown, true);
			}
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
	// 방향 벡터 복사
	FVector DodgeDirection = Direction;
	// 닷지 방향 고정위해 Z축 강제로 0 수직이동 방지
	DodgeDirection.Z = 0.0f;

	// 벡터 정규화(Z축 빠짐)다시 속도 보존
	DodgeDirection = DodgeDirection.GetSafeNormal();
	
	// 점프와 충돌하지 않도록 Z축 속도는 보존
	LaunchCharacter(DodgeDirection * DodgeDistance, true, false);
}

void AMyCharacter::DodgeForward_Implementation()
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

void AMyCharacter::DodgeBackward_Implementation()
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

void AMyCharacter::DodgeRight_Implementation()
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

void AMyCharacter::DodgeLeft_Implementation()
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

// 슬라이딩 리플레케이트를 위한 복제
void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyCharacter, bSliding);
	DOREPLIFETIME(AMyCharacter, CurrentDashCharges);
	DOREPLIFETIME(AMyCharacter, CurrentHealth);
}

void AMyCharacter::OnRep_bSliding()
{
	if (bSliding)
	{
		Crouch();
		SlideTimeline.PlayFromStart();
	}
	else
	{
		UnCrouch();
		SlideTimeline.Stop();
	}
}
void AMyCharacter::StartSlide_Implementation()
{
	// 캐릭터가 땅에있고 속도가 최소 슬라이드 속도보다 빠른 때만 작동
	if (GetCharacterMovement()->IsMovingOnGround() && GetCharacterMovement()->Velocity.Size() >= MinSlideSpeed)
	{
		// 서버는 슬라이딩 판별 변수만 수정
		bSliding = true;
		// 서버도 Rep_bSliding 함수를 수동호출해 즉시 적용
		OnRep_bSliding();
		// 슬라이딩 시작 시점 속도 저장
		SlideInitialSpeed = GetVelocity().Size2D();
		SlideDirection = GetVelocity().GetSafeNormal();

		// 마찰력 제동력 비활성화
		GetCharacterMovement()->GroundFriction = 0.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 0.f;
	}
	//if (GetCharacterMovement()->Velocity.Size() >= MinSlideSpeed && GetCharacterMovement()->IsMovingOnGround())
	//{
	//	// 원래 마찰력 저장
	//	DefaultFriction = GetCharacterMovement()->GroundFriction;
	//	// 슬라이딩 마찰력 적용
	//	GetCharacterMovement()->GroundFriction = SlideFriction;
	//	
	//	Crouch();

	//	//FVector SlideDirection = GetLastMovementInputVector().IsNearlyZero() ? GetActorForwardVector() : GetLastMovementInputVector();

	//	FVector SlideDirection = GetLastMovementInputVector();
	//	// 케릭터의 입력이 없으면
	//	if (SlideDirection.IsNearlyZero())
	//	{
	//		// 앞 방향으로
	//		SlideDirection = GetActorForwardVector();
	//	}
	//	LaunchCharacter(SlideDirection * MinSlideSpeed, false, false);
	//	// 가속감 추가 위해서 포브 변경
	//	if (auto* Camera = FindComponentByClass<UCameraComponent>())
	//	{
	//		Camera->SetFieldOfView(105.f);
	//	}
	//	// 슬라이딩 타이머 설정
	//	GetWorld()->GetTimerManager().SetTimer(SlideTimerHandle, this, &AMyCharacter::StopSlide, 0.75f, false);
	//}
}

void AMyCharacter::StopSlide_Implementation()
{
	bSliding = false;

	OnRep_bSliding();
	// 슬라이딩 끝나고 수치 복구
	GetCharacterMovement()->GroundFriction = 10.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 8000.f;

	/*if (auto* Camera = FindComponentByClass<UCameraComponent>())
	{
		Camera->SetFieldOfView(90.0f);
	}*/
	//// 마찰력 복구
	//GetCharacterMovement()->GroundFriction = DefaultFriction;

	//// 카메라 포브 값 복구
	//if (auto* Camera = FindComponentByClass<UCameraComponent>())
	//{
	//	Camera->SetFieldOfView(90.f);
	//}
	//// 타이머 초기화
	//GetWorld()->GetTimerManager().ClearTimer(SlideTimerHandle);
}

void AMyCharacter::UpdateSlide(float Value)
{
	if (HasAuthority()) // 서버 확인
	{
		// 커브 비율로 계산 (1.0 ~ 0.1로 설정했음)
		float NewSpeed = SlideInitialSpeed * Value;
		if (GEngine)
		{
			// 서버 화면 왼쪽 위에 NewSpeed와 SlideDirection 값을 초록색으로 표시합니다.
			FString DebugMsg = FString::Printf(TEXT("SERVER SLIDE - Speed: %.2f, Direction: %s"), NewSpeed, *SlideDirection.ToString());
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, DebugMsg);
		}
		//FVector Direction = GetCharacterMovement()->GetLastUpdateVelocity().GetSafeNormal();
		GetCharacterMovement()->Velocity = SlideDirection * NewSpeed;
	}

	// FOV를 속도 비율에 따라서 조절 하도록
	if (auto* Camera = FindComponentByClass<UCameraComponent>())
	{
		// 포브 설정
		float DefaultFOV = 90.f;
		float MaxFOV = 105.f;

		// Value에 따라서 변경 보간 추가
		float NewFOV = FMath::Lerp(DefaultFOV, MaxFOV, Value);
		Camera->SetFieldOfView(NewFOV);
	}
}

void AMyCharacter::Fire()
{
	if (!bCanFire || !CurrentWeaponData)
	{
		return;
	}

	bCanFire = false;
	// 쿨다운 타이머
	GetWorld()->GetTimerManager().SetTimer(FireCooldownTimerHandle, this, &AMyCharacter::ResetFireCooldown, CurrentWeaponData->FireCooldown, false);

	FVector CamLocation;
	FRotator CamRotation;
	Controller->GetPlayerViewPoint(CamLocation, CamRotation);
	FVector TraceEnd = CamLocation + (CamRotation.Vector() * CurrentWeaponData->MaxRange);
	
	// 서버 RPC함수를 호출, 서버에 발사 요청
	Server_Fire(CamLocation, TraceEnd);
}

void AMyCharacter::Server_Fire_Implementation(const FVector& TraceStart, const FVector& TraceEnd)
{
	FHitResult HitResult;

	// 디버그: 라인으로 총알 궤적 표시 (빨간선이 2초 표시)
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f, 0, 5.0f);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility))
	{
		// 디버그: 스피어로 총알이 맞은 위치에 표시
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 25.0f, 12, FColor::Green, false, 2.0f);
		// 디버그: 맞은 액터 이름 표시
		if (GEngine && HitResult.GetActor())
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Hit(%s)"), *HitResult.GetActor()->GetName()));
		}

		float DamageToApply = CurrentWeaponData->BaseDamage;
		FString HitBone = HitResult.BoneName.ToString();

		if (HitBone.Contains(TEXT("head"), ESearchCase::IgnoreCase))
		{
			DamageToApply *= CurrentWeaponData->HeadDamageMultiplier;
			UE_LOG(LogTemp, Warning, TEXT("HeadShot"));
		}
		else if (HitBone.Contains(TEXT("arm")) || HitBone.Contains(TEXT("leg")))
		{
			DamageToApply *= CurrentWeaponData->LimbDamageMultiplier;
			UE_LOG(LogTemp, Warning, TEXT("Limb Hit"));
		}
		// Point 정보로 데미지 적용: 맞은 액터, 적용 데미지, 총알 방향, 피격 정보 전달, 공격자 컨트롤러, 공격자 액터, 데미지 타입 클래스(기본값 설정)
		UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), DamageToApply, (TraceEnd - TraceStart).GetSafeNormal(), HitResult, GetController(), this, nullptr);
	}
}

void AMyCharacter::ResetFireCooldown()
{
	bCanFire = true;
}

void AMyCharacter::Server_WallJump_Implementation(const FVector& LaunchVelocity)
{
	// 벽 점프 실행
	LaunchCharacter(LaunchVelocity, true, false);
	// 벽 점프후 더블 점프(스택이 있다는 하에) 점프 횟수 초기화 
	JumpCurrentCount = JumpMaxCount - 1;
}

void AMyCharacter::Jump()
{
	// 케릭터 컴포넌트 가져오기
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	// 케릭터 컴포넌트 통해서 공중 확인
	if (MoveComp->IsFalling())
	{
		// 라인 트레이스 (전방위)
		FVector Start = GetActorLocation(); // 케릭터 위치
		FVector Right = GetActorRightVector(); 
		FVector Forward = GetActorForwardVector();
		FHitResult HitResult;

		// 오른쪽, 왼쪽 벽을 순서대로 체크
		FVector Directions[] = { Right, -Right, Forward, -Forward };
		for (const FVector& Direction : Directions)
		{
			// 캐릭터 위치에서 옆으로 100 유닛까지 라인을 그려서 벽과 충돌 확인
			if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, Start + Direction * 100.f, ECC_Visibility))
			{
				// 벽 점프 계산
				FVector WallNormal = HitResult.Normal;
				FVector LaunchVelocity = WallNormal * WallJumpHorizontalForce + FVector::UpVector * WallJumpUpwardForce;

				// 서버에 벽 점프 실행 요청
				Server_WallJump(LaunchVelocity);
				
				UE_LOG(LogTemp, Warning, TEXT("Wall with Normal: %s"), *WallNormal.ToString());
				
				// 벽점프했으면 점프 함수 바로 종료
				return;
			}
		}
	}
	// 더블 점프 디버그용
	UE_LOG(LogTemp, Warning, TEXT("jump: current jump count %d"), JumpCurrentCount);
	Super::Jump();	
}

void AMyCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Dead"), *GetName());

	// 켑슐 컴포넌트, 메쉬 콜리전 비활성화, 움직임 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	// 입력 비활성화
	if (AController* PlayerController = GetController())
	{
		DisableInput(Cast<APlayerController>(PlayerController));
	}

	GetMesh()->SetSimulatePhysics(true);

	//AController* MyController = GetController();
	//if (MyController)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("%s is dying. Controller is %s Class: %s"), *GetName(), 
	//		*MyController->GetName(),
	//		*MyController->GetClass()->GetName()
	//	);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("%s is dying. Controller is NULL"), *GetName());
	//}

	if (ADeathmatchGameMode* GameMode = GetWorld()->GetAuthGameMode<ADeathmatchGameMode>())
	{
		GameMode->PlayerDead(this, GetController(), LastDamageInstigator, bByHeadshotDead);
	}

	DetachFromControllerPendingDestroy();
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (FinalDamage > 0.f)
	{
		// 데미지 이벤트 ID 판별 (PointDamageEvent와 일치하는가(해드샷은 총으로만 하게끔))
		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			// 데미지 이번트 포인트 데미지 이벤트로 케스트
			FPointDamageEvent const* PointDamageEvent = (FPointDamageEvent const*)&DamageEvent;
			
			// HitInfo의 본네임으로 대가리 판별
			if (PointDamageEvent->HitInfo.BoneName.ToString().Contains(TEXT("head")))
			{
				bByHeadshotDead = true;
			}
			else
			{
				bByHeadshotDead = false;
			}
			
		}
		
		// 데미지 입힌 컨트롤러 저장
		LastDamageInstigator = EventInstigator;
		// 현재 체력에서 받은 데미지만큼 감소
		CurrentHealth -= FinalDamage;
		UE_LOG(LogTemp, Warning, TEXT("%s, %f Damage"), *GetName(), FinalDamage);
		// 체력이 0이하일 경우 죽음 함수 작동
		if (CurrentHealth <= 0.f)
		{
			Die();
		}
	}
	
	return FinalDamage;
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SlideTimeline.TickTimeline(DeltaTime);
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
		// 공격
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMyCharacter::Fire);

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


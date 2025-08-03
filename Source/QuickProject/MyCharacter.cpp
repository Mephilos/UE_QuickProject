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
	FireAction = nullptr;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	CurrentDashCharges = MaxDashCharges; // ���� ���۽� �뽬 ���� ����

	JumpMaxCount = 2;
	// �ɸ��� ������Ʈ ��������
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	// �ִ� �̵��ӵ�
	MoveComp->MaxWalkSpeed = 1500.0f;
	// ���ӵ�
	MoveComp->MaxAcceleration = 2000.0f;
	// ������
	MoveComp->GroundFriction = 100.0f;
	// ������
	MoveComp->BrakingDecelerationWalking = 8000.0f;
	// ���� �����
	MoveComp->AirControl = 0.5f;
	// �߷� ������ ����
	MoveComp->GravityScale = 1.0f;
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
	//Ŀ�� ���� ���ε�, ������Ʈ �Լ� ����
	if (SlideSpeedCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("UpdateSlide"));
		SlideTimeline.AddInterpFloat(SlideSpeedCurve, ProgressFunction);
	}
	// ü�� �ʱ�ȭ(�ִ��)
	CurrentHealth = MaxHealth;
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// ��Ʈ�ѷ� ȸ�� �� ��������
		const FRotator Rotation = Controller->GetControlRotation();
		// ���� ���⸸ �������� ��, ��ġ�� 0��
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// YawRotation���� ���� x y ����
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

void AMyCharacter::Dash_Implementation()
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

		// Z���� ������ 0���� �����ؼ� �����̵� ����
		DashDirection.Z = 0.0f;

		// �ٽ� ����ȭ�ؼ� �뽬 �ӵ��� ������ ����
		DashDirection = DashDirection.GetSafeNormal();
		// �뽬
		LaunchCharacter(DashDirection * DashDistance, true, false);
		
		// ��ٿ� Ÿ�̸Ӵ� ���������� ��� �Ǵ���
		if (HasAuthority())
		{
			// ���� Ÿ�̸Ӱ� ���� ���� �ƴ� ���� ����
			if (!GetWorld()->GetTimerManager().IsTimerActive(DashRechargeCooldownHandle))
			{
				// DashRechargeCooldown ���� �ݺ� ����ǵ���
				GetWorld()->GetTimerManager().SetTimer(DashRechargeCooldownHandle, this, &AMyCharacter::DashRecharge, DashRechargeCooldown, true);
			}
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
	// ���� ���� ����
	FVector DodgeDirection = Direction;
	// ���� ���� �������� Z�� ������ 0 �����̵� ����
	DodgeDirection.Z = 0.0f;

	// ���� ����ȭ(Z�� ����)�ٽ� �ӵ� ����
	DodgeDirection = DodgeDirection.GetSafeNormal();
	
	// ������ �浹���� �ʵ��� Z�� �ӵ��� ����
	LaunchCharacter(DodgeDirection * DodgeDistance, true, false);
}

void AMyCharacter::DodgeForward_Implementation()
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

void AMyCharacter::DodgeBackward_Implementation()
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
		PerformDodge(-GetActorRightVector()); // ������ ������ ������ �ݴ�
		LastLeftTapTime = 0.f;
	}
	else
	{
		LastLeftTapTime = GetWorld()->GetTimeSeconds();
	}
}

// �����̵� ���÷�����Ʈ�� ���� ����
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
	// ĳ���Ͱ� �����ְ� �ӵ��� �ּ� �����̵� �ӵ����� ���� ���� �۵�
	if (GetCharacterMovement()->IsMovingOnGround() && GetCharacterMovement()->Velocity.Size() >= MinSlideSpeed)
	{
		// ������ �����̵� �Ǻ� ������ ����
		bSliding = true;
		// ������ Rep_bSliding �Լ��� ����ȣ���� ��� ����
		OnRep_bSliding();
		// �����̵� ���� ���� �ӵ� ����
		SlideInitialSpeed = GetVelocity().Size2D();
		SlideDirection = GetVelocity().GetSafeNormal();

		// ������ ������ ��Ȱ��ȭ
		GetCharacterMovement()->GroundFriction = 0.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 0.f;
	}
	//if (GetCharacterMovement()->Velocity.Size() >= MinSlideSpeed && GetCharacterMovement()->IsMovingOnGround())
	//{
	//	// ���� ������ ����
	//	DefaultFriction = GetCharacterMovement()->GroundFriction;
	//	// �����̵� ������ ����
	//	GetCharacterMovement()->GroundFriction = SlideFriction;
	//	
	//	Crouch();

	//	//FVector SlideDirection = GetLastMovementInputVector().IsNearlyZero() ? GetActorForwardVector() : GetLastMovementInputVector();

	//	FVector SlideDirection = GetLastMovementInputVector();
	//	// �ɸ����� �Է��� ������
	//	if (SlideDirection.IsNearlyZero())
	//	{
	//		// �� ��������
	//		SlideDirection = GetActorForwardVector();
	//	}
	//	LaunchCharacter(SlideDirection * MinSlideSpeed, false, false);
	//	// ���Ӱ� �߰� ���ؼ� ���� ����
	//	if (auto* Camera = FindComponentByClass<UCameraComponent>())
	//	{
	//		Camera->SetFieldOfView(105.f);
	//	}
	//	// �����̵� Ÿ�̸� ����
	//	GetWorld()->GetTimerManager().SetTimer(SlideTimerHandle, this, &AMyCharacter::StopSlide, 0.75f, false);
	//}
}

void AMyCharacter::StopSlide_Implementation()
{
	bSliding = false;

	OnRep_bSliding();
	// �����̵� ������ ��ġ ����
	GetCharacterMovement()->GroundFriction = 10.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 8000.f;

	/*if (auto* Camera = FindComponentByClass<UCameraComponent>())
	{
		Camera->SetFieldOfView(90.0f);
	}*/
	//// ������ ����
	//GetCharacterMovement()->GroundFriction = DefaultFriction;

	//// ī�޶� ���� �� ����
	//if (auto* Camera = FindComponentByClass<UCameraComponent>())
	//{
	//	Camera->SetFieldOfView(90.f);
	//}
	//// Ÿ�̸� �ʱ�ȭ
	//GetWorld()->GetTimerManager().ClearTimer(SlideTimerHandle);
}

void AMyCharacter::UpdateSlide(float Value)
{
	if (HasAuthority()) // ���� Ȯ��
	{
		// Ŀ�� ������ ��� (1.0 ~ 0.1�� ��������)
		float NewSpeed = SlideInitialSpeed * Value;
		if (GEngine)
		{
			// ���� ȭ�� ���� ���� NewSpeed�� SlideDirection ���� �ʷϻ����� ǥ���մϴ�.
			FString DebugMsg = FString::Printf(TEXT("SERVER SLIDE - Speed: %.2f, Direction: %s"), NewSpeed, *SlideDirection.ToString());
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, DebugMsg);
		}
		//FVector Direction = GetCharacterMovement()->GetLastUpdateVelocity().GetSafeNormal();
		GetCharacterMovement()->Velocity = SlideDirection * NewSpeed;
	}

	// FOV�� �ӵ� ������ ���� ���� �ϵ���
	if (auto* Camera = FindComponentByClass<UCameraComponent>())
	{
		// ���� ����
		float DefaultFOV = 90.f;
		float MaxFOV = 105.f;

		// Value�� ���� ���� ���� �߰�
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
	// ��ٿ� Ÿ�̸�
	GetWorld()->GetTimerManager().SetTimer(FireCooldownTimerHandle, this, &AMyCharacter::ResetFireCooldown, CurrentWeaponData->FireCooldown, false);

	FVector CamLocation;
	FRotator CamRotation;
	Controller->GetPlayerViewPoint(CamLocation, CamRotation);
	FVector TraceEnd = CamLocation + (CamRotation.Vector() * CurrentWeaponData->MaxRange);
	
	// ���� RPC�Լ��� ȣ��, ������ �߻� ��û
	Server_Fire(CamLocation, TraceEnd);
}

void AMyCharacter::Server_Fire_Implementation(const FVector& TraceStart, const FVector& TraceEnd)
{
	FHitResult HitResult;

	// �����: �������� �Ѿ� ���� ǥ�� (�������� 2�� ǥ��)
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f, 0, 5.0f);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility))
	{
		// �����: ���Ǿ�� �Ѿ��� ���� ��ġ�� ǥ��
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 25.0f, 12, FColor::Green, false, 2.0f);
		// �����: ���� ���� �̸� ǥ��
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
		// Point ������ ������ ����: ���� ����, ���� ������, �Ѿ� ����, �ǰ� ���� ����, ������ ��Ʈ�ѷ�, ������ ����, ������ Ÿ�� Ŭ����(�⺻�� ����)
		UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), DamageToApply, (TraceEnd - TraceStart).GetSafeNormal(), HitResult, GetController(), this, nullptr);
	}
}

void AMyCharacter::ResetFireCooldown()
{
	bCanFire = true;
}

void AMyCharacter::Server_WallJump_Implementation(const FVector& LaunchVelocity)
{
	// �� ���� ����
	LaunchCharacter(LaunchVelocity, true, false);
	// �� ������ ���� ����(������ �ִٴ� �Ͽ�) ���� Ƚ�� �ʱ�ȭ 
	JumpCurrentCount = JumpMaxCount - 1;
}

void AMyCharacter::Jump()
{
	// �ɸ��� ������Ʈ ��������
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	// �ɸ��� ������Ʈ ���ؼ� ���� Ȯ��
	if (MoveComp->IsFalling())
	{
		// ���� Ʈ���̽� (������)
		FVector Start = GetActorLocation(); // �ɸ��� ��ġ
		FVector Right = GetActorRightVector(); 
		FVector Forward = GetActorForwardVector();
		FHitResult HitResult;

		// ������, ���� ���� ������� üũ
		FVector Directions[] = { Right, -Right, Forward, -Forward };
		for (const FVector& Direction : Directions)
		{
			// ĳ���� ��ġ���� ������ 100 ���ֱ��� ������ �׷��� ���� �浹 Ȯ��
			if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, Start + Direction * 100.f, ECC_Visibility))
			{
				// �� ���� ���
				FVector WallNormal = HitResult.Normal;
				FVector LaunchVelocity = WallNormal * WallJumpHorizontalForce + FVector::UpVector * WallJumpUpwardForce;

				// ������ �� ���� ���� ��û
				Server_WallJump(LaunchVelocity);
				
				UE_LOG(LogTemp, Warning, TEXT("Wall with Normal: %s"), *WallNormal.ToString());
				
				// ������������ ���� �Լ� �ٷ� ����
				return;
			}
		}
	}
	// ���� ���� ����׿�
	UE_LOG(LogTemp, Warning, TEXT("jump: current jump count %d"), JumpCurrentCount);
	Super::Jump();	
}

void AMyCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Dead"), *GetName());

	// �ν� ������Ʈ, �޽� �ݸ��� ��Ȱ��ȭ, ������ ��Ȱ��ȭ
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	// �Է� ��Ȱ��ȭ
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
		// ������ �̺�Ʈ ID �Ǻ� (PointDamageEvent�� ��ġ�ϴ°�(�ص弦�� �����θ� �ϰԲ�))
		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			// ������ �̹�Ʈ ����Ʈ ������ �̺�Ʈ�� �ɽ�Ʈ
			FPointDamageEvent const* PointDamageEvent = (FPointDamageEvent const*)&DamageEvent;
			
			// HitInfo�� ���������� �밡�� �Ǻ�
			if (PointDamageEvent->HitInfo.BoneName.ToString().Contains(TEXT("head")))
			{
				bByHeadshotDead = true;
			}
			else
			{
				bByHeadshotDead = false;
			}
			
		}
		
		// ������ ���� ��Ʈ�ѷ� ����
		LastDamageInstigator = EventInstigator;
		// ���� ü�¿��� ���� ��������ŭ ����
		CurrentHealth -= FinalDamage;
		UE_LOG(LogTemp, Warning, TEXT("%s, %f Damage"), *GetName(), FinalDamage);
		// ü���� 0������ ��� ���� �Լ� �۵�
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
		// ����
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


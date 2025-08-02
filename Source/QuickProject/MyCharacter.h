#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "WeaponDataAsset.h"
#include "MyCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class QUICKPROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
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


	UPROPERTY(EditAnywhere, Category = "Movement-Dodge")
	float DodgeDistance = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Movement-Dodge")
	float DodgeTapTime = 0.25f;

	float LastForwardTapTime = 0.f;
	float LastBackwardTapTime = 0.f;
	float LastRightTapTime = 0.f;
	float LastLeftTapTime = 0.f;

	// 앉기 (슬라이딩)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Movement-Slide")
	float MinSlideSpeed = 100.0f; // 슬라이딩 발동 가능 속도

	UPROPERTY(EditAnywhere, Category = "Movement-Slide")
	float SlideFriction = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Movement-Slide")
	UCurveFloat* SlideSpeedCurve;

	FTimeline SlideTimeline;
	FTimerHandle SlideTimerHandle;
	float SlideInitialSpeed; // 슬라이딩 시점 속도를 저장하는 변수
	float DefaultFriction; // 기존 마찰력 보존 하기 위한 변수

	// 벽점프
	UPROPERTY(EditAnywhere, Category = "Movement-WallJump")
	float WallJumpHorizontalForce = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "Movement-WallJump")
	float WallJumpUpwardForce = 1000.0f;

	// 대쉬
	UPROPERTY(EditAnywhere, Category = "Movement-Dash")
	float DashDistance = 5000.0f;
	UPROPERTY(EditAnywhere, Category = "Movement-Dash")
	float DashRechargeCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement-Dash")
	int32 MaxDashCharges = 3;

	UPROPERTY(EditAnywhere, Category = "Movement-Dash")
	int32 CurrentDashCharges; // 대쉬 스택

	FTimerHandle DashRechargeCooldownHandle;

	// 무기 셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	UWeaponDataAsset* CurrentWeaponData;

	FTimerHandle FireCooldownTimerHandle;
	bool bCanFire = true;

	// 바이탈 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health") 
	float MaxHealth = 100.0f; // 체력

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	// 마지막으로 나를 공격한 컨트롤러(스코어 판정위해)
	AController* LastDamageInstigator;
	bool bByHeadshotDead = false; // 해드샷 판정 변수


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
	UFUNCTION()
	void UpdateSlide(float Value);

	// 기본 이동 시점 함수
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// 무기 관련 함수
	void Fire();
	void ResetFireCooldown();
	
	// 바이탈 관련 함수
	void Die();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
public:

	virtual void Tick(float DeltaTime) override;
	virtual void Jump() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
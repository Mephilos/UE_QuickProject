// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter_Test.h"

// Sets default values
AMyCharacter_Test::AMyCharacter_Test()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	JumpMaxCount = 2;
}

// Called when the game starts or when spawned
void AMyCharacter_Test::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCharacter_Test::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter_Test::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}


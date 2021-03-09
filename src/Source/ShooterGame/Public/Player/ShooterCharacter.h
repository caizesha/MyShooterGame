// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTERGAME_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//process character's move
	void MoveForward(float value);

	void MoveRight(float value);

protected:
	//创建蓝图可编辑的相机组件
	UPROPERTY(EditAnyWhere, Category = "Camera")
		UCameraComponent *FPCamera;

	//加入第一人称手臂Mesh
	UPROPERTY(EditAnyWhere, Category = "Mesh")
		USkeletalMeshComponent* FPArm;
	
};

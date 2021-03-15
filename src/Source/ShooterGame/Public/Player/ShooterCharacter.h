// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"


class AShooterWeapon;

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

	virtual void PostInitializeComponents() override;
	//process character's move
	void MoveForward(float value);

	void MoveRight(float value); 

	USkeletalMeshComponent* GetFirstPersonMesh();

	FName GetWeaponAttachPoint() const;
protected:
	//������ͼ�ɱ༭��������
	UPROPERTY(EditAnyWhere, Category = "Camera")
		UCameraComponent *FPCamera;

	//�����һ�˳��ֱ�Mesh
	UPROPERTY(EditAnyWhere, Category = "Mesh")
		USkeletalMeshComponent* FPArm;

	UPROPERTY(EditAnyWhere, Category = "Weapon")
	TSubclassOf<AShooterWeapon> ShooterWeaponClass;

	AShooterWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachPoint;
	
};

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

	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation);
	//process character's move
	void MoveForward(float value);

	void MoveRight(float value); 

	USkeletalMeshComponent* GetFirstPersonMesh();

	FName GetWeaponAttachPoint() const;

	UFUNCTION(BlueprintCallable, Category="Weapon")
	FRotator GetAimOffset() const;

	void OnStartTargeting();

	void OnStopTargeting();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool GetIsTargeting() const;

	void SetIsTargeting(bool bNewIsTargeting);

	void OnStartFire();

	void OnStopFire();

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	int32 GetCurrentHealth() const;

	int32 GetMaxHealth() const;

	AShooterWeapon* GetCurrentWeapon();

	bool CanFire() const;

	bool IsAlive() const;

	void OnReload();

protected:
	//创建蓝图可编辑的相机组件
	//UPROPERTY(EditAnyWhere, Category = "Camera")
	//UCameraComponent *FPCamera;

	//加入第一人称手臂Mesh
	UPROPERTY(EditAnyWhere, Category = "Mesh")
	USkeletalMeshComponent* FPArm;

	UPROPERTY(EditAnyWhere, Category = "Weapon")
	TSubclassOf<AShooterWeapon> ShooterWeaponClass;

	AShooterWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachPoint;

	bool bIsTargeting;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;
};

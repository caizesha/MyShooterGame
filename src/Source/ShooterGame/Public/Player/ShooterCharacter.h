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

	USkeletalMeshComponent* GetSpecificPawnMesh(bool bWant1P = true) const;

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

	void SpawnDefaultInventory();

	void AddWeaponObject(AShooterWeapon* weapon);
	
	void OnNextWeapon();

	//��д���ಥ����̫�涯���Ľӿ�
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate=1.0f, FName StartSectionName = NAME_None) override;

	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;
protected:

	//������ͼ�ɱ༭��������
	//UPROPERTY(EditAnyWhere, Category = "Camera")
	//UCameraComponent *FPCamera;

	//�����һ�˳��ֱ�Mesh
	UPROPERTY(EditAnyWhere, Category = "Mesh")
	USkeletalMeshComponent* FPArm;

	//Inventory
	//���������������Ͷ������飬��¶���༭������
	UPROPERTY(EditAnyWhere, Category = "Inventory")
	TArray<TSubclassOf<AShooterWeapon>> DefaultInventoryClass;
	//���屳�������������洢��������
	//��������ʱ�������ǲ���Ҫ�洢����ʱ����
	UPROPERTY(Transient)
	TArray<AShooterWeapon*> Inventory;

	AShooterWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachPoint;

	bool bIsTargeting;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	void EquipWeapon(AShooterWeapon* weapon);

	void SetCurrentWeapon(AShooterWeapon* NewWeapon, AShooterWeapon* LastWeapon);
};

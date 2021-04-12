// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TakeHitInfo.h"
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

	virtual void Destroyed() override;

	bool IsFirstPerson() const;

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

	void DestroyedInventory();

	void AddWeaponObject(AShooterWeapon* weapon);

	void RemoveWeaponObject(AShooterWeapon* weapon);

	void OnNextWeapon();

	//重写父类播放蒙太奇动画的接口
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate=1.0f, FName StartSectionName = NAME_None) override;

	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;
protected:

	//创建蓝图可编辑的相机组件
	//UPROPERTY(EditAnyWhere, Category = "Camera")
	//UCameraComponent *FPCamera;

	//加入第一人称手臂Mesh
	UPROPERTY(EditAnyWhere, Category = "Mesh")
	USkeletalMeshComponent* FPArm;

	//Inventory
	//定义武器反射类型对象数组，暴露到编辑器配置
	UPROPERTY(EditAnyWhere, Category = "Inventory")
	TArray<TSubclassOf<AShooterWeapon>> DefaultInventoryClass;
	//定义背包数组容器，存储武器对象
	//程序运行时创建，是不需要存储的临时数据
	UPROPERTY(Transient, Replicated)
	TArray<AShooterWeapon*> Inventory;

	//同步枪
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	AShooterWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachPoint;

	bool bIsTargeting;
	
	//同步生命值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Health")
	float Health;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakenHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	void EquipWeapon(AShooterWeapon* weapon);

	void SetCurrentWeapon(AShooterWeapon* NewWeapon, AShooterWeapon* LastWeapon);

	//响应换枪，参数由系统自己传递
	UFUNCTION()
	void OnRep_CurrentWeapon(AShooterWeapon* LastWeapon);

	//同步伤害
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const & DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	//同步伤害到客户端
	void ReplicateHit(float DamageTaken, struct FDamageEvent const & DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled);
	
	UFUNCTION()
	void OnRep_LastTakenHitInfo();

	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(AShooterWeapon* NewWeapon);
};

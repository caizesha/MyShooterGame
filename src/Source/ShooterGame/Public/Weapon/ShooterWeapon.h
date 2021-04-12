// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ShooterWeapon.generated.h"

class AShooterCharacter;

//定义武器状态
namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,//装子弹
		Equiping//装备武器
	};
}

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 AmmoPerClip;//弹夹容量

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 InitialClips;//初始总弹夹数量

	UPROPERTY(EditDefaultsOnly, Category = "WeaponState")
	float TimeBetweenShots;

	FWeaponData()
	{
		AmmoPerClip = 100;
		InitialClips = 20;
		TimeBetweenShots = 0.1f;
	}
};


USTRUCT()
struct FWeaponAnim 
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* Pawn1P;

	//添加第三人称开火动画资源
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* Pawn3P;
};

UCLASS()
class SHOOTERGAME_API AShooterWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnEquipFinished();

	//客户端开火通知服务器
	UFUNCTION(reliable, server, WithValidation)
	void ServerStartFire();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStopFire();

	UFUNCTION()
	void OnRep_Fire();

	bool ShouldDealDamage(AActor* TestActor) const;

	UFUNCTION(reliable, server, WithValidation)
	void ServerStartReload();

	UFUNCTION()
	void OnRep_Reload();

	UFUNCTION(reliable, server, WithValidation)
	void ServerHandleFiring();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetPawnOwner(AShooterCharacter* pawnOwner);

	void AttachMeshToPawn();

	void DetachMeshFromPawn();

	//获取子弹发射方向
	FVector GetAdjustAim();

	virtual void FireWeapon();

	USkeletalMeshComponent* GetWeaponMesh()const;

	FVector GetMuzzleLocation() const;

	FVector GetMuzzleDirection() const;

	void StartFire();

	void StopFire();

	void SimulateWeaponFire();

	void StopSimulateWeaponFire();

	UAudioComponent* PlayWeaponSound(USoundCue *Sound);
	
	//int GetCurrentAmmoCount();

	//int GetMaxAmmoCount();

	void DetermineWeaponState();

	bool CanFire() const;

	bool CanReload() const;
	
	void SetWeaponState(EWeaponState::Type NewState);

	void HandleDependCurrentState();

	void HandleStartFireState();

	void HandleEndFireState();

	void HandleStartReloadState();

	void HandleEndReloadState();

	void HandleStartEquipState();

	void HandleEndEquipState();

	void OnEquip(const AShooterWeapon* _LastWeapon);

	void OnUnEquip();

	void StartReload();

	void StopReload();

	void ReloadWeapon();

	void HandleFiring();

	int32 GetCurrentAmmo() const;
	
	int32 GetCurrentAmmoInClip() const;

	int32 GetAmmoPerClip() const;

	void UsedAmmo();

	//相机伤害位置调整算法
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	EWeaponState::Type GetCurrentWeaponState() const;

	float PlayMontageAnimation(const FWeaponAnim& Animation);

	void StopMontageAnimation(const FWeaponAnim& Animation);

	virtual void OnEnterInventory(AShooterCharacter * NewOwner);
	virtual void OnLeaveInventory();

	bool IsAttachedToPawn() const;

	virtual void Destroyed() override;
protected:
	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* WeaponMesh1P;

	//创建第三人称武器
	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* WeaponMesh3P;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName MuzzleAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue *FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue *EquipSound;


	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue *ReloadSound;

	//UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	//int CurrentAmmoCount;
	
	//添加蒙太奇动画资源
	bool bPlayingFireAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FWeaponAnim FireAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FWeaponAnim ReloadAnim;
		
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FWeaponAnim EquipAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FWeaponData WeaponConfig;

	//同步子弹数量
	//当前子弹总数量
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmo;
	
	//当前弹夹子弹数量
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmoInClip;
	
	AShooterCharacter* PawnOwner;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UCameraShake> FireCameraShake;

	EWeaponState::Type CurrentState;
private:
	
	bool bIsEquiped;
	//是否需要装子弹
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Reload)
	bool bPendingReload;
	
	//设置同步属性
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Fire)
	bool bWantToFire;
	
	bool bPendingEquip;
	bool bRefiring;

	EWeaponState::Type PreState;

	AShooterWeapon* LastWeapon;

	FTimerHandle TimerHandle_OnEquipFinished;

	FTimerHandle TimerHandle_ReloadWeapon;

	FTimerHandle TimerHandle_StopReload;

	FTimerHandle TimerHandle_HandleFiring;
};

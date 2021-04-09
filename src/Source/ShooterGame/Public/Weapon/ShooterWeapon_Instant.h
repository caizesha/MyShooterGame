// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon/ShooterWeapon.h"
#include "ShooterWeapon_Instant.generated.h"

/**
 * 
 */
//定义客户端碰撞信息
USTRUCT()
struct FInstantHitInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float ReticleSpread;

	UPROPERTY()
	int32 RandomSeed;
};


//及时伤害武器的配置
USTRUCT()
struct FInstantWeaponData
{
	GENERATED_USTRUCT_BODY()

	//张角范围
	UPROPERTY(EditDefaultsOnly, Category = "WeaponState")
	float WeaponSpread;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponState")
	float WeaponRange;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponState")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponState")
	int32 HitDamage;
};

UCLASS()
class SHOOTERGAME_API AShooterWeapon_Instant : public AShooterWeapon
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FInstantWeaponData InstantConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	UParticleSystem* TrailFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FName TrailTargetParam;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<class AShooterImpactEffect> ImpactTemplate;

	//定义同步变量
	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
	FInstantHitInfo HitNotify;

protected:
	UFUNCTION()
	void OnRep_HitNotify();

	void SimulateInstantHit(const FVector& ShootOrigin, float ReticleSpread,int32 RandomSeed);
public:
	void FireWeapon();

	//预留：为了拓展后面网络对战服务器与客户端的不同处理
	//处理及时碰撞
	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);
	
	void ProcessInstantHit_Confirm(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	void DealDamage(const FHitResult& Impact, const FVector ShootDir);

	//追踪光束特效
	void SpawnTrailEffect(const FVector& EndPoint);

	//击中火花特效
	void SpawnImpactEffect(const FHitResult& Impact);
};

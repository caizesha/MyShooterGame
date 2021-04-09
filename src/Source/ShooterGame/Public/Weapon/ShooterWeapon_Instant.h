// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon/ShooterWeapon.h"
#include "ShooterWeapon_Instant.generated.h"

/**
 * 
 */
//����ͻ�����ײ��Ϣ
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


//��ʱ�˺�����������
USTRUCT()
struct FInstantWeaponData
{
	GENERATED_USTRUCT_BODY()

	//�ŽǷ�Χ
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

	//����ͬ������
	UPROPERTY(Transient, ReplicatedUsing = OnRep_HitNotify)
	FInstantHitInfo HitNotify;

protected:
	UFUNCTION()
	void OnRep_HitNotify();

	void SimulateInstantHit(const FVector& ShootOrigin, float ReticleSpread,int32 RandomSeed);
public:
	void FireWeapon();

	//Ԥ����Ϊ����չ���������ս��������ͻ��˵Ĳ�ͬ����
	//����ʱ��ײ
	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);
	
	void ProcessInstantHit_Confirm(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	void DealDamage(const FHitResult& Impact, const FVector ShootDir);

	//׷�ٹ�����Ч
	void SpawnTrailEffect(const FVector& EndPoint);

	//���л���Ч
	void SpawnImpactEffect(const FHitResult& Impact);
};

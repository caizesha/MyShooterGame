// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon/ShooterWeapon.h"
#include "ShooterWeapon_Projectile.generated.h"

/**
 * 
 */

USTRUCT()
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

	//�ռ��ӵ��ķ�����Ϣ
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AShooterProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	int32 ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	int32 ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ProjectileLife;

	FProjectileWeaponData()
	{
		ProjectileClass = NULL;
		ExplosionDamage = 100;
		ExplosionRadius = 300;
		DamageType = UDamageType::StaticClass();
		ProjectileLife = 10.0f;
	}
};

UCLASS()
class SHOOTERGAME_API AShooterWeapon_Projectile : public AShooterWeapon
{
	GENERATED_BODY()

public:
	void FireWeapon();

	void ApplyWeaponConfig(FProjectileWeaponData& Data);

	UFUNCTION(reliable, server, WithValidation)
	void ServerFireProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FProjectileWeaponData ProjectileConfig;
	
};

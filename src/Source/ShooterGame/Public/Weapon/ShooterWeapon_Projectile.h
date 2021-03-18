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

	//收集子弹的反射信息
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AShooterProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	int32 ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	int32 ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<UDamageType> DamageType;
};

UCLASS()
class SHOOTERGAME_API AShooterWeapon_Projectile : public AShooterWeapon
{
	GENERATED_BODY()

public:
	void FireWeapon();

	void ApplyWeaponConfig(FProjectileWeaponData& Data);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FProjectileWeaponData ProjectileConfig;
	
};

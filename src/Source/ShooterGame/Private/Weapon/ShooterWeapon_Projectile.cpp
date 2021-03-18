// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeapon_Projectile.h"
#include "ShooterProjectile.h"

void AShooterWeapon_Projectile::FireWeapon()
{
	//获取子弹发射方向
	FVector ShooterDirection, MuzzleLocation;
	ShooterDirection = GetAdjustAim();
	//获取子弹发射位置
	MuzzleLocation = GetMuzzleLocation();

	//子弹生成位置以及方向
	FTransform SpawnTM(ShooterDirection.Rotation(), MuzzleLocation);
	//延迟生成对象，设置子弹对象相关设置值
	AShooterProjectile* Projectile = Cast<AShooterProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));

	if (Projectile)
	{
		//保持instigator与对象一致
		Projectile->Instigator = Instigator;
		Projectile->SetOwner(this);
		Projectile->InitializeVelocity(ShooterDirection);
		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}
}

void AShooterWeapon_Projectile::ApplyWeaponConfig(FProjectileWeaponData& Data)
{
	Data = ProjectileConfig;
}


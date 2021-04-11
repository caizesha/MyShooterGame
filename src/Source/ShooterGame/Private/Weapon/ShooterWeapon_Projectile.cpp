// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeapon_Projectile.h"
#include "ShooterProjectile.h"

void AShooterWeapon_Projectile::FireWeapon()
{
	/*if (CurrentAmmoCount <= 0)
	{
		return;
	}
	CurrentAmmoCount--;*/
	
	//获取子弹发射方向
	FVector ShooterDirection, MuzzleLocation;
	ShooterDirection = GetAdjustAim();

	//发射范围
	const float ProjectileAdjustRange = 10000.0f;
	const FVector StartTrace = GetCameraDamageStartLocation(ShooterDirection);
	const FVector EndTrace = StartTrace + ShooterDirection * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	//获取子弹发射位置
	MuzzleLocation = GetMuzzleLocation();

	if (Impact.bBlockingHit)
	{
		FVector AdjustedDir = (Impact.ImpactPoint - MuzzleLocation).GetSafeNormal();

		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShooterDirection);

		bool bWeaponPenetration = false;
		//钝角：敌人离得非常近，肯定会插入敌人mesh
		if (DirectionDot < 0.0f)
		{
			bWeaponPenetration = true;
		}
		//经验值
		else if (DirectionDot < 0.5f)
		{
			FVector MuzzleStartTrace = MuzzleLocation - GetMuzzleDirection() * 150.0f;
			FVector MuzzleEndTrace = MuzzleLocation;
			FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleStartTrace);
			if (MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}
		if (bWeaponPenetration)
		{
			//调整出射位置
			MuzzleLocation = Impact.ImpactPoint - ShooterDirection * 10.0f;
		}
		else
		{
			ShooterDirection = AdjustedDir;
		}
	}

	ServerFireProjectile(MuzzleLocation, ShooterDirection);


}

void AShooterWeapon_Projectile::ApplyWeaponConfig(FProjectileWeaponData& Data)
{
	Data = ProjectileConfig;
}

bool AShooterWeapon_Projectile::ServerFireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void AShooterWeapon_Projectile::ServerFireProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
	//子弹生成位置以及方向
	FTransform SpawnTM(ShootDir.Rotation(), Origin);
	//延迟生成对象，设置子弹对象相关设置值
	AShooterProjectile* Projectile = Cast<AShooterProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));

	if (Projectile)
	{
		//保持instigator与对象一致
		Projectile->Instigator = Instigator;
		Projectile->SetOwner(this);
		Projectile->InitializeVelocity(ShootDir);
		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}
}
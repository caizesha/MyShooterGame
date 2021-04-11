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
	
	//��ȡ�ӵ����䷽��
	FVector ShooterDirection, MuzzleLocation;
	ShooterDirection = GetAdjustAim();

	//���䷶Χ
	const float ProjectileAdjustRange = 10000.0f;
	const FVector StartTrace = GetCameraDamageStartLocation(ShooterDirection);
	const FVector EndTrace = StartTrace + ShooterDirection * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	//��ȡ�ӵ�����λ��
	MuzzleLocation = GetMuzzleLocation();

	if (Impact.bBlockingHit)
	{
		FVector AdjustedDir = (Impact.ImpactPoint - MuzzleLocation).GetSafeNormal();

		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShooterDirection);

		bool bWeaponPenetration = false;
		//�۽ǣ�������÷ǳ������϶���������mesh
		if (DirectionDot < 0.0f)
		{
			bWeaponPenetration = true;
		}
		//����ֵ
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
			//��������λ��
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
	//�ӵ�����λ���Լ�����
	FTransform SpawnTM(ShootDir.Rotation(), Origin);
	//�ӳ����ɶ��������ӵ������������ֵ
	AShooterProjectile* Projectile = Cast<AShooterProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));

	if (Projectile)
	{
		//����instigator�����һ��
		Projectile->Instigator = Instigator;
		Projectile->SetOwner(this);
		Projectile->InitializeVelocity(ShootDir);
		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
	}
}
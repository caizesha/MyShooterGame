// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeapon_Projectile.h"
#include "ShooterProjectile.h"

void AShooterWeapon_Projectile::FireWeapon()
{
	//��ȡ�ӵ����䷽��
	FVector ShooterDirection, MuzzleLocation;
	ShooterDirection = GetAdjustAim();
	//��ȡ�ӵ�����λ��
	MuzzleLocation = GetMuzzleLocation();

	//�ӵ�����λ���Լ�����
	FTransform SpawnTM(ShooterDirection.Rotation(), MuzzleLocation);
	//�ӳ����ɶ��������ӵ������������ֵ
	AShooterProjectile* Projectile = Cast<AShooterProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));

	if (Projectile)
	{
		//����instigator�����һ��
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


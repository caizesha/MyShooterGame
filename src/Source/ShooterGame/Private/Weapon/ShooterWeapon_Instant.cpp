// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterCharacter.h"
#include "ShooterWeapon_Instant.h"
#include "ShooterImpactEffect.h"

void AShooterWeapon_Instant::FireWeapon()
{
	//
	//�����ӵ����䷽��
	//

	//��ȡ���ֵ
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	//����Բ׶���
	const float CurrentSpread = InstantConfig.WeaponSpread;
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread / 2.0f);
	//��ȡ��׼���������ӵ�������䷽��
	const FVector AimDir = GetAdjustAim();
	FVector ShooterDirection = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);

	//
	//�����˺�����
	//

	//��ȡ�ӵ�����λ��
	const float WeaponRange = InstantConfig.WeaponRange;
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	const FVector EndTrace = StartTrace + ShooterDirection * WeaponRange;
	//��ȡ�˺�����
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	//
	//�����˺�����
	//
	ProcessInstantHit(Impact, StartTrace, ShooterDirection, RandomSeed, CurrentSpread);

}

void AShooterWeapon_Instant::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	ProcessInstantHit_Confirm(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}


void AShooterWeapon_Instant::ProcessInstantHit_Confirm(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	//ȷ��ֻ�ɷ�����/����������˺�
	if (ShouldDealDamage(Impact.GetActor()))
	{
		//����ֵ����
		DealDamage(Impact,ShootDir);
	}

	//������֪ͨ�ͻ���
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = Origin;
		HitNotify.RandomSeed = RandomSeed;
		HitNotify.ReticleSpread = ReticleSpread;
	}

	/*����������Ч*/
	const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
	//������Ч��ֹ��
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	SpawnTrailEffect(EndPoint);
	
	/*���ɻ�����Ч*/
	SpawnImpactEffect(Impact);
}

void AShooterWeapon_Instant::DealDamage(const FHitResult& Impact, const FVector ShootDir)
{
	FPointDamageEvent PointDamage;
	PointDamage.DamageTypeClass = InstantConfig.DamageType;
	PointDamage.HitInfo = Impact;
	PointDamage.ShotDirection = ShootDir;
	PointDamage.Damage = InstantConfig.HitDamage;

	if (Impact.GetActor() != nullptr)
	{
		Impact.GetActor()->TakeDamage(PointDamage.Damage, PointDamage, PawnOwner->Controller, this);
	}

}

void AShooterWeapon_Instant::SpawnTrailEffect(const FVector& EndPoint)
{
	if (TrailFX)
	{
		const FVector Origin = GetMuzzleLocation();
		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin);
		if (TrailPSC)
		{
			TrailPSC->SetVectorParameter(TrailTargetParam,EndPoint);
		}
	}
}

void AShooterWeapon_Instant::SpawnImpactEffect(const FHitResult& Impact)
{
	if (Impact.bBlockingHit && ImpactTemplate)
	{
		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(),Impact.ImpactPoint);
		AShooterImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AShooterImpactEffect>(ImpactTemplate, SpawnTransform);
		if (EffectActor)
		{
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}

}

//������ײ��Ϣ����֪ͨ�ͻ��ˣ�ʹ��ģ�⿪��������Ч
void AShooterWeapon_Instant::OnRep_HitNotify()
{
	SimulateInstantHit(HitNotify.Origin, HitNotify.ReticleSpread, HitNotify.RandomSeed);
}

void AShooterWeapon_Instant::SimulateInstantHit(const FVector& ShootOrigin, float ReticleSpread, int32 RandomSeed)
{
	FRandomStream WeaponRandomStream(RandomSeed);
	//����Բ׶���
	const float ConeHalfAngle = FMath::DegreesToRadians(ReticleSpread / 2.0f);
	//��ȡ��׼���������ӵ�������䷽��
	const FVector AimDir = GetAdjustAim();
	FVector ShooterDirection = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);

	//��ȡ�ӵ�����λ��
	const float WeaponRange = InstantConfig.WeaponRange;
	const FVector StartTrace = ShootOrigin;
	const FVector EndTrace = StartTrace + ShooterDirection * WeaponRange;
	//��ȡ�˺�����
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	//����ײ��
	if (Impact.bBlockingHit)
	{
		SpawnImpactEffect(Impact);
		SpawnTrailEffect(Impact.ImpactPoint);
	}
	else
	{
		SpawnTrailEffect(EndTrace);
	}
}

//��ȡ��������ͬ�����ԣ��ú�����ϵͳ����
void AShooterWeapon_Instant::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//����ͬ��������Owner�ĸ���
	DOREPLIFETIME_CONDITION(AShooterWeapon_Instant, HitNotify, COND_SkipOwner);
}
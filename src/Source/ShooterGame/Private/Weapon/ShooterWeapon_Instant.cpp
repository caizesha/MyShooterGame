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
	if (PawnOwner && PawnOwner->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			ServerNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
		}
		else if (Impact.GetActor() == NULL)
		{
			if (Impact.bBlockingHit)
			{
				ServerNotifyHit(Impact, ShootDir, RandomSeed, ReticleSpread);
			}
		}
	}
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

bool AShooterWeapon_Instant::ServerNotifyHit_Validate(const FHitResult& Impact, const FVector_NetQuantizeNormal& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	return true;
}

void AShooterWeapon_Instant::ServerNotifyHit_Implementation(const FHitResult& Impact, const FVector_NetQuantizeNormal& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread*PI / 180.0f));
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		//��ȡ����ֵ
		const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > InstantConfig.AllowedViewDotHitDir - WeaponAngleDot)//ǹû�в���mesh�������������
		{
			if (CurrentState != EWeaponState::Idle)
			{
				//�����ǽ��
				if (Impact.GetActor() == NULL)
				{
					if (Impact.bBlockingHit)
					{
						ProcessInstantHit_Confirm(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
					}
				}	
				//�������̬����
				else if (Impact.GetActor()->IsRootComponentStatic() && Impact.GetActor()->IsRootComponentStationary())
				{
					ProcessInstantHit_Confirm(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
				}
				//���������
				else
				{
					//��ȡ��Χ��
					const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

					//�����Χ�г���
					FVector BoxExtent = (HitBox.Max - HitBox.Min)*0.5f;
					//�԰�Χ�н��е�������
					BoxExtent *= InstantConfig.ClientSideHitLeeWay;

					//��ֹһЩ�ǳ����Ķ��󣬷�����㲻��׼
					BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
					BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
					BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);
					
					//�����Χ������
					const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5f;
					//�ڰ�Χ�з�Χ��������˺�����
					if (FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X
						&& FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y
						&& FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z)
					{
						ProcessInstantHit_Confirm(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
					}
				} 

			}
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
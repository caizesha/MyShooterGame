// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterCharacter.h"
#include "ShooterWeapon_Instant.h"
#include "ShooterImpactEffect.h"

void AShooterWeapon_Instant::FireWeapon()
{
	//
	//计算子弹发射方向
	//

	//获取随机值
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	//计算圆锥半角
	const float CurrentSpread = InstantConfig.WeaponSpread;
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread / 2.0f);
	//获取瞄准方向，生成子弹随机发射方向
	const FVector AimDir = GetAdjustAim();
	FVector ShooterDirection = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);

	//
	//计算伤害对象
	//

	//获取子弹发射位置
	const float WeaponRange = InstantConfig.WeaponRange;
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	const FVector EndTrace = StartTrace + ShooterDirection * WeaponRange;
	//获取伤害对象
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	//
	//进行伤害处理
	//
	ProcessInstantHit(Impact, StartTrace, ShooterDirection, RandomSeed, CurrentSpread);

}

void AShooterWeapon_Instant::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	ProcessInstantHit_Confirm(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}


void AShooterWeapon_Instant::ProcessInstantHit_Confirm(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	//确保只由服务器/单机版计算伤害
	if (ShouldDealDamage(Impact.GetActor()))
	{
		//生命值处理
		DealDamage(Impact,ShootDir);
	}

	//服务器通知客户端
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = Origin;
		HitNotify.RandomSeed = RandomSeed;
		HitNotify.ReticleSpread = ReticleSpread;
	}

	/*生成武器特效*/
	const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
	//计算特效终止点
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	SpawnTrailEffect(EndPoint);
	
	/*生成击中特效*/
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

//传递碰撞信息，以通知客户端，使其模拟开火粒子特效
void AShooterWeapon_Instant::OnRep_HitNotify()
{
	SimulateInstantHit(HitNotify.Origin, HitNotify.ReticleSpread, HitNotify.RandomSeed);
}

void AShooterWeapon_Instant::SimulateInstantHit(const FVector& ShootOrigin, float ReticleSpread, int32 RandomSeed)
{
	FRandomStream WeaponRandomStream(RandomSeed);
	//计算圆锥半角
	const float ConeHalfAngle = FMath::DegreesToRadians(ReticleSpread / 2.0f);
	//获取瞄准方向，生成子弹随机发射方向
	const FVector AimDir = GetAdjustAim();
	FVector ShooterDirection = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);

	//获取子弹发射位置
	const float WeaponRange = InstantConfig.WeaponRange;
	const FVector StartTrace = ShootOrigin;
	const FVector EndTrace = StartTrace + ShooterDirection * WeaponRange;
	//获取伤害对象
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	//有碰撞到
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

//获取生命周期同步属性，该函数由系统调用
void AShooterWeapon_Instant::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//条件同步：跳过Owner的复制
	DOREPLIFETIME_CONDITION(AShooterWeapon_Instant, HitNotify, COND_SkipOwner);
}
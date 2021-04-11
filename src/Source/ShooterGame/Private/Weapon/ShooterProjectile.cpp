// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterProjectile.h"

// Sets default values
AShooterProjectile::AShooterProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//设置子弹碰撞通道
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;//运动过程追踪复杂物体
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RootComponent = CollisionComp;

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->MaxSpeed = 2000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.0f;

	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->bAutoActivate = true;
	ParticleComp->bAutoDestroy = false;
	ParticleComp->SetupAttachment(RootComponent);

	//设置同步开关
	bReplicates = true;
	bReplicateMovement = true;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	bExploded = false;
}

// Called when the game starts or when spawned
void AShooterProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShooterProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterProjectile::InitializeVelocity(FVector ShooterDirection)
{
	MovementComp->Velocity = ShooterDirection * MovementComp->InitialSpeed;
}

void AShooterProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//使用系统函数，在子弹停止时生成爆炸特效
	MovementComp->OnProjectileStop.AddDynamic(this, &AShooterProjectile::OnImpact);
	CollisionComp->MoveIgnoreActors.Add(Instigator);

	AShooterWeapon_Projectile* OwnerWeapon = Cast<AShooterWeapon_Projectile>(GetOwner());
	if (OwnerWeapon)
	{
		OwnerWeapon->ApplyWeaponConfig(WeaponConfig);
	}

	//设置炮弹生命周期
	SetLifeSpan(WeaponConfig.ProjectileLife);
	MyController = GetInstigatorController();
}

void AShooterProjectile::OnImpact(const FHitResult& ImpactResult)
{
	if (Role == ROLE_Authority && !bExploded)
	{
		Explode(ImpactResult);
	}
}

void AShooterProjectile::Explode(const FHitResult& ImpactResult)
{
	if(ParticleComp)
	{
		ParticleComp->Deactivate();
	}

	//在法线前10个单位处爆炸
	const FVector NudgedImpactLocation = ImpactResult.ImpactPoint + ImpactResult.ImpactNormal * 10.0f;
	if (WeaponConfig.ExplosionDamage > 0.0 && WeaponConfig.ExplosionRadius > 0.0)
	{
		UGameplayStatics::ApplyRadialDamage(this, WeaponConfig.ExplosionDamage, NudgedImpactLocation, WeaponConfig.ExplosionRadius, WeaponConfig.DamageType, TArray<AActor*>(), this, MyController.Get());
	}

	//生成爆炸特效对象
	if (Explosion)
	{
		FTransform const SpawnTransform(ImpactResult.ImpactNormal.Rotation(), NudgedImpactLocation);
		AShooterExplosionEffect* const EffectObj = GetWorld()->SpawnActorDeferred<AShooterExplosionEffect>(Explosion, SpawnTransform);
		if (EffectObj)
		{
			UGameplayStatics::FinishSpawningActor(EffectObj,SpawnTransform);
		}
	}

	//只爆炸一次
	bExploded = true;
}

void AShooterProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (MovementComp)
	{
		//速度由服务器同步到客户端
		MovementComp->Velocity = NewVelocity;
	}
}

void AShooterProjectile::OnRep_Exploded()
{
	//获取子弹前进方向
	const FVector ProjDiretion = GetActorForwardVector();
	const FVector StartTrace = GetActorLocation() - ProjDiretion * 200;
	const FVector EndTrace = GetActorLocation() + ProjDiretion * 150;
	FHitResult Impact;
	//没有相交点\产生延迟，在子弹位置发生爆炸
	if (!GetWorld()->LineTraceSingleByChannel(Impact, StartTrace, EndTrace, COLLISION_PROJECTILE, FCollisionQueryParams(TEXT("ProjectClient"), true, Instigator)))
	{
		Impact.ImpactPoint = GetActorLocation();
		Impact.ImpactNormal = -ProjDiretion;
	}
	Explode(Impact);
}

//获取生命周期同步属性，该函数由系统调用
void AShooterProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//收集改写的属性，属性会在所有的客户端连接里面同步
	DOREPLIFETIME(AShooterProjectile, bExploded);
}
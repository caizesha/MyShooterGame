// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeapon.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "ShooterAIController.h"


// Sets default values
AShooterWeapon::AShooterWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1PComp"));
	WeaponMesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	WeaponMesh1P->CastShadow = false;
	//由于第三人称Mesh有具体的碰撞设置，选择关闭第一人称Mesh的碰撞
	WeaponMesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponMesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = WeaponMesh1P;
	FTransform  newTransform(FRotator(0.0f, 0.0f, -90.0f));
	WeaponMesh1P->SetRelativeTransform(newTransform);

	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3PComp"));
	WeaponMesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	WeaponMesh3P->CastShadow = true;
	WeaponMesh3P->bReceivesDecals = false;
	WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponMesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	WeaponMesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	WeaponMesh3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	WeaponMesh3P->SetupAttachment(WeaponMesh1P);

	FireSound = nullptr;
	//CurrentAmmoCount = 0;

	PreState = EWeaponState::Idle;
	CurrentState = EWeaponState::Idle;
	bIsEquiped = false;
	bPendingReload = false;
	bWantToFire = false;
	bPendingEquip = false;
	bRefiring = false;

	LastWeapon = nullptr;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;

	bPlayingFireAnim = false;

	//设置枪的远程角色
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	//打开同步开关
	bReplicates = true;
	//计算网络相关性
	bNetUseOwnerRelevancy = true;
}

// Called when the game starts or when spawned
void AShooterWeapon::BeginPlay()
{
	Super::BeginPlay();
	//CurrentAmmoCount = GetMaxAmmoCount();
	if (WeaponConfig.InitialClips > 0)
	{
		CurrentAmmo = WeaponConfig.InitialClips * WeaponConfig.AmmoPerClip;
		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
	}
}

// Called every frame
void AShooterWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterWeapon::SetPawnOwner(AShooterCharacter* pawnOwner)
{

	if (PawnOwner != pawnOwner)
	{
		Instigator = pawnOwner;
		PawnOwner = pawnOwner;
		SetOwner(pawnOwner);
	}
}

void AShooterWeapon::AttachMeshToPawn()
{
	if (PawnOwner)
	{
		USkeletalMeshComponent* PawnMesh1p = PawnOwner->GetSpecificPawnMesh(true);
		FName attachPoint = PawnOwner->GetWeaponAttachPoint();
		if (PawnMesh1p)
		{
			WeaponMesh1P->SetHiddenInGame(false);
			WeaponMesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, attachPoint);
		}

		USkeletalMeshComponent* PawnMesh3p = PawnOwner->GetSpecificPawnMesh(false);
		if (PawnMesh3p)
		{
			WeaponMesh3P->SetHiddenInGame(false);
			WeaponMesh3P->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, attachPoint);
		}
	}
}

void AShooterWeapon::DetachMeshFromPawn()
{
	WeaponMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh1P->SetHiddenInGame(true);

	//补充第三人称武器的卸载逻辑
	WeaponMesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh3P->SetHiddenInGame(true);
}
//获取子弹发射方向
FVector AShooterWeapon::GetAdjustAim()
{
	FVector FinalAim = FVector::ZeroVector;
	
	AShooterPlayerController* PlayerController = Instigator ? Cast<AShooterPlayerController>(Instigator->GetController()) : nullptr;
	if (PlayerController)
	{	
		FVector CameraLocation;
		FRotator CameraRotation;

		//GetPlayerViewPoint()获取眼睛位置，使用引用进行传值
		PlayerController->GetPlayerViewPoint(CameraLocation,CameraRotation);
		FinalAim = CameraRotation.Vector();
	}
	else if (Instigator)
	{
		AShooterAIController* AIController = PawnOwner ? Cast<AShooterAIController>(PawnOwner->GetController()) : nullptr;
		if (AIController)
		{
			FinalAim = AIController->GetControlRotation().Vector();
		}
		else
		{
			FinalAim = Instigator->GetBaseAimRotation().Vector();
		}
	}
	return FinalAim;
}

void AShooterWeapon::FireWeapon() {}

void AShooterWeapon::StartFire()
{
	//客户端：通知服务端执行
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}
	if (!bWantToFire)
	{
		bWantToFire = true;
		DetermineWeaponState();
		HandleDependCurrentState();
	}
	//SimulateWeaponFire();
	//FireWeapon();
}

bool AShooterWeapon::ServerStartFire_Validate()
{
	return true;
}

void AShooterWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool AShooterWeapon::ServerHandleFiring_Validate()
{
	return true;
}

void AShooterWeapon::ServerHandleFiring_Implementation()
{
	//更新子弹数量
	const bool bShouldUpdate = CurrentAmmoInClip > 0 && CanFire();
	if (bShouldUpdate)
	{
		UsedAmmo();
	}
}

void AShooterWeapon::StopFire()
{
	//客户端：通知服务端执行
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}
	if (bWantToFire)
	{
		bWantToFire = false;
		DetermineWeaponState();
		HandleDependCurrentState();
	}
}

bool AShooterWeapon::ServerStopFire_Validate()
{
	return true;
}

void AShooterWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

//服务器开火，通知客户端模拟开火
void AShooterWeapon::OnRep_Fire()
{
	if (bWantToFire)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulateWeaponFire();
	}

}

//确保只由服务器计算伤害值
bool AShooterWeapon::ShouldDealDamage(AActor* TestActor) const
{
	if (TestActor)
	{
		if (GetNetMode() != NM_Client || TestActor->Role == ROLE_Authority || TestActor->bTearOff)

		{
			return true;
		}
		
	}
	return false;
}

bool AShooterWeapon::ServerStartReload_Validate()
{
	return true;
}

void AShooterWeapon::ServerStartReload_Implementation()
{
	StartReload();
}

void AShooterWeapon::OnRep_Reload()
{
	if (bPendingReload)
	{
		PlayMontageAnimation(ReloadAnim);

		if (PawnOwner)
		{
			PlayWeaponSound(ReloadSound);
		}
	}
	else
	{
		StopMontageAnimation(ReloadAnim);
	}

}

void AShooterWeapon::SimulateWeaponFire()
{
	//播放开火动画
	if (!bPlayingFireAnim)
	{
		bPlayingFireAnim = true;
		PlayMontageAnimation(FireAnim);
	}

	if (FireSound)
	{
		PlayWeaponSound(FireSound);
	}
	
	AShooterPlayerController *MyPC = (PawnOwner != nullptr) ? Cast<AShooterPlayerController>(PawnOwner->Controller) : nullptr;
	if (MyPC && MyPC->IsLocalController())
	{
		if (FireCameraShake)
		{
			MyPC->ClientPlayCameraShake(FireCameraShake,1);
		}
	}
}

void AShooterWeapon::StopSimulateWeaponFire()
{
	//停止开火动画
	if (bPlayingFireAnim)
	{
		bPlayingFireAnim = false;
		StopMontageAnimation(FireAnim);
	}
}

UAudioComponent* AShooterWeapon::PlayWeaponSound(USoundCue *Sound)
{
	UAudioComponent* AudioComp = nullptr;
	if (Sound)
	{
		//声音跟随父组件播放
		AudioComp = UGameplayStatics::SpawnSoundAttached(Sound, PawnOwner->GetRootComponent());
	}
	return AudioComp;
}

USkeletalMeshComponent* AShooterWeapon::GetWeaponMesh() const
{
	return (PawnOwner != NULL && PawnOwner->IsFirstPerson()) ? WeaponMesh1P : WeaponMesh3P;
}

FVector AShooterWeapon::GetMuzzleLocation() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketLocation(MuzzleAttachPoint);
}

FVector AShooterWeapon::GetMuzzleDirection() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}

//确定武器所处状态
void AShooterWeapon::DetermineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquiped)
	{
		if (bPendingReload)
		{
			if (CanReload())
			{
				NewState = EWeaponState::Reloading;
			}
			else
			{
				NewState = CurrentState;
			}
		}
		else
		{
			if (bWantToFire && CanFire())
			{
				NewState = EWeaponState::Firing;
			}
			else
			{
				NewState = EWeaponState::Idle;
			}
		}
		/*
		//觉得应该加上的地方:不需要了，因为换装备的时候会把bPendingEquip置为false
		if (bPendingEquip)
		{
			NewState = EWeaponState::Equiping;
		}*/
	}
	else
	{
		if (bPendingEquip)
		{
			NewState = EWeaponState::Equiping;
		}
	}
	SetWeaponState(NewState);
}

bool AShooterWeapon::CanFire() const
{
	bool bCanFire = PawnOwner && PawnOwner->CanFire();
	return true;
}

bool AShooterWeapon::CanReload() const
{
	bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0);
	bool bStateOKToReload = (CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing);
	
	return bGotAmmo && bStateOKToReload;
}

void AShooterWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	PreState = CurrentState;
	CurrentState = NewState;
}

//处理状态转换过程
void AShooterWeapon::HandleDependCurrentState()
{
	if (PreState == EWeaponState::Idle&& CurrentState == EWeaponState::Firing)
	{
		//处理开始开火状态
		HandleStartFireState();
	}
	else if (PreState == EWeaponState::Firing && (CurrentState == EWeaponState::Idle || CurrentState == EWeaponState::Reloading))
	{
		//处理结束开火状态
		HandleEndFireState();
	}

	if ((PreState == EWeaponState::Firing || PreState == EWeaponState::Idle)&&  CurrentState == EWeaponState::Reloading)
	{
		//处理开始装弹状态
		HandleStartReloadState();
	}
	else if (PreState == EWeaponState::Reloading && CurrentState == EWeaponState::Idle)
	{
		//处理结束装弹状态
		HandleEndReloadState();
	}

	if (PreState == EWeaponState::Idle && CurrentState == EWeaponState::Equiping)
	{
		//处理开始换枪状态
		HandleStartEquipState();
	}
	else if (PreState == EWeaponState::Equiping && CurrentState == EWeaponState::Idle)
	{
		//处理结束换枪状态
		HandleEndEquipState();
	}
}

void AShooterWeapon::HandleStartFireState()
{
	HandleFiring();
}

void AShooterWeapon::HandleEndFireState()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;

	//停止开火动画
	StopSimulateWeaponFire();
}

void AShooterWeapon::HandleStartReloadState()
{
	//播放装弹动画
	float AnimDuration = PlayMontageAnimation(ReloadAnim);
	if (AnimDuration <= 0.0f)
	{
		AnimDuration = 0.5;
	}

	GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &AShooterWeapon::StopReload, AnimDuration, false);
	if (Role == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &AShooterWeapon::ReloadWeapon, FMath::Max(AnimDuration - 0.1f, 0.1f), false);
	}
	
	if(PawnOwner)
	{
		PlayWeaponSound(ReloadSound);
	}

}

void AShooterWeapon::HandleEndReloadState()
{
	//停止播放动画
	StopMontageAnimation(ReloadAnim);
	GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);

}

void AShooterWeapon::HandleStartEquipState()
{
	AttachMeshToPawn();

	if (LastWeapon != nullptr)
	{	
		//播放换枪动画
		float Duration = PlayMontageAnimation(EquipAnim);
		if (Duration <= 0.0f)
		{
			Duration = 0.5f;
		}
		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &AShooterWeapon::OnEquipFinished, Duration, false);

	}

	
	//角色刚创建
	else
	{
		OnEquipFinished();
	}

	if (PawnOwner && EquipSound)
	{
		PlayWeaponSound(EquipSound);
	}

	bIsEquiped = true;
}

void AShooterWeapon::HandleEndEquipState()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	
	//停止武器动画播放
	StopMontageAnimation(EquipAnim);
}

void AShooterWeapon::OnEquip(const AShooterWeapon* _LastWeapon)
{
	LastWeapon = (AShooterWeapon*)_LastWeapon;
	if (!bPendingEquip)
	{
		bPendingEquip = true;
		DetermineWeaponState();
		HandleDependCurrentState();
	}
}

void AShooterWeapon::OnEquipFinished()
{
	if (!bIsEquiped)
	{
		bIsEquiped = true;
	}
	
	if (bPendingEquip)
	{
		bPendingEquip = false;
	}

	DetermineWeaponState();
	HandleDependCurrentState();
}

void AShooterWeapon::StartReload()
{
	if (Role < ROLE_Authority)
	{
		ServerStartReload();
	}
	if (!bPendingReload && CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();
		HandleDependCurrentState();
	}
}

void AShooterWeapon::StopReload()
{
	if (bPendingReload)
	{
		bPendingReload = false;
		DetermineWeaponState();
		HandleDependCurrentState();
	}
}

void AShooterWeapon::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(WeaponConfig.AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);
	if (ClipDelta > 0)
	{
		CurrentAmmoInClip += ClipDelta;
	}
	//currentammo应该没有变化，不用这么写吧？？？
	//CurrentAmmo = FMath::Max(CurrentAmmo, CurrentAmmoInClip);
}

void AShooterWeapon::HandleFiring()
{
	if (CurrentAmmo <= 0)
	{
		//todo:提示子弹数量不够
		return;
	}
	else
	{
		SimulateWeaponFire();
		//只让本地控制的玩家触发该逻辑
		if (PawnOwner && PawnOwner->IsLocallyControlled())
		{
			FireWeapon();
			//更新子弹数量
			UsedAmmo();
		}
	}

	if (CurrentAmmoInClip <= 0 && CanReload())
	{
		StartReload();
	}
	
	//通知服务器
	if (PawnOwner && PawnOwner->IsLocallyControlled())
	{
		if (Role < ROLE_Authority)
		{
			ServerHandleFiring();
		}
	}

	bRefiring = (CurrentState == EWeaponState::Firing) && (WeaponConfig.TimeBetweenShots > 0.0f);
	if (bRefiring)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AShooterWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
	}

}

int32 AShooterWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int32 AShooterWeapon::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}

int32 AShooterWeapon::GetAmmoPerClip() const
{
	return WeaponConfig.AmmoPerClip;
}

void AShooterWeapon::UsedAmmo()
{
	CurrentAmmo--;
	CurrentAmmoInClip--;
}

//相机伤害位置调整
FVector AShooterWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	AShooterPlayerController* PC = PawnOwner ? Cast<AShooterPlayerController>(PawnOwner->Controller) : NULL;
	AShooterAIController* AIPC = PawnOwner ? Cast<AShooterAIController>(PawnOwner->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		// use player's camera
		FRotator UnuseRot;//不使用
		PC->GetPlayerViewPoint(OutStartTrace, UnuseRot);
		
		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((Instigator->GetActorLocation() - OutStartTrace) | AimDir);
	}
	else if (AIPC)
	{
		OutStartTrace = GetMuzzleLocation();
	}
	return OutStartTrace;
}

FHitResult AShooterWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);

	return Hit;
}

EWeaponState::Type AShooterWeapon::GetCurrentWeaponState() const
{
	return CurrentState;
}

void AShooterWeapon::OnUnEquip()
{
	//此函数不应纳入状态机流程管理（没有动作触发）

	//卸载Mesh
	DetachMeshFromPawn();

	bIsEquiped = false;

	StopFire();
	
	if (bPendingReload)
	{
		bPendingReload = false;

		//停止播放装弹动画
		StopMontageAnimation(ReloadAnim);

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		bPendingEquip = false;

		//停止播放换枪动画
		StopMontageAnimation(EquipAnim);

		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}
	DetermineWeaponState();
}

float AShooterWeapon::PlayMontageAnimation(const FWeaponAnim& Animation)
{
	float Duration = 0.0f;
	if (PawnOwner)
	{
		UAnimMontage*  UseAnim = PawnOwner->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			Duration = PawnOwner->PlayAnimMontage(UseAnim);
		}
	}
	return Duration;
}

void AShooterWeapon::StopMontageAnimation(const FWeaponAnim& Animation)
{
	if(PawnOwner)
	{
		UAnimMontage*  UseAnim = PawnOwner->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		if (UseAnim)
		{
			PawnOwner->StopAnimMontage(UseAnim);
		}
	}
}

//获取生命周期同步属性，该函数由系统调用
void AShooterWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//条件同步：跳过Owner的复制
	DOREPLIFETIME_CONDITION(AShooterWeapon, bWantToFire, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShooterWeapon, bPendingReload, COND_SkipOwner);
	//只通知到Owner
	DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmoInClip, COND_OwnerOnly);
}
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
	//���ڵ����˳�Mesh�о������ײ���ã�ѡ��رյ�һ�˳�Mesh����ײ
	WeaponMesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponMesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = WeaponMesh1P;
	FTransform  newTransform(FRotator(0.0f, 0.0f, -90.0f));
	WeaponMesh1P->SetRelativeTransform(newTransform);

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
	}
	
}

void AShooterWeapon::AttachMeshToPawn()
{
	if (PawnOwner)
	{
		USkeletalMeshComponent* PawnMesh1p = PawnOwner->GetArmMesh();
		FName attachPoint = PawnOwner->GetWeaponAttachPoint();
		if (PawnMesh1p)
		{
			WeaponMesh1P->SetHiddenInGame(false);
			WeaponMesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, attachPoint);
		}
	}
}

void AShooterWeapon::DetachMeshFromPawn()
{
	WeaponMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh1P->SetHiddenInGame(true);
}
//��ȡ�ӵ����䷽��
FVector AShooterWeapon::GetAdjustAim()
{
	FVector FinalAim = FVector::ZeroVector;
	
	AShooterPlayerController* PlayerController = Instigator ? Cast<AShooterPlayerController>(Instigator->GetController()) : nullptr;
	if (PlayerController)
	{	
		FVector CameraLocation;
		FRotator CameraRotation;

		//GetPlayerViewPoint()��ȡ�۾�λ�ã�ʹ�����ý��д�ֵ
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
	if (!bWantToFire)
	{
		bWantToFire = true;
		DetermineWeaponState();
		HandleDependCurrentState();
	}
	//SimulateWeaponFire();
	//FireWeapon();
}

void AShooterWeapon::StopFire()
{
	if (bWantToFire)
	{
		bWantToFire = false;
		DetermineWeaponState();
		HandleDependCurrentState();
	}
}

void AShooterWeapon::SimulateWeaponFire()
{
	//���ſ��𶯻�
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
	//ֹͣ���𶯻�
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
		//�������游�������
		AudioComp = UGameplayStatics::SpawnSoundAttached(Sound, PawnOwner->GetRootComponent());
	}
	return AudioComp;
}


FVector AShooterWeapon::GetMuzzleLocation()
{
	if (WeaponMesh1P)
	{
		return WeaponMesh1P->GetSocketLocation(MuzzleAttachPoint);
	}
	else
	{
		return FVector();
	}
}
//
//int AShooterWeapon::GetCurrentAmmoCount()
//{
//	return CurrentAmmoCount;
//}
//
//
//int AShooterWeapon::GetMaxAmmoCount()
//{
//	return GetClass()->GetDefaultObject<AShooterWeapon>()->CurrentAmmoCount;
//}

//ȷ����������״̬
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
		//����Ӧ�ü��ϵĵط�:����Ҫ�ˣ���Ϊ��װ����ʱ����bPendingEquip��Ϊfalse
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

//����״̬ת������
void AShooterWeapon::HandleDependCurrentState()
{
	if (PreState == EWeaponState::Idle&& CurrentState == EWeaponState::Firing)
	{
		//����ʼ����״̬
		HandleStartFireState();
	}
	else if (PreState == EWeaponState::Firing && (CurrentState == EWeaponState::Idle || CurrentState == EWeaponState::Reloading))
	{
		//�����������״̬
		HandleEndFireState();
	}

	if ((PreState == EWeaponState::Firing || PreState == EWeaponState::Idle)&&  CurrentState == EWeaponState::Reloading)
	{
		//����ʼװ��״̬
		HandleStartReloadState();
	}
	else if (PreState == EWeaponState::Reloading && CurrentState == EWeaponState::Idle)
	{
		//�������װ��״̬
		HandleEndReloadState();
	}

	if (PreState == EWeaponState::Idle && CurrentState == EWeaponState::Equiping)
	{
		//����ʼ��ǹ״̬
		HandleStartEquipState();
	}
	else if (PreState == EWeaponState::Equiping && CurrentState == EWeaponState::Idle)
	{
		//���������ǹ״̬
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

	//ֹͣ���𶯻�
	StopSimulateWeaponFire();
}

void AShooterWeapon::HandleStartReloadState()
{
	//����װ������
	float AnimDuration = PlayMontageAnimation(ReloadAnim);
	if (AnimDuration <= 0.0f)
	{
		AnimDuration = 0.5;
	}

	GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &AShooterWeapon::StopReload, AnimDuration, false);
	GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &AShooterWeapon::ReloadWeapon, FMath::Max(AnimDuration - 0.1f, 0.1f), false);

	if(PawnOwner)
	{
		PlayWeaponSound(ReloadSound);
	}

}

void AShooterWeapon::HandleEndReloadState()
{
	//ֹͣ���Ŷ���
	StopMontageAnimation(ReloadAnim);
	GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);

}

void AShooterWeapon::HandleStartEquipState()
{
	AttachMeshToPawn();

	if (LastWeapon != nullptr)
	{	
		//���Ż�ǹ����
		float Duration = PlayMontageAnimation(EquipAnim);
		if (Duration <= 0.0f)
		{
			Duration = 0.5f;
		}
		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &AShooterWeapon::OnEquipFinished, Duration, false);

	}

	
	//��ɫ�մ���
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
	
	//ֹͣ������������
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
	//currentammoӦ��û�б仯��������ôд�ɣ�����
	//CurrentAmmo = FMath::Max(CurrentAmmo, CurrentAmmoInClip);
}

void AShooterWeapon::HandleFiring()
{
	if (CurrentAmmo <= 0)
	{
		//todo:��ʾ�ӵ���������
		return;
	}
	else
	{
		SimulateWeaponFire();
		FireWeapon();
		//�����ӵ�����
		UsedAmmo();
	}

	if (CurrentAmmoInClip <= 0 && CanReload())
	{
		StartReload();
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
	//�˺�����Ӧ����״̬�����̹���û�ж���������

	//ж��Mesh
	DetachMeshFromPawn();

	bIsEquiped = false;

	StopFire();
	
	if (bPendingReload)
	{
		bPendingReload = false;

		//ֹͣ����װ������
		StopMontageAnimation(ReloadAnim);

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		bPendingEquip = false;

		//ֹͣ���Ż�ǹ����
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
		UAnimMontage*  UseAnim = Animation.Pawn1P;
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
		UAnimMontage*  UseAnim = Animation.Pawn1P;
		if (UseAnim)
		{
			PawnOwner->StopAnimMontage(UseAnim);
		}
	}
}
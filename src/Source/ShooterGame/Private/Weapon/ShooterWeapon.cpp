// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeapon.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"


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
}

// Called when the game starts or when spawned
void AShooterWeapon::BeginPlay()
{
	Super::BeginPlay();
	
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
		USkeletalMeshComponent* PawnMesh1p = PawnOwner->GetFirstPersonMesh();
		FName attachPoint = PawnOwner->GetWeaponAttachPoint();
		if (PawnMesh1p)
		{
			WeaponMesh1P->SetHiddenInGame(false);
			WeaponMesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, attachPoint);
		}
	}
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
	return FinalAim;
}

void AShooterWeapon::FireWeapon() {}

void AShooterWeapon::StartFire()
{
	SimulateWeaponFire();
	FireWeapon();
}

void AShooterWeapon::SimulateWeaponFire()
{
	if (FireSound)
	{
		PlayWeaponSound(FireSound);
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
// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeapon.h"
#include "ShooterCharacter.h"


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
	PawnOwner = pawnOwner;
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

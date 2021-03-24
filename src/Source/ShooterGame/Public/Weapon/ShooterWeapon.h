// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ShooterWeapon.generated.h"

class AShooterCharacter;

//��������״̬
namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,//װ�ӵ�
		Equiping//װ������
	};
}

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 AmmoPerClip;//��������

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 InitialClips;//��ʼ�ܵ�������

	UPROPERTY(EditDefaultsOnly, Category = "WeaponState")
	float TimeBetweenShots;

	FWeaponData()
	{
		AmmoPerClip = 100;
		InitialClips = 20;
		TimeBetweenShots = 1.0f;
	}
};

UCLASS()
class SHOOTERGAME_API AShooterWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StopEquip();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetPawnOwner(AShooterCharacter* pawnOwner);
	void AttachMeshToPawn();

	//��ȡ�ӵ����䷽��
	FVector GetAdjustAim();

	virtual void FireWeapon();

	FVector GetMuzzleLocation();

	void StartFire();

	void StopFire();

	void SimulateWeaponFire();

	void StopSimulateWeaponFire();

	UAudioComponent* PlayWeaponSound(USoundCue *Sound);
	
	//int GetCurrentAmmoCount();

	//int GetMaxAmmoCount();

	void DetermineWeaponState();

	bool CanFire() const;

	bool CanReload() const;
	
	void SetWeaponState(EWeaponState::Type NewState);

	void HandleDependCurrentState();

	void HandleStartFireState();

	void HandleEndFireState();

	void HandleStartReloadState();

	void HandleEndReloadState();

	void HandleStartEquipState();

	void HandleEndEquipState();

	void StartEquip(const AShooterWeapon* _LastWeapon);

	void StartReload();

	void StopReload();

	void ReloadWeapon();

	void HandleFiring();

	int32 GetCurrentAmmo() const;
	
	int32 GetCurrentAmmoInClip() const;

	int32 GetAmmoPerClip() const;

	void UsedAmmo();
protected:
	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* WeaponMesh1P;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName MuzzleAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue *FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue *EquipSound;


	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue *ReloadSound;

	//UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	//int CurrentAmmoCount;


	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FWeaponData WeaponConfig;

	//��ǰ�ӵ�������
	UPROPERTY(Transient)
	int32 CurrentAmmo;
	
	//��ǰ�����ӵ�����
	UPROPERTY(Transient)
	int32 CurrentAmmoInClip;
private:
	AShooterCharacter* PawnOwner;
	
	
	bool bIsEquiped;
	//�Ƿ���Ҫװ�ӵ�
	bool bPendingReload;
	bool bWantToFire;
	bool bPendingEquip;
	bool bRefiring;

	EWeaponState::Type CurrentState;

	EWeaponState::Type PreState;

	AShooterWeapon* LastWeapon;

	FTimerHandle TimerHandle_StopEquip;

	FTimerHandle TimerHandle_ReloadWeapon;

	FTimerHandle TimerHandle_StopReload;

	FTimerHandle TimerHandle_HandleFiring;
};

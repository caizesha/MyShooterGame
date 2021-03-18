// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ShooterWeapon_Projectile.h"
#include "ShooterExplosionEffect.h"
#include "ShooterProjectile.generated.h"


UCLASS()
class SHOOTERGAME_API AShooterProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeVelocity(FVector ShooterDirection);

	void PostInitializeComponents();

	UFUNCTION()
	void OnImpact(const FHitResult& ImpactResult);

	void Explode(const FHitResult& ImpactResult);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	USphereComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UProjectileMovementComponent* MovementComp;

	struct FProjectileWeaponData WeaponConfig;

	TWeakObjectPtr<AController> MyController;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UParticleSystemComponent* ParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<class AShooterExplosionEffect> Explosion;

};

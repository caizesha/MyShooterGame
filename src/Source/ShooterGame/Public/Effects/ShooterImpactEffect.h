// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ShooterImpactEffect.generated.h"

UCLASS()
class SHOOTERGAME_API AShooterImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterImpactEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	UParticleSystem* DefaultFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	USoundCue* DefaultSound;
};

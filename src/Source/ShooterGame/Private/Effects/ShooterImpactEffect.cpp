// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterImpactEffect.h"


// Sets default values
AShooterImpactEffect::AShooterImpactEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterImpactEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShooterImpactEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//生成粒子特效
	if (DefaultFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, DefaultFX, GetActorLocation(), GetActorRotation());
	}

	if (DefaultSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DefaultSound, GetActorLocation());
	}

}

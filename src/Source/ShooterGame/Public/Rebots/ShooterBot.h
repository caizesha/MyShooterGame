// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/ShooterCharacter.h"
#include "ShooterBot.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterBot : public AShooterCharacter
{
	GENERATED_BODY()
	
	
public:
	AShooterBot();
	
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	class UBehaviorTree* BotBehavior;
};

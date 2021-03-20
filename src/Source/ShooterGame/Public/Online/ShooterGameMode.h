// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

class AShooterAIController;
/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AShooterGameMode();//创建构造函数
	
	void StartPlay() override;

	AShooterAIController* CreateBotController();

	void AddBots();

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode")
	TSubclassOf<APawn> BotPawnClass;
};

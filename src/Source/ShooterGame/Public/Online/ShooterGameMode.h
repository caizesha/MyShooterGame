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

	void DeafultTimer();

	virtual void PreInitializeComponents() override;

	virtual void HandleMatchHasStarted() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
protected:
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;
protected:
	FTimerHandle TimeHandle_DefaultTimer;

	UPROPERTY(config)
	int32 RoundTime;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AShooterPlayerController();
	
	//同步函数 reliable：确保可达（TCP协议）
	UFUNCTION(reliable, client)
	void ClientStartOnlineGame();

	virtual void SetupInputComponent() override;

	void OnToggleInGameMenu();
private:
	FTimerHandle TimerHandle_ClientStartOnlineGame;
};

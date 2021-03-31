// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

/**
 * 
 */

//负责管理菜单的中间类
class FShooterMainMenu;

UCLASS()
class SHOOTERGAME_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	//定时调用的函数
	bool Tick(float DeltaSeconds);
private:
	void BeginMainMenuState();
	
	TSharedPtr<FShooterMainMenu> MainMenuUI;

	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;
};

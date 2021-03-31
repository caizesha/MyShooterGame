// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameInstance.h"
#include "ShooterMainMenu.h"


void UShooterGameInstance::Init()
{
	Super::Init();
	//定时代理
	TickDelegate = FTickerDelegate::CreateUObject(this, &UShooterGameInstance::Tick);
	//调用系统管理器
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
}

//按固定时间调用Tick函数
bool UShooterGameInstance::Tick(float DeltaSeconds)
{
	//全局变量，只初始化一次
	static bool bExecuted = false;
	if (!bExecuted)
	{
		//只初始化一次UI
		BeginMainMenuState();
		bExecuted = true;
	}

	return true;
}

void UShooterGameInstance::BeginMainMenuState()
{
	//新建对象，转化为共享指针
	MainMenuUI = MakeShareable(new FShooterMainMenu);

	//为了区别服务器上其他玩家
	ULocalPlayer* const Player = GetFirstGamePlayer();
	MainMenuUI->Construct(this, Player);
	MainMenuUI->AddMenuToViewport();
}


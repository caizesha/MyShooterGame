// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameInstance.h"
#include "ShooterMainMenu.h"


void UShooterGameInstance::Init()
{
	Super::Init();
	//��ʱ����
	TickDelegate = FTickerDelegate::CreateUObject(this, &UShooterGameInstance::Tick);
	//����ϵͳ������
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
}

//���̶�ʱ�����Tick����
bool UShooterGameInstance::Tick(float DeltaSeconds)
{
	//ȫ�ֱ�����ֻ��ʼ��һ��
	static bool bExecuted = false;
	if (!bExecuted)
	{
		//ֻ��ʼ��һ��UI
		BeginMainMenuState();
		bExecuted = true;
	}

	return true;
}

void UShooterGameInstance::BeginMainMenuState()
{
	//�½�����ת��Ϊ����ָ��
	MainMenuUI = MakeShareable(new FShooterMainMenu);

	//Ϊ��������������������
	ULocalPlayer* const Player = GetFirstGamePlayer();
	MainMenuUI->Construct(this, Player);
	MainMenuUI->AddMenuToViewport();
}


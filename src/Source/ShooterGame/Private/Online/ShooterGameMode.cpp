// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameMode.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "ShooterPlayerState.h"
#include "ShooterGameState.h"
#include "ShooterSpectatorPawn.h"
#include "ShooterHUD.h"
#include "ShooterAIController.h"

AShooterGameMode::AShooterGameMode()
{
	//ģ�庯����ͨ�����Ҹ����������Ҷ�Ӧ����ͼ��playerpawn����������Ӧ�Ķ��󣨸���ΪASHooterCharacter������ָ�����ࣩ
	static ConstructorHelpers::FClassFinder<AShooterCharacter> PlayerPawnOb(TEXT("/Game/Blueprints/Pawns/PlayerPawn"));
	//��ֵplayerpawn�ķ�����Ϣ�����
	DefaultPawnClass = PlayerPawnOb.Class;
	
	//��ֵbotpawn�ķ�����Ϣ�����
	static ConstructorHelpers::FClassFinder<AShooterCharacter> BotPawnOb(TEXT("/Game/Blueprints/Pawns/BotPawn"));
	BotPawnClass = BotPawnOb.Class;

	//������ҿ�������ķ�����Ϣ
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	//�������״̬��ķ�����Ϣ
	PlayerStateClass = AShooterPlayerState::StaticClass();
	//������Ϸ״̬��ķ�����Ϣ
	GameStateClass = AShooterGameState::StaticClass();
	//�����۲�����ķ�����Ϣ
	SpectatorClass = AShooterSpectatorPawn::StaticClass();
	//�������������ķ�����Ϣ
	HUDClass = AShooterHUD::StaticClass();

}

void AShooterGameMode::StartPlay()
{
	Super::StartPlay();

	CreateBotController();

	AddBots();
}

AShooterAIController* AShooterGameMode::CreateBotController()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = nullptr;

	AShooterAIController* AIController = GetWorld()->SpawnActor<AShooterAIController>(SpawnInfo);
	return AIController;
}

void AShooterGameMode::AddBots()
{
	UWorld* world = GetWorld();
	for (FConstControllerIterator It = world->GetControllerIterator(); It; It++)
	{
		AShooterAIController* AIC = Cast<AShooterAIController>(*It);
		if(AIC)
		{
			RestartPlayer(AIC);
		}
	}
}

UClass* AShooterGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<AShooterAIController>())
	{
		return BotPawnClass;
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AShooterGameMode::DeafultTimer()
{
	//ʵ�ֵ�����ʱ��
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState);
	if (ShooterGameState && (ShooterGameState->RemainingTime) > 0)
	{
		ShooterGameState->RemainingTime--;
		if (ShooterGameState->RemainingTime <= 0)
		{
			RestartGame();
		}
	}
	
}

void AShooterGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//���õ�����ʱ��
	GetWorldTimerManager().SetTimer(TimeHandle_DefaultTimer, this, &AShooterGameMode::DeafultTimer, 1.0f, true);
}

void AShooterGameMode::HandleMatchHasStarted()
{
	//������Ϸʣ��ʱ��
	Super::HandleMatchHasStarted();
	AShooterGameState* ShooterGameState = Cast<AShooterGameState>(GameState);
	ShooterGameState->RemainingTime = RoundTime;
}

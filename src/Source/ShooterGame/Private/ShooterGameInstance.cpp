// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameInstance.h"
#include "ShooterMainMenu.h"
#include "ShooterGameSession.h"

void UShooterGameInstance::Init()
{
	Super::Init();
	//定时代理
	TickDelegate = FTickerDelegate::CreateUObject(this, &UShooterGameInstance::Tick);
	//调用系统管理器
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
}

void UShooterGameInstance::Shutdown()
{
	Super::Shutdown();
	//反注册定时器句柄
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void UShooterGameInstance::StartGameInstance()
{
	//获取命令行参数
	const TCHAR* Cmd= FCommandLine::Get();
	TCHAR Param[4096] = TEXT("");
	//解析命令行解析 token:分割出空格分隔的字串 false:不需要使用转义符
	if (FParse::Token(Cmd, Param, ARRAY_COUNT(Param), false) && Param[0] != '-')
	{
		//可能覆盖默认地图
		if (!MainMenuMap.Contains(Param))
		{
			//构造URL
			FURL DefaultURL;
			DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);

			FURL URL(&DefaultURL, Param, TRAVEL_Partial);
			if (URL.Valid)
			{
				UEngine* const Engine = GetEngine();
				FString Error;
				const EBrowseReturnVal::Type BrowseResult = Engine->Browse(*WorldContext, URL, Error);
			
				//todo
				return;
			}
		}
	}
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

		//to test 
		FString const StartURL = TEXT("/Game/Maps/Sanctuary?game=FFA?listen?bIsLanMatch?Bots=1");
		HostGame(GetFirstGamePlayer(), TEXT("Sanctuary"), StartURL);

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

bool UShooterGameInstance::HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL)
{
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		TravelURL = InTravelURL;
		bool IsLanMatch = TravelURL.Contains(TEXT("?bIsLanMatch"));
		OnCreatePresenceSessionCompleteDelegateHandle = GameSession->OnCreatePresenceSessionComplete().AddUObject(this, &UShooterGameInstance::OnCreatePresenceSessionComplete);
	
		const FString& MapNameSubStr = "/Game/Maps/";
		const FString& ChopedMapName = TravelURL.RightChop(MapNameSubStr.Len());
		const FString& MapName = TravelURL.LeftChop(ChopedMapName.Len() - ChopedMapName.Find("?game"));

		if (GameSession->HostSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, GameType, MapName, IsLanMatch, true, AShooterGameSession::DEFAULT_NUM_PLAYERS))
		{
			return true;
		}
	}
	return false;
}

AShooterGameSession* UShooterGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		//只有服务器端能拿到gamemode
		AGameModeBase* const GameMode = World->GetAuthGameMode();
		if (GameMode)
		{
			return Cast<AShooterGameSession>(GameMode->GameSession);
		}
	}
	return nullptr;
}

//创建session成功的代理函数
void UShooterGameInstance::OnCreatePresenceSessionComplete(FName InSessionName, bool bWasSuccessfull)
{
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		GameSession->OnCreatePresenceSessionComplete().Remove(OnCreatePresenceSessionCompleteDelegateHandle);

		//注册其他玩家
		if (bWasSuccessfull && LocalPlayers.Num() > 1)
		{
			auto Session = Online::GetSessionInterface();
			if (Session.IsValid()&&LocalPlayers[1]->GetPreferredUniqueNetId().IsValid())
			{
				Session->RegisterLocalPlayer(*LocalPlayers[1]->GetPreferredUniqueNetId(), InSessionName, FOnRegisterLocalPlayerCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnRegisterLocalPlayerComplete));
			}
		}
		else 
		{
			FinishSessionCreation(bWasSuccessfull ? EOnJoinSessionCompleteResult::Success : EOnJoinSessionCompleteResult::UnknownError);
		}
	}

}

//注册完成的代理函数
void UShooterGameInstance::OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result)
{
	FinishSessionCreation(Result);
}

void UShooterGameInstance::FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result)
{
	//切换地图
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		GetWorld()->ServerTravel(TravelURL);
	}
	else
	{
		//出错通知
	}

}
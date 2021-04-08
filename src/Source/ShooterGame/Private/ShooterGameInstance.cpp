// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameInstance.h"
#include "ShooterMainMenu.h"
#include "ShooterGameSession.h"

UShooterGameInstance::UShooterGameInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//重写构造函数，初始化当前状态
	CurrentState = ShooterGameInstanceState::None;
}

void UShooterGameInstance::Init()
{
	Super::Init();
	//定时代理
	TickDelegate = FTickerDelegate::CreateUObject(this, &UShooterGameInstance::Tick);
	//调用系统管理器
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
	OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnEndSessionComplete);
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
				//程序启动进入mainmenu状态
				if (BrowseResult == EBrowseReturnVal::Success)
				{
					GotoState(ShooterGameInstanceState::MainMenu);
				}
				
				return;
			}
		}
	}

	GotoInitialState();
}

//按固定时间调用Tick函数
bool UShooterGameInstance::Tick(float DeltaSeconds)
{
	MayChangeState();
	return true;
}

//客户端、服务器端都会进入该函数
void UShooterGameInstance::BeginMainMenuState()
{
	//加载默认地图
	LoadFrontEndMap(MainMenuMap);

	//新建对象，转化为共享指针
	MainMenuUI = MakeShareable(new FShooterMainMenu);
	//为了区别服务器上其他玩家
	ULocalPlayer* const Player = GetFirstGamePlayer();
	MainMenuUI->Construct(this, Player);

	MainMenuUI->AddMenuToViewport();

}

void UShooterGameInstance::EndMainMenuState()
{
	if (MainMenuUI.IsValid())
	{
		MainMenuUI->RemoveMenuToViewport();

		MainMenuUI = nullptr;
	}

}

bool UShooterGameInstance::HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL)
{
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		TravelURL = InTravelURL;
		bool IsLanMatch = TravelURL.Contains(TEXT("?bIsLanMatch"));
		OnCreatePresenceSessionCompleteDelegateHandle = GameSession->OnCreatePresenceSessionComplete().AddUObject(this, &UShooterGameInstance::OnCreatePresenceSessionComplete);
	
		//处理URL输入的信息
		const FString& MapNameSubStr = "/Game/Maps/";
		const FString& ChopedMapName = TravelURL.RightChop(MapNameSubStr.Len());
		const FString& MapName = ChopedMapName.LeftChop(ChopedMapName.Len() - ChopedMapName.Find("?game"));

		if (GameSession->HostSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, GameType, MapName, IsLanMatch, true, AShooterGameSession::DEFAULT_NUM_PLAYERS))
		{
			if (PendingState == ShooterGameInstanceState::None)
			{
				GotoState(ShooterGameInstanceState::Playing);
			}
			return true;
		}
	}
	return false;
}

bool UShooterGameInstance::FindSessions(ULocalPlayer* LocalPlayer, bool bIsLanMatch)
{
	bool bResult = false;

	check(LocalPlayer);
	
	if (LocalPlayer)
	{
		AShooterGameSession* const GameSession = GetGameSession();
		if (GameSession)
		{
			//清除shootergameinstance所有代理，防止重复绑定
			GameSession->OnFindSessionsComplete().RemoveAll(this);

			//添加代理函数，设置句柄，方便清理
			OnSearchSessionsCompleteDelegateHandle = GameSession->OnFindSessionsComplete().AddUObject(this, &UShooterGameInstance::OnSearchSessionsComplete);

			GameSession->FindSessions(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, bIsLanMatch, true);

			bResult = true;
		}
	}
	return bResult;
}

bool UShooterGameInstance::JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults)
{
	check(LocalPlayer);

	if (LocalPlayer)
	{
		//获取gamesession
		AShooterGameSession* const GameSession = GetGameSession();
		if (GameSession)
		{
			OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSessionComplete().AddUObject(this, &UShooterGameInstance::OnJoinSessionComplete);
			//调用底层joinsessions函数
			if (GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, SessionIndexInSearchResults))
			{
				//避免还在切换状态时调用，造成状态紊乱
				if (PendingState == ShooterGameInstanceState::None)
				{
					//调用成功，切换到游戏状态
					GotoState(ShooterGameInstanceState::Playing);
					return true;
				}
				
			}
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
		//清除代理句柄
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

void UShooterGameInstance::OnSearchSessionsComplete(bool bWasSuccessfull)
{
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		//清除代理句柄
		GameSession->OnFindSessionsComplete().Remove(OnSearchSessionsCompleteDelegateHandle);
	}
}

void UShooterGameInstance::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		//清除代理句柄
		GameSession->OnJoinSessionComplete().Remove(OnJoinSessionCompleteDelegateHandle);
		//加入成功，绑定注册完成代理函数
		if (Result == EOnJoinSessionCompleteResult::Success)
		{
			if (LocalPlayers.Num() > 1)
			{
				auto Session = Online::GetSessionInterface();
				if (Session.IsValid() && LocalPlayers[1]->GetPreferredUniqueNetId().IsValid())
				{
					Session->RegisterLocalPlayer(*LocalPlayers[1]->GetPreferredUniqueNetId(), GameSessionName, FOnRegisterLocalPlayerCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnRegisterJoiningLocalPlayerComplete));
				}
			}
			else
			{
				FinishJoinSession(Result);
			}
		}
		else
		{
			FinishJoinSession(Result);
		}
	}
}


void UShooterGameInstance::OnRegisterJoiningLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result)
{
	FinishJoinSession(Result);
}

void UShooterGameInstance::FinishJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	//加入成功，通知客户端切换地图
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		InternalTravelToSession(GameSessionName);
	}
	//加入失败，找出失败原因并通知用户
	else
	{

	}
}

void UShooterGameInstance::CleanupSession()
{
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
		if (Session.IsValid())
		{
			//获取session状态
			EOnlineSessionState::Type SessionState = Session->GetSessionState(GameSessionName);
			if (SessionState == EOnlineSessionState::InProgress)
			{
				OnEndSessionCompleteDelegateHandle = Session->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
				Session->EndSession(GameSessionName);//进入ending状态,endsession结束后，底层会调用绑定的代理函数
			}
			else if (SessionState == EOnlineSessionState::Ending)
			{
				//再一次绑定代理，以再一次调用OnEndSessionComplete，从而有机会再次检测状态
				OnEndSessionCompleteDelegateHandle = Session->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
			}
			else if (SessionState == EOnlineSessionState::Ended)
			{
				//Ended:意味着所有服务停止，可以安全销毁session
				OnDestroySessionCompleteDelegateHandle = Session->AddOnDestroySessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
				Session->DestroySession(GameSessionName);
			}
		}
	}
}

void UShooterGameInstance::OnEndSessionComplete(FName InSessionName, bool bWasSuccessfull)
{
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
		if (Session.IsValid())
		{
			//清除代理句柄
			Session->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);
			Session->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		}
	}
	CleanupSession();
}

void UShooterGameInstance::MayChangeState()
{
	if (CurrentState != PendingState && PendingState != ShooterGameInstanceState::None)
	{
		FName const OldState = CurrentState;
		EndCurrentState(PendingState);
		BeginNewState(PendingState, OldState);
		PendingState = ShooterGameInstanceState::None;
	}
}

void UShooterGameInstance::EndCurrentState(FName NextState)
{
	if (CurrentState == ShooterGameInstanceState::MainMenu)
	{
		EndMainMenuState();
	}
	else if (CurrentState == ShooterGameInstanceState::Playing)
	{
		EndPlayingState();
	}
	CurrentState = ShooterGameInstanceState::None;
}

void UShooterGameInstance::BeginNewState(FName NewState, FName PreState)
{
	if (NewState == ShooterGameInstanceState::MainMenu)
	{
		BeginMainMenuState();
	}
	else if (NewState == ShooterGameInstanceState::Playing)
	{
		BeginPlayingState();
	}
	CurrentState = NewState;
}


void UShooterGameInstance::BeginPlayingState()
{
	//获取焦点，聚集到游戏界面中
	FSlateApplication::Get().SetAllUserFocusToGameViewport();

}
void UShooterGameInstance::EndPlayingState()
{
	CleanupSession();
}

void UShooterGameInstance::GotoState(FName NewState)
{
	PendingState = NewState;
}

void UShooterGameInstance::GotoInitialState()
{
	GotoState(GetInitialState());
}

FName UShooterGameInstance::GetInitialState()
{
	return ShooterGameInstanceState::MainMenu;
}

bool UShooterGameInstance::LoadFrontEndMap(const FString& MapName)
{
	bool bSuccess = true;
	
	UWorld* const World = GetWorld();
	if (World)
	{
		//针对服务器：要加载的Map与当前Map一致则跳过
		//获取地图名字（地图在资源包里的路径）
		FString const CurrentMapPath = World->PersistentLevel->GetOutermost()->GetName();
		if (MapName == CurrentMapPath)
		{
			return bSuccess;
		}
	}
	
	//针对客户端：加载默认地图
	FURL URL(*FString::Printf(TEXT("%s"), *MapName));
	FString Error;
	EBrowseReturnVal::Type BrowseResult = EBrowseReturnVal::Failure;
	if (URL.Valid && !HasAnyFlags(RF_ClassDefaultObject))
	{
		BrowseResult = GetEngine()->Browse(*WorldContext, URL, Error);
		if (BrowseResult != EBrowseReturnVal::Success)
		{
			bSuccess = false;
		}
	}
	return bSuccess;
}

//客户端地图切换
void UShooterGameInstance::InternalTravelToSession(const FName& InSessionName)
{
	//ClientTravel定义在PlayerController中，获取PlayerController
	APlayerController* const PC = GetFirstLocalPlayerController();
	if (PC == nullptr)
	{
		return;
	}
	IOnlineSubsystem* OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys == nullptr)
	{
		return;
	}
	auto Session = Online::GetSessionInterface();
	FString URL;
	//通过获取连接信息获取服务器URL地址
	if (!Session.IsValid() || !Session->GetResolvedConnectString(InSessionName, URL))
	{
		return;
	}

	//ClientTravel切换地图  TRAVEL_Relative（无缝切换、非阻塞式）
	PC->ClientTravel(URL, TRAVEL_Absolute);
	
}
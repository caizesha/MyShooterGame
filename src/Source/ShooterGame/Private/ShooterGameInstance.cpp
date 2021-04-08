// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameInstance.h"
#include "ShooterMainMenu.h"
#include "ShooterGameSession.h"

UShooterGameInstance::UShooterGameInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//��д���캯������ʼ����ǰ״̬
	CurrentState = ShooterGameInstanceState::None;
}

void UShooterGameInstance::Init()
{
	Super::Init();
	//��ʱ����
	TickDelegate = FTickerDelegate::CreateUObject(this, &UShooterGameInstance::Tick);
	//����ϵͳ������
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
	OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnEndSessionComplete);
}

void UShooterGameInstance::Shutdown()
{
	Super::Shutdown();
	//��ע�ᶨʱ�����
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void UShooterGameInstance::StartGameInstance()
{
	//��ȡ�����в���
	const TCHAR* Cmd= FCommandLine::Get();
	TCHAR Param[4096] = TEXT("");
	//���������н��� token:�ָ���ո�ָ����ִ� false:����Ҫʹ��ת���
	if (FParse::Token(Cmd, Param, ARRAY_COUNT(Param), false) && Param[0] != '-')
	{
		//���ܸ���Ĭ�ϵ�ͼ
		if (!MainMenuMap.Contains(Param))
		{
			//����URL
			FURL DefaultURL;
			DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);

			FURL URL(&DefaultURL, Param, TRAVEL_Partial);
			if (URL.Valid)
			{
				UEngine* const Engine = GetEngine();
				FString Error;
				const EBrowseReturnVal::Type BrowseResult = Engine->Browse(*WorldContext, URL, Error);
				//������������mainmenu״̬
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

//���̶�ʱ�����Tick����
bool UShooterGameInstance::Tick(float DeltaSeconds)
{
	MayChangeState();
	return true;
}

//�ͻ��ˡ��������˶������ú���
void UShooterGameInstance::BeginMainMenuState()
{
	//����Ĭ�ϵ�ͼ
	LoadFrontEndMap(MainMenuMap);

	//�½�����ת��Ϊ����ָ��
	MainMenuUI = MakeShareable(new FShooterMainMenu);
	//Ϊ��������������������
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
	
		//����URL�������Ϣ
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
			//���shootergameinstance���д�����ֹ�ظ���
			GameSession->OnFindSessionsComplete().RemoveAll(this);

			//��Ӵ����������þ������������
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
		//��ȡgamesession
		AShooterGameSession* const GameSession = GetGameSession();
		if (GameSession)
		{
			OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSessionComplete().AddUObject(this, &UShooterGameInstance::OnJoinSessionComplete);
			//���õײ�joinsessions����
			if (GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, SessionIndexInSearchResults))
			{
				//���⻹���л�״̬ʱ���ã����״̬����
				if (PendingState == ShooterGameInstanceState::None)
				{
					//���óɹ����л�����Ϸ״̬
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
		//ֻ�з����������õ�gamemode
		AGameModeBase* const GameMode = World->GetAuthGameMode();
		if (GameMode)
		{
			return Cast<AShooterGameSession>(GameMode->GameSession);
		}
	}
	return nullptr;
}

//����session�ɹ��Ĵ�����
void UShooterGameInstance::OnCreatePresenceSessionComplete(FName InSessionName, bool bWasSuccessfull)
{
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		//���������
		GameSession->OnCreatePresenceSessionComplete().Remove(OnCreatePresenceSessionCompleteDelegateHandle);

		//ע���������
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

//ע����ɵĴ�����
void UShooterGameInstance::OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result)
{
	FinishSessionCreation(Result);
}

void UShooterGameInstance::FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result)
{
	//�л���ͼ
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		GetWorld()->ServerTravel(TravelURL);
	}
	else
	{
		//����֪ͨ
	}

}

void UShooterGameInstance::OnSearchSessionsComplete(bool bWasSuccessfull)
{
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		//���������
		GameSession->OnFindSessionsComplete().Remove(OnSearchSessionsCompleteDelegateHandle);
	}
}

void UShooterGameInstance::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		//���������
		GameSession->OnJoinSessionComplete().Remove(OnJoinSessionCompleteDelegateHandle);
		//����ɹ�����ע����ɴ�����
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
	//����ɹ���֪ͨ�ͻ����л���ͼ
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		InternalTravelToSession(GameSessionName);
	}
	//����ʧ�ܣ��ҳ�ʧ��ԭ��֪ͨ�û�
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
			//��ȡsession״̬
			EOnlineSessionState::Type SessionState = Session->GetSessionState(GameSessionName);
			if (SessionState == EOnlineSessionState::InProgress)
			{
				OnEndSessionCompleteDelegateHandle = Session->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
				Session->EndSession(GameSessionName);//����ending״̬,endsession�����󣬵ײ����ð󶨵Ĵ�����
			}
			else if (SessionState == EOnlineSessionState::Ending)
			{
				//��һ�ΰ󶨴�������һ�ε���OnEndSessionComplete���Ӷ��л����ٴμ��״̬
				OnEndSessionCompleteDelegateHandle = Session->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
			}
			else if (SessionState == EOnlineSessionState::Ended)
			{
				//Ended:��ζ�����з���ֹͣ�����԰�ȫ����session
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
			//���������
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
	//��ȡ���㣬�ۼ�����Ϸ������
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
		//��Է�������Ҫ���ص�Map�뵱ǰMapһ��������
		//��ȡ��ͼ���֣���ͼ����Դ�����·����
		FString const CurrentMapPath = World->PersistentLevel->GetOutermost()->GetName();
		if (MapName == CurrentMapPath)
		{
			return bSuccess;
		}
	}
	
	//��Կͻ��ˣ�����Ĭ�ϵ�ͼ
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

//�ͻ��˵�ͼ�л�
void UShooterGameInstance::InternalTravelToSession(const FName& InSessionName)
{
	//ClientTravel������PlayerController�У���ȡPlayerController
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
	//ͨ����ȡ������Ϣ��ȡ������URL��ַ
	if (!Session.IsValid() || !Session->GetResolvedConnectString(InSessionName, URL))
	{
		return;
	}

	//ClientTravel�л���ͼ  TRAVEL_Relative���޷��л���������ʽ��
	PC->ClientTravel(URL, TRAVEL_Absolute);
	
}
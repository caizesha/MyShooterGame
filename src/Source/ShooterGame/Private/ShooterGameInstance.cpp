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

void UShooterGameInstance::BeginMainMenuState()
{
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
	//todo
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
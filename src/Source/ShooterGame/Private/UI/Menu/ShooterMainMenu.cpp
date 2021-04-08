#include "ShooterGame.h"
#include "ShooterMainMenu.h"
#include "ShooterGameInstance.h"
#include "SShooterMenuWidget.h"
#include "ShooterMenuItem.h"
#include "ShooterGameSession.h"

#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"

FShooterMainMenu::FShooterMainMenu()
{
	bSearchForServers = false;
}

FShooterMainMenu::~FShooterMainMenu()
{

}

void FShooterMainMenu::Tick(float DeltaSeconds)
{
	if (bSearchForServers)
	{
		UpdateSearchStatus();
	}
}

bool FShooterMainMenu::IsTickable() const
{
	return true;
}

TStatId FShooterMainMenu::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FShooterMainMenu, STATGROUP_Tickables);
}

void FShooterMainMenu::Construct(TWeakObjectPtr<UShooterGameInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner)
{
	GameInstance = _GameInstance;
	PlayerOwner = _PlayerOwner;

	MenuWidget.Reset();
	MenuWidgetContainer.Reset();

	ShooterOptions = MakeShareable(new FShooterOptions());
	ShooterOptions->Construct(_PlayerOwner.Get());
	ShooterOptions->OnApplyChanges.BindSP(this, &FShooterMainMenu::CloseSubMenu);
	//�����˵�UI�������
	if (GEngine && GEngine->GameViewport)
	{
		SAssignNew(MenuWidget, SShooterMenuWidget)
			.Cursor(EMouseCursor::Default)
			.PlayerOwner(GetPlayerOwner());//Ĭ�������

		SAssignNew(MenuWidgetContainer, SWeakWidget)
			.PossiblyNullContent(MenuWidget);//����widget����

		TSharedPtr<FShooterMenuItem> RootMenuItem;

		TSharedPtr<FShooterMenuItem> MenuItem;

		MenuItem = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Host", "������"));

		MenuHelper::AddMenuItemSP<FShooterMainMenu>(MenuItem, LOCTEXT("FFA", "����ģʽ"), this, &FShooterMainMenu::OnUIHostFreeForAll);

		MenuItem = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Join", "������Ϸ"));

		MenuHelper::AddMenuItemSP<FShooterMainMenu>(MenuItem, LOCTEXT("Search", "����������"), this, &FShooterMainMenu::OnSearchServer);

		MenuHelper::AddMenuItemSP<FShooterMainMenu>(MenuItem, LOCTEXT("Join", "���������"), this, &FShooterMainMenu::OnJoinGame);

		MenuHelper::AddExistingMenuItem(RootMenuItem, ShooterOptions->OptionsItem.ToSharedRef());

		MenuHelper::AddMenuItemSP<FShooterMainMenu>(RootMenuItem, LOCTEXT("����", "����"), this, &FShooterMainMenu::OnUIQuit);

		MenuWidget->CurrentMenu = RootMenuItem->SubMenu;
		MenuWidget->OnMenuHidden.BindSP(this, &FShooterMainMenu::OnMenuHidden);
		MenuWidget->BuildAndShowMenu();
	}
}

void FShooterMainMenu::AddMenuToViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		//��ȡ��Ϸ����
		UGameViewportClient* const  GVC = GEngine->GameViewport;
		GVC->AddViewportWidgetContent(MenuWidgetContainer.ToSharedRef());

	}
}

void FShooterMainMenu::RemoveMenuToViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		//��ȡ��Ϸ����
		UGameViewportClient* const  GVC = GEngine->GameViewport;
		GVC->RemoveViewportWidgetContent(MenuWidgetContainer.ToSharedRef());

	}
}

ULocalPlayer* FShooterMainMenu::GetPlayerOwner() const
{
	return PlayerOwner.Get();
}

void FShooterMainMenu::OnUIQuit()
{
	if (ensure(GameInstance.IsValid()))
	{
		UGameViewportClient* const ViewportClient = GameInstance->GetGameViewportClient();
		if (ensure(ViewportClient))
		{
			ViewportClient->ConsoleCommand("quit");
		}
	}
}

void FShooterMainMenu::CloseSubMenu()
{
	//���˵��ϼ��˵�
	MenuWidget->MenuGoBack();
}

void FShooterMainMenu::OnMenuHidden()
{
	RemoveMenuToViewport();
}

void FShooterMainMenu::OnUIHostFreeForAll()
{
	if (ensure(GameInstance.IsValid() && GetPlayerOwner() != NULL))
	{
		//����������
		FString const StartURL = TEXT("/Game/Maps/Sanctuary?game=FFA?listen?bIsLanMatch?Bots=1");
		GameInstance->HostGame(GetPlayerOwner(), TEXT("FFA"), StartURL);

		OnMenuHidden();
	}
}

void FShooterMainMenu::OnSearchServer()
{
	//��ֹ�û���ε������
	if (bSearchForServers)
	{
		return;
	}
	if (ensure(GameInstance.IsValid() && GetPlayerOwner() != NULL))
	{	
		bSearchForServers = true;
		GameInstance->FindSessions(GetPlayerOwner(), true);
	}
}

void FShooterMainMenu::OnJoinGame()
{
	ConnentToServer();
}

void FShooterMainMenu::ConnentToServer()
{
	if (bSearchForServers)
	{
		return;
	}

	if (ServerList.Num() < 0)
	{
		return;
	}

	//��ȡ��ǰѡ���������
	SelectedServer = ServerList[0];
	if (SelectedServer.IsValid())
	{
		//�������UI
		if (GEngine && GEngine->GameViewport)
		{
			//��ȡ��Ϸ����
			UGameViewportClient* const  GVC = GEngine->GameViewport;
			GVC->RemoveAllViewportWidgets();
		}
		//���ü���Session�ӿ�
		UShooterGameInstance* const GameInstance = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
		GameInstance->JoinSession(PlayerOwner.Get(), SelectedServer->SearchResultsIndex);
	}
}

AShooterGameSession* FShooterMainMenu::GetGameSession() const
{
	UShooterGameInstance* const GameInstance = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
	return GameInstance ? GameInstance->GetGameSession() : nullptr;
}

//��ѯ
void FShooterMainMenu::UpdateSearchStatus()
{
	bool bFinishSearch = false;

	AShooterGameSession* GameSession = GetGameSession();
	if (GameSession)
	{
		int32 CurrentSearchIndex, NumSearchResults;
		EOnlineAsyncTaskState::Type SearchState = GameSession->GetSearchResultState(CurrentSearchIndex, NumSearchResults);
		switch (SearchState)
		{
		case EOnlineAsyncTaskState::NotStarted:
		case EOnlineAsyncTaskState::InProgress:
			bFinishSearch = false;
			break;
		case EOnlineAsyncTaskState::Done:
		{
			ServerList.Empty();
			const TArray<FOnlineSessionSearchResult>& SearchResults = GameSession->GetSearchResults();
			check(NumSearchResults == SearchResults.Num());

			for (int32 Index = 0; Index < SearchResults.Num(); Index++)
			{
				//�����������
				TSharedPtr<FServerEntry> NewServerEntry = MakeShareable(new FServerEntry);
				const FOnlineSessionSearchResult& Result = SearchResults[Index];
				NewServerEntry->ServerName = Result.Session.OwningUserName;
				NewServerEntry->Ping = FString::FromInt(Result.PingInMs);
				NewServerEntry->CurrentPlayers = FString::FromInt(Result.Session.SessionSettings.NumPublicConnections +
					Result.Session.SessionSettings.NumPrivateConnections - Result.Session.NumOpenPublicConnections
					- Result.Session.NumOpenPrivateConnections);
				NewServerEntry->MaxPlayers = FString::FromInt(Result.Session.SessionSettings.NumPublicConnections +
					Result.Session.SessionSettings.NumPrivateConnections);
				NewServerEntry->SearchResultsIndex = Index;
				Result.Session.SessionSettings.Get(SETTING_GAMEMODE, NewServerEntry->GameType);
				Result.Session.SessionSettings.Get(SETTING_MAPNAME, NewServerEntry->MapName);

				ServerList.Add(NewServerEntry);
				bFinishSearch = true;
			}
		}
		break;
		case EOnlineAsyncTaskState::Failed:
			bFinishSearch = false;
			break;
		default:
			break;
		}
	}
	
	if (bFinishSearch)
	{
		OnServerSearchFinished();
	}
}

void FShooterMainMenu::OnServerSearchFinished()
{
	bSearchForServers = false;
}

#undef LOCTEXT_NAMESPACE
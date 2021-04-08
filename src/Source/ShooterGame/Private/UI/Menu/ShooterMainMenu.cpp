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
	//创建菜单UI管理对象
	if (GEngine && GEngine->GameViewport)
	{
		SAssignNew(MenuWidget, SShooterMenuWidget)
			.Cursor(EMouseCursor::Default)
			.PlayerOwner(GetPlayerOwner());//默认鼠标光标

		SAssignNew(MenuWidgetContainer, SWeakWidget)
			.PossiblyNullContent(MenuWidget);//传递widget内容

		TSharedPtr<FShooterMenuItem> RootMenuItem;

		TSharedPtr<FShooterMenuItem> MenuItem;

		MenuItem = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Host", "服务器"));

		MenuHelper::AddMenuItemSP<FShooterMainMenu>(MenuItem, LOCTEXT("FFA", "任意模式"), this, &FShooterMainMenu::OnUIHostFreeForAll);

		MenuItem = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Join", "加入游戏"));

		MenuHelper::AddMenuItemSP<FShooterMainMenu>(MenuItem, LOCTEXT("Search", "搜索服务器"), this, &FShooterMainMenu::OnSearchServer);

		MenuHelper::AddMenuItemSP<FShooterMainMenu>(MenuItem, LOCTEXT("Join", "加入服务器"), this, &FShooterMainMenu::OnJoinGame);

		MenuHelper::AddExistingMenuItem(RootMenuItem, ShooterOptions->OptionsItem.ToSharedRef());

		MenuHelper::AddMenuItemSP<FShooterMainMenu>(RootMenuItem, LOCTEXT("放弃", "放弃"), this, &FShooterMainMenu::OnUIQuit);

		MenuWidget->CurrentMenu = RootMenuItem->SubMenu;
		MenuWidget->OnMenuHidden.BindSP(this, &FShooterMainMenu::OnMenuHidden);
		MenuWidget->BuildAndShowMenu();
	}
}

void FShooterMainMenu::AddMenuToViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		//获取游戏窗口
		UGameViewportClient* const  GVC = GEngine->GameViewport;
		GVC->AddViewportWidgetContent(MenuWidgetContainer.ToSharedRef());

	}
}

void FShooterMainMenu::RemoveMenuToViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		//获取游戏窗口
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
	//回退到上级菜单
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
		//创建服务器
		FString const StartURL = TEXT("/Game/Maps/Sanctuary?game=FFA?listen?bIsLanMatch?Bots=1");
		GameInstance->HostGame(GetPlayerOwner(), TEXT("FFA"), StartURL);

		OnMenuHidden();
	}
}

void FShooterMainMenu::OnSearchServer()
{
	//防止用户多次点击搜索
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

	//获取当前选择服务器项
	SelectedServer = ServerList[0];
	if (SelectedServer.IsValid())
	{
		//清空所有UI
		if (GEngine && GEngine->GameViewport)
		{
			//获取游戏窗口
			UGameViewportClient* const  GVC = GEngine->GameViewport;
			GVC->RemoveAllViewportWidgets();
		}
		//调用加入Session接口
		UShooterGameInstance* const GameInstance = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
		GameInstance->JoinSession(PlayerOwner.Get(), SelectedServer->SearchResultsIndex);
	}
}

AShooterGameSession* FShooterMainMenu::GetGameSession() const
{
	UShooterGameInstance* const GameInstance = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
	return GameInstance ? GameInstance->GetGameSession() : nullptr;
}

//轮询
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
				//保存搜索结果
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
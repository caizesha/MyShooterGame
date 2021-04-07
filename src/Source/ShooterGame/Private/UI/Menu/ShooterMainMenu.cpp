#include "ShooterGame.h"
#include "ShooterMainMenu.h"
#include "ShooterGameInstance.h"
#include "SShooterMenuWidget.h"
#include "ShooterMenuItem.h"

#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"

FShooterMainMenu::FShooterMainMenu()
{

}

FShooterMainMenu::~FShooterMainMenu()
{

}

void FShooterMainMenu::Tick(float DeltaSeconds)
{
	//todo
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

#undef LOCTEXT_NAMESPACE
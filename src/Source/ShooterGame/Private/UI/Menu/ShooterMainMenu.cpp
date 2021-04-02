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

	//创建菜单UI管理对象
	if (GEngine && GEngine->GameViewport)
	{
		SAssignNew(MenuWidget, SShooterMenuWidget)
			.Cursor(EMouseCursor::Default)
			.PlayerOwner(GetPlayerOwner());//默认鼠标光标

		SAssignNew(MenuWidgetContainer, SWeakWidget)
			.PossiblyNullContent(MenuWidget);//传递widget内容

		//创建菜单根组件
		TSharedPtr<FShooterMenuItem> RootMenuItem;

		TArray<FText> OnOffList;
		OnOffList.Add(LOCTEXT("关", "关"));
		OnOffList.Add(LOCTEXT("开", "开"));

		MenuHelper::AddMenuOptionSP<FShooterMainMenu>(RootMenuItem, LOCTEXT("全屏", "全屏"), OnOffList, this, &FShooterMainMenu::FullScreenOptionChanged);

		MenuHelper::AddMenuItemSP<FShooterMainMenu>(RootMenuItem, LOCTEXT("放弃", "放弃"), this, &FShooterMainMenu::OnUIQuit);

		MenuWidget->CurrentMenu = RootMenuItem->SubMenu;
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
		UGameViewportClient* const ViewportClient  = GameInstance->GetGameViewportClient();
		if (ensure(ViewportClient))
		{
			ViewportClient->ConsoleCommand("quit");
		}
	}
}

void FShooterMainMenu::FullScreenOptionChanged(TSharedPtr<FShooterMenuItem> MenuItem, int32 OptionIndex)
{
	//todo

}
#undef LOCTEXT_NAMESPACE
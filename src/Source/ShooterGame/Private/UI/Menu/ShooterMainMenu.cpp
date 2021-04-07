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

#undef LOCTEXT_NAMESPACE
// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterOptions.h"
#include "ShooterMenuItem.h"
#include "SShooterMenuWidget.h"


#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"

FShooterOptions::FShooterOptions()
{
}

FShooterOptions::~FShooterOptions()
{
}

//�����˵����ṹ
void FShooterOptions::Construct(ULocalPlayer* InPlayerOwner)
{
	PlayerOwner = InPlayerOwner;
	//�������ڵ�
	TSharedPtr<FShooterMenuItem> OptionsRoot = FShooterMenuItem::CreateRoot();
	OptionsItem = MenuHelper::AddMenuItem(OptionsRoot, LOCTEXT("ѡ��", "ѡ��"));

	TArray<FText> OnOffList;
	OnOffList.Add(LOCTEXT("��", "��"));
	OnOffList.Add(LOCTEXT("��", "��"));

	MenuHelper::AddMenuOptionSP<FShooterOptions>(OptionsItem, LOCTEXT("ȫ��", "ȫ��"), OnOffList, this, &FShooterOptions::FullScreenOptionChanged);

	MenuHelper::AddMenuItemSP<FShooterOptions>(OptionsItem, LOCTEXT("Ӧ��", "Ӧ��"), this, &FShooterOptions::OnApplySettings);
}

void FShooterOptions::FullScreenOptionChanged(TSharedPtr<FShooterMenuItem> MenuItem, int32 OptionIndex)
{
	//todo

}

//��������Ӧ������
void FShooterOptions::OnApplySettings()
{
	//todo
}
#undef LOCTEXT_NAMESPACE
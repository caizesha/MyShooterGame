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

//构建菜单树结构
void FShooterOptions::Construct(ULocalPlayer* InPlayerOwner)
{
	PlayerOwner = InPlayerOwner;
	//创建根节点
	TSharedPtr<FShooterMenuItem> OptionsRoot = FShooterMenuItem::CreateRoot();
	OptionsItem = MenuHelper::AddMenuItem(OptionsRoot, LOCTEXT("选项", "选项"));

	TArray<FText> OnOffList;
	OnOffList.Add(LOCTEXT("关", "关"));
	OnOffList.Add(LOCTEXT("开", "开"));

	MenuHelper::AddMenuOptionSP<FShooterOptions>(OptionsItem, LOCTEXT("全屏", "全屏"), OnOffList, this, &FShooterOptions::FullScreenOptionChanged);

	MenuHelper::AddMenuItemSP<FShooterOptions>(OptionsItem, LOCTEXT("应用", "应用"), this, &FShooterOptions::OnApplySettings);
}

void FShooterOptions::FullScreenOptionChanged(TSharedPtr<FShooterMenuItem> MenuItem, int32 OptionIndex)
{
	//todo

}

//处理所有应用配置
void FShooterOptions::OnApplySettings()
{
	//todo
}
#undef LOCTEXT_NAMESPACE
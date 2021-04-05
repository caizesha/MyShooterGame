// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SShooterMenuItem.h"

/**
 * 
 */

//定义菜单项类型
namespace EShooterMenuItemType
{
	enum Type
	{
		Root,
		Standard,
		MultiChoice,
		CustomWidget
	};
}

typedef TArray<TSharedPtr<class FShooterMenuItem>> MenuPtr;

//保存菜单信息
class FShooterMenuInfo
{
public:
	MenuPtr Menu;
	int32 SelectedIndex;
	FText MenuTitle;
	
	FShooterMenuInfo(MenuPtr _Menu, int32 _SelectedIndex, FText _MenuTitle)
	{
		Menu = _Menu;
		SelectedIndex = _SelectedIndex;
		MenuTitle = _MenuTitle;
	}
};

//加入TSharedFromThis内存管理
class FShooterMenuItem : public TSharedFromThis<FShooterMenuItem>
{
public:

	DECLARE_DELEGATE(FOnConfirmMenuItem)

	DECLARE_DELEGATE_TwoParams(FOnOptionChanged, TSharedPtr<FShooterMenuItem>, int32)

	FOnConfirmMenuItem OnConfirmMenuItem;

	FOnOptionChanged OnOptionChanged;

	FShooterMenuItem(FText _Text)
	{
		bVisible = true;
		//仅做简单引用，防止内存进行一些不必要的拷贝
		Text = MoveTemp(_Text);
		MenuItemType = EShooterMenuItemType::Standard;

	}

	FShooterMenuItem(FText _Text, TArray<FText> _MultiChoice, int32 DefaultIndex = 0)
	{
		bVisible = true;
		Text = MoveTemp(_Text);
		MultiChoice = MoveTemp(_MultiChoice);
		SelectedMultiChoice = DefaultIndex;
		MinMultiChoiceIndex = MaxMultiChoiceIndex = -1;
		MenuItemType = EShooterMenuItemType::MultiChoice;
	}

	~FShooterMenuItem();


	const FText& GetText() const;

	void SetText(FText UpdatedText);

	//创建根（虚拟）菜单项
	static TSharedRef<FShooterMenuItem> CreateRoot()
	{
		return MakeShareable(new FShooterMenuItem());
	}

	TSharedPtr<SShooterMenuItem> Widget;

	MenuPtr SubMenu;

	bool bVisible;

	TArray<FText> MultiChoice;

	int32 MinMultiChoiceIndex;

	int32 MaxMultiChoiceIndex;

	int32 SelectedMultiChoice;

	EShooterMenuItemType::Type MenuItemType;

private:

	FShooterMenuItem()
	{
		bVisible = false;
	}
	FText Text;

};

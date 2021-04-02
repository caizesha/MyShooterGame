// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/ShooterMenuItem.h"

/**
 * 
 */

struct FShooterMenuStyle;

//UI管理类：用于界面搭建、界面相应逻辑管理
class SShooterMenuWidget: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SShooterMenuWidget)
		:_PlayerOwner()
	{

	}

	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)//变量声明
	SLATE_END_ARGS()


	SShooterMenuWidget();

	virtual ~SShooterMenuWidget();

	void Construct (const FArguments& InArgs);
	
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)override;

	virtual bool SupportsKeyboardFocus()const override { return true; }

	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	FText GetMenuTitle() const;

	FLinearColor GetBottomColor() const;

	FText GetOptionText(TSharedPtr<FShooterMenuItem> MenuItem) const;

	void ChangeOption(int32 MoveBy);

	void UpdateArrow(TSharedPtr<FShooterMenuItem> MenuItem);

	//左右容器填充图片作为测试控件
	void BuildAndShowMenu();

	void BuildLeftPanel(bool bGoingBack = false);

	void BuildRightPanel();

	//实现菜单居中显示，获得菜单偏移量
	FMargin GetMenuOffset() const;

	//ButtonIndex:当前点击的菜单项
	FReply ButtonClicked(int32 ButtonIndex);

	void ConfirmMenuItem();

	MenuPtr CurrentMenu;//当前一级菜单项列表

	MenuPtr NextMenu;//下一菜单项列表

	//int32 test = 1;
private:
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	float MenuHeaderWidth;
	
	float MenuHeaderHeight;

	float OutlineWidth;

	FText CurrentMenuTitle;

	const struct FShooterMenuStyle* MenuStyle;

	TSharedPtr<SVerticalBox> LeftBox;

	TSharedPtr<SVerticalBox> RightBox;

	FIntPoint ScreenResolution;

	int32 SelectedIndex;

};

//提供创建标准菜单项、多选项菜单项的辅助函数
namespace MenuHelper
{
	//对传入的父菜单项进行判断
	FORCEINLINE void EnsureValid(TSharedPtr<FShooterMenuItem>& ParentMenuItem)
	{
		if (!ParentMenuItem.IsValid())
		{
			ParentMenuItem = FShooterMenuItem::CreateRoot();
		}
	}

	//创建标准菜单项并绑定响应函数 内联函数：将函数内容全部内迁到调用处
	template<class UserClass>
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuItemSP(TSharedPtr<FShooterMenuItem>& ParentMenuItem, const FText& Text, UserClass* InObj, typename FShooterMenuItem::FOnConfirmMenuItem::TSPMethodDelegate<UserClass>::FMethodPtr InMethod)
	{
		EnsureValid(ParentMenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text));
		Item->OnConfirmMenuItem.BindSP(InObj, InMethod);
		ParentMenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	//创建多选项菜单项并绑定响应函数
	template<class UserClass>
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuOptionSP(TSharedPtr<FShooterMenuItem>& ParentMenuItem, const FText& Text, const TArray<FText>& OptionList, UserClass* InObj, typename FShooterMenuItem::FOnOptionChanged::TSPMethodDelegate<UserClass>::FMethodPtr InMethod)
	{
		EnsureValid(ParentMenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text, OptionList));
		Item->OnOptionChanged.BindSP(InObj, InMethod);
		ParentMenuItem->SubMenu.Add(Item);
		return ParentMenuItem->SubMenu.Last().ToSharedRef();
	}
}

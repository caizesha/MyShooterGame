// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/ShooterMenuItem.h"

/**
 * 
 */

struct FShooterMenuStyle;

//UI�����ࣺ���ڽ�����������Ӧ�߼�����
class SShooterMenuWidget: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SShooterMenuWidget)
		:_PlayerOwner()
	{

	}

	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)//��������
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

	//�����������ͼƬ��Ϊ���Կؼ�
	void BuildAndShowMenu();

	void BuildLeftPanel(bool bGoingBack = false);

	void BuildRightPanel();

	//ʵ�ֲ˵�������ʾ����ò˵�ƫ����
	FMargin GetMenuOffset() const;

	//ButtonIndex:��ǰ����Ĳ˵���
	FReply ButtonClicked(int32 ButtonIndex);

	void ConfirmMenuItem();

	MenuPtr CurrentMenu;//��ǰһ���˵����б�

	MenuPtr NextMenu;//��һ�˵����б�

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

//�ṩ������׼�˵����ѡ��˵���ĸ�������
namespace MenuHelper
{
	//�Դ���ĸ��˵�������ж�
	FORCEINLINE void EnsureValid(TSharedPtr<FShooterMenuItem>& ParentMenuItem)
	{
		if (!ParentMenuItem.IsValid())
		{
			ParentMenuItem = FShooterMenuItem::CreateRoot();
		}
	}

	//������׼�˵������Ӧ���� ��������������������ȫ����Ǩ�����ô�
	template<class UserClass>
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuItemSP(TSharedPtr<FShooterMenuItem>& ParentMenuItem, const FText& Text, UserClass* InObj, typename FShooterMenuItem::FOnConfirmMenuItem::TSPMethodDelegate<UserClass>::FMethodPtr InMethod)
	{
		EnsureValid(ParentMenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text));
		Item->OnConfirmMenuItem.BindSP(InObj, InMethod);
		ParentMenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	//������ѡ��˵������Ӧ����
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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterMenuItem.h"

/**
 * 
 */

struct FShooterMenuStyle;

//UI�����ࣺ���ڽ�����������Ӧ�߼�����
class SShooterMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SShooterMenuWidget)
		:_PlayerOwner()
	{

	}

	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)//��������
	SLATE_END_ARGS()

	DECLARE_DELEGATE(FOnMenuHidden)

	SShooterMenuWidget();
	virtual ~SShooterMenuWidget();

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	virtual bool SupportsKeyboardFocus() const override { return true; }

	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	FText GetMenuTitle() const;

	FLinearColor GetBottomColor() const;

	FText GetOptionText(TSharedPtr<FShooterMenuItem> MenuItem) const;
	
	//ButtonIndex:��ǰ����Ĳ˵���
	FReply ButtonClicked(int32 ButtonIndex);

	void ChangeOption(int32 MoveBy);

	void UpdateArrow(TSharedPtr<FShooterMenuItem> MenuItem);

	void BuildAndShowMenu();

	void BuildLeftPanel(bool bGoingBack = false);

	void BuildRightPanel();

	//ʵ�ֲ˵�������ʾ����ò˵�ƫ����
	FMargin GetMenuOffset() const;

	void ConfirmMenuItem();

	FVector2D GetBottomScale() const;
	
	//��ȡ��һ������
	int32 GetNextValidIndex(int32 MoveBy);

	int32 GetOwnerUserIndex();

	void EnterSubMenu();

	void MenuGoBack();

	MenuPtr CurrentMenu; //��ǰһ���˵����б�

	MenuPtr NextMenu;//��һ���˵����б�

	MenuPtr PendingLeftMenu;//�������µ��󲿲˵�����ʱ�˵�

	FOnMenuHidden OnMenuHidden;

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

	//��ʾ��ͬ���ѡ����Ӳ˵�
	bool bSubMenuChanging;

	//��־��߲˵��Ƿ����滻
	bool bLeftMenuChanging;

	TArray<FShooterMenuInfo> MenuHistory;

	bool bGoingBack;
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

	//������׼�˵���,������Ӧ����
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuItem(TSharedPtr<FShooterMenuItem>& ParentMenuItem, const FText& Text)
	{
		EnsureValid(ParentMenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text));
		ParentMenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	//��������������Ѵ��ڲ˵���
	FORCEINLINE TSharedRef<FShooterMenuItem> AddExistingMenuItem(TSharedPtr<FShooterMenuItem>& ParentMenuItem, TSharedRef<FShooterMenuItem> SubMenuItem)
	{
		EnsureValid(ParentMenuItem);
		ParentMenuItem->SubMenu.Add(SubMenuItem);
		return ParentMenuItem->SubMenu.Last().ToSharedRef();
	}

	//������׼�˵������Ӧ���� ��������������������ȫ����Ǩ�����ô�
	template<class UserClass>
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuItemSP(TSharedPtr<FShooterMenuItem>& ParentMenuItem, const FText& Text, UserClass* InObj, typename FShooterMenuItem::FOnConfirmMenuItem::TSPMethodDelegate<UserClass>::FMethodPtr inMethod)
	{
		EnsureValid(ParentMenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text));
		Item->OnConfirmMenuItem.BindSP(InObj, inMethod);
		ParentMenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	//������ѡ��˵������Ӧ����
	template<class UserClass>
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuOptionSP(TSharedPtr<FShooterMenuItem>& ParentMenuItem, const FText& Text, const TArray<FText>& OptionList, UserClass* InObj, typename FShooterMenuItem::FOnOptionChanged::TSPMethodDelegate<UserClass>::FMethodPtr inMethod)
	{
		EnsureValid(ParentMenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text, OptionList));
		Item->OnOptionChanged.BindSP(InObj, inMethod);
		ParentMenuItem->SubMenu.Add(Item);
		return ParentMenuItem->SubMenu.Last().ToSharedRef();
	}
}

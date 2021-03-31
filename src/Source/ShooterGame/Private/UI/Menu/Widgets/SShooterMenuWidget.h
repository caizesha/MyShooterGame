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

	FText GetMenuTitle() const;

	FLinearColor GetBottomColor() const;

	//�����������ͼƬ��Ϊ���Կؼ�
	void BuildAndShowMenu();

	void BuildLeftPanel(bool bGoingBack = false);

	void BuildRightPanel();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)override;

	virtual bool SupportsKeyboardFocus()const override { return true; }

	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	//ButtonIndex:��ǰ����Ĳ˵���
	FReply ButtonClicked(int32 ButtonIndex);

	void ConfirmMenuItem();


	//ʵ�ֲ˵�������ʾ����ò˵�ƫ����
	FMargin GetMenuOffset() const;

	MenuPtr CurrentMenu;//��ǰһ���˵����б�

	MenuPtr NextMenu;//��һ�˵����б�

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

//�ṩ������׼�˵���ĸ�������
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
}

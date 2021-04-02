// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"

/**
 * 
 */
class SShooterMenuItem : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnArrowPressed, int)

	SLATE_BEGIN_ARGS(SShooterMenuItem)
	{

	}
	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)//��������

	SLATE_EVENT (FOnClicked, OnClicked)

	SLATE_EVENT(FOnArrowPressed, OnArrowPressed)
	
	SLATE_ATTRIBUTE(FText, Text)

	SLATE_ATTRIBUTE(FText, OptionText)

	SLATE_ARGUMENT(bool, bIsMultichoice)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	SShooterMenuItem();
	~SShooterMenuItem();

	void UpdateItemText(const FText& UpdatedText);

	virtual bool SupportsKeyboardFocus()const override { return true; }

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	void SetMenuItemActive(bool IsMenuItemActive);

	//�󲿼�ͷ�Ƿ�ɼ�
	EVisibility LeftArrowVisible;

	//�Ҳ���ͷ�Ƿ�ɼ�
	EVisibility RightArrowVisible;
protected:
	FOnClicked OnClicked;

	FOnArrowPressed OnArrowPressed;

private:
	FSlateColor GetButtonTextColor() const;
	
	FSlateColor GetButtonBgColor() const;
	
	EVisibility GetLeftArrowVisibility() const;

	EVisibility GetRightArrowVisibility() const;

	FReply OnLeftArrowDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FReply OnRightArrowDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	//�˵����������
	FMargin GetOptionPadding() const;

	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	TAttribute<FText> Text;

	TAttribute<FText> OptionText;

	float ItemMargin;

	TSharedPtr<STextBlock> TextWidget;

	FLinearColor TextColor;

	const struct FShooterMenuItemStyle* MenuItemStyle;
	
	bool bIsActiveMenuItem;

	bool bIsMultichoice;	

};

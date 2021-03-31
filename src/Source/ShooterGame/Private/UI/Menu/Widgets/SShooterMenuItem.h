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
	SLATE_BEGIN_ARGS(SShooterMenuItem)
	{

	}
	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)//±äÁ¿ÉùÃ÷

	SLATE_ATTRIBUTE(FText, Text)

	SLATE_EVENT (FOnClicked, OnClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	SShooterMenuItem();
	~SShooterMenuItem();

	void UpdateItemText(const FText& UpdatedText);

	virtual bool SupportsKeyboardFocus()const override { return true; }

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	void SetMenuItemActive(bool IsMenuItemActive);


protected:
	FOnClicked OnClicked;

private:
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	TAttribute<FText> Text;

	float ItemMargin;

	TSharedPtr<STextBlock> TextWidget;

	FSlateColor GetButtonTextColor() const;

	FLinearColor TextColor;

	const struct FShooterMenuItemStyle* MenuItemStyle;
	
	bool bIsActiveMenuItem;

	FSlateColor GetButtonBgColor() const;

};

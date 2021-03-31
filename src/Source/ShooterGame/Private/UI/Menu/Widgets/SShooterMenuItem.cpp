// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "SShooterMenuItem.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "ShooterStyle.h"

SShooterMenuItem::SShooterMenuItem()
{
}

SShooterMenuItem::~SShooterMenuItem()
{
}

void SShooterMenuItem::Construct(const FArguments& InArgs)
{
	PlayerOwner = InArgs._PlayerOwner;
	Text = InArgs._Text;
	ItemMargin = 10.0f;
	TextColor = FLinearColor(155, 164, 182);
	OnClicked = InArgs._OnClicked;

	MenuItemStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuItemStyle>("DefaultShooterMenuItemStyle");

	//构建ui
	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(374.0f)
			.HeightOverride(23.0f)
			[
				SNew(SImage)
				.ColorAndOpacity(this, &SShooterMenuItem::GetButtonBgColor)
				.Image(&MenuItemStyle->BackgroundBrush)
			]
		]
	+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(ItemMargin, 0, 0, 0))
		[
			SAssignNew(TextWidget, STextBlock)
			.ColorAndOpacity(this, &SShooterMenuItem::GetButtonTextColor)
			.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuTextStyle")
			.Text(Text)
		]
	];
}

FSlateColor SShooterMenuItem::GetButtonTextColor() const
{
	FLinearColor Result;
	Result = TextColor;
	return Result;
}

void SShooterMenuItem::UpdateItemText(const FText& UpdatedText)
{
	Text = UpdatedText;
	if (TextWidget.IsValid())
	{
		TextWidget->SetText(Text);
	}

}

FReply SShooterMenuItem::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (OnClicked.IsBound() == true)
	{
		//执行事件：SShooterMenuWidget::ButtonClicked
		return OnClicked.Execute();
	}
	return FReply::Handled();
}

void SShooterMenuItem::SetMenuItemActive(bool IsMenuItemActive)
{
	bIsActiveMenuItem = IsMenuItemActive;
}

FSlateColor SShooterMenuItem::GetButtonBgColor() const
{
	const float BgAlpha = bIsActiveMenuItem ? 0.5 : 0.0;
	return FLinearColor(1.0, 1.0, 1.0, BgAlpha);

}
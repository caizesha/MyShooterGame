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
	OnClicked = InArgs._OnClicked;
	OnArrowPressed = InArgs._OnArrowPressed;
	Text = InArgs._Text;
	OptionText = InArgs._OptionText;
	bIsMultichoice = InArgs._bIsMultichoice;

	bIsActiveMenuItem = false;
	LeftArrowVisible = EVisibility::Collapsed;
	RightArrowVisible = EVisibility::Collapsed;

	ItemMargin = 10.0f;
	const float ArrowMargin = 3.0f;

	TextColor = FLinearColor(FColor(155, 164, 182));

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
		.HAlign(bIsMultichoice ? HAlign_Left : HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(ItemMargin, 0, 0, 0))
		[
			SAssignNew(TextWidget, STextBlock)
			.ColorAndOpacity(this, &SShooterMenuItem::GetButtonTextColor)
			.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuTextStyle")
			.Text(Text)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
				.Padding(FMargin(0, 0, ArrowMargin,0))
				.Visibility(this, &SShooterMenuItem::GetLeftArrowVisibility)
				.OnMouseButtonDown(this, &SShooterMenuItem::OnLeftArrowDown)
				[
					SNew(SOverlay)
					+SOverlay::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SImage)
						.Image(&MenuItemStyle->LeftArrowBrush)
					]
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(TAttribute<FMargin>(this, &SShooterMenuItem::GetOptionPadding))
			[
				SNew(STextBlock)
				.ColorAndOpacity(this, &SShooterMenuItem::GetButtonTextColor)
				.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuTextStyle")
				.Text(OptionText)
				.Visibility(bIsMultichoice ? EVisibility::Visible : EVisibility::Collapsed)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
				.Padding(FMargin(0, 0, ItemMargin, 0))
				.Visibility(this, &SShooterMenuItem::GetRightArrowVisibility)
				.OnMouseButtonDown(this, &SShooterMenuItem::OnRightArrowDown)
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SImage)
						.Image(&MenuItemStyle->RightArrowBrush)
					]
				]
			]

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
	const float BgAlpha = bIsActiveMenuItem ? 0.5f : 0.0f;
	return FLinearColor(1.0f, 1.0f, 1.0f, BgAlpha);

}

EVisibility SShooterMenuItem::GetLeftArrowVisibility() const
{
	return LeftArrowVisible;
}

EVisibility SShooterMenuItem::GetRightArrowVisibility() const
{
	return RightArrowVisible;
}


FReply SShooterMenuItem::OnLeftArrowDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Result = FReply::Unhandled();
	const int32 MoveLeft = -1;
	if (OnArrowPressed.IsBound() && bIsActiveMenuItem)
	{
		OnArrowPressed.Execute(MoveLeft);
		Result = FReply::Handled();
	}
	return Result;
}

FReply SShooterMenuItem::OnRightArrowDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Result = FReply::Unhandled();
	const int32 MoveRight = 1;
	if (OnArrowPressed.IsBound() && bIsActiveMenuItem)
	{
		OnArrowPressed.Execute(MoveRight);
		Result = FReply::Handled();
	}
	return Result;
}

FMargin SShooterMenuItem::GetOptionPadding() const
{
	return RightArrowVisible == EVisibility::Visible ? FMargin(0) : FMargin(0, 0, ItemMargin, 0);
}
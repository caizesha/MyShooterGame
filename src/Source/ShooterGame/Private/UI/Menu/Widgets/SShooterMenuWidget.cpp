// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterMenuWidgetStyle.h"
#include "SShooterMenuWidget.h"
#include "ShooterStyle.h"

SShooterMenuWidget::SShooterMenuWidget()
{
	FString Text = TEXT("主菜单");

	CurrentMenuTitle = FText::FromString(Text);
}

SShooterMenuWidget::~SShooterMenuWidget()
{
}

void SShooterMenuWidget::Construct(const FArguments& InArgs)
{
	PlayerOwner = InArgs._PlayerOwner;

	//从样式集合中获取Menu类型样式，其名为DefaultShooterMenuStyle
	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultShooterMenuStyle");

	FLinearColor MenuTitleTextColor = FLinearColor(155, 164, 182);
	MenuHeaderWidth = 287.0f;
	MenuHeaderHeight = 62.0f;
	OutlineWidth = 20.0f;

	SelectedIndex = 0;//默认第一个菜单项被激活

	//构建ui
	ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)//设置对齐属性
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)//包含菜单标题和左右背景容器
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				.Padding(TAttribute<FMargin>(this, &SShooterMenuWidget::GetMenuOffset))
				[
					SNew(SVerticalBox)//包含菜单标题
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SOverlay)
						+ SOverlay::Slot()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Fill)
						[
							SNew(SBox)
							.WidthOverride(MenuHeaderWidth)
							.HeightOverride(MenuHeaderHeight)
							[
								SNew(SImage)
								.Image(&MenuStyle->HeaderBackgroundBrush)
							]
						]
						+ SOverlay::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Fill)
							[
								SNew(SBox)
								.WidthOverride(MenuHeaderWidth)
								.HeightOverride(MenuHeaderHeight)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
									.ColorAndOpacity(MenuTitleTextColor)
									.Text(this, &SShooterMenuWidget::GetMenuTitle)
								]
							]
					]
					+ SVerticalBox::Slot()//包含左右背景容器
					.AutoHeight()
					[
						SNew(SBorder)
						.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
						.ColorAndOpacity(this, &SShooterMenuWidget::GetBottomColor)
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Top)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()//左边一级菜单
							.AutoWidth()
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(SBorder)
									.BorderImage(&MenuStyle->LeftBackgroundBrush)
									.BorderBackgroundColor(FLinearColor(1.0, 1.0, 1.0, 1.0))
									.Padding(FMargin(OutlineWidth))//设置前后左右对齐距离
									.HAlign(HAlign_Left)
									.VAlign(VAlign_Top)
									[
									SAssignNew(LeftBox, SVerticalBox)//运行时动态添加子控件（菜单项）
									//cachedesigned 左右容器填充图片作为测试控件
									]
								]
							]
							+ SHorizontalBox::Slot()//右边二级菜单
							.AutoWidth()
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(SBorder)
									.BorderImage(&MenuStyle->RightBackgroundBrush)
									.BorderBackgroundColor(FLinearColor(1.0, 1.0, 1.0, 1.0))
									.Padding(FMargin(OutlineWidth))//设置前后左右对齐距离
									.DesiredSizeScale(this, &SShooterMenuWidget::GetBottomScale)
									.HAlign(HAlign_Left)
									.VAlign(VAlign_Top)
									[
										SAssignNew(RightBox, SVerticalBox)//运行时动态添加子控件（菜单项）
									]
								]
							]
						]
					]
				]
			]
		];
}


FText SShooterMenuWidget::GetMenuTitle() const
{
	return CurrentMenuTitle;
}

FLinearColor SShooterMenuWidget::GetBottomColor() const
{
	return FLinearColor(1, 1, 1, 1);
}

void SShooterMenuWidget::BuildAndShowMenu()
{
	ScreenResolution = GEngine->GetGameUserSettings()->GetScreenResolution();

	BuildLeftPanel();

	if (CurrentMenu.Num() > 0 && CurrentMenu.IsValidIndex(SelectedIndex) && CurrentMenu[SelectedIndex]->bVisible)
	{
		NextMenu = CurrentMenu[SelectedIndex]->SubMenu;
		if (NextMenu.Num() > 0)
		{
			BuildRightPanel();
		}
	}

}

void SShooterMenuWidget::BuildLeftPanel(bool bGoingBack)
{
	if (CurrentMenu.Num() == 0)
	{
		return;
	}

	LeftBox->ClearChildren();

	for (int32 i = 0; i < CurrentMenu.Num(); i++)
	{
		if (CurrentMenu[i]->bVisible)
		{
			TSharedPtr<SWidget> TmpWidget;
			if (CurrentMenu[i]->MenuItemType == EShooterMenuItemType::Standard)
			{
				TmpWidget = SAssignNew(CurrentMenu[i]->Widget, SShooterMenuItem)
					.PlayerOwner(PlayerOwner)
					.OnClicked(this, &SShooterMenuWidget::ButtonClicked, i)
					.Text(CurrentMenu[i]->GetText())
					.bIsMultichoice(false);
			}
			else if (CurrentMenu[i]->MenuItemType == EShooterMenuItemType::MultiChoice)
			{
				TmpWidget = SAssignNew(CurrentMenu[i]->Widget, SShooterMenuItem)
					.PlayerOwner(PlayerOwner)
					.OnClicked(this, &SShooterMenuWidget::ButtonClicked, i)
					.Text(CurrentMenu[i]->GetText())
					.bIsMultichoice(true)
					.OptionText(this, &SShooterMenuWidget::GetOptionText, CurrentMenu[i])
					.OnArrowPressed(this, &SShooterMenuWidget::ChangeOption);

				//更新箭头图标
				UpdateArrow(CurrentMenu[i]);
			}

			if (TmpWidget.IsValid())
			{
				if (SelectedIndex == -1)
				{
					SelectedIndex = i;
				}
				LeftBox->AddSlot()
					.HAlign(HAlign_Left)
					.AutoHeight()
					[
						TmpWidget.ToSharedRef()
					];
			}
		}
	}
	//激活第一个菜单项
	TSharedPtr<FShooterMenuItem> FirstMenuItem = CurrentMenu.IsValidIndex(SelectedIndex) ? CurrentMenu[SelectedIndex] : nullptr;
	if (FirstMenuItem.IsValid())
	{
		FirstMenuItem->Widget->SetMenuItemActive(true);
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
	}
}

void SShooterMenuWidget::BuildRightPanel()
{
	if (NextMenu.Num() == 0)
	{
		return;
	}

	RightBox->ClearChildren();

	for (int32 i = 0; i < NextMenu.Num(); i++)
	{
		if (NextMenu[i]->bVisible)
		{
			TSharedPtr<SWidget> TmpWidget;
			if (NextMenu[i]->MenuItemType == EShooterMenuItemType::Standard)
			{
				TmpWidget = SAssignNew(NextMenu[i]->Widget, SShooterMenuItem)
					.PlayerOwner(PlayerOwner)
					.Text(NextMenu[i]->GetText())
					.bIsMultichoice(false);
			}
			else if (NextMenu[i]->MenuItemType == EShooterMenuItemType::MultiChoice)
			{
				TmpWidget = SAssignNew(NextMenu[i]->Widget, SShooterMenuItem)
					.PlayerOwner(PlayerOwner)
					.Text(NextMenu[i]->GetText())
					.bIsMultichoice(true)
					.OptionText(this, &SShooterMenuWidget::GetOptionText, NextMenu[i]);
			}
			if (TmpWidget.IsValid())
			{
				RightBox->AddSlot()
					.HAlign(HAlign_Center)
					.AutoHeight()
					[
						TmpWidget.ToSharedRef()
					];
			}
		}
	}
}

FMargin SShooterMenuWidget::GetMenuOffset() const
{
	const float WidgetWidth = LeftBox->GetDesiredSize().X + RightBox->GetDesiredSize().X;
	const float WidgetHeight = LeftBox->GetDesiredSize().Y + MenuHeaderHeight;

	const float OffsetX = (ScreenResolution.X - WidgetWidth - OutlineWidth * 2) / 2;
	const float OffsetY = (ScreenResolution.Y - WidgetHeight) / 2;

	FMargin Result(OffsetX, OffsetY, 0, 0);
	return Result;
}

//界面一出现就将键盘焦点设置给该控件
bool bFadedIn = false;//界面又出现
void SShooterMenuWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	if (!bFadedIn)
	{
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
		bFadedIn = true;
	}
}

//当键盘焦点设置到此控件时，系统将发送事件给此控件
FReply SShooterMenuWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	//声明式语法
	return FReply::Handled().ReleaseMouseCapture().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly, true);
}

FReply SShooterMenuWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

FReply SShooterMenuWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Result = FReply::Unhandled();

	//获取当前用户索引
	const int32 UserIndex = InKeyEvent.GetUserIndex();
	const int32 OwnerIndex = GetOwnerUserIndex();

	bool bEventUserCanInteract = UserIndex == OwnerIndex;
	if (bEventUserCanInteract)
	{
		//获取按下的按键
		const FKey Key = InKeyEvent.GetKey();

		if (Key == EKeys::Up)
		{
			int32 NextValidIndex = GetNextValidIndex(-1);
			//进行边界控制
			if (NextValidIndex != SelectedIndex)
			{
				//复用buttonclick逻辑
				ButtonClicked(NextValidIndex);
			}
			Result = FReply::Handled();
		}
		else if (Key == EKeys::Down)
		{
			int32 NextValidIndex = GetNextValidIndex(1);
			if (NextValidIndex != SelectedIndex)
			{
				ButtonClicked(NextValidIndex);
			}
			Result = FReply::Handled();

		}
		else if (Key == EKeys::Enter)
		{
			ConfirmMenuItem();
			Result = FReply::Handled();
		}
	}
	return Result;
}

FReply SShooterMenuWidget::ButtonClicked(int32 ButtonIndex)
{
	//第一次被点击
	if (SelectedIndex != ButtonIndex)
	{
		TSharedPtr<SShooterMenuItem> MenuItem = CurrentMenu[SelectedIndex]->Widget;//之前菜单项
		//Deactive之前菜单项
		MenuItem->SetMenuItemActive(false);

		SelectedIndex = ButtonIndex;
		MenuItem = CurrentMenu[SelectedIndex]->Widget;
		//Active当前菜单项
		MenuItem->SetMenuItemActive(true);

		//更新下一级菜单
		NextMenu = CurrentMenu[SelectedIndex]->SubMenu;

	}
	//第二次被点击
	else if (SelectedIndex == ButtonIndex)
	{
		ConfirmMenuItem();
	}
	return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

void SShooterMenuWidget::ConfirmMenuItem()
{
	if (CurrentMenu[SelectedIndex]->OnConfirmMenuItem.IsBound())
	{
		CurrentMenu[SelectedIndex]->OnConfirmMenuItem.Execute();
	}
	else if (CurrentMenu[SelectedIndex]->SubMenu.Num() > 0)
	{
		//todo
		//进入二级菜单
	}

}

//根据index获取选项对应文本
FText SShooterMenuWidget::GetOptionText(TSharedPtr<FShooterMenuItem> MenuItem) const
{
	FText Result = FText::GetEmpty();

	if (MenuItem->SelectedMultiChoice > -1 && MenuItem->SelectedMultiChoice < MenuItem->MultiChoice.Num())
	{
		Result = MenuItem->MultiChoice[MenuItem->SelectedMultiChoice];
	}
	return Result;
}

//更改选项
void SShooterMenuWidget::ChangeOption(int32 MoveBy)
{
	TSharedPtr<FShooterMenuItem> MenuItem = CurrentMenu[SelectedIndex];
	const int32 MinIndex = MenuItem->MinMultiChoiceIndex > -1 ? MenuItem->MinMultiChoiceIndex : 0;
	const int32 MaxIndex = MenuItem->MaxMultiChoiceIndex > -1 ? MenuItem->MaxMultiChoiceIndex : MenuItem->MultiChoice.Num() - 1;

	const int32 CurrentIndex = MenuItem->SelectedMultiChoice;

	if (MenuItem->MenuItemType == EShooterMenuItemType::MultiChoice)
	{
		if (CurrentIndex + MoveBy >= MinIndex && CurrentIndex + MoveBy <= MaxIndex)
		{
			MenuItem->SelectedMultiChoice += MoveBy;
			//定义OnOptionChanged代理，处理选项关联的实际逻辑
			MenuItem->OnOptionChanged.ExecuteIfBound(MenuItem, MenuItem->SelectedMultiChoice);
		}
		UpdateArrow(MenuItem);
	}
}


//根据当前index判断左右箭头是否显示
void SShooterMenuWidget::UpdateArrow(TSharedPtr<FShooterMenuItem> MenuItem)
{
	const int32 MinIndex = MenuItem->MinMultiChoiceIndex > -1 ? MenuItem->MinMultiChoiceIndex : 0;
	const int32 MaxIndex = MenuItem->MaxMultiChoiceIndex > -1 ? MenuItem->MaxMultiChoiceIndex : MenuItem->MultiChoice.Num() - 1;

	const int32 CurrentIndex = MenuItem->SelectedMultiChoice;

	if (CurrentIndex > MinIndex)
	{
		MenuItem->Widget->LeftArrowVisible = EVisibility::Visible;
	}
	else
	{
		MenuItem->Widget->LeftArrowVisible = EVisibility::Collapsed;
	}

	if (CurrentIndex < MaxIndex)
	{
		MenuItem->Widget->RightArrowVisible = EVisibility::Visible;
	}
	else
	{
		MenuItem->Widget->RightArrowVisible = EVisibility::Collapsed;
	}

}

int32 SShooterMenuWidget::GetNextValidIndex(int32 MoveBy)
{
	int32 Result = SelectedIndex;
	//对上下边界的处理
	if (MoveBy != 0 && SelectedIndex + MoveBy > -1 && SelectedIndex + MoveBy < CurrentMenu.Num())
	{
		Result += MoveBy;
		while (!CurrentMenu[Result]->Widget.IsValid())
		{
			MoveBy > 0 ? Result++ : Result--;
			if (!CurrentMenu.IsValidIndex(Result))
			{
				Result = SelectedIndex;
				break;
			}
		}
	}
	return Result;
}

int32 SShooterMenuWidget::GetOwnerUserIndex()
{
	return PlayerOwner.IsValid() ? PlayerOwner->GetControllerId() : 0;
}

FVector2D SShooterMenuWidget::GetBottomScale() const
{
	if (NextMenu.Num() > 0)
	{
		return FVector2D(1.0f, 1.0f);
	}
	else
	{
		return FVector2D(0.0f, 0.0f);
	}
}
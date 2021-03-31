// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterMenuWidgetStyle.h"
#include "SShooterMenuWidget.h"
#include "ShooterStyle.h"

SShooterMenuWidget::SShooterMenuWidget()
{
	FString Text = TEXT("���˵�");

	CurrentMenuTitle = FText::FromString(Text);
}

SShooterMenuWidget::~SShooterMenuWidget()
{
}

void SShooterMenuWidget::Construct(const FArguments& InArgs)
{
	PlayerOwner = InArgs._PlayerOwner;

	//����ʽ�����л�ȡMenu������ʽ������ΪDefaultShooterMenuStyle
	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultShooterMenuStyle");

	FLinearColor MenuTitleTextColor = FLinearColor(155, 164, 182);
	MenuHeaderWidth = 287.0f;
	MenuHeaderHeight = 62.0f;
	OutlineWidth = 20.0f;

	SelectedIndex = 0;//Ĭ�ϵ�һ���˵������

	//����ui
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)//���ö�������
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)//�����˵���������ұ�������
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(TAttribute<FMargin>(this, &SShooterMenuWidget::GetMenuOffset))
			[
				SNew(SVerticalBox)//�����˵�����
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
				+ SVerticalBox::Slot()//�������ұ�������
				.AutoHeight()
				[
					SNew(SBorder)
					.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
					.ColorAndOpacity(this, &SShooterMenuWidget::GetBottomColor)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()//���һ���˵�
						.AutoWidth()
						[
							SNew(SVerticalBox)
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SBorder)
								.BorderImage(&MenuStyle->LeftBackgroundBrush)
								.BorderBackgroundColor(FLinearColor(1.0, 1.0, 1.0, 1.0))
								.Padding(FMargin(OutlineWidth))//����ǰ�����Ҷ������
								.HAlign(HAlign_Left)
								.VAlign(VAlign_Top)
								[
									SAssignNew(LeftBox, SVerticalBox)//����ʱ��̬����ӿؼ����˵��
									//cachedesigned �����������ͼƬ��Ϊ���Կؼ�
								]
							]
						]
						+ SHorizontalBox::Slot()//�ұ߶����˵�
						.AutoWidth()
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SBorder)
								.BorderImage(&MenuStyle->RightBackgroundBrush)
								.BorderBackgroundColor(FLinearColor(1.0, 1.0, 1.0, 1.0))
								.Padding(FMargin(OutlineWidth))//����ǰ�����Ҷ������
								.HAlign(HAlign_Left)
								.VAlign(VAlign_Top)
								[
									SAssignNew(RightBox, SVerticalBox)//����ʱ��̬����ӿؼ����˵��
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

	BuildRightPanel();
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
			TmpWidget = SAssignNew(CurrentMenu[i]->Widget, SShooterMenuItem)
				.PlayerOwner(PlayerOwner)
				.OnClicked(this, &SShooterMenuWidget::ButtonClicked, i)
				.Text(CurrentMenu[i]->GetText());
			if (TmpWidget.IsValid())
			{
				LeftBox->AddSlot()
				.HAlign(HAlign_Left)
				.AutoHeight()
				[
					TmpWidget.ToSharedRef()
				];
			}
		}
		//�����һ���˵���
		TSharedPtr<FShooterMenuItem> FirstMenuItem = CurrentMenu.IsValidIndex(SelectedIndex) ? CurrentMenu[SelectedIndex] : nullptr;
		if (FirstMenuItem.IsValid())
		{
			FirstMenuItem->Widget->SetMenuItemActive(true);
			FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
		}
	}
	//to test ���͸��ͼƬ����˵���
	/*LeftBox->AddSlot()
	.HAlign(HAlign_Left)
	.AutoHeight()
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
			]
		]
	];*/
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
			TmpWidget = SAssignNew(NextMenu[i]->Widget, SShooterMenuItem)
				.PlayerOwner(PlayerOwner)
				.Text(NextMenu[i]->GetText());
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
	//to test ���͸��ͼƬ����˵���
	/*RightBox->AddSlot()
		.HAlign(HAlign_Left)
		.AutoHeight()
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
				]
			]
		];*/
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

//����һ���־ͽ����̽������ø��ÿؼ�
bool bFadedIn = false;//�����ֳ���
void SShooterMenuWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	if (!bFadedIn)
	{
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
		bFadedIn = true;
	}
}

//�����̽������õ��˿ؼ�ʱ��ϵͳ�������¼����˿ؼ�
FReply SShooterMenuWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	//����ʽ�﷨
	return FReply::Handled().ReleaseMouseCapture().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly, true);
}

FReply SShooterMenuWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

FReply SShooterMenuWidget::ButtonClicked(int32 ButtonIndex)
{
	//��һ�α����
	if (SelectedIndex != ButtonIndex)
	{
		TSharedPtr<SShooterMenuItem> MenuItem = CurrentMenu[SelectedIndex]->Widget;//֮ǰ�˵���
		//Deactive֮ǰ�˵���
		MenuItem->SetMenuItemActive(false);

		SelectedIndex = ButtonIndex;
		MenuItem = CurrentMenu[SelectedIndex]->Widget;
		//Active��ǰ�˵���
		MenuItem->SetMenuItemActive(true);

		//������һ���˵�
		NextMenu = CurrentMenu[SelectedIndex]->SubMenu;

	}
	//�ڶ��α����
	else
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
		//��������˵�
	}
	
}
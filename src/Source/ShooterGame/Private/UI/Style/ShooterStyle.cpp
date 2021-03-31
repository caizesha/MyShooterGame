// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "SlateGameResources.h"

TSharedPtr<class FSlateStyleSet> FShooterStyle::ShooterStyleSetInstance = nullptr;

FShooterStyle::FShooterStyle()
{
}

FShooterStyle::~FShooterStyle()
{
}

void FShooterStyle::Initialize()
{
	if (!ShooterStyleSetInstance.IsValid())
	{
		ShooterStyleSetInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*ShooterStyleSetInstance);
	}

}

void FShooterStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*ShooterStyleSetInstance);
	ensure(ShooterStyleSetInstance.IsUnique());//ȷ��ָ����ж���Ψһ
	ShooterStyleSetInstance.Reset();
}

const ISlateStyle& FShooterStyle::Get()
{
	//��ȡָ�����
	return *ShooterStyleSetInstance;
}

FName FShooterStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ShooterStyle"));
	return StyleSetName;
}

//Ϊ�����乹�캯���Ŀɱ��������__VA_ARGS__��ʵ��
#define TTF_FONT(RelativePath, ...) FSlateFontInfo(FPaths::GameContentDir()/"Slate"/RelativePath + TEXT(".ttf"), __VA_ARGS__)

TSharedRef<FSlateStyleSet> FShooterStyle::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(GetStyleSetName(), "/Game/UI/Styles", "/Game/UI/Styles");

	//�õ���������Ķ���
	FSlateStyleSet& Style = StyleRef.Get();

	//ע��˵�����������ʽ�Ͳ˵���������ʽ
	//FTextBlockStyle�������Զ����FShooterMenuStyle
	Style.Set<FTextBlockStyle>(TEXT("ShooterGame.MenuTextStyle"), FTextBlockStyle()
		.SetFont(TTF_FONT("Fonts/Roboto-Black", 20))//����ʽ�﷨�ĵײ�֧�ţ�����FTextBlockStyle
		.SetColorAndOpacity(FLinearColor::White)////����FTextBlockStyle
		.SetShadowOffset(FIntPoint(-1, 1)));

	Style.Set<FTextBlockStyle>(TEXT("ShooterGame.MenuHeaderTextStyle"), FTextBlockStyle()
		.SetFont(TTF_FONT("Fonts/Roboto-Black", 26))//����ʽ�﷨�ĵײ�֧�ţ�����FTextBlockStyle
		.SetColorAndOpacity(FLinearColor::White)////����FTextBlockStyle
		.SetShadowOffset(FIntPoint(-1, 1)));
	return StyleRef;
}

#undef TTF_FONT
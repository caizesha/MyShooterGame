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
	ensure(ShooterStyleSetInstance.IsUnique());//确保指针持有对象唯一
	ShooterStyleSetInstance.Reset();
}

const ISlateStyle& FShooterStyle::Get()
{
	//获取指向对象
	return *ShooterStyleSetInstance;
}

FName FShooterStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ShooterStyle"));
	return StyleSetName;
}

//为了适配构造函数的可变参数采用__VA_ARGS__宏实现
#define TTF_FONT(RelativePath, ...) FSlateFontInfo(FPaths::GameContentDir()/"Slate"/RelativePath + TEXT(".ttf"), __VA_ARGS__)

TSharedRef<FSlateStyleSet> FShooterStyle::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(GetStyleSetName(), "/Game/UI/Styles", "/Game/UI/Styles");

	//拿到具体包含的对象
	FSlateStyleSet& Style = StyleRef.Get();

	//注册菜单标题文字样式和菜单项文字样式
	//FTextBlockStyle类似于自定义的FShooterMenuStyle
	Style.Set<FTextBlockStyle>(TEXT("ShooterGame.MenuTextStyle"), FTextBlockStyle()
		.SetFont(TTF_FONT("Fonts/Roboto-Black", 20))//声明式语法的底层支撑：返回FTextBlockStyle
		.SetColorAndOpacity(FLinearColor::White)////返回FTextBlockStyle
		.SetShadowOffset(FIntPoint(-1, 1)));

	Style.Set<FTextBlockStyle>(TEXT("ShooterGame.MenuHeaderTextStyle"), FTextBlockStyle()
		.SetFont(TTF_FONT("Fonts/Roboto-Black", 26))//声明式语法的底层支撑：返回FTextBlockStyle
		.SetColorAndOpacity(FLinearColor::White)////返回FTextBlockStyle
		.SetShadowOffset(FIntPoint(-1, 1)));
	return StyleRef;
}

#undef TTF_FONT
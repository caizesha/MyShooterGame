// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"
/**
 * 
 */

//FShooterStyle：管理所有在编辑器中拓展的Style（样式）集合，包括一些集合操作（获取、注册、反注册到FSlateStyleRegistry）
class FShooterStyle
{
public:
	FShooterStyle();
	~FShooterStyle();

	//静态：调用时不用再创建对象
	static void Initialize();

	static void Shutdown();

	static const ISlateStyle& Get();

	static FName GetStyleSetName();
private:
	static TSharedRef<FSlateStyleSet> Create();

private:
	static TSharedPtr<class FSlateStyleSet> ShooterStyleSetInstance;
};

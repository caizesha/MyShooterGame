// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"
/**
 * 
 */

//FShooterStyle�����������ڱ༭������չ��Style����ʽ�����ϣ�����һЩ���ϲ�������ȡ��ע�ᡢ��ע�ᵽFSlateStyleRegistry��
class FShooterStyle
{
public:
	FShooterStyle();
	~FShooterStyle();

	//��̬������ʱ�����ٴ�������
	static void Initialize();

	static void Shutdown();

	static const ISlateStyle& Get();

	static FName GetStyleSetName();
private:
	static TSharedRef<FSlateStyleSet> Create();

private:
	static TSharedPtr<class FSlateStyleSet> ShooterStyleSetInstance;
};

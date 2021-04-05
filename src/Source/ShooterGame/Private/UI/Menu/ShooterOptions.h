// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ShooterMenuItem.h"
#include "SShooterMenuWidget.h"
/**
 *
 */
 //处理一些外部逻辑
DECLARE_DELEGATE(FOnApplyChanges)

class FShooterOptions : public TSharedFromThis<FShooterOptions>
{
public:
	FShooterOptions();
	~FShooterOptions();

	void Construct(ULocalPlayer* InPlayerOwner);

	void OnApplySettings();

	TSharedPtr<FShooterMenuItem> OptionsItem;

	FOnApplyChanges OnApplyChanges;

protected:
	ULocalPlayer* PlayerOwner;

	EWindowMode::Type bIsFullScreen;

	void FullScreenOptionChanged(TSharedPtr<FShooterMenuItem> MenuItem, int32 OptionIndex);
};

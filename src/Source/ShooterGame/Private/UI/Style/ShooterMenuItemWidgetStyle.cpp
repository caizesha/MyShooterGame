// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterMenuItemWidgetStyle.h"

FShooterMenuItemStyle::FShooterMenuItemStyle()
{

}

FShooterMenuItemStyle::~FShooterMenuItemStyle()
{

}


const FName FShooterMenuItemStyle::TypeName(TEXT("FShooterMenuItemStyle"));


void FShooterMenuItemStyle::GetResources(TArray<const FSlateBrush *> & OutBrushes) const
{
	OutBrushes.Add(&BackgroundBrush);
}

const FShooterMenuItemStyle& FShooterMenuItemStyle::GetDefault()
{
	static FShooterMenuItemStyle Default;
	return Default;
}

UShooterMenuItemWidgetStyle::UShooterMenuItemWidgetStyle(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}


UShooterMenuItemWidgetStyle::~UShooterMenuItemWidgetStyle()
{
}

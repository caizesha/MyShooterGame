// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterMenuWidgetStyle.h"

FShooterMenuStyle::FShooterMenuStyle()
{

}

FShooterMenuStyle::~FShooterMenuStyle()
{

}

const FName FShooterMenuStyle::TypeName(TEXT("FShooterMenuStyle"));


void FShooterMenuStyle::GetResources(TArray<const FSlateBrush *> & OutBrushes) const
{
	OutBrushes.Add(&HeaderBackgroundBrush);
}

const FShooterMenuStyle& FShooterMenuStyle::GetDefault()
{
	static FShooterMenuStyle Default;
	return Default;
}


UShooterMenuWidgetStyle::UShooterMenuWidgetStyle(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UShooterMenuWidgetStyle::~UShooterMenuWidgetStyle()
{
}



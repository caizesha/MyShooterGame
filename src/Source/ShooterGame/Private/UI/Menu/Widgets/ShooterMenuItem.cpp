// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterMenuItem.h"
#include "SShooterMenuItem.h"

FShooterMenuItem::~FShooterMenuItem()
{
}

const FText& FShooterMenuItem::GetText() const
{
	return Text;
}
void FShooterMenuItem::SetText(FText UpdatedText)
{
	Text = MoveTemp(UpdatedText);

	if (Widget.IsValid())
	{
		Widget->UpdateItemText(Text);
	}
}

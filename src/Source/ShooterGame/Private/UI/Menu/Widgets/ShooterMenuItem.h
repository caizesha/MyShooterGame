// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SShooterMenuItem.h"

/**
 * 
 */

typedef TArray<TSharedPtr<class FShooterMenuItem>> MenuPtr;

//加入TSharedFromThis内存管理
class FShooterMenuItem : public TSharedFromThis<FShooterMenuItem>
{
public:

	DECLARE_DELEGATE(FOnConfirmMenuItem)

	FOnConfirmMenuItem OnConfirmMenuItem;

	FShooterMenuItem(FText _Text)
	{
		bVisible = true;
		//仅做简单引用，防止内存进行一些不必要的拷贝
		Text = MoveTemp(_Text);
	}

	~FShooterMenuItem();


	const FText& GetText() const;

	void SetText(FText UpdatedText);

	//创建根（虚拟）菜单项
	static TSharedRef<FShooterMenuItem> CreateRoot()
	{
		return MakeShareable(new FShooterMenuItem());
	}

	TSharedPtr<SShooterMenuItem> Widget;

	MenuPtr SubMenu;

	bool bVisible;

private:

	FShooterMenuItem()
	{
		bVisible = false;
	}
	FText Text;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SShooterMenuItem.h"

/**
 * 
 */

typedef TArray<TSharedPtr<class FShooterMenuItem>> MenuPtr;

//����TSharedFromThis�ڴ����
class FShooterMenuItem : public TSharedFromThis<FShooterMenuItem>
{
public:

	DECLARE_DELEGATE(FOnConfirmMenuItem)

	FOnConfirmMenuItem OnConfirmMenuItem;

	FShooterMenuItem(FText _Text)
	{
		bVisible = true;
		//���������ã���ֹ�ڴ����һЩ����Ҫ�Ŀ���
		Text = MoveTemp(_Text);
	}

	~FShooterMenuItem();


	const FText& GetText() const;

	void SetText(FText UpdatedText);

	//�����������⣩�˵���
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

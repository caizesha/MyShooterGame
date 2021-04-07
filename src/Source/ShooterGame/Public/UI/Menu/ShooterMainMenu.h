// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterOptions.h"
/**
 *
 */

class UShooterGameInstance;

class SShooterMenuWidget;

class FShooterMenuItem;


//����˵����м��ࣺ����˵��Ĵ�������ʾ������
class SHOOTERGAME_API FShooterMainMenu : public TSharedFromThis<FShooterMainMenu>, public FTickableGameObject
{

public:
	FShooterMainMenu();

	virtual ~FShooterMainMenu();

	virtual void Tick(float DeltaSeconds) override;

	//�ж϶����Ƿ���Զ�ʱ����
	virtual bool IsTickable() const override;

	//ϵͳ����ÿ��tick���󶼻����һ��tickid
	virtual TStatId GetStatId() const override;

	void Construct(TWeakObjectPtr<UShooterGameInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner);

	void AddMenuToViewport();

	void RemoveMenuToViewport();

	ULocalPlayer* GetPlayerOwner() const;

	void OnUIQuit();

	void CloseSubMenu();

	void OnMenuHidden();

protected:

	void OnUIHostFreeForAll();

protected:
	//��ʶ���
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	TWeakObjectPtr<UShooterGameInstance> GameInstance;

	TSharedPtr<SShooterMenuWidget> MenuWidget;

	//��MenuWidget�ŵ������в��ܼ���UIϵͳ
	TSharedPtr<class SWeakWidget> MenuWidgetContainer;

	TSharedPtr<FShooterOptions> ShooterOptions;
};

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

class AShooterGameSession;

struct FServerEntry
{
	FString ServerName;
	FString CurrentPlayers;
	FString MaxPlayers;
	FString GameType;
	FString MapName;
	FString Ping;
	int32 SearchResultsIndex;
};

//管理菜单的中间类：负责菜单的创建、显示、隐藏
class SHOOTERGAME_API FShooterMainMenu : public TSharedFromThis<FShooterMainMenu>, public FTickableGameObject
{

public:
	FShooterMainMenu();

	virtual ~FShooterMainMenu();

	virtual void Tick(float DeltaSeconds) override;

	//判断对象是否可以定时调用
	virtual bool IsTickable() const override;

	//系统管理每个tick对象都会分配一个tickid
	virtual TStatId GetStatId() const override;

	void Construct(TWeakObjectPtr<UShooterGameInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner);

	void AddMenuToViewport();

	void RemoveMenuToViewport();

	ULocalPlayer* GetPlayerOwner() const;

	void OnUIQuit();

	void CloseSubMenu();

	void OnMenuHidden();

	AShooterGameSession* GetGameSession() const;

	//轮询
	void UpdateSearchStatus();

protected:

	void OnUIHostFreeForAll();

	void OnSearchServer();

	void OnServerSearchFinished();

	void OnJoinGame();

	void ConnentToServer();
protected:
	//标识玩家
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	TWeakObjectPtr<UShooterGameInstance> GameInstance;

	TSharedPtr<SShooterMenuWidget> MenuWidget;

	//将MenuWidget放到容器中才能加入UI系统
	TSharedPtr<class SWeakWidget> MenuWidgetContainer;

	TSharedPtr<FShooterOptions> ShooterOptions;

	//可用服务器列表
	TArray<TSharedPtr<FServerEntry>> ServerList;

	bool bSearchForServers;

	TSharedPtr<FServerEntry> SelectedServer;
};

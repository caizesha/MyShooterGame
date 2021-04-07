// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

/**
 * 
 */

//负责管理菜单的中间类
class FShooterMainMenu;
class AShooterGameSession;
UCLASS()
class SHOOTERGAME_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	virtual void Shutdown()override;

	virtual void StartGameInstance()override;

	//定时调用的函数
	bool Tick(float DeltaSeconds);

	//url地址可以附带很多参数
	bool HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL);

	AShooterGameSession* GetGameSession() const;
private:
	void BeginMainMenuState();

	void OnCreatePresenceSessionComplete(FName InSessionName, bool bWasSuccessfull);

	void OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);
	
	void FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result);

	TSharedPtr<FShooterMainMenu> MainMenuUI;

	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;

	FString TravelURL;

	FDelegateHandle OnCreatePresenceSessionCompleteDelegateHandle;

	UPROPERTY(config)
	FString MainMenuMap;
};

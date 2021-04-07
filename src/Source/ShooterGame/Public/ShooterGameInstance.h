// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

/**
 * 
 */

//�������˵����м���
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

	//��ʱ���õĺ���
	bool Tick(float DeltaSeconds);

	//url��ַ���Ը����ܶ����
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

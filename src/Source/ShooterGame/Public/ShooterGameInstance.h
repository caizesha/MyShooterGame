// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

/**
 * 
 */
//������Ϸ״̬
namespace ShooterGameInstanceState
{
	const FName None = FName(TEXT("None"));
	const FName MainMenu = FName(TEXT("MainMenu"));
	const FName Playing = FName(TEXT("Playing"));

}

//�������˵����м���
class FShooterMainMenu;
class AShooterGameSession;
UCLASS()
class SHOOTERGAME_API UShooterGameInstance : public UGameInstance
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void Init() override;

	virtual void Shutdown()override;

	virtual void StartGameInstance()override;

	//��ʱ���õĺ���
	bool Tick(float DeltaSeconds);

	//url��ַ���Ը����ܶ����
	bool HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL);

	AShooterGameSession* GetGameSession() const; 

	void GotoState(FName NewState);
	void GotoInitialState();
	FName GetInitialState();

private:
	void BeginMainMenuState();
	void EndMainMenuState();

	void OnCreatePresenceSessionComplete(FName InSessionName, bool bWasSuccessfull);

	void OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);
	
	void FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result);

	//��⵱ǰ״̬�Ƿ����仯
	void MayChangeState();

	void EndCurrentState(FName NextState);
	void BeginNewState(FName NewState, FName PreState);

	void BeginPlayingState();
	void EndPlayingState();

	TSharedPtr<FShooterMainMenu> MainMenuUI;

	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;

	FString TravelURL;

	FDelegateHandle OnCreatePresenceSessionCompleteDelegateHandle;

	UPROPERTY(config)
	FString MainMenuMap;

	FName CurrentState;
	FName PendingState;
};

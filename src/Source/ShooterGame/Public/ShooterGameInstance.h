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

	bool FindSessions(ULocalPlayer* LocalPlayer, bool bIsLanMatch);

	bool JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults);

	AShooterGameSession* GetGameSession() const; 

	void GotoState(FName NewState);
	void GotoInitialState();
	FName GetInitialState();

private:
	void BeginMainMenuState();
	void EndMainMenuState();

	void OnCreatePresenceSessionComplete(FName InSessionName, bool bWasSuccessfull);

	//��Է�����
	void OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);
	
	void FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result);

	void OnSearchSessionsComplete(bool bWasSuccessfull);

	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);

	//��Կͻ���
	void OnRegisterJoiningLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);

	void FinishJoinSession(EOnJoinSessionCompleteResult::Type Result);

	//��⵱ǰ״̬�Ƿ����仯
	void MayChangeState();

	void EndCurrentState(FName NextState);
	void BeginNewState(FName NewState, FName PreState);

	void BeginPlayingState();
	void EndPlayingState();

	//����ǰ�˵�ͼ
	bool LoadFrontEndMap(const FString& MapName);

	void InternalTravelToSession(const FName& InSessionName);

	void CleanupSession();

	void OnEndSessionComplete(FName InSessionName, bool bWasSuccessfull);

	TSharedPtr<FShooterMainMenu> MainMenuUI;

	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;
	FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;

	FString TravelURL;

	FDelegateHandle OnCreatePresenceSessionCompleteDelegateHandle;
	FDelegateHandle OnSearchSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	UPROPERTY(config)
	FString MainMenuMap;

	FName CurrentState;
	FName PendingState;
};

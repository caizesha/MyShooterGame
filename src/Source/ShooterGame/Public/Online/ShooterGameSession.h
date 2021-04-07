// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Online.h"
#include "GameFramework/GameSession.h"
#include "ShooterGameSession.generated.h"

/**
 * 
 */

 //������ʱsession�ؼ�����
struct FShooterGameSessionParam
{
	FName SessionName;
	bool bIsLan; 
	bool bIsPresence;
	TSharedPtr<const FUniqueNetId> UserId;
	int32 BestSessionIndex;

	FShooterGameSessionParam()
		:SessionName(NAME_None),
		bIsLan(false),
		bIsPresence(false),
		BestSessionIndex(0)
	{

	}
};

UCLASS()
class SHOOTERGAME_API AShooterGameSession : public AGameSession
{
	GENERATED_UCLASS_BODY()

protected:
	FShooterGameSessionParam CurrentSessionParams;
	
	TSharedPtr<class FShooterOnlineSessionSettings> HostSettings;

	virtual void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessfull);
	
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;

	DECLARE_EVENT_TwoParams(AShooterGameSession, FOnCreatePresenceSessionComplete, FName, bool);
	FOnCreatePresenceSessionComplete CreatePresenceSessionCompleteEvent;

	
public:
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, const FString& GameType, const FString& MapName, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers);

	//�Դ�������ID��ʶ
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;

	FOnCreatePresenceSessionComplete& OnCreatePresenceSessionComplete() {
		return CreatePresenceSessionCompleteEvent;
	}

	static const int32 DEFAULT_NUM_PLAYERS = 8;
};
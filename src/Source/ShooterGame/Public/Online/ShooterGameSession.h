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

	TSharedPtr<class FShooterOnlineSearchSettings> SearchSettings;

	virtual void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessfull);

	void OnFindSessionsComplete(bool bWasSuccessfull);

	void OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result);
	
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;

	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	DECLARE_EVENT_TwoParams(AShooterGameSession, FOnCreatePresenceSessionComplete, FName, bool);
	FOnCreatePresenceSessionComplete CreatePresenceSessionCompleteEvent;

	DECLARE_EVENT_OneParam(AShooterGameSession, FOnFindSessionsComplete, bool);
	FOnFindSessionsComplete FindSessionsCompleteEvent;

	DECLARE_EVENT_OneParam(AShooterGameSession, FOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type);
	FOnJoinSessionComplete JoinSessionCompleteEvent;
	
public:
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, const FString& GameType, const FString& MapName, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers);

	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, bool bIsLan, bool bIsPresence);
	
	//��ѯ����������״̬
	EOnlineAsyncTaskState::Type GetSearchResultState(int32& SearchResultIndex, int32& NumSearchResults);

	const TArray<FOnlineSessionSearchResult>& GetSearchResults() const;

	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, int32 SessionIndexInSearchResult);

	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, const FOnlineSessionSearchResult& SearchResult);

	//�Դ�������ID��ʶ
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;

	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	FOnCreatePresenceSessionComplete& OnCreatePresenceSessionComplete() {
		return CreatePresenceSessionCompleteEvent;
	}

	FOnFindSessionsComplete& OnFindSessionsComplete() {
		return FindSessionsCompleteEvent;
	}

	FOnJoinSessionComplete& OnJoinSessionComplete() {
		return JoinSessionCompleteEvent;
	}
	static const int32 DEFAULT_NUM_PLAYERS = 8;
};
// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameSession.h"
#include "ShooterOnlineSessionSettings.h"
#include "OnlineSubsystemSessionSettings.h"

namespace
{
	const FString CustomMatchKeyword("Custom");
}

AShooterGameSession::AShooterGameSession(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//过滤默认对象
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &AShooterGameSession::OnCreateSessionComplete);
		OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &AShooterGameSession::OnFindSessionsComplete);
		OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &AShooterGameSession::OnJoinSessionComplete);

	}
}

//实现HostSession接口
bool AShooterGameSession::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, const FString& GameType, const FString& MapName, bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	//获取IOnlineSessionPtr指向的对象
	IOnlineSubsystem* const OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		CurrentSessionParams.SessionName = InSessionName;
		CurrentSessionParams.bIsLan = bIsLan;
		CurrentSessionParams.bIsPresence = bIsPresence;
		CurrentSessionParams.UserId = UserId;
		MaxPlayers = MaxNumPlayers;
		
		IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
		if (Session.IsValid()&& CurrentSessionParams.UserId.IsValid())
		{
			HostSettings = MakeShareable(new FShooterOnlineSessionSettings(bIsLan, bIsPresence, MaxNumPlayers));
			//公开部分参数,gametype通过在线服务公开,matchinghopper:起绰号,matchingtimeout:游戏匹配时间
			HostSettings->Set(SETTING_GAMEMODE, GameType, EOnlineDataAdvertisementType::ViaOnlineService);
			HostSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
			HostSettings->Set(SETTING_MATCHING_HOPPER, FString("TeamDeathmactch"), EOnlineDataAdvertisementType::DontAdvertise);
			HostSettings->Set(SETTING_MATCHING_TIMEOUT, 120.0f, EOnlineDataAdvertisementType::ViaOnlineService);
			HostSettings->Set(SETTING_SESSION_TEMPLATE_NAME , FString("GameSession"), EOnlineDataAdvertisementType::DontAdvertise);
			HostSettings->Set(SEARCH_KEYWORDS, CustomMatchKeyword, EOnlineDataAdvertisementType::ViaOnlineService);

			//绑定创建完成代理
			OnCreateSessionCompleteDelegateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
			return Session->CreateSession(*CurrentSessionParams.UserId, CurrentSessionParams.SessionName, *HostSettings);
		}
	}
	return false;
}

void AShooterGameSession::FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, bool bIsLan, bool bIsPresence)
{
	IOnlineSubsystem* const OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		CurrentSessionParams.SessionName = InSessionName;
		CurrentSessionParams.bIsLan = bIsLan;
		CurrentSessionParams.bIsPresence = bIsPresence;
		CurrentSessionParams.UserId = UserId;

		IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
		if (Session.IsValid() && CurrentSessionParams.UserId.IsValid())
		{
			SearchSettings = MakeShareable(new FShooterOnlineSearchSettings(bIsLan, bIsPresence));
			SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, CustomMatchKeyword, EOnlineComparisonOp::Equals);

			TSharedRef<FShooterOnlineSearchSettings> SearchSettingRef = SearchSettings.ToSharedRef();

			OnFindSessionsCompleteDelegateHandle = Session->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			Session->FindSessions(*CurrentSessionParams.UserId, SearchSettingRef);
		}
	}
	else
	{
		OnFindSessionsComplete(false);
	}
}

void AShooterGameSession::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessfull)
{
	IOnlineSubsystem* const OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
		//清除代理句柄
		Session->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}

	//调用代理上绑定的所有函数，通知外层session已经创建完成
	OnCreatePresenceSessionComplete().Broadcast(InSessionName, bWasSuccessfull);
	
}

void AShooterGameSession::OnFindSessionsComplete(bool bWasSuccessfull)
{
	IOnlineSubsystem* const OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
		Session->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

		for (int32 SearchIndex = 0; SearchIndex < SearchSettings->SearchResults.Num(); SearchIndex++)
		{
			const FOnlineSessionSearchResult& SearchResult = SearchSettings->SearchResults[SearchIndex];

			//打印函数
			DumpSession(&SearchResult.Session);
		}
	//调用代理上绑定的所有函数，通知外层session已经完成搜索
	OnFindSessionsComplete().Broadcast(bWasSuccessfull);
	}
}

void AShooterGameSession::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* const OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
		Session->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	}
	OnJoinSessionComplete().Broadcast(Result);
}

EOnlineAsyncTaskState::Type AShooterGameSession::GetSearchResultState(int32& SearchResultIndex, int32& NumSearchResults)
{
	SearchResultIndex = 0;
	NumSearchResults = 0;

	if (SearchSettings.IsValid())
	{
		if (SearchSettings->SearchState == EOnlineAsyncTaskState::Done)
		{
			SearchResultIndex = CurrentSessionParams.BestSessionIndex;
			NumSearchResults = SearchSettings->SearchResults.Num();
		}
		return SearchSettings->SearchState;
	}

	return EOnlineAsyncTaskState::NotStarted;
}

const TArray<FOnlineSessionSearchResult>& AShooterGameSession::GetSearchResults() const
{
	return SearchSettings->SearchResults;
}


bool AShooterGameSession::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, int32 SessionIndexInSearchResult)
{
	bool bResult = false;
	if (SessionIndexInSearchResult >= 0 && SessionIndexInSearchResult < SearchSettings->SearchResults.Num())
	{
		bResult = JoinSession(UserId, InSessionName, SearchSettings->SearchResults[SessionIndexInSearchResult]);
	}
	return bResult;
}

bool AShooterGameSession::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, const FOnlineSessionSearchResult& SearchResult)
{
	bool bResult = false;
	IOnlineSubsystem* const OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
		if (Session.IsValid() && UserId.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			bResult = Session->JoinSession(*UserId, InSessionName, SearchResult);
		}
	}
	return bResult;
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameSession.h"
#include "ShooterOnlineSessionSettings.h"
#include "OnlineSubsystemSessionSettings.h"

namespace
{
	const FString CustomMatchKeyWord("Custom");
}

AShooterGameSession::AShooterGameSession(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//过滤默认对象
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &AShooterGameSession::OnCreateSessionComplete);
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
			HostSettings->Set(SEARCH_KEYWORDS, CustomMatchKeyWord, EOnlineDataAdvertisementType::ViaOnlineService);

			//绑定创建完成代理
			OnCreateSessionCompleteDelegateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
			return Session->CreateSession(*CurrentSessionParams.UserId, CurrentSessionParams.SessionName, *HostSettings);
		}
	}
	return false;
}


void AShooterGameSession::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessfull)
{
	IOnlineSubsystem* const OnlineSubsys = IOnlineSubsystem::Get();
	if (OnlineSubsys)
	{
		IOnlineSessionPtr Session = OnlineSubsys->GetSessionInterface();
		Session->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}

	//调用代理上绑定的所有函数，有机会告诉外层session已经创建完成
	OnCreatePresenceSessionComplete().Broadcast(InSessionName, bWasSuccessfull);
	
}
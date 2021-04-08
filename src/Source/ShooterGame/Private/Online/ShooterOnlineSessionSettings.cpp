// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterOnlineSessionSettings.h"

FShooterOnlineSessionSettings::FShooterOnlineSessionSettings(bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	//允许的公有链接
	NumPublicConnections = MaxNumPlayers;
	if (NumPublicConnections < 0)
	{
		NumPublicConnections = 0;
	}
	//不允许私下邀请
	NumPrivateConnections = 0;
	//是否为局域网
	bIsLANMatch = bIsLan;
	//是否允许将session公布到网上去
	bShouldAdvertise = true;
	//游戏进行过程中，是否允许玩家加入
	bAllowJoinInProgress = true;
	bAllowInvites = true;
	//允许出席玩家邀请玩家
	bUsesPresence = bIsPresence;
	bAllowJoinViaPresence = true;
	bAllowJoinViaPresenceFriendsOnly = false;
}

FShooterOnlineSessionSettings::~FShooterOnlineSessionSettings()
{
}

FShooterOnlineSearchSettings::FShooterOnlineSearchSettings(bool bSearchingLan, bool bSearchingPresence)
{
	bIsLanQuery = bSearchingLan;
	MaxSearchResults = 10;
	PingBucketSize = 50;

	if (bSearchingPresence)
	{
		QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	}

}
FShooterOnlineSearchSettings::~FShooterOnlineSearchSettings()
{

}

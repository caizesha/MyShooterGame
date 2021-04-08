// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterOnlineSessionSettings.h"

FShooterOnlineSessionSettings::FShooterOnlineSessionSettings(bool bIsLan, bool bIsPresence, int32 MaxNumPlayers)
{
	//����Ĺ�������
	NumPublicConnections = MaxNumPlayers;
	if (NumPublicConnections < 0)
	{
		NumPublicConnections = 0;
	}
	//������˽������
	NumPrivateConnections = 0;
	//�Ƿ�Ϊ������
	bIsLANMatch = bIsLan;
	//�Ƿ�����session����������ȥ
	bShouldAdvertise = true;
	//��Ϸ���й����У��Ƿ�������Ҽ���
	bAllowJoinInProgress = true;
	bAllowInvites = true;
	//�����ϯ����������
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

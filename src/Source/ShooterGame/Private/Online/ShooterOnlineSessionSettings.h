// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OnlineSessionSettings.h"

/**
 *
 */
class FShooterOnlineSessionSettings : public FOnlineSessionSettings
{
public:
	FShooterOnlineSessionSettings(bool bIsLan = false, bool bIsPresence = false, int32 MaxNumPlayers = 4);
	~FShooterOnlineSessionSettings();
};

class FShooterOnlineSearchSettings : public FOnlineSessionSearch
{
public:
	//bSearchingPresence:�Ƿ���Ѱ����������
	FShooterOnlineSearchSettings(bool bSearchingLan = false, bool bSearchingPresence = false);
	~FShooterOnlineSearchSettings();
};
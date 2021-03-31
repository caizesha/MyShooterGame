// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"


//�Խӿ��������д
class FShooterGameModule : public FDefaultGameModuleImpl 
{
	virtual void StartupModule() override
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(FShooterStyle::GetStyleSetName());
		FShooterStyle::Initialize();
	}
	virtual void ShutdownModule() override
	{
		FShooterStyle::Shutdown();
	}

};
//��¶�ӿڣ��Ա�������ø�ģ��
//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ShooterGame, "ShooterGame" );
IMPLEMENT_PRIMARY_GAME_MODULE(FShooterGameModule, ShooterGame, "ShooterGame" );

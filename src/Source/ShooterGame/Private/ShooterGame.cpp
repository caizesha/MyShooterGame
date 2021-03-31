// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"


//对接口类进行重写
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
//暴露接口，以便引擎调用该模块
//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ShooterGame, "ShooterGame" );
IMPLEMENT_PRIMARY_GAME_MODULE(FShooterGameModule, ShooterGame, "ShooterGame" );

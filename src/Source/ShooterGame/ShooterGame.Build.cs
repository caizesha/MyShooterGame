// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ShooterGame : ModuleRules
{
	public ShooterGame(TargetInfo Target)
	{
        //暴露private目录头文件
        PrivateIncludePaths.AddRange(new string[]
            {
                "ShooterGame/Private",
                "ShooterGame/Private/UI",
                "ShooterGame/Private/UI/Menu",
                "ShooterGame/Private/UI/Style",
                "ShooterGame/Private/UI/Menu/Widgets",
        });
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayTasks" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

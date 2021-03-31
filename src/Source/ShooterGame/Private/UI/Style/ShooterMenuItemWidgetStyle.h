// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SlateWidgetStyleContainerBase.h"
#include "ShooterMenuItemWidgetStyle.generated.h"

/**
 * 
 */
USTRUCT()
struct FShooterMenuItemStyle :public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FShooterMenuItemStyle();
	virtual ~FShooterMenuItemStyle();

	virtual void GetResources(TArray<const FSlateBrush *> & OutBrushes) const override;

	static const FName TypeName;

	virtual const FName GetTypeName() const override { return TypeName; }

	static const FShooterMenuItemStyle& GetDefault();

	//图片样式资源类
	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateBrush BackgroundBrush;
	FShooterMenuItemStyle& SetBackgroundBrush(FSlateBrush& InBackgroundBrush) { BackgroundBrush = InBackgroundBrush; }
};

UCLASS(hidecategories = Object, MinimalAPI)
class UShooterMenuItemWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	~UShooterMenuItemWidgetStyle();
	
	UPROPERTY(EditAnywhere, Category = "Appearance", meta = (ShowOnlyInnerProperties))
	FShooterMenuItemStyle MenuItemStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&MenuItemStyle);
	}
};

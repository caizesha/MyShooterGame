// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SlateWidgetStyleContainerBase.h"
#include "ShooterMenuWidgetStyle.generated.h"

/**
 * 
 */

//FShooterMenuStyle:管理SlateBrush具体图片资源，以便于获取并暴露到编辑器
USTRUCT()
struct FShooterMenuStyle :public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FShooterMenuStyle();
	virtual ~FShooterMenuStyle();

	virtual void GetResources(TArray<const FSlateBrush *> & OutBrushes) const override;

	static const FName TypeName;
	
	virtual const FName GetTypeName() const override { return TypeName;}

	static const FShooterMenuStyle& GetDefault();

	//图片样式资源类
	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateBrush HeaderBackgroundBrush;
	FShooterMenuStyle& SetHeaderBackgroundBrush(FSlateBrush& InHeaderBackgroundBrush) { HeaderBackgroundBrush = InHeaderBackgroundBrush; }

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateBrush LeftBackgroundBrush;
	FShooterMenuStyle& SetLeftackgroundBrush(FSlateBrush& InLeftBackgroundBrush) { LeftBackgroundBrush = InLeftBackgroundBrush; }

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FSlateBrush RightBackgroundBrush;
	FShooterMenuStyle& SetRightBackgroundBrush(FSlateBrush& InRightBackgroundBrush) { RightBackgroundBrush = InRightBackgroundBrush; }
};


//对实际数据的封装容器类，方便蓝图拓展此类并编辑配置。
//进一步封装，将上面结构体暴露在编辑器，从而可以编辑配置。
//hidecategories：类只暴露结构体MenuStyle,隐藏Object类成员变量、函数； MinimalAPI：减少反射信息的收集
UCLASS(hidecategories = Object, MinimalAPI)
class UShooterMenuWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	~UShooterMenuWidgetStyle();

	UPROPERTY(EditAnywhere, Category = "Appearance", meta = (ShowOnlyInnerProperties))
	FShooterMenuStyle MenuStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&MenuStyle);
	}
};

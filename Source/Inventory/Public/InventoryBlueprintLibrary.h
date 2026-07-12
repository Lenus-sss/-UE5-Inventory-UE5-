#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryBlueprintLibrary.generated.h"

class AActor;
class UInventoryComponent;

/**
 * Inventory 蓝图辅助函数库，提供从 Actor 查找背包组件的入口。
 */
UCLASS()
class INVENTORY_API UInventoryBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 从指定 Actor 上查找 InventoryComponent；Actor 为空或没有组件时返回 nullptr。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	static UInventoryComponent* FindInventoryComponent(AActor* Actor);
};

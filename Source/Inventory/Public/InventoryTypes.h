#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.generated.h"

class UTexture2D;

/**
 * 一种可进入背包的物品数据，ItemId 相同就认为是同一类物品。
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FInventoryItemData
{
	GENERATED_BODY()

public:
	/** 物品唯一 ID；同类物品必须使用同一个 ItemId。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName ItemId = NAME_None;

	/** 物品显示名，主要给 UI 或提示文字使用。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FText DisplayName;

	/** 物品图标；蓝图 Widget 读取它后显示到对应格子里。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon;

	/** 单个格子的最大堆叠数量，背包规则硬上限为 64。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "1", ClampMax = "64"))
	int32 MaxStackSize = 64;

	/** 判断物品数据是否能进入背包；ItemId 为空时视为无效物品。 */
	bool IsValidItem() const;

	/** 返回安全的堆叠上限，避免外部把 MaxStackSize 配成 0 或超过 64。 */
	int32 GetSafeMaxStackSize() const;
};

/**
 * 背包中的一个格子；第一版规则是一个 ItemId 只占一个格子。
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FInventorySlotData
{
	GENERATED_BODY()

public:
	/** 当前格子的物品数据；空格子时 ItemId 为 None。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	FInventoryItemData ItemData;

	/** 当前格子的物品数量；为 0 时表示空格子。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "0"))
	int32 Quantity = 0;

	/** 判断当前格子是否为空。 */
	bool IsEmpty() const;

	/** 判断当前格子是否可以继续叠加指定物品。 */
	bool CanStackWith(const FName InItemId) const;

	/** 清空当前格子里的物品和数量。 */
	void Clear();
};

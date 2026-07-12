#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "InventoryItemComponent.generated.h"

class UTexture2D;

/**
 * 挂在世界物品身上的背包物品组件，保存拾取后要加入背包的数据。
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class INVENTORY_API UInventoryItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** 初始化默认拾取数量；组件本身不需要 Tick。 */
	UInventoryItemComponent();

	/** 返回当前物品要写入背包的物品数据。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FInventoryItemData GetItemData() const;

	/** 返回一次交互拾取的数量，默认 1。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetPickupQuantity() const;

	/** 判断当前物品配置是否可以尝试加入背包。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsValidInventoryItem() const;

private:
	/** 物品唯一 ID；同类物品必须保持一致。 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName ItemId = TEXT("TestPickup");

	/** UI 显示名，蓝图 Widget 可以从背包格子里读取。 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText DisplayName;

	/** UI 图标；你在 UE 里导入贴图后，可以给测试物品指定这里。 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon;

	/** 单类物品最大数量，背包规则硬上限为 64。 */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ClampMin = "1", ClampMax = "64"))
	int32 MaxStackSize = 64;

	/** 每次交互给背包增加几个，测试物品默认加 1。 */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ClampMin = "1"))
	int32 PickupQuantity = 1;
};

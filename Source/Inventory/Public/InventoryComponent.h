#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "InventoryComponent.generated.h"

/** 背包内容改变时广播，蓝图 Widget 监听后调用 GetSlots 刷新图标和数量。 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChangedSignature);

/** 添加物品失败时广播，常用于 UI 提示背包已满或数量已到上限。 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryAddFailedSignature, const FInventoryItemData&, ItemData, int32, RequestedQuantity);

/**
 * 挂在玩家身上的背包组件，负责物品数量、格子和事件广播。
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class INVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** 初始化默认格子数量；组件不需要每帧 Tick。 */
	UInventoryComponent();

	/** 游戏开始时确保格子数组和 SlotCapacity 对齐。 */
	virtual void BeginPlay() override;

	/** 背包变化事件；你的蓝图 Widget 应绑定它来刷新 UI。 */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryChangedSignature OnInventoryChanged;

	/** 添加失败事件；物品无效、数量无效、已满或超过 64 都会触发。 */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryAddFailedSignature OnInventoryAddFailed;

	/** 添加指定数量物品；成功返回 true，超过同类上限或没有空格返回 false。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(const FInventoryItemData& ItemData, int32 Quantity = 1);

	/** 减少指定数量物品；数量不足时不改背包并返回 false。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemId, int32 Quantity = 1);

	/** 查询背包里某类物品数量是否足够。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(FName ItemId, int32 Quantity = 1) const;

	/** 返回某类物品当前总数量；第一版同类物品只会存在一个格子里。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemCount(FName ItemId) const;

	/** 返回当前所有格子数据，蓝图 Widget 用它刷新每个格子的图标和数量。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FInventorySlotData> GetSlots() const { return Slots; }

	/** 返回背包格子上限，默认 8 格，对应你当前画好的横向背包栏。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetSlotCapacity() const { return SlotCapacity; }

	/** 清空整个背包，并广播刷新事件。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearInventory();

private:
	/** 背包格子数量上限；第一版默认 8 格。 */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ClampMin = "1"))
	int32 SlotCapacity = 8;

	/** 当前背包格子数据；固定长度方便 UI 直接按索引绘制。 */
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	TArray<FInventorySlotData> Slots;

	/** 确保 Slots 数组长度等于 SlotCapacity。 */
	void EnsureSlotArray();

	/** 查找指定 ItemId 所在格子；找不到返回 INDEX_NONE。 */
	int32 FindSlotIndexByItemId(FName ItemId) const;

	/** 查找第一个空格子；没有空格时返回 INDEX_NONE。 */
	int32 FindFirstEmptySlotIndex() const;

	/** 广播添加失败事件，并统一返回 false。 */
	bool FailAddItem(const FInventoryItemData& ItemData, int32 Quantity);
};

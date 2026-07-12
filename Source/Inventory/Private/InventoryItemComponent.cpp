#include "InventoryItemComponent.h"

UInventoryItemComponent::UInventoryItemComponent()
{
	// 物品数据组件只保存静态拾取信息，不需要每帧 Tick。
	PrimaryComponentTick.bCanEverTick = false;

	// 给测试物品一个默认显示名，避免蓝图没配置时 UI 读到空文本。
	DisplayName = FText::FromString(TEXT("测试物品"));
}

FInventoryItemData UInventoryItemComponent::GetItemData() const
{
	FInventoryItemData ItemData;
	ItemData.ItemId = ItemId;
	ItemData.DisplayName = DisplayName;
	ItemData.Icon = Icon;
	ItemData.MaxStackSize = MaxStackSize;
	return ItemData;
}

int32 UInventoryItemComponent::GetPickupQuantity() const
{
	// 拾取数量至少为 1，避免物品交互成功但背包没有变化。
	return FMath::Max(1, PickupQuantity);
}

bool UInventoryItemComponent::IsValidInventoryItem() const
{
	// 只要 ItemId 有效，就可以尝试进入背包；图标可以后续再配置。
	return GetItemData().IsValidItem();
}

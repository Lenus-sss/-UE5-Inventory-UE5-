#include "InventoryTypes.h"

bool FInventoryItemData::IsValidItem() const
{
	// ItemId 是物品唯一身份，没有 ID 就不能放入背包。
	return !ItemId.IsNone();
}

int32 FInventoryItemData::GetSafeMaxStackSize() const
{
	// 背包堆叠至少为 1，最多为 64，严格匹配当前背包规则。
	return FMath::Clamp(MaxStackSize, 1, 64);
}

bool FInventorySlotData::IsEmpty() const
{
	// 数量为 0 或 ItemId 为空时，都把格子视为空格。
	return Quantity <= 0 || ItemData.ItemId.IsNone();
}

bool FInventorySlotData::CanStackWith(const FName InItemId) const
{
	// 只有同一个 ItemId 且未达到堆叠上限时，才允许叠加。
	return !IsEmpty() && ItemData.ItemId == InItemId && Quantity < ItemData.GetSafeMaxStackSize();
}

void FInventorySlotData::Clear()
{
	// 清空数据后 UI 读取这个格子时会自然显示为空。
	ItemData = FInventoryItemData();
	Quantity = 0;
}

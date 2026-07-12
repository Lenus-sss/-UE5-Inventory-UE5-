#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	// 背包数据只在调用接口时变化，不需要每帧 Tick。
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	EnsureSlotArray();
}

bool UInventoryComponent::AddItem(const FInventoryItemData& ItemData, int32 Quantity)
{
	EnsureSlotArray();

	// 无效物品或无效数量不能进入背包，直接返回失败。
	if (!ItemData.IsValidItem() || Quantity <= 0)
	{
		return FailAddItem(ItemData, Quantity);
	}

	// 第一版规则：同一类物品只占一个格子，总数最多 MaxStackSize，默认 64。
	const int32 ExistingSlotIndex = FindSlotIndexByItemId(ItemData.ItemId);
	if (ExistingSlotIndex != INDEX_NONE)
	{
		FInventorySlotData& ExistingSlot = Slots[ExistingSlotIndex];
		const int32 MaxStackSize = ExistingSlot.ItemData.GetSafeMaxStackSize();
		if (ExistingSlot.Quantity + Quantity > MaxStackSize)
		{
			return FailAddItem(ItemData, Quantity);
		}

		ExistingSlot.Quantity += Quantity;
		OnInventoryChanged.Broadcast();
		return true;
	}

	// 新物品从 0 到 1 时需要一个空格；没有空格则添加失败。
	const int32 MaxStackSize = ItemData.GetSafeMaxStackSize();
	if (Quantity > MaxStackSize)
	{
		return FailAddItem(ItemData, Quantity);
	}

	const int32 EmptySlotIndex = FindFirstEmptySlotIndex();
	if (EmptySlotIndex == INDEX_NONE)
	{
		return FailAddItem(ItemData, Quantity);
	}

	Slots[EmptySlotIndex].ItemData = ItemData;
	Slots[EmptySlotIndex].Quantity = Quantity;
	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::RemoveItem(FName ItemId, int32 Quantity)
{
	EnsureSlotArray();

	// 移除数量必须有效，ItemId 也必须明确。
	if (ItemId.IsNone() || Quantity <= 0)
	{
		return false;
	}

	const int32 SlotIndex = FindSlotIndexByItemId(ItemId);
	if (SlotIndex == INDEX_NONE || Slots[SlotIndex].Quantity < Quantity)
	{
		return false;
	}

	Slots[SlotIndex].Quantity -= Quantity;
	if (Slots[SlotIndex].Quantity <= 0)
	{
		Slots[SlotIndex].Clear();
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::HasItem(FName ItemId, int32 Quantity) const
{
	// 查询数量小于等于 0 时没有实际意义，按 false 处理。
	if (ItemId.IsNone() || Quantity <= 0)
	{
		return false;
	}

	return GetItemCount(ItemId) >= Quantity;
}

int32 UInventoryComponent::GetItemCount(FName ItemId) const
{
	if (ItemId.IsNone())
	{
		return 0;
	}

	const int32 SlotIndex = FindSlotIndexByItemId(ItemId);
	return SlotIndex == INDEX_NONE ? 0 : Slots[SlotIndex].Quantity;
}

void UInventoryComponent::ClearInventory()
{
	EnsureSlotArray();

	for (FInventorySlotData& Slot : Slots)
	{
		Slot.Clear();
	}

	OnInventoryChanged.Broadcast();
}

void UInventoryComponent::EnsureSlotArray()
{
	// 格子数量至少为 1，避免蓝图误配导致背包完全不可用。
	SlotCapacity = FMath::Max(1, SlotCapacity);
	if (Slots.Num() != SlotCapacity)
	{
		Slots.SetNum(SlotCapacity);
	}
}

int32 UInventoryComponent::FindSlotIndexByItemId(FName ItemId) const
{
	if (ItemId.IsNone())
	{
		return INDEX_NONE;
	}

	for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
	{
		if (!Slots[SlotIndex].IsEmpty() && Slots[SlotIndex].ItemData.ItemId == ItemId)
		{
			return SlotIndex;
		}
	}

	return INDEX_NONE;
}

int32 UInventoryComponent::FindFirstEmptySlotIndex() const
{
	for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
	{
		if (Slots[SlotIndex].IsEmpty())
		{
			return SlotIndex;
		}
	}

	return INDEX_NONE;
}

bool UInventoryComponent::FailAddItem(const FInventoryItemData& ItemData, int32 Quantity)
{
	// 失败也广播，方便 UI 做“背包已满”或“数量已达上限”的反馈。
	OnInventoryAddFailed.Broadcast(ItemData, Quantity);
	return false;
}

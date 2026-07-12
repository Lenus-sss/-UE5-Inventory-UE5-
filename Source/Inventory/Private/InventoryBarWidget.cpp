#include "InventoryBarWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "InventoryComponent.h"
#include "InventoryTypes.h"

void UInventoryBarWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	if (InventoryComponent == InInventoryComponent)
	{
		RefreshInventory();
		return;
	}

	// 切换背包组件前先解绑旧组件，避免同一个 Widget 被重复回调。
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &ThisClass::HandleInventoryChanged);
	}

	InventoryComponent = InInventoryComponent;

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddUniqueDynamic(this, &ThisClass::HandleInventoryChanged);
	}

	RefreshInventory();
}

void UInventoryBarWidget::RefreshInventory()
{
	CacheSlotWidgets();

	const TArray<FInventorySlotData> Slots = InventoryComponent ? InventoryComponent->GetSlots() : TArray<FInventorySlotData>();
	const int32 SlotCount = FMath::Max(SlotIconImages.Num(), SlotQuantityTexts.Num());

	for (int32 SlotIndex = 0; SlotIndex < SlotCount; ++SlotIndex)
	{
		UImage* IconImage = SlotIconImages.IsValidIndex(SlotIndex) ? SlotIconImages[SlotIndex] : nullptr;
		UTextBlock* QuantityText = SlotQuantityTexts.IsValidIndex(SlotIndex) ? SlotQuantityTexts[SlotIndex] : nullptr;

		const bool bHasSlotData = Slots.IsValidIndex(SlotIndex);
		const bool bHasItem = bHasSlotData && !Slots[SlotIndex].IsEmpty();

		if (IconImage)
		{
			if (bHasItem && Slots[SlotIndex].ItemData.Icon)
			{
				// 有物品时把物品图标写入 Image，尺寸和位置继续使用你的 Widget 设计。
				IconImage->SetBrushFromTexture(Slots[SlotIndex].ItemData.Icon, false);
				IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				// 空格子隐藏图标，背包底图仍然保留。
				IconImage->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (QuantityText)
		{
			if (bHasItem && Slots[SlotIndex].Quantity > 0)
			{
				// 只要格子里有物品就显示数量，方便测试拾取 1 个物品时确认刷新成功。
				QuantityText->SetText(FText::AsNumber(Slots[SlotIndex].Quantity));
				QuantityText->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				QuantityText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UInventoryBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CacheSlotWidgets();
	RefreshInventory();
}

void UInventoryBarWidget::NativeDestruct()
{
	if (InventoryComponent)
	{
		// Widget 被移除时解绑事件，避免背包组件之后调用到无效 UI。
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &ThisClass::HandleInventoryChanged);
	}

	Super::NativeDestruct();
}

void UInventoryBarWidget::CacheSlotWidgets()
{
	if (!WidgetTree)
	{
		return;
	}

	const int32 DesiredSlotCount = InventoryComponent ? InventoryComponent->GetSlotCapacity() : 8;
	SlotIconImages.SetNum(DesiredSlotCount);
	SlotQuantityTexts.SetNum(DesiredSlotCount);

	for (int32 SlotIndex = 0; SlotIndex < DesiredSlotCount; ++SlotIndex)
	{
		const FName SlotName(*FString::Printf(TEXT("WBP_InventorySlot_%d"), SlotIndex));
		UWidget* SlotRootWidget = WidgetTree->FindWidget(SlotName);
		if (!SlotRootWidget)
		{
			SlotIconImages[SlotIndex] = nullptr;
			SlotQuantityTexts[SlotIndex] = nullptr;
			continue;
		}

		SlotIconImages[SlotIndex] = FindFirstDescendantWidget<UImage>(SlotRootWidget);
		SlotQuantityTexts[SlotIndex] = FindFirstDescendantWidget<UTextBlock>(SlotRootWidget);
	}
}

template <typename WidgetType>
WidgetType* UInventoryBarWidget::FindFirstDescendantWidget(UWidget* RootWidget) const
{
	if (!RootWidget)
	{
		return nullptr;
	}

	if (WidgetType* TypedRoot = Cast<WidgetType>(RootWidget))
	{
		return TypedRoot;
	}

	if (UUserWidget* UserWidget = Cast<UUserWidget>(RootWidget))
	{
		if (UserWidget->WidgetTree)
		{
			return FindFirstDescendantWidget<WidgetType>(UserWidget->WidgetTree->RootWidget);
		}
	}

	if (UPanelWidget* PanelWidget = Cast<UPanelWidget>(RootWidget))
	{
		const int32 ChildCount = PanelWidget->GetChildrenCount();
		for (int32 ChildIndex = 0; ChildIndex < ChildCount; ++ChildIndex)
		{
			if (WidgetType* FoundWidget = FindFirstDescendantWidget<WidgetType>(PanelWidget->GetChildAt(ChildIndex)))
			{
				return FoundWidget;
			}
		}
	}

	return nullptr;
}

void UInventoryBarWidget::HandleInventoryChanged()
{
	RefreshInventory();
}

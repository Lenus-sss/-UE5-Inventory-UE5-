#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryBarWidget.generated.h"

class UImage;
class UInventoryComponent;
class UTextBlock;
class UWidget;

/**
 * 背包条 Widget 基类，负责把 InventoryComponent 的格子数据刷新到设计好的 UMG 控件上。
 */
UCLASS(BlueprintType, Blueprintable)
class INVENTORY_API UInventoryBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 绑定玩家背包组件，并立即刷新一次 UI。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetInventoryComponent(UInventoryComponent* InInventoryComponent);

	/** 手动刷新背包 UI；通常由 OnInventoryChanged 自动调用。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshInventory();

protected:
	/** Widget 构造完成后缓存格子里的 Image 和 TextBlock。 */
	virtual void NativeConstruct() override;

	/** Widget 销毁前解绑背包事件，避免组件继续回调已销毁的 UI。 */
	virtual void NativeDestruct() override;

private:
	/** 当前绑定的玩家背包组件。 */
	UPROPERTY(Transient)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	/** 每个格子里的图标 Image，按 0 到 7 的顺序缓存。 */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UImage>> SlotIconImages;

	/** 每个格子里的数量 TextBlock，按 0 到 7 的顺序缓存。 */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> SlotQuantityTexts;

	/** 根据格子命名缓存所有 UI 引用。 */
	void CacheSlotWidgets();

	/** 找到指定 Widget 子树中的第一个指定类型控件。 */
	template <typename WidgetType>
	WidgetType* FindFirstDescendantWidget(UWidget* RootWidget) const;

	/** 背包组件广播变化时调用，转发到 RefreshInventory。 */
	UFUNCTION()
	void HandleInventoryChanged();
};

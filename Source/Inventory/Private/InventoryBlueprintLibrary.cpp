#include "InventoryBlueprintLibrary.h"

#include "InventoryComponent.h"

UInventoryComponent* UInventoryBlueprintLibrary::FindInventoryComponent(AActor* Actor)
{
	// 背包第一版直接挂在玩家 Actor 上，所以只需要查找拥有者自身组件。
	return Actor ? Actor->FindComponentByClass<UInventoryComponent>() : nullptr;
}

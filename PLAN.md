# Inventory 插件计划

## 第一版目标

做一个挂在玩家身上的运行时背包插件，用 C++ 负责背包数据和事件广播，用蓝图 Widget 负责显示。

第一版只支持：
- 添加物品。
- 减少物品。
- 背包数据改变后通知 HUD 刷新。
- 背包格子数量有上限。
- 每格同类物品最多堆叠 64 个。
- 新物品类型没有空格时提示背包已满。

第一版暂不支持：
- 存档。
- 商店。
- 拖拽。
- 网络同步。
- 装备栏。

## 核心规则

- 每种物品用 `FName ItemId` 作为唯一 ID。
- `ItemId` 相同就认为是同类物品。
- 每个 `ItemId` 对应一个图标和一个世界物品 Actor 类。
- 新类型物品会占用一个空格。
- 已有类型物品会优先叠加到已有格子。
- 单格达到 `MaxStackSize` 后，剩余数量会尝试放入新格子。
- 没有足够空格时，添加失败并广播背包已满事件。
- 物品数量减到 0 后，该格子清空。

## 建议数据结构

### `FInventoryItemData`

表示一种物品的静态显示和生成信息。

关键字段：
- `FName ItemId`：物品唯一 ID。
- `FText DisplayName`：物品显示名。
- `TObjectPtr<UTexture2D> Icon`：HUD 显示图标。
- `TSubclassOf<AActor> ItemActorClass`：世界里的物品 Actor 类。
- `int32 MaxStackSize`：单格最大堆叠数，默认 64。

### `FInventorySlotData`

表示背包里的一个格子。

关键字段：
- `FInventoryItemData ItemData`：格子里的物品数据。
- `int32 Quantity`：当前数量。

关键函数：
- `IsEmpty()`：判断格子是否为空。
- `CanStackWith(ItemId)`：判断是否能和指定物品叠加。

## 建议核心类

### `UInventoryComponent`

挂在玩家身上的背包组件，负责所有背包数据变化。

关键属性：
- `int32 SlotCapacity`：背包格子上限，默认按 HUD 使用 8 格。
- `int32 DefaultMaxStackSize`：默认单格堆叠上限，默认 64。
- `TArray<FInventorySlotData> Slots`：当前背包格子。

关键函数：
- `AddItem(ItemData, Quantity)`：添加物品。
- `RemoveItem(ItemId, Quantity)`：减少物品。
- `GetSlots()`：给蓝图读取当前背包。
- `HasItem(ItemId, Quantity)`：查询数量是否足够。
- `GetItemCount(ItemId)`：查询某类物品总数量。
- `ClearInventory()`：清空背包。

关键事件：
- `OnInventoryChanged`：背包变化后广播，HUD 用它刷新图标和数字。
- `OnInventoryFull`：添加新类型物品但没有空格时广播，HUD 或角色蓝图用它提示“背包已满”。

### `UInventoryBlueprintLibrary`

给蓝图提供辅助入口，第一版可以很薄。

建议函数：
- `FindInventoryComponent(Actor)`：从玩家或任意 Actor 上找到 `UInventoryComponent`。

## HUD 接入方式

你的背包 HUD 蓝图常驻屏幕左下角。

推荐流程：
1. 玩家 BeginPlay 时创建 HUD Widget 并加入视口。
2. HUD Widget 获取玩家身上的 `UInventoryComponent`。
3. HUD Widget 绑定 `OnInventoryChanged`。
4. 每次事件触发时，HUD Widget 调用 `GetSlots()`。
5. HUD Widget 根据格子数组更新图标和数量。
6. `OnInventoryFull` 触发时，HUD Widget 显示“背包已满”提示。

C++ 不直接绑定 HUD 里的 Image 或 TextBlock 名字，避免限制你的 UI 布局。

## 插件文件计划

预计创建：
- `Inventory.uplugin`
- `Source/Inventory/Inventory.Build.cs`
- `Source/Inventory/Public/Inventory.h`
- `Source/Inventory/Private/Inventory.cpp`
- `Source/Inventory/Public/InventoryTypes.h`
- `Source/Inventory/Public/InventoryComponent.h`
- `Source/Inventory/Private/InventoryComponent.cpp`
- `Source/Inventory/Public/InventoryBlueprintLibrary.h`
- `Source/Inventory/Private/InventoryBlueprintLibrary.cpp`
- `README.md`

## 实施步骤

1. 创建插件描述文件和 Runtime 模块骨架。
2. 定义物品数据和格子数据结构。
3. 实现 `UInventoryComponent` 的添加、减少、查询和事件广播。
4. 实现蓝图辅助函数。
5. 写 `README.md`，说明玩家组件、HUD 绑定和蓝图调用方式。
6. 由你在 UE 里生成项目文件并编译。
7. 编译通过后，在玩家蓝图上添加 `InventoryComponent`。
8. 在 HUD 蓝图里监听事件并刷新你画好的背包栏。

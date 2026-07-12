# Inventory

![Inventory Demo](Shows/Inventory.gif)

Inventory 是一个 UE5 运行时背包插件，用 C++ 提供玩家背包组件、可拾取物品组件和背包 Widget 基类。

适合用于：
- 玩家拾取世界物品。
- 背包栏显示物品图标和数量。
- 每种物品最多堆叠 64 个。
- 背包数据变化后自动刷新 UI。

## 功能

- `InventoryComponent`：挂在玩家身上，保存背包格子数据。
- `InventoryItemComponent`：挂在可拾取物品身上，配置物品 ID、名称、图标和数量。
- `InventoryBarWidget`：背包 Widget 的 C++ 基类，自动刷新图标和数量。
- `InventoryBlueprintLibrary`：提供 `FindInventoryComponent` 辅助函数。

## 安装

把整个 `Inventory` 文件夹放到项目的 `Plugins` 目录：

```text
YourProject/Plugins/Inventory
```

然后重新生成项目文件并编译。

## 玩家接入

在玩家角色上添加：

```cpp
InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
```

如果你希望 C++ 自动创建背包 UI，还需要准备一个继承 `InventoryBarWidget` 的 Widget 蓝图。

## Widget 接入

创建一个 Widget 蓝图，例如：

```text
WBP_InventoryBar
```

把它的 Parent Class 设置为：

```text
InventoryBarWidget
```

格子命名需要按顺序：

```text
WBP_InventorySlot_0
WBP_InventorySlot_1
...
WBP_InventorySlot_7
```

每个格子内部放：

```text
Overlay
 ├─ Image        // 物品图标
 └─ TextBlock    // 数量
```

`InventoryBarWidget` 会自动在每个格子里查找第一个 `Image` 和第一个 `TextBlock`，不需要写蓝图 Graph。

## 物品接入

给可拾取 Actor 添加：

```text
InventoryItemComponent
```

在 Details 面板配置：

- `Item Id`：物品唯一 ID，同类物品必须相同。
- `Display Name`：物品显示名。
- `Icon`：背包里显示的 `Texture2D` 图标。
- `Max Stack Size`：最大堆叠数，硬上限 64。
- `Pickup Quantity`：每次拾取增加的数量。

拾取成功后调用：

```cpp
InventoryComponent->AddItem(ItemData, Quantity);
```

返回 `true` 表示加入成功，返回 `false` 表示物品无效、背包已满或数量超过上限。

## 常用接口

```cpp
bool AddItem(const FInventoryItemData& ItemData, int32 Quantity = 1);
bool RemoveItem(FName ItemId, int32 Quantity = 1);
bool HasItem(FName ItemId, int32 Quantity = 1) const;
int32 GetItemCount(FName ItemId) const;
TArray<FInventorySlotData> GetSlots() const;
void ClearInventory();
```

## 上传 GitHub

建议上传：

- `Inventory.uplugin`
- `Source/`
- `README.md`
- `Shows/`
- `.gitignore`

不要上传：

- `Binaries/`
- `Intermediate/`
- `Saved/`
- `.vs/`
- `DerivedDataCache/`

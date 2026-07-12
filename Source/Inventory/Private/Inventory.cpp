#include "Inventory.h"

#define LOCTEXT_NAMESPACE "FInventoryModule"

void FInventoryModule::StartupModule()
{
	// 插件当前只提供运行时组件，不需要在模块启动时注册额外资源。
}

void FInventoryModule::ShutdownModule()
{
	// 插件当前没有全局缓存资源，模块关闭时保持空实现即可。
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FInventoryModule, Inventory)

using UnrealBuildTool;

// 定义 Inventory 模块依赖，让项目和蓝图可以使用背包数据组件。
public class Inventory : ModuleRules
{
	// 构造模块规则，由 Unreal Build Tool 在生成项目和编译时调用。
	public Inventory(ReadOnlyTargetRules Target) : base(Target)
	{
		// 使用显式或共享预编译头，保持插件编译方式和 UE 常规 C++ 模块一致。
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// 公开依赖供外部模块调用本插件公开头文件时使用。
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UMG"
		});

		// 当前插件第一版只做数据和组件逻辑，暂时不需要私有依赖。
		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}

# BYG Imgui Logger

Displays Unreal's `UE_LOG` output in an ImGui window.

Heavily based on the Console example from imgui_demo.cpp included with ImGui.

Features:
* Filter by verbosity: `Error`, `Warning`, `Display` etc.
* Filter by log category: `LogTemp`, `Warning`, `Display` etc.
* 
## Usage

You will need to draw the logger once every tick. You can try putting it in a `APlayerController` subclass and calling it there. Something like this:

```cpp
if (ImGui::BeginMainMenuBar())
{
	if (ImGui::BeginMenu("Window"))
	{
		if (ImGui::MenuItem("Log"))
		{
			bShowLogWindow = !bShowLogWindow;
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

if (bShowLogWindow)
{
	UBYGImguiLoggerSubsystem* Logger = GetGameInstance()->GetSubsystem<UBYGImguiLoggerSubsystem>();
	Logger->Draw(&bShowLogWindow);
}
```

## Unreal Version Support

* Unreal Engine 4.27, 5.0EA

## Requirements

* [UnrealImGui](https://github.com/segross/UnrealImGui)

## License

* [3-clause BSD license](LICENSE)

## Contact

* Created and maintained by [@_benui](https://twitter.com/_benui) at [Brace Yourself Games](https://braceyourselfgames.com/)



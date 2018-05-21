// ImGui Test on Nintendo Switch

#include <switch.h>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_sw.h"

int main(int argc, char* argv[])
{
	int width, height;
	u32 *framebuf;

	gfxInitDefault();
	framebuf = (u32*) gfxGetFramebuffer((u32*)&width, (u32*)&height);

	std::vector<uint32_t> pixel_buffer(width * height, 0);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)width, (float)height);
 	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	imgui_sw::bind_imgui_painting();
	imgui_sw::SwOptions sw_options;
	imgui_sw::make_style_fast();

	while (appletMainLoop()) {
		hidScanInput();
		u32 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
		JoystickPosition pos;
		hidJoystickRead(&pos, CONTROLLER_P1_AUTO, JOYSTICK_LEFT);

		if (kHeld & KEY_PLUS) break;

		memset(io.NavInputs, 0, sizeof(io.NavInputs));
		#define MAP_BUTTON(NAV, BUTTON)       { if (kHeld & BUTTON) io.NavInputs[NAV] = 1.0f; }
		MAP_BUTTON(ImGuiNavInput_Activate,    KEY_A);
		MAP_BUTTON(ImGuiNavInput_Cancel,      KEY_B);
		MAP_BUTTON(ImGuiNavInput_Menu,        KEY_Y);
		MAP_BUTTON(ImGuiNavInput_Input,       KEY_X);
		MAP_BUTTON(ImGuiNavInput_DpadLeft,    KEY_DLEFT);
		MAP_BUTTON(ImGuiNavInput_DpadRight,   KEY_DRIGHT);
		MAP_BUTTON(ImGuiNavInput_DpadUp,      KEY_DUP);
		MAP_BUTTON(ImGuiNavInput_DpadDown,    KEY_DDOWN);
		MAP_BUTTON(ImGuiNavInput_FocusPrev,   KEY_L);
		MAP_BUTTON(ImGuiNavInput_FocusNext,   KEY_R);
		MAP_BUTTON(ImGuiNavInput_TweakSlow,   KEY_L);
		MAP_BUTTON(ImGuiNavInput_TweakFast,   KEY_R);
		MAP_BUTTON(ImGuiNavInput_LStickLeft,  KEY_LSTICK_LEFT);
		MAP_BUTTON(ImGuiNavInput_LStickRight, KEY_LSTICK_RIGHT);
		MAP_BUTTON(ImGuiNavInput_LStickUp,    KEY_LSTICK_UP);
		MAP_BUTTON(ImGuiNavInput_LStickDown,  KEY_LSTICK_DOWN);
		#undef MAP_BUTTON
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

		io.DeltaTime = 1.0f / 60.0f;

		ImGui::NewFrame();
		ImGui::ShowDemoWindow(NULL);
		ImGui::Render();

		// fill gray (this could be any previous rendering)
		std::fill_n(pixel_buffer.data(), width * height, 0x19191919u);

		// overlay the GUI
		paint_imgui(pixel_buffer.data(), width, height, sw_options);

		// draw to screen
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				u32 pos = i * width + j;
				framebuf[pos] = pixel_buffer.data()[pos];
			}

		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
	}

	imgui_sw::unbind_imgui_painting();
	ImGui::DestroyContext();
	gfxExit();

	return 0;
}

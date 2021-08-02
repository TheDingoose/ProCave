#pragma once
#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "Rendering/DevUIDriver.h"

class TestUI : public DevUI
{
public:
	TestUI() {
		Name = "Testo";
	};

	void Draw() override {

		ImGui::Begin("My First Tool");
		ImGui::Text("This is some useful text.");
		if (ImGui::Button("TestReturn", ImVec2(50, 25))) {
			spdlog::info("Hello World!");
		}

		ImGui::End();
		return;
	};
};

void SetupSpdlog() {
	if (AllocConsole())
	{
		HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// Increase screen buffer to allow more lines of text than the default.
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(lStdHandle, &consoleInfo);
		consoleInfo.dwSize.Y = 20;
		SetConsoleScreenBufferSize(lStdHandle, consoleInfo.dwSize);
		SetConsoleCursorPosition(lStdHandle, { 0, 0 });

		// Redirect unbuffered STDOUT to the console.
		int hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0);
		FILE* fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stdout);
		setvbuf(stdout, nullptr, _IONBF, 0);

		// Redirect unbuffered STDIN to the console.
		lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0);
		fp = _fdopen(hConHandle, "r");
		freopen_s(&fp, "CONIN$", "r", stdin);
		setvbuf(stdin, nullptr, _IONBF, 0);

		// Redirect unbuffered STDERR to the console.
		lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0);
		fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stderr);
		setvbuf(stderr, nullptr, _IONBF, 0);

		// Clear the error state for each of the C++ standard stream objects. We
	   // need to do this, as attempts to access the standard streams before
	   // they refer to a valid target will cause the iostream objects to enter
	   // an error state. In versions of Visual Studio after 2005, this seems
	   // to always occur during startup regardless of whether anything has
	   // been read from or written to the console or not.
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
	}

	DevUIDriver::get()->Windows.push_back(new TestUI());
}



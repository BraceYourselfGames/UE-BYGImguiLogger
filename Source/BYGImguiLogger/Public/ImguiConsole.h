// Copyright Brace Yourself Games. All Rights Reserved.

// Taken from imgui_demo.cpp
#pragma once

#include "imgui.h"
#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"
// Merge icons into default tool font
#include "IconsForkAwesome.h"
struct FBYGAppConsole
{
	static int Stricmp(const char* str1, const char* str2)
	{
		int d;
		while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
		{
			str1++;
			str2++;
		}
		return d;
	}

	static int Strnicmp(const char* str1, const char* str2, int n)
	{
		int d = 0;
		while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
		{
			str1++;
			str2++;
			n--;
		}
		return d;
	}

	static char* Strdup(const char* str)
	{
		size_t len = strlen(str) + 1;
		void* buf = malloc(len);
		IM_ASSERT(buf);
		return (char*)memcpy(buf, (const void*)str, len);
	}

	static void Strtrim(char* str)
	{
		char* str_end = str + strlen(str);
		while (str_end > str && str_end[-1] == ' ')
			str_end--;
		*str_end = 0;
	}

	struct LogItem
	{
		ELogVerbosity::Type Verbosity = ELogVerbosity::NoLogging;
		char* Category = nullptr;
		char* Message = nullptr;

		LogItem(ELogVerbosity::Type InVerbosity, const char* InCategory, char* InMessage)
		{
			Verbosity = InVerbosity;
			Category = Strdup(InCategory);
			Message = Strdup(InMessage);
		}

		~LogItem()
		{
			//free(Category);
			//free(Message);
		}
	};

	char InputBuf[256];
	ImVector<LogItem> Items;
	ImVector<const char*> Commands;
	ImVector<char*> History;
	ImVector<char*> Categories;
	int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter Filter;
	bool AutoScroll;
	bool ScrollToBottom;
	bool bShowError = true;
	bool bShowWarning = true;
	bool bShowLog = true;
	bool bShowVerbose = false;
	int32 LongestCategoryName = 8;
	int32 LongestVerbosityName = 8;

	TMap<ELogVerbosity::Type, FString> StrForVerbosity;
	TMap<ELogVerbosity::Type, bool> FilterForVerbosity;
	TMap<ELogVerbosity::Type, ImVec4> ColorForVerbosity;
	TMap<FString, bool> FilterForCategory;

	FBYGAppConsole()
	{
		ClearLog();
		memset(InputBuf, 0, sizeof(InputBuf));
		HistoryPos = -1;
		Commands.push_back("HELP");
		Commands.push_back("HISTORY");
		Commands.push_back("CLEAR");
		AutoScroll = true;
		ScrollToBottom = false;

		StrForVerbosity = {
			{ELogVerbosity::Fatal, "Fatal"},
			{ELogVerbosity::Error, "Error"},
			{ELogVerbosity::Warning, "Warning"},
			{ELogVerbosity::Display, "Display"},
			{ELogVerbosity::Log, "Log"},
			{ELogVerbosity::Verbose, "Verbose"},
			{ELogVerbosity::VeryVerbose, "VeryVerbose"}
		};
		LongestVerbosityName = 0;
		for (const auto& Pair : StrForVerbosity )
		{
		if (Pair.Value.Len() > LongestVerbosityName)
		LongestVerbosityName = Pair.Value.Len();
		}
		FilterForVerbosity = {
			{ELogVerbosity::Fatal, true},
			{ELogVerbosity::Error, true},
			{ELogVerbosity::Warning, true},
			{ELogVerbosity::Display, true},
			{ELogVerbosity::Log, true},
			{ELogVerbosity::Verbose, false},
			{ELogVerbosity::VeryVerbose, false}
		};
		ColorForVerbosity = {
			 { ELogVerbosity::Fatal, ImVec4(1.0f, 33/255.f, 111/255.f, 1.0f)},
			 { ELogVerbosity::Error, ImVec4(1.0f, 42/255.f, 42/255.f, 1.0f)},
			 { ELogVerbosity::Warning, ImVec4(1.0f, 222/255.f, 42/255.f, 1.0f)},
			 { ELogVerbosity::Display, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)},
			 { ELogVerbosity::Log, ImVec4(0.9f, 0.9f, 0.9f, 1.0f)},
			 { ELogVerbosity::Verbose, ImVec4(0.6f, 0.6f, 0.6f, 1.0f)},
			 { ELogVerbosity::VeryVerbose, ImVec4(0.4f, 0.4f, 0.4f, 1.0f)}
		};
		
	}

	~FBYGAppConsole()
	{
		ClearLog();
		for (int i = 0; i < History.Size; i++)
			free(History[i]);
	}

	void InitImgui()
	{
#if 0
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontDefault();

		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
		static const ImWchar icon_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
		const FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("BYGImguiLogger"))->GetContentDir();
		const FString FullRelPath = FPaths::Combine(ContentDir, TEXT("forkawesome-webfont.ttf"));
		const FString FullPath = FPaths::ConvertRelativePathToFull(FullRelPath);
		//io.Fonts->AddFontFromFileTTF(TCHAR_TO_ANSI(*FullPath), 13.0f, &config, icon_ranges);
#endif
	}


	// Portable helpers
	void ClearLog()
	{
		for (int i = 0; i < Items.Size; i++)
		{
			free(Items[i].Category);
			free(Items[i].Message);
		}
		Items.clear();
	}

	void AddLog(ELogVerbosity::Type Verbosity, const char* Category, const char* fmt, ...)
	IM_FMTARGS(4)
	{
		// FIXME-OPT
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);
		if (!FilterForCategory.Contains(Category))
		{
			const int32 CategoryLen = strlen(Category);
			FilterForCategory.Add(Category, true);
			if (CategoryLen > LongestCategoryName)
			{
				LongestCategoryName = CategoryLen;
			}
		}
		Items.push_back(LogItem(Verbosity, Category, buf));
	}

	char* GetVerbosityText(ELogVerbosity::Type Verbosity) const
	{
		return TCHAR_TO_ANSI(*StrForVerbosity[Verbosity]);
	}

	void Draw(const char* title, bool* p_open)
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
		// Here we create a context menu only available from the title bar.
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Close Console"))
				*p_open = false;
			ImGui::EndPopup();
		}

		//ImGui::TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
		//ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

		if (ImGui::BeginPopup("Verbosity"))
		{
			bool bAllOn = true;
			for (const auto& Pair: FilterForVerbosity)
			{
				if (!Pair.Value)
				{
					bAllOn = false;
					break;
				}
			}
			if (ImGui::Checkbox("All", &bAllOn))
			{
				for (auto& Pair: FilterForVerbosity)
				{
					Pair.Value = bAllOn;
				}
			}
			ImGui::Separator();
			for (const auto& Pair: StrForVerbosity)
			{
				ImGui::Checkbox(TCHAR_TO_ANSI(*Pair.Value), &FilterForVerbosity[Pair.Key]);
			}
			ImGui::EndPopup();
		}
		if (ImGui::Button("Verbosity..."))
			ImGui::OpenPopup("Verbosity");

		ImGui::SameLine();
		
		if (ImGui::BeginPopup("Category"))
		{
			bool bAllOn = true;
			for (const auto& Pair: FilterForCategory)
			{
				if (!Pair.Value)
				{
					bAllOn = false;
					break;
				}
			}
			if (ImGui::Checkbox("All", &bAllOn))
			{
				for (auto& Pair: FilterForCategory)
				{
					Pair.Value = bAllOn;
				}
			}
			ImGui::Separator();
			for (auto& Pair: FilterForCategory)
			{
				ImGui::Checkbox(TCHAR_TO_ANSI(*Pair.Key), &Pair.Value);
			}
			ImGui::EndPopup();
		}
		if (ImGui::Button("Category..."))
			ImGui::OpenPopup("Category");
			
		ImGui::SameLine();

		// Options, Filter
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
		ImGui::SameLine();
		if (ImGui::Button("Clear")) { ClearLog(); }
		ImGui::SameLine();
		bool copy_to_clipboard = ImGui::Button(ICON_FK_FILES_O "Copy");
		ImGui::Separator();

		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();             // 1 separator, 1 input text
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear"))
				ClearLog();
			ImGui::EndPopup();
		}

		// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
		// NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
		// You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
		// To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
		//     ImGuiListClipper clipper(Items.Size);
		//     while (clipper.Step())
		//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		// However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
		// A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
		// and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
		// If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
		ImGui::Columns(3);
		// HACK: Only want to call SetColumnWidth one time...
		static float Col0Spacing = 80;
		if (Col0Spacing > 0)
		{
			ImGui::SetColumnWidth(0, Col0Spacing);
			Col0Spacing = 0;
		}
		static float Col1Spacing = 120;
		if (Col1Spacing > 0)
		{
			ImGui::SetColumnWidth(1, Col1Spacing);
			Col1Spacing = 0;
		}
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		if (copy_to_clipboard)
			ImGui::LogToClipboard();
		for (int i = 0; i < Items.Size; i++)
		{
			const char* message = Items[i].Message;
			if (!Filter.PassFilter(message))
				continue;

			const bool* bCategoryFilterEnabled = FilterForCategory.Find(Items[i].Category);
			if (bCategoryFilterEnabled == nullptr || *bCategoryFilterEnabled == false)
				continue;
			
			const bool* bVerbosityFilterEnabled = FilterForVerbosity.Find(Items[i].Verbosity);
			if (bVerbosityFilterEnabled == nullptr || *bVerbosityFilterEnabled == false)
				continue;

			// Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
			ImGui::PushStyleColor(ImGuiCol_Text, ColorForVerbosity[Items[i].Verbosity]);
			//ImGui::Text("%*s %*s %s", LongestVerbosityName, TCHAR_TO_ANSI(*StrForVerbosity[Items[i].Verbosity]), LongestCategoryName, Items[i].Category, Items[i].Message);
			ImGui::TextUnformatted(TCHAR_TO_ANSI(*StrForVerbosity[Items[i].Verbosity]));
			ImGui::NextColumn();
			ImGui::TextUnformatted(Items[i].Category);
			ImGui::NextColumn();
			ImGui::TextUnformatted(Items[i].Message);
			ImGui::NextColumn();
			ImGui::PopStyleColor();
		}
		if (copy_to_clipboard)
			ImGui::LogFinish();
		ImGui::Columns(1);

		if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();

		// Command-line
		bool reclaim_focus = false;
		if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
		{
			char* s = InputBuf;
			Strtrim(s);
			if (s[0])
				ExecCommand(s);
			strcpy(s, "");
			reclaim_focus = true;
		}

		// Auto-focus on window apparition
		ImGui::SetItemDefaultFocus();
		if (reclaim_focus)
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

		ImGui::End();
	}

	void ExecCommand(const char* command_line)
	{
		AddLog(ELogVerbosity::Verbose, "Command", "# %s\n", command_line);

		// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
		HistoryPos = -1;
		for (int i = History.Size - 1; i >= 0; i--)
			if (Stricmp(History[i], command_line) == 0)
			{
				free(History[i]);
				History.erase(History.begin() + i);
				break;
			}
		History.push_back(Strdup(command_line));

		// Process command
		if (Stricmp(command_line, "CLEAR") == 0)
		{
			ClearLog();
		}
		else if (Stricmp(command_line, "HELP") == 0)
		{
			AddLog(ELogVerbosity::Verbose, "Command", "Commands:");
			for (int i = 0; i < Commands.Size; i++)
				AddLog(ELogVerbosity::Verbose, "Command", "- %s", Commands[i]);
		}
		else if (Stricmp(command_line, "HISTORY") == 0)
		{
			int first = History.Size - 10;
			for (int i = first > 0 ? first : 0; i < History.Size; i++)
				AddLog(ELogVerbosity::Verbose, "Command", "%3d: %s\n", i, History[i]);
		}
		else
		{
			AddLog(ELogVerbosity::Verbose, "Command", "Unknown command: '%s'\n", command_line);
		}

		// On commad input, we scroll to bottom even if AutoScroll==false
		ScrollToBottom = true;
	}

	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
	{
		FBYGAppConsole* console = (FBYGAppConsole*)data->UserData;
		return console->TextEditCallback(data);
	}

	int TextEditCallback(ImGuiInputTextCallbackData* data)
	{
		//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
		switch (data->EventFlag)
		{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			// Example of TEXT COMPLETION

			// Locate beginning of current word
			const char* word_end = data->Buf + data->CursorPos;
			const char* word_start = word_end;
			while (word_start > data->Buf)
			{
				const char c = word_start[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';')
					break;
				word_start--;
			}

			// Build a list of candidates
			ImVector<const char*> candidates;
			for (int i = 0; i < Commands.Size; i++)
				if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
					candidates.push_back(Commands[i]);

			if (candidates.Size == 0)
			{
				// No match
				AddLog(ELogVerbosity::Verbose, "Command", "No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
			}
			else if (candidates.Size == 1)
			{
				// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0]);
				data->InsertChars(data->CursorPos, " ");
			}
			else
			{
				// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
				int match_len = (int)(word_end - word_start);
				for (;;)
				{
					int c = 0;
					bool all_candidates_matches = true;
					for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
						if (i == 0)
							c = toupper(candidates[i][match_len]);
						else if (c == 0 || c != toupper(candidates[i][match_len]))
							all_candidates_matches = false;
					if (!all_candidates_matches)
						break;
					match_len++;
				}

				if (match_len > 0)
				{
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
				}

				// List matches
				AddLog(ELogVerbosity::Verbose, "Command", "Possible matches:\n");
				for (int i = 0; i < candidates.Size; i++)
					AddLog(ELogVerbosity::Verbose, "Command", "- %s\n", candidates[i]);
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (HistoryPos == -1)
					HistoryPos = History.Size - 1;
				else if (HistoryPos > 0)
					HistoryPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (HistoryPos != -1)
					if (++HistoryPos >= History.Size)
						HistoryPos = -1;
			}

			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != HistoryPos)
			{
				const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str);
			}
		}
		}
		return 0;
	}
};

/*
	utils.h
	Programmer: Khoi Ho
	Credits: 
		The Cherno for the "Save File" and "Open File" dialog (https://www.youtube.com/watch?v=zn7N7zHgCcs&ab_channel=TheCherno)
		SparkyRobinson for the "Browse Folder" dialog (https://stackoverflow.com/questions/12034943/win32-select-directory-dialog-from-c-c)
*/

#pragma once

#include <string>
#include <vector>
#include <SDL.h>
#include <Windows.h>

// Open the "Save File" dialog. Return the file path.
std::string SaveFileDialog(SDL_Window* window, const char* filter);

// Open the "Open File" dialog. Return the file path.
std::string OpenFileDialog(SDL_Window* window, const char* filter);

// Open the "Browse Folder" dialog. Return the folder path.
std::string BrowseFolder(const std::string& saved_path);

// Get all files in a folder.
std::vector<std::string> GetAllFileNames(const std::string& templateFolderPath, const std::string& filter = "*.*");
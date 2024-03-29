// $1 Single-Stroke Gesture Recognizer.
// Programmer: Khoi Ho

// Fix error C2338.
#define WINDOWS_IGNORE_PACKING_MISMATCH

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include "SDL_gpu.h"
#include "SDL_syswm.h"
#include "NFont_gpu.h"
#include "Random.h"
#include "Vector2.h"
#include "Stroke.h"

// Open the stroke file and read the strokes.
void OpenStrokeFile(const std::string& fileName, std::vector<Stroke>& strokes);

// Save the strokes to a file. Return true is the file is successfully saved.
bool SaveStrokesToFile(const std::string& fileName, const std::vector<Stroke>& strokes);

// Switch from main window to console window. Need the path of the executable.
void SwitchToConsoleWindow(const char* programPath);

// Switch from console window to main window. Need the reference to the SDL_Window.
void SwitchToMainWindow(SDL_Window* window);

int main(int argc, char* argv[])
{
	const char* WINDOW_TITLE = "$1 Single-Stroke Gesture Recognizer";
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const char* FONT_FILENAME = "FreeSans.ttf";
	const int FONT_SIZE = 14;
	const std::string STROKE_FILENAME = "mystrokes.txt";

	// Initialize SDL_GPU.
	GPU_Target* screen = GPU_Init(SCREEN_WIDTH, SCREEN_HEIGHT, GPU_DEFAULT_INIT_FLAGS);
	if (screen == nullptr)
		return 1;

	// Create a window.
	SDL_SetWindowTitle(SDL_GetWindowFromID(screen->context->windowID), WINDOW_TITLE);
	
	// Load a font to display text on screen.
	NFont font;
	font.load(FONT_FILENAME, FONT_SIZE);

	// Coordinates of the mouse.
	int mouseX = 0, mouseY = 0;

	bool isDrawing = false;

	Stroke drawnStroke;

	std::vector<Stroke> strokes;
	OpenStrokeFile(STROKE_FILENAME, strokes);

	SDL_Event event;
	bool done = false;
	while (!done)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				done = true;

			// Hold left mouse to draw.
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					drawnStroke.points.clear();
					isDrawing = true;
				}
			}

			// Release left mouse to stop drawing.
			if (event.type == SDL_MOUSEBUTTONUP)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
					isDrawing = false;
			}

			if (event.type == SDL_KEYDOWN)
			{
				// Don't process key press when the user is still drawing.
				if (!isDrawing)
				{
					// Press ESC to exit the program.
					if (event.key.keysym.sym == SDLK_ESCAPE)
						done = true;

					// Press S to save the stroke.
					if (event.key.keysym.sym == SDLK_s)
					{
						SwitchToConsoleWindow(argv[0]);

						// Cannot save the drawn stroke if it is too short.
						if (drawnStroke.points.size() < 10)
							std::cout << "Cannot save the stroke: The stroke is too short." << std::endl;

						else
						{
							system("cls");

							// Ask the user the enter the name of the stroke.
							while (true)
							{
								std::cout << "Enter the name of the stroke: ";

								if (getline(std::cin, drawnStroke.name))
									break;

								else
								{
									std::cout << "Invalid name. ";
									std::cin.clear();
									std::cin.ignore(100, '\n');
								}
							}

							// Add the stroke to the stroke vector.
							strokes.push_back(drawnStroke);

							// Sort the strokes.
							sort(strokes.begin(), strokes.end());

							// Save the strokes.
							bool canSave = SaveStrokesToFile("mystrokes.txt", strokes);

							if (canSave)
								std::cout << "The stroke \"" << drawnStroke.name << "\" has been successfully saved to " << STROKE_FILENAME << std::endl;
						}

						SwitchToMainWindow(SDL_GetWindowFromID(screen->context->windowID));
					}

					// Press C to clear the drawn stroke.
					if (event.key.keysym.sym == SDLK_c)
					{
						drawnStroke.points.clear();
					}

					// Press R to recognize the drawn stroke.
					if (event.key.keysym.sym == SDLK_r)
					{
						// Cannot recognize the drawn stroke if it is too short.
						if (drawnStroke.points.size() < 10)
							std::cout << "The stroke is too short." << std::endl;

						else
						{
							if (strokes.size() == 0)
								std::cout << "There is no saved stroke." << std::endl;

							else
							{
								// Process the drawn stroke.
								Stroke drawnStrokeCopy = drawnStroke;
								drawnStrokeCopy = drawnStrokeCopy.Resample();
								drawnStrokeCopy = drawnStrokeCopy.RotateBy(-drawnStrokeCopy.GetIndicativeAngle());
								drawnStrokeCopy = drawnStrokeCopy.ScaleTo();
								drawnStrokeCopy = drawnStrokeCopy.TranslateTo();

								// Process the saved strokes.
								std::vector<Stroke> strokesCopy(strokes);
								for (Stroke& stroke : strokesCopy)
								{
									stroke = stroke.Resample();
									stroke = stroke.RotateBy(-stroke.GetIndicativeAngle());
									stroke = stroke.ScaleTo();
									stroke = stroke.TranslateTo();
								}

								// Recognize the stroke.
								Stroke matchingStroke;
								float score;
								drawnStrokeCopy.Recognize(strokesCopy, 250, matchingStroke, score);

								// Display the matching stroke and the score.
								std::stringstream matchingStrokeSS;
								matchingStrokeSS << std::fixed << std::setprecision(2);
								matchingStrokeSS << matchingStroke.name << " (Score = " << score << ")" << std::endl;
								SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Matching Stroke", matchingStrokeSS.str().c_str(), SDL_GetWindowFromID(screen->context->windowID));
							}
						}
					}

					// Press V to view an existing stroke.
					if (event.key.keysym.sym == SDLK_v)
					{
						SwitchToConsoleWindow(argv[0]);

						system("cls");

						// Display the saved strokes.
						std::cout << "List of saved strokes: " << std::endl;
						for (const Stroke& stroke : strokes)
							std::cout << "\t" << stroke.name << std::endl;
						std::cout << std::endl;

						// Ask the user to enter which stroke they want to view.
						std::string strokeToView;
						while (true)
						{
							std::cout << "Enter the name of the stroke to view: ";

							if (getline(std::cin, strokeToView))
							{
								break;
							}
							else
							{
								std::cout << "Invalid name. ";
								std::cin.clear();
								std::cin.ignore(100, '\n');
							}
						}

						// Find the stroke.
						bool strokeFound = false;
						for (int i = 0; i < strokes.size(); ++i)
						{
							if (strokes[i].name == strokeToView)
							{
								drawnStroke = strokes[i];
								strokeFound = true;
								break;
							}
						}

						if (strokeFound)
							std::cout << "Viewing the stroke \"" << drawnStroke.name << "\"." << std::endl;
						else
							std::cout << "Cannot view the stroke \"" << strokeToView << "\": The stroke does not exist." << std::endl;

						SwitchToMainWindow(SDL_GetWindowFromID(screen->context->windowID));
					}

					// Press D to delete a save stroke.
					if (event.key.keysym.sym == SDLK_d)
					{
						SwitchToConsoleWindow(argv[0]);

						system("cls");

						// Display the saved strokes.
						std::cout << "List of saved strokes: " << std::endl;
						for (const Stroke& stroke : strokes)
							std::cout << "\t" << stroke.name << std::endl;
						std::cout << std::endl;

						// Ask the user to enter which stroke they want to delete.
						std::string strokeToDelete;
						while (true)
						{
							std::cout << "Enter the name of the stroke to delete: ";

							if (getline(std::cin, strokeToDelete))
							{
								break;
							}
							else
							{
								std::cout << "Invalid name. " << std::endl;
								std::cin.clear();
								std::cin.ignore(100, '\n');
							}
						}

						// Find and delete the strokes that match the name.
						int i = 0;
						while (i < strokes.size())
						{
							if (strokes[i].name == strokeToDelete)
								strokes.erase(strokes.begin() + i);
							else
								++i;
						}

						std::cout << "\"" << strokeToDelete << "\" has been removed from " << STROKE_FILENAME << std::endl;

						SwitchToMainWindow(SDL_GetWindowFromID(screen->context->windowID));
					}

					// Press T to resample the drawn stroke.
					if (event.key.keysym.sym == SDLK_t)
					{
						drawnStroke = drawnStroke.Resample();
						drawnStroke = drawnStroke.RotateBy(-drawnStroke.GetIndicativeAngle());
						drawnStroke = drawnStroke.ScaleTo();
						drawnStroke = drawnStroke.TranslateTo(Vector2(SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 2));
					}
				}
			}
		}

		// Get the current position of the mouse.
		SDL_GetMouseState(&mouseX, &mouseY);

		// Draw onto screen if the user is holding the mouse button.
		if(isDrawing)
		{
			Vector2 mousePosition(mouseX, mouseY);

			// Don't at the latest cursor position to the array if it is not moving.
			if (drawnStroke.points.size() == 0 || mousePosition != drawnStroke.points[drawnStroke.points.size() - 1])
				drawnStroke.points.push_back(mousePosition);
		}

		GPU_ClearRGB(screen, 255, 255, 255);

		// Draw the circle marking the first point.
		if (drawnStroke.points.size() > 0)
			GPU_Circle(screen, drawnStroke.points[0].x, drawnStroke.points[0].y, 5.0f, GPU_MakeColor(0, 0, 255, 255));

		// Draw lines.
		GPU_SetLineThickness(5.0f);
		for (int i = 1; i < drawnStroke.points.size(); ++i)
		{
			Vector2 currentPoint = drawnStroke.points[i-1];
			Vector2 nextPoint = drawnStroke.points[i];
			GPU_Line(screen, currentPoint.x, currentPoint.y, nextPoint.x, nextPoint.y, GPU_MakeColor(255, 0, 0, 255));
		}
		GPU_SetLineThickness(1.0f);

		// Draw the circle marking the last point.
		if (drawnStroke.points.size() > 0)
		{
			const int lastPoint = drawnStroke.points.size() - 1;
			GPU_Circle(screen, drawnStroke.points[lastPoint].x, drawnStroke.points[lastPoint].y, 5.0f, GPU_MakeColor(0, 122, 0, 255));
		}

		font.draw(screen, screen->w - 50.0f, 10.0f, NFont::AlignEnum::RIGHT, 
			"Left click: Draw a stroke\n"
			"C: Clear the stroke\n"
			"R: Recognize the stroke\n"
			"S: Save the stroke\n"
			"V: View an existing stroke\n"
			"D: Delete a saved stroke\n"
		    "T: Resample the stroke\n"
		);

		GPU_Flip(screen);

		SDL_Delay(1);
	}

	font.free();

	GPU_Quit();

	return 0;
}

void OpenStrokeFile(const std::string& fileName, std::vector<Stroke>& strokes)
{
	strokes.clear();

	std::fstream inputFile(fileName, std::ifstream::in);

	// If the file has not existed, create a new one.
	if (!inputFile)
	{
		std::cerr << "Cannot open the file " << fileName << ". Creating a new one..." << std::endl;

		std::fstream outputFile(fileName, std::ofstream::out);
		if (outputFile)
		{
			outputFile << 0 << std::endl;
			outputFile.close();
		}

		inputFile.close();

		return;
	}

	// Read the number of strokes.
	int numStrokes;
	inputFile >> numStrokes;
	std::cout << "Number of strokes in the file: " << numStrokes << std::endl;

	strokes.resize(numStrokes);

	for (int i = 0; i < numStrokes; ++i)
	{
		// Read the name of the stroke.
		inputFile.ignore(100, '\n');
		getline(inputFile, strokes[i].name);

		// Read the number of points.
		int numPoints;
		inputFile >> numPoints;

		strokes[i].points.resize(numPoints);

		// Read the points.
		for (int j = 0; j < numPoints; ++j)
		{
			inputFile >> strokes[i].points[j].x >> strokes[i].points[j].y;
		}

		std::cout << "Read the stroke:\t" << strokes[i].name << "\t(Size = " << strokes[i].points.size() << ")" << std::endl;
	}

	inputFile.close();
}

bool SaveStrokesToFile(const std::string& fileName, const std::vector<Stroke>& strokes)
{
	std::fstream outputFile(fileName, std::ofstream::out | std::ofstream::trunc);
	if (!outputFile)
	{
		std::cerr << "Error: Cannot open the file " << fileName << "." << std::endl;

		outputFile.close();
		return false;
	}

	// Write the number of strokes.
	outputFile << strokes.size() << std::endl;

	for (const Stroke& stroke : strokes)
	{
		// Write the name.
		outputFile << stroke.name << std::endl;

		// Write the number of points.
		outputFile << stroke.points.size() << std::endl;

		// Write the points.
		for (int i = 0; i < stroke.points.size(); ++i)
		{
			outputFile << stroke.points[i].x << "\t" << stroke.points[i].y << std::endl;
		}
	}

	outputFile.close();
	return true;	
}

void SwitchToConsoleWindow(const char* programPath)
{
	HWND hWnd = ::FindWindow(NULL, programPath);
	if (hWnd)
		::SetForegroundWindow(hWnd);
}

void SwitchToMainWindow(SDL_Window* window)
{
	static SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);

	::SetForegroundWindow(wmInfo.info.win.window);
}
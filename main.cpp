/*
Pokemon Screen Reader and Utility Multi Tool
Author: Robert Cichon
Description: A C++ program that is intended to be ran in the background while playing specifically the Pokemon Emerald Battle Tower singles,
and will grab screenshots of the screen while battling to display relevant information to user in real time. This will allow
for the player to conserve time spent using these tools separately.
*/

#include <iostream>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <thread>
#include <chrono>
using namespace std;

cv::Mat captureScreen(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    HDC hdcWindow = GetDC(hwnd);
    HDC hdcMemDC = CreateCompatibleDC(hdcWindow);
    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcWindow, width, height);
    SelectObject(hdcMemDC, hbmScreen);
    BitBlt(hdcMemDC, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative to indicate top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    cv::Mat mat(height, width, CV_8UC3);
    GetDIBits(hdcMemDC, hbmScreen, 0, height, mat.data, &bmi, DIB_RGB_COLORS);

    DeleteObject(hbmScreen);
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcWindow);

    return mat;
}

cv::Mat cropToDialogue(const cv::Mat& screenshot) {
    cv::Rect dialogueBox(242, 670, 900, 211); //Coordinates for the dialogue box area to capture
    return screenshot(dialogueBox);
}

cv::Mat cropToStreakCount(const cv::Mat& screenshot, const std::string& level = "50") { //Currently hardcoded to level 50, can be changed to take either 50 or 100 from user when ready to move beyond 50.
    cv::Rect streakBox;
    if (level == "50") {
        streakBox = cv::Rect(1580, 375, 120, 90); //Coordinates for the streak count area for level 50
    } else if (level == "100") {
        streakBox = cv::Rect(1580, 620, 120, 90); //Coordinates for the streak count area for level 100
    } else {
        cerr << "Invalid level selected for scropToStreakCount()." << endl;
        return cv::Mat();
    }
    return screenshot(streakBox);
}

cv::Mat preprocessImage(const cv::Mat& input) {
    cv::Mat gray, thresh;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::resize(thresh, thresh, cv::Size(), 2, 2, cv::INTER_LINEAR);
    return thresh;
}

string analyzeImage(cv::Mat image) {
    string result;
    tesseract::TessBaseAPI tess;
    if (tess.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY)) {
        cerr << "Error intiializing tesseract" << endl;
        return "";
    }

    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.!?'(),- ");
    tess.SetImage(image.data, image.cols, image.rows, image.channels(), image.step);

    result = tess.GetUTF8Text();
    tess.End();
    return result;
}

void captureLoop(double interval) {
    cv::setNumThreads(1);
    HWND hwnd = FindWindow(NULL, L"4K Capture Utility"); //Ensure window title matches your setup
    if (hwnd == NULL) {
        cerr << "Error: window not found." << endl;
        return;
    }

    vector<string> testImages = {
    "C:/Users/umbre/Documents/Coding for _fun_/C++/PokemonReaderFinal/TestScreenshots/screenshot_49.png",
    "C:/Users/umbre/Documents/Coding for _fun_/C++/PokemonReaderFinal/TestScreenshots/screenshot_100.png",
    "C:/Users/umbre/Documents/Coding for _fun_/C++/PokemonReaderFinal/TestScreenshots/screenshot_300.png"
    };

    for (int i = 0; i < testImages.size(); ++i) {
        cv::Mat img = cv::imread(testImages[i]);
        if (img.empty()) {
            cerr << "Error loading: " << testImages[i] << endl;
            continue;
        }
        cv::Mat foundDialogue = preprocessImage(cropToDialogue(img));
        //cout << "Dialogue Test " << i + 1 << " Text: \n" << analyzeImage(foundDialogue) << endl;

        cv::Mat foundStreak = preprocessImage(cropToStreakCount(img));
        //cout << "Streak Test " << i + 1 << " Text: \n" << analyzeImage(foundStreak) << endl;

    }



	//Proper loop, currently commented out to prevent infinite loop during testing
    /*
     while (true) {
        cv::Mat screenshot = captureScreen(hwnd);
        if (!screenshot.empty()){
            string filename = "C:/Users/umbre/Documents/Coding for _fun_/C++/PokemonReader/TestScreenshots/screenshot.png"; //No longer proper path, change to within program directory and have it overwrite the same screenshot after processing.
            cv::imwrite(filename, screenshot);
        }
        else {
            cerr << "Error: Screenshot capture failed." << endl;
        }
		foubDialogue = cropToDialogue(screenshot);
        string foundText = analyzeImage(foundDialogue);
        cout << foundText << endl;


		// Sleep for the specified interval
        this_thread::sleep_for(chrono::milliseconds(static_cast<int>(interval * 1000)));

		//TODO: Implement proper exit condition for when the program should stop capturing screenshots.
    } */

}



int main() {
    double interval = 0.333; //Timing to adjust for faster or slower screenshots
    captureLoop(interval);

    return 0;
}
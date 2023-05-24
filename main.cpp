#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <opencv2/core.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
#include "portable-file-dialogs.h"

namespace fs = std::filesystem;

const short MAX_THREADS = 5;

void processImage(std::pair<std::string, std::string> &images, std::string &outputPath);

int main() {
    std::cout << "============================" << std::endl;
    std::cout << "Welcome to" << std::endl;
    std::cout << " GGGG   EEEEE  IIIII  M   M" << std::endl;
    std::cout << "G       E        I    MM MM" << std::endl;
    std::cout << "G   GG  EEE      I    M M M" << std::endl;
    std::cout << "G    G  E        I    M   M" << std::endl;
    std::cout << " GGGG   EEEEE  IIIII  M   M" << std::endl;
    std::cout << "Google  Earth  Image  Merger" << std::endl;
    std::cout << "============================" << std::endl;

    std::string leftPath = pfd::select_folder("Select folder with images with watermark on left side", "").result();
    std::string rightPath = pfd::select_folder("Select folder with images with watermark on right side", "").result();

    fs::directory_iterator leftDir(leftPath);
    fs::directory_iterator rightDir(rightPath);
    std::vector<std::string> leftFiles;
    std::vector<std::string> rightFiles;

    short leftCount = 0;
    for (const auto &entry: leftDir) {
        if (entry.path().extension() == ".jpeg") {
            leftCount++;
            leftFiles.push_back(entry.path().string());
        }
    }

    short rightCount = 0;
    for (const auto &entry: rightDir) {
        if (entry.path().extension() == ".jpeg") {
            rightCount++;
            rightFiles.push_back(entry.path().string());
        }
    }

    if (leftCount != rightCount) {
        std::cout << "Different number of images, can't proceed." << std::endl;
        return 0;
    }

    std::string outputPath = pfd::select_folder("Select folder where to save the results", "").result();

    std::vector<std::pair<std::string, std::string>> images;
    std::transform(leftFiles.begin(), leftFiles.end(), rightFiles.begin(), std::back_inserter(images),
                   [](const auto &left, const auto &right) {
                       return std::pair<std::string, std::string>(left, right);
                   });

    std::vector<std::thread> threads;

    std::cout << "Image folder with watermark on left side: " << leftPath << std::endl;
    std::cout << "Image folder with watermark on right side: " << rightPath << std::endl;
    std::cout << "Output folder: " << outputPath << std::endl;
    std::cout << "============================" << std::endl;

    for (auto &image: images) {
        std::cout << "\rProcessing image " << &image - &images[0] + 1 << " of " << images.size() << std::flush;
        threads.emplace_back(processImage, std::ref(image), std::ref(outputPath));
        while (threads.size() >= MAX_THREADS) {
            for (auto it = threads.begin(); it != threads.end();) {
                if (it->joinable()) {
                    it->join();
                    it = threads.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    for (auto &thread: threads) {
        thread.join();
    }

    std::cout << std::endl << "Done!" << std::endl;
    return 0;
}

void processImage(std::pair<std::string, std::string> &images, std::string &outputPath) {
    cv::Mat left = cv::imread(images.first);
    int width = left.cols;
    int height = left.rows;
    cv::Mat cleanLeft = left(cv::Rect(width / 2, 0, width / 2, height));
    cv::Mat right = cv::imread(images.second);
    cv::Mat cleanRight = right(cv::Rect(0, 0, width / 2, height));
    cv::Mat result(height, width, CV_8UC3);
    cleanLeft.copyTo(result(cv::Rect(width / 2, 0, width / 2, height)));
    cleanRight.copyTo(result(cv::Rect(0, 0, width / 2, height)));
    cv::imwrite(outputPath + "/" + images.first.substr(images.first.find_last_of("/\\") + 1), result);
}
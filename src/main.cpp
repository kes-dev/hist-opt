#include <iostream>

#include <chrono>
#include <vector>
#include <numeric>
#include <random>
#include <cmath>
#include "histogram.h"

int main(int argc, char** argv) {
    // record startTime
    auto testStartTime =  std::chrono::steady_clock::now();
    /*** initialise ***/
    // parse commandline arguments
    uint32_t imageWidth = std::atoi(argv[1]);
    uint32_t imageHeight = std::atoi(argv[2]);
    uint32_t imageChannel = std::atoi(argv[3]);
    uint32_t imageBitDepth = std::atoi( argv[4]);
    uint32_t imageCount = std::atoi(argv[5]);
    uint32_t loopCount = std::atoi(argv[6]);
    HistogramCalcMethod methodType = (HistogramCalcMethod) std::atoi(argv[7]);
    uint32_t threadToUse = std::atoi(argv[8]);

    // set up random image values
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<uint16_t> dist((uint16_t) 0, (uint16_t) std::pow(2,imageBitDepth));

    // allocate memory for image
    imageCount = std::min((uint32_t) imageCount, loopCount);
    uint16_t** imageArray = new uint16_t*[imageCount];
    for (int imageIdx = 0; imageIdx < imageCount; imageIdx++) {
        imageArray[imageIdx] = new uint16_t[imageWidth * imageHeight * imageChannel];
        for (int pix = 0; pix < imageWidth * imageHeight * imageChannel; pix++) {
            imageArray[imageIdx][pix] = dist(mt);
        }
    }
    std::uniform_int_distribution<int> imageDist( 0, imageCount - 1);

    // initialise
    Histogram* histogram = new Histogram();
    HistogramConfig histCfg = HistogramConfig {
        imageWidth,
        imageHeight,
        imageChannel,
        imageBitDepth,
        methodType,
        threadToUse
    };
    int initResult = histogram->init(histCfg);

    auto finishInitTime = std::chrono::steady_clock::now();
    std::cout << "finished initialisation" << std::endl;
    std::vector<long> startTime;
    std::vector<long> endTime;
    for (int i = 0; i < loopCount; i++) {
        // randomly choose an image
        int idx = imageDist(mt);
        // Record startTime
        auto tempStartTime =  std::chrono::steady_clock::now();

        // Start processing
        histogram->calcSync(imageArray[idx]);

        // End processing
        // Record endTime
        auto tempEndTime = std::chrono::steady_clock::now();
        startTime.push_back(std::chrono::duration_cast<std::chrono::microseconds>(tempStartTime.time_since_epoch()).count());
        endTime.push_back(std::chrono::duration_cast<std::chrono::microseconds>(tempEndTime.time_since_epoch()).count());
    }

    histogram->uninit();
    // sleep awhile to wait for things get printed etc
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    /*** calculate statistics ***/
    // Mean duration
    std::vector<double> duration;
    for(int i = 0; i < startTime.size(); i++) {
        long tempDuration = endTime[i] - startTime[i];
        duration.push_back((double) tempDuration);
    }

    double totalDuration = std::accumulate(duration.begin(), duration.end(), 0.0);
    double meanDuration = totalDuration / ((double) duration.size());

    std::cout << "meanDuration over " << loopCount << " loop(s): " << meanDuration << " us" << std::endl;

    // Standard deviation of duration
    std::vector<double> squaredDifference;
    for(int i = 0; i < duration.size(); i++) {
        squaredDifference.push_back(std::pow(duration[i] - meanDuration, 2));
    }
    double stDevDuration = std::sqrt(std::accumulate(squaredDifference.begin(),
            squaredDifference.end(), 0.0) / ((double) duration.size()));

    std::cout << "Standard deviation: " << stDevDuration <<  " us" << std::endl;

    // Release memory
    for (int imageIdx = 0; imageIdx < imageCount; imageIdx++) {
        delete[] imageArray[imageIdx];
    }
    delete[] imageArray;

    // Record endTime
    auto testEndTime =  std::chrono::steady_clock::now();
    long testStartTimeEpoch = std::chrono::duration_cast<std::chrono::seconds>(testStartTime.time_since_epoch()).count();
    long testEndTimeEpoch = std::chrono::duration_cast<std::chrono::seconds>(testEndTime.time_since_epoch()).count();
    long finishInitTimeEpoch = std::chrono::duration_cast<std::chrono::seconds>(finishInitTime.time_since_epoch()).count();

    std::cout << "Test duration: " << (testEndTimeEpoch - testStartTimeEpoch) << " s" << std::endl;
    std::cout << "Preparation duration: " << (finishInitTimeEpoch - testStartTimeEpoch) << " s" << std::endl;
    std::cout << "Running duration: " << (testEndTimeEpoch - finishInitTimeEpoch) << " s" << std::endl;

    return 0;
}

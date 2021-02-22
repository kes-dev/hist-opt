//
// Created by kes-dev on 28/12/2019.
//
#include <iostream>
#include <cmath>
#include <atomic>
#include <pthread.h>
#include "histogram.h"

Histogram::Histogram() : endThread(false) {
}

Histogram::~Histogram() {

}

void Histogram::initHistResult(){
    histResult = new std::vector<uint32_t>[histCfg.channel];
    for(int channel = 0; channel < histCfg.channel; channel++) {
        histResult[channel].resize(pow(2, histCfg.bitDepth), 0);
    }
}

int Histogram::init(HistogramConfig histCfg) {
    this->histCfg = histCfg;
    initHistResult();
    // Select function to use
    switch (this->histCfg.method) {
        case SINGLE_1_PER_LOOP:
            calc = &Histogram::calcSingle;
            break;
        case SINGLE_2_PER_LOOP:
            calc = &Histogram::calcSingle2;
            break;
        case SINGLE_4_PER_LOOP:
            calc = &Histogram::calcSingle4;
            break;
        case SINGLE_8_PER_LOOP:
            calc = &Histogram::calcSingle8;
            break;
        case SINGLE_16_PER_LOOP:
            calc = &Histogram::calcSingle16;
            break;
        case MULTI_NAIVE:
            calc = &Histogram::calcMultiNaive;
            break;
        case MULTI_SIMPLE:
            initMultiSimple();
            calc = &Histogram::calcMultiSimple;
            break;
        default:
            histCfg.method = MULTI_SIMPLE;
            initMultiSimple();
            calc = &Histogram::calcMultiSimple;
    }

    return 0;
}

std::vector<uint32_t>* Histogram::calcSync(uint16_t* const imgSrc) {
    (this->*calc)(imgSrc);
    return histResult;
}

int Histogram::uninit() {
    switch (this->histCfg.method) {
        case SINGLE_1_PER_LOOP:
        case SINGLE_2_PER_LOOP:
        case SINGLE_4_PER_LOOP:
        case SINGLE_8_PER_LOOP:
        case SINGLE_16_PER_LOOP:
        case MULTI_NAIVE:
            // nothing to uninit()
            break;
        case MULTI_SIMPLE:
            uninitMultiSimple();
            break;
        default:
            uninitMultiSimple();
            break;

    }

    delete[] histResult;
    return 0;
}

void Histogram::calcSingle(uint16_t* const imgSrc) {
    for (int channel = 0; channel < histCfg.channel; channel++) {
        std::fill(histResult[channel].begin(), histResult[channel].end(), 0);
    }

    for (int channel = 0; channel < histCfg.channel; channel++) {
        const uint16_t* channelImg = imgSrc + histCfg.width * histCfg.height * channel;

        // calculate histogram for the channel
        for (int pix = 0; pix < histCfg.width * histCfg.height; pix += 1) {
            histResult[channel][channelImg[pix]]++;
        }
    }
}

void Histogram::calcSingle2(uint16_t* const imgSrc) {
    for (int channel = 0; channel < histCfg.channel; channel++) {
        std::fill(histResult[channel].begin(), histResult[channel].end(), 0);
    }
    for (int channel = 0; channel < histCfg.channel; channel++) {
        const uint16_t* channelImg = imgSrc + histCfg.width * histCfg.height * channel;

        // calculate histogram for the channel
        for (int pix = 0; pix < histCfg.width * histCfg.height; pix += 2) {
            histResult[channel][channelImg[pix]]++;
            histResult[channel][channelImg[pix + 1]]++;
        }
    }
}

void Histogram::calcSingle4(uint16_t* const imgSrc) {
    for (int channel = 0; channel < histCfg.channel; channel++) {
        std::fill(histResult[channel].begin(), histResult[channel].end(), 0);
    }
    for (int channel = 0; channel < histCfg.channel; channel++) {
        const uint16_t* channelImg = imgSrc + histCfg.width * histCfg.height * channel;

        // calculate histogram for the channel
        for (int pix = 0; pix < histCfg.width * histCfg.height; pix += 4) {
            histResult[channel][channelImg[pix]]++;
            histResult[channel][channelImg[pix + 1]]++;
            histResult[channel][channelImg[pix + 2]]++;
            histResult[channel][channelImg[pix + 3]]++;
        }
    }
}

void Histogram::calcSingle8(uint16_t* const imgSrc) {
    for (int channel = 0; channel < histCfg.channel; channel++) {
        std::fill(histResult[channel].begin(), histResult[channel].end(), 0);
    }
    for (int channel = 0; channel < histCfg.channel; channel++) {
        const uint16_t* channelImg = imgSrc + histCfg.width * histCfg.height * channel;

        // calculate histogram for the channel
        for (int pix = 0; pix < histCfg.width * histCfg.height; pix += 8) {
            histResult[channel][channelImg[pix]]++;
            histResult[channel][channelImg[pix + 1]]++;
            histResult[channel][channelImg[pix + 2]]++;
            histResult[channel][channelImg[pix + 3]]++;
            histResult[channel][channelImg[pix + 4]]++;
            histResult[channel][channelImg[pix + 5]]++;
            histResult[channel][channelImg[pix + 6]]++;
            histResult[channel][channelImg[pix + 7]]++;
        }
    }
}
void Histogram::calcSingle16(uint16_t* const imgSrc) {
    for (int channel = 0; channel < histCfg.channel; channel++) {
        std::fill(histResult[channel].begin(), histResult[channel].end(), 0);
    }
    for (int channel = 0; channel < histCfg.channel; channel++) {
        const uint16_t* channelImg = imgSrc + histCfg.width * histCfg.height * channel;

        // calculate histogram for the channel
        for (int pix = 0; pix < histCfg.width * histCfg.height; pix += 16) {
            histResult[channel][channelImg[pix]]++;
            histResult[channel][channelImg[pix + 1]]++;
            histResult[channel][channelImg[pix + 2]]++;
            histResult[channel][channelImg[pix + 3]]++;
            histResult[channel][channelImg[pix + 4]]++;
            histResult[channel][channelImg[pix + 5]]++;
            histResult[channel][channelImg[pix + 6]]++;
            histResult[channel][channelImg[pix + 7]]++;
            histResult[channel][channelImg[pix + 8]]++;
            histResult[channel][channelImg[pix + 9]]++;
            histResult[channel][channelImg[pix + 10]]++;
            histResult[channel][channelImg[pix + 11]]++;
            histResult[channel][channelImg[pix + 12]]++;
            histResult[channel][channelImg[pix + 13]]++;
            histResult[channel][channelImg[pix + 14]]++;
            histResult[channel][channelImg[pix + 15]]++;
        }
    }
}

// data struct for instructing thread to calculate histogram for a specific size of image
typedef struct NaiveThreadCfg {
    const uint16_t* imgSrc;
    uint32_t width;
    uint32_t startCol;
    uint32_t endCol;
    uint32_t startRow;
    uint32_t endRow;
    uint32_t bitDepth;

    std::vector<uint32_t> resultHist;
} NaiveThreadCfg;

void* calcMultiNaiveThreadProc(void* threadCfg) {
    NaiveThreadCfg* histThreadCfg;
    histThreadCfg = (NaiveThreadCfg*) threadCfg;

    // std::cout << "thread " << histThreadCfg->threadId << " finishes parsing thread data" << std::endl;
    // preallocate histogram
    int binCount = std::pow(2, histThreadCfg->bitDepth);
    for (int bin = 0; bin < binCount; bin++) {
        histThreadCfg->resultHist.push_back(0);
    }
    // std::cout << "thread " << histThreadCfg->threadId << " finishes init" << std::endl;

    // calculate histogram for the channel
    for (uint32_t row = histThreadCfg->startRow; row < histThreadCfg->endRow; row += 1) {
        const uint16_t* imgRow = histThreadCfg->imgSrc + row * histThreadCfg->width;
        for (uint32_t col = histThreadCfg->startCol; col < histThreadCfg->endCol; col += 8) {
            histThreadCfg->resultHist[imgRow[col]]++;
            histThreadCfg->resultHist[imgRow[col + 1]]++;
            histThreadCfg->resultHist[imgRow[col + 2]]++;
            histThreadCfg->resultHist[imgRow[col + 3]]++;
            histThreadCfg->resultHist[imgRow[col + 4]]++;
            histThreadCfg->resultHist[imgRow[col + 5]]++;
            histThreadCfg->resultHist[imgRow[col + 6]]++;
            histThreadCfg->resultHist[imgRow[col + 7]]++;
            /*
            histThreadCfg->resultHist[imgRow[col + 8]]++;
            histThreadCfg->resultHist[imgRow[col + 9]]++;
            histThreadCfg->resultHist[imgRow[col + 10]]++;
            histThreadCfg->resultHist[imgRow[col + 11]]++;
            histThreadCfg->resultHist[imgRow[col + 12]]++;
            histThreadCfg->resultHist[imgRow[col + 13]]++;
            histThreadCfg->resultHist[imgRow[col + 14]]++;
            histThreadCfg->resultHist[imgRow[col + 15]]++;
             */
        }
    }

    // std::cout << "thread " << histThreadCfg->threadId << " finishes calculating histogram" << std::endl;
    pthread_exit(NULL);
};

void Histogram::calcMultiNaive(uint16_t* const imgSrc) {
    auto threads = new pthread_t[histCfg.threadsToUse];
    histResult = new std::vector<uint32_t>[histCfg.channel];
    std::vector<NaiveThreadCfg> threadCfgArray;
    pthread_attr_t attr;
    void *status;

    // Prepare data
    uint32_t threadPerChannel = histCfg.threadsToUse / histCfg.channel;
    for (uint32_t channel = 0; channel < histCfg.channel; channel++) {
        for (uint32_t channelThread = 0; channelThread < threadPerChannel; channelThread++) {
            NaiveThreadCfg currThreadCfg = NaiveThreadCfg{
                imgSrc + channel * histCfg.width * histCfg.height,
                histCfg.width,
                0,
                histCfg.width - 1,
                histCfg.height / threadPerChannel * channelThread,
                histCfg.height / threadPerChannel * (channelThread + 1) - 1,
                histCfg.bitDepth,
                std::vector<uint32_t>()};
            threadCfgArray.push_back(currThreadCfg);
        }
    }

    // Create thread
    for (int thread = 0; thread < histCfg.threadsToUse; thread++) {
        int createResult = pthread_create(&threads[thread], NULL,
                &calcMultiNaiveThreadProc, (void*) &threadCfgArray[thread]);
        if(createResult) {
            std::cout << "Error: unable to create thread: " << thread << " result: " << createResult << std::endl;
            exit(-1);
        }
    }

    // free attribute and wait for the other threads
    for(int thread = 0; thread < histCfg.threadsToUse; thread++ ) {

        int joinResult = pthread_join(threads[thread], NULL);
        if (joinResult) {
            std::cout << "Error:unable to join threadId: " << thread ;
            switch (joinResult) {
                case EDEADLK:
                    std::cout << " cause: EDEADLK " << std::endl;
                    break;
                case EINVAL:
                    std::cout << " cause: EINVAL " << std::endl;
                    break;
                case ESRCH:
                    std::cout << " cause: ESRCH " << std::endl;
                    break;
                default:
                    std::cout << " cause: " << joinResult << std::endl;
            }
            exit(-1);
        }
    }

    // combine result from each thread
    for(int channel = 0; channel < histCfg.channel; channel++) {
        // Equals to the first
        std::vector<uint32_t> channelHist = threadCfgArray[channel * threadPerChannel].resultHist;
        for(int channelThread = 1; channelThread < threadPerChannel; channelThread++) {
            NaiveThreadCfg curr = threadCfgArray[channel * threadPerChannel + channelThread];
            for (int bin = 0; bin < pow(2, histCfg.bitDepth); bin += 8) {
                channelHist[bin] += curr.resultHist[bin];
                channelHist[bin + 1] += curr.resultHist[bin + 1];
                channelHist[bin + 2] += curr.resultHist[bin + 2];
                channelHist[bin + 3] += curr.resultHist[bin + 3];
                channelHist[bin + 4] += curr.resultHist[bin + 4];
                channelHist[bin + 5] += curr.resultHist[bin + 5];
                channelHist[bin + 6] += curr.resultHist[bin + 6];
                channelHist[bin + 7] += curr.resultHist[bin + 7];
                /*
                channelHist[bin + 8] += curr.resultHist[bin + 8];
                channelHist[bin + 9] += curr.resultHist[bin + 9];
                channelHist[bin + 10] += curr.resultHist[bin + 10];
                channelHist[bin + 11] += curr.resultHist[bin + 11];
                channelHist[bin + 12] += curr.resultHist[bin + 12];
                channelHist[bin + 13] += curr.resultHist[bin + 13];
                channelHist[bin + 14] += curr.resultHist[bin + 14];
                channelHist[bin + 15] += curr.resultHist[bin + 15];
                 */
            }
        }
        histResult[channel] = channelHist;
    }
    delete[] threads;
    delete[] histResult;
}

void Histogram::initMultiSimple() {
    endThread = false;
    unfinished = new uint32_t[histCfg.channel];
    resultMutex = new std::mutex*[histCfg.channel];
    resultCv = new std::condition_variable*[histCfg.channel];

    // configure threads
    histCfg.threadsToUse = (histCfg.threadsToUse / histCfg.channel) * histCfg.channel;
    uint32_t threadPerChannel = histCfg.threadsToUse / histCfg.channel;
    int binCount = std::pow(2, histCfg.bitDepth);
    for (uint32_t channel = 0; channel < histCfg.channel; channel++) {
        // histogram for channel
        unfinished[channel] = 0;
        resultMutex[channel] = (std::mutex*) new std::mutex();
        resultCv[channel] = (std::condition_variable*) new std::condition_variable();

        for (uint32_t channelThread = 0; channelThread < threadPerChannel; channelThread++) {
            threadCfgArray.push_back( SimpleThreadCfg {
                    NULL, // imgSrc should be changed during calcMultiSimple
                    histCfg.width,
                    0,
                    histCfg.width - 1,
                    histCfg.height / threadPerChannel * channelThread,
                    histCfg.height / threadPerChannel * (channelThread + 1) - 1,
                    histCfg.bitDepth,
                    false,
                    std::vector<uint32_t>(binCount, 0),
                    &histResult[channel],
                    &unfinished[channel],
                    resultMutex[channel],
                    resultCv[channel],
                    channel * threadPerChannel + channelThread
            });
        }
    }

    // create thread
    for (int thread = 0; thread < histCfg.threadsToUse; thread++) {
        processThreads.emplace_back(std::thread(&Histogram::calcMultiSimpleThreadProc,
                this, &this->threadCfgArray[thread]));
        std::cout << "create thread: " << thread << std::endl;
    }
}

void Histogram::uninitMultiSimple() {
    std::unique_lock<std::mutex> endLock(readyMutex);
    endThread = true;
    endLock.unlock();
    readyCv.notify_all();


    for (int channel = 0; channel < histCfg.channel; channel++) {
        delete resultMutex[channel];
        delete resultCv[channel];
    }
    delete[] resultMutex;
    delete[] resultCv;
    delete[] histResult;
    delete[] unfinished;
}

void Histogram::calcMultiSimpleThreadProc(SimpleThreadCfg* histThreadCfg) {
    //std::cout << "thread: " << histThreadCfg->threadId << " started" << std::endl;
    while(true) {
        // lock and wait for data to be ready to calculate
        //std::cout << "thread: " << histThreadCfg->threadId << " try to acquire readyMutex" << std::endl;
        std::unique_lock<std::mutex> readyLock(readyMutex);
        //std::cout << "thread: " << histThreadCfg->threadId << " acquired readyMutex" << std::endl;
        while (!histThreadCfg->ready2Calc && !this->endThread) {
            //std::cout << "thread: " << histThreadCfg->threadId << " starts waiting on readyCv" << std::endl;
            readyCv.wait(readyLock);
            //std::cout << "thread: " << histThreadCfg->threadId << " awoken" << std::endl;
        }

        // finish if endThread is called
        if (endThread) {
            std::cout << "thread: " << histThreadCfg->threadId << " exit now" << std::endl;
            return;
        }
        //std::cout << "thread: " << histThreadCfg->threadId << " start calculation" << std::endl;

        // ready to calculate, unlock the readyLock
        readyLock.unlock();
        histThreadCfg->ready2Calc = false;
        // std::cout << "thread " << histThreadCfg->threadId << " finishes parsing thread data" << std::endl;
        // clear histogram
        int binCount = std::pow(2, histThreadCfg->bitDepth);
        std::fill(histThreadCfg->hist.begin(), histThreadCfg->hist.end(), 0);
        // std::cout << "thread " << histThreadCfg->threadId << " finishes init" << std::endl;

        // calculate histogram for the channel
        for (uint32_t row = histThreadCfg->startRow; row < histThreadCfg->endRow; row += 1) {
            uint16_t* imgRow = histThreadCfg->imgSrc + row * histThreadCfg->width;
            for (uint32_t col = histThreadCfg->startCol; col < histThreadCfg->endCol; col += 8) {
            //for (uint32_t col = histThreadCfg->startCol; col < histThreadCfg->endCol; col += 16) {

                histThreadCfg->hist[imgRow[col]]++;
                histThreadCfg->hist[imgRow[col + 1]]++;
                histThreadCfg->hist[imgRow[col + 2]]++;
                histThreadCfg->hist[imgRow[col + 3]]++;
                histThreadCfg->hist[imgRow[col + 4]]++;
                histThreadCfg->hist[imgRow[col + 5]]++;
                histThreadCfg->hist[imgRow[col + 6]]++;
                histThreadCfg->hist[imgRow[col + 7]]++;
                 /*
                histThreadCfg->hist[imgRow[col]]++;
                histThreadCfg->hist[imgRow[col + 2]]++;
                histThreadCfg->hist[imgRow[col + 4]]++;
                histThreadCfg->hist[imgRow[col + 6]]++;
                histThreadCfg->hist[imgRow[col + 8]]++;
                histThreadCfg->hist[imgRow[col + 10]]++;
                histThreadCfg->hist[imgRow[col + 12]]++;
                histThreadCfg->hist[imgRow[col + 14]]++;
                 */
            }
        }

        // std::cout << "thread: " << histThreadCfg->threadId << " finish calculation" << std::endl;
        // finish calculating histogram, acquire result lock and add to final result
        std::unique_lock<std::mutex> resultLock(*histThreadCfg->finishMutex);
        for (int bin = 0; bin < pow(2, histCfg.bitDepth); bin += 8) {
            histThreadCfg->finalHist->at(bin) += histThreadCfg->hist[bin];
            histThreadCfg->finalHist->at(bin + 1) += histThreadCfg->hist[bin + 1];
            histThreadCfg->finalHist->at(bin + 2) += histThreadCfg->hist[bin + 2];
            histThreadCfg->finalHist->at(bin + 3) += histThreadCfg->hist[bin + 3];
            histThreadCfg->finalHist->at(bin + 4) += histThreadCfg->hist[bin + 4];
            histThreadCfg->finalHist->at(bin + 5) += histThreadCfg->hist[bin + 5];
            histThreadCfg->finalHist->at(bin + 6) += histThreadCfg->hist[bin + 6];
            histThreadCfg->finalHist->at(bin + 7) += histThreadCfg->hist[bin + 7];
        }

        // std::cout << "thread: " << histThreadCfg->threadId << " finish combining result" << std::endl;
        // finished adding result:
        (*histThreadCfg->channelUnfinished)--;
        resultLock.unlock();
        histThreadCfg->finishCv->notify_all();
    }
}

// More complex multithread histogram
void Histogram::calcMultiSimple(uint16_t* const imgSrc) {
    // lock and setup calculation
    // set 0
    for (int channelIdx = 0; channelIdx < histCfg.channel; channelIdx++) {
        std::fill(histResult[channelIdx].begin(), histResult[channelIdx].end(), 0);
        unfinished[channelIdx] = histCfg.threadsToUse / histCfg.channel;
    }

    std::unique_lock<std::mutex> readyLock(readyMutex);
    //std::cout << "main thread acquired ready to calc histogram" << std::endl;

    for (auto it = threadCfgArray.begin(); it <= threadCfgArray.end(); it++) {
        it->imgSrc = imgSrc;
        it->ready2Calc = true;
    }

    //std::cout << "unlock ready mutex" << std::endl;
    // unlock mutexand notify processThreads to calculate:
    readyLock.unlock();

    //std::cout << "notify and starts calculation" << std::endl;
    readyCv.notify_all();

    // wait for all channel to finish:
    //std::cout << "waiting for threads to finish" << std::endl;
    for (int channelIdx = 0; channelIdx < histCfg.channel; channelIdx++) {
        //std::cout << "acquired resultLock for channel: " << channelIdx << std::endl;
        std::unique_lock<std::mutex> finishLock(*resultMutex[channelIdx]);
        while (unfinished[channelIdx] != 0) {
            // std::cout << "wait for result of channel: " << channelIdx << std::endl;
            resultCv[channelIdx]->wait(finishLock);
        }
        //std::cout << "got result of channel: " << channelIdx << std::endl;
        // no need to do anything
        finishLock.unlock();
    }
}

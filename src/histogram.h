//
// Created by kes-dev on 28/12/2019.
//

#ifndef OPTIMISATION_HISTOGRAM_H
#define OPTIMISATION_HISTOGRAM_H

#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

#ifdef DEBUG
#define DLOG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DLOG(str) do { } while ( false )
#endif

#define ROW_LIMIT 8

typedef enum HistogramCalcMethod{
    SINGLE_1_PER_LOOP,
    SINGLE_2_PER_LOOP,
    SINGLE_4_PER_LOOP,
    SINGLE_8_PER_LOOP,
    SINGLE_16_PER_LOOP,

    MULTI_NAIVE,
    MULTI_SIMPLE,
    MULTI_SINGLE_QUEUE
} HistogramCalcMethod;

typedef struct HistogramConfig {
    uint32_t width;
    uint32_t height;
    uint32_t channel;
    uint32_t bitDepth;
    HistogramCalcMethod method;
    uint32_t threadsToUse;
} HistogramConfig;

class Histogram {
public:

    Histogram();
    ~Histogram();

    /** Initialise for histogram calculation
     * Must be called before calling calc()
     *
     * @param histCfg configuration of histogram calculation
     * @return
     */
    int init(HistogramConfig histCfg);

    /** Calculate histogram of image imgSrc with previously config parameters
     *
     * @param imgSrc pointer to uint16_t array of image data
     * @return
     */
    std::vector<uint32_t>* calcSync(uint16_t* const imgSrc);

    /** Ends histogram calculation, stop threads and release memory
     *
     * @return
     */
    int uninit();

private:
    HistogramConfig histCfg;
    void (Histogram::*calc) (uint16_t* const);
    std::vector<uint32_t>* histResult;

    /** init functions **/
    void initHistResult();

    /** Calculate histogram in single thread **/
    void calcSingle(uint16_t* const imgSrc);
    void calcSingle2(uint16_t* const imgSrc);
    void calcSingle4(uint16_t* const imgSrc);
    void calcSingle8(uint16_t* const imgSrc);
    void calcSingle16(uint16_t* const imgSrc);

    /** Calculate histogram in multiple threads **/
    // MULTI_NAIVE
    void calcMultiNaive(uint16_t* const imgSrc);

    // MULTI_SIMPLE
    typedef struct SimpleThreadCfg {
        uint16_t* imgSrc;
        uint32_t width;
        uint32_t startCol;
        uint32_t endCol;
        uint32_t startRow;
        uint32_t endRow;
        uint32_t bitDepth;
        bool ready2Calc;
        std::vector<uint32_t> hist;
        std::vector<uint32_t>* const finalHist;
        uint32_t* const channelUnfinished;
        std::mutex* const finishMutex;
        std::condition_variable* const finishCv;
        uint32_t threadId;
    } SimpleThreadCfg;

    std::vector<SimpleThreadCfg> threadCfgArray;
    std::vector<std::thread> processThreads;
    std::mutex readyMutex;
    std::condition_variable readyCv;

    std::mutex** resultMutex;
    std::condition_variable** resultCv;
    uint32_t* unfinished;
    bool endThread;

    void initMultiSimple();
    void uninitMultiSimple();
    void calcMultiSimple(uint16_t* const imgSrc);
    void calcMultiSimpleThreadProc(SimpleThreadCfg* histThreadCfg);

};

#endif //OPTIMISATION_HISTOGRAM_H

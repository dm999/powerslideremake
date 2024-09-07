
#include "TEXLoader.h"

#include "../tools/OgreTools.h"

#include <algorithm>

namespace
{
    struct header
    {
        typedef unsigned char BYTE;

        BYTE unknown[20];
        Ogre::uint32 width;
        Ogre::uint32 height;
        Ogre::uint32 depth;
    };

    template<class T, class Compare>
    constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
    {
        return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
    }

    template<class T>
    constexpr const T& clamp(const T& v, const T& lo, const T& hi)
    {
        return clamp(v, lo, hi, std::less<T>());
    }

    float bicubicW(float x)
    {

        float res = 0.0f;

        float a = -0.5f;

        float abs = std::abs(x);

        if(abs <= 1.0f)
        {
            res = ((a + 2.0f) * (abs * abs * abs)) - ((a + 3.0f) * (abs * abs)) + 1.0f;
        }
        else if(1.0f < abs && abs < 2.0f)
        {
            res = ((a * (abs * abs * abs)) - (5.0f * a * (abs * abs)) + (8.0f * a * abs) - 4.0f * a);
        }

        return res;
    }

    const uint16_t L = 1 << 4;
}//anonymous

#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>

//https://github.com/agruzdev/Yato/blob/master/modules/actors/yato/actors/private/thread_pool.h
class thread_pool
{
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;

    std::mutex m_mutex;
    std::condition_variable m_cvar;

    bool m_stop = false;

public:
    thread_pool(size_t threads_num) {
        auto thread_function = [this] {
            for(;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_cvar.wait(lock, [this] { return m_stop || !m_tasks.empty(); });
                    if(m_stop && m_tasks.empty()) {
                        break;
                    }
                    if(!m_tasks.empty()) {
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                }
                // Execute
                if(task) {
                    try {
                        task();
                    }
                    catch(std::exception & e) {
                        (void) e;
                        //TODO
                    }
                    catch(...) {
                        //TODO
                    }
                }
            }
        };

        for(size_t i = 0; i < threads_num; ++i) {
            m_threads.emplace_back(thread_function);
        }
    }

    ~thread_pool() {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop = true;
            m_cvar.notify_all();
        }
        for(auto & thread : m_threads) {
            thread.join();
        }
    }

    thread_pool(const thread_pool&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;

    // Add task
    template <typename FunTy_, typename ... Args_>
    void enqueue(FunTy_ && function, Args_ && ... args) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(!m_stop) {
            m_tasks.emplace(std::bind(std::forward<FunTy_>(function), std::forward<Args_>(args)...));
        }
        else {
            //TODO
        }
        m_cvar.notify_one();
    }
};

void TEXLoader::doBicubicUpscale(Ogre::Image& img) const
{
    size_t width = img.getWidth();
    size_t height = img.getHeight();

    size_t scaleFactor = 2;
    float scale = 1.0f / scaleFactor;

    size_t dest_height = height * scaleFactor;
    size_t dest_width = width * scaleFactor;

    size_t bufSize = Ogre::PixelUtil::getMemorySize(dest_width, dest_height, 1, img.getFormat());
    Ogre::uchar* pixelData = OGRE_ALLOC_T(Ogre::uchar, bufSize, Ogre::MEMCATEGORY_GENERAL);

    Ogre::Image img2;
    img2.loadDynamicImage(pixelData, dest_width, dest_height, 1, img.getFormat(), true);

    {
#if defined(__ANDROID__)
        size_t threadsAmount = 4;
#else
        size_t threadsAmount = 8;
#endif
        thread_pool tPool(threadsAmount);

        auto bicubicLambda = [&](size_t y_start, size_t y_end){
            for(size_t y = y_start; y < y_end; ++y)
            {
                for(size_t x = 0; x < dest_width; ++x)
                {
                    float xx = ((static_cast<float>(x) + 0.5f) * scale) - 0.5f;
                    float yy = ((static_cast<float>(y) + 0.5f) * scale) - 0.5f;

                    int xxi = static_cast<int>(xx);
                    int yyi = static_cast<int>(yy);

                    float u = xx - xxi;
                    float v = yy - yyi;

                    Ogre::ColourValue pixel = Ogre::ColourValue::ZERO;

                    for(int m = -1; m < 3; ++m){
                        for(int n = -1; n < 3; ++n){
                            /*
                            if(
                            (xxi + n < 0) ||
                            (xxi + n >= static_cast<int>(width)) ||
                            (yyi + m < 0) ||
                            (yyi + m >= static_cast<int>(height))
                            ) continue;*/
                            if(
                                (xxi + n < 0) &&
                                (yyi + m < 0)
                                ) continue;
                            if(
                                (xxi + n < 0) &&
                                (yyi + m >= static_cast<int>(height))
                                ) continue;
                            if(
                                (yyi + m < 0) &&
                                (xxi + n >= static_cast<int>(width))
                                ) continue;
                            if(
                                (xxi + n >= static_cast<int>(width)) &&
                                (yyi + m >= static_cast<int>(height))
                                ) continue;
                            if(xxi + n < 0 && yyi + m >= 0)
                            {
                                pixel += img.getColourAt(xxi - n, yyi + m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                                continue;
                            }
                            if(xxi + n >= 0 && yyi + m < 0)
                            {
                                pixel += img.getColourAt(xxi + n, yyi - m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                                continue;
                            }
                            if(xxi + n >= static_cast<int>(width) && yyi + m < static_cast<int>(height))
                            {
                                pixel += img.getColourAt(xxi - n, yyi + m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                                continue;
                            }
                            if(xxi + n < static_cast<int>(width) && yyi + m >= static_cast<int>(height))
                            {
                                pixel += img.getColourAt(xxi + n, yyi - m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                                continue;
                            }

                            pixel += img.getColourAt(xxi + n, yyi + m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                        }
                    }

                    pixel.saturate();

                    img2.setColourAt(pixel, x, y, 0);
                }
            }
        };

        size_t batch = dest_height / threadsAmount;

        for(size_t q = 0; q < threadsAmount; ++q)
        {
            size_t start = q * batch;
            size_t end = (q + 1) * batch;
            tPool.enqueue(bicubicLambda, start, end);
        }
    }

#if 0
#pragma omp parallel for
    for(int y = 0; y < static_cast<int>(dest_height); ++y)
    {
        for(size_t x = 0; x < dest_width; ++x)
        {
            float xx = ((static_cast<float>(x) + 0.5f) * scale) - 0.5f;
            float yy = ((static_cast<float>(y) + 0.5f) * scale) - 0.5f;

            int xxi = static_cast<int>(xx);
            int yyi = static_cast<int>(yy);

            float u = xx - xxi;
            float v = yy - yyi;

            Ogre::ColourValue pixel = Ogre::ColourValue::ZERO;

            for(int m = -1; m < 3; ++m){
                for(int n = -1; n < 3; ++n){
                    /*
                    if(
                        (xxi + n < 0) ||
                        (xxi + n >= static_cast<int>(width)) ||
                        (yyi + m < 0) ||
                        (yyi + m >= static_cast<int>(height))
                        ) continue;*/
                    if(
                        (xxi + n < 0) &&
                        (yyi + m < 0)
                        ) continue;
                    if(
                        (xxi + n < 0) &&
                        (yyi + m >= static_cast<int>(height))
                        ) continue;
                    if(
                        (yyi + m < 0) &&
                        (xxi + n >= static_cast<int>(width))
                        ) continue;
                    if(
                        (xxi + n >= static_cast<int>(width)) &&
                        (yyi + m >= static_cast<int>(height))
                        ) continue;
                    if(xxi + n < 0 && yyi + m >= 0)
                    {
                        pixel += img.getColourAt(xxi - n, yyi + m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                        continue;
                    }
                    if(xxi + n >= 0 && yyi + m < 0)
                    {
                        pixel += img.getColourAt(xxi + n, yyi - m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                        continue;
                    }
                    if(xxi + n >= static_cast<int>(width) && yyi + m < static_cast<int>(height))
                    {
                        pixel += img.getColourAt(xxi - n, yyi + m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                        continue;
                    }
                    if(xxi + n < static_cast<int>(width) && yyi + m >= static_cast<int>(height))
                    {
                        pixel += img.getColourAt(xxi + n, yyi - m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                        continue;
                    }

                    pixel += img.getColourAt(xxi + n, yyi + m, 0) * (bicubicW(u - n) * bicubicW(v - m));
                }
            }

            pixel.saturate();

            img2.setColourAt(pixel, x, y, 0);
        }
    }
#endif

    std::swap(img, img2);
}

Ogre::ColourValue TEXLoader::getPixel(int x, int y, const Ogre::Image& src) const
{
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    if(x >= static_cast<int>(src.getWidth())) x = src.getWidth() - 1;
    if(y >= static_cast<int>(src.getHeight())) x = src.getHeight() - 1;

    return src.getColourAt(x, y, 0);
}

std::vector<int16_t> TEXLoader::getLUTValsLSB(const std::vector<int8_t>& lut, const Ogre::ColourValue& valA, const Ogre::ColourValue& valB) const
{
    uint16_t valA_r = static_cast<uint16_t>(valA.r * 255.0f) & 0xF;
    uint16_t valA_g = static_cast<uint16_t>(valA.g * 255.0f) & 0xF;
    uint16_t valA_b = static_cast<uint16_t>(valA.b * 255.0f) & 0xF;

    uint16_t valB_r = static_cast<uint16_t>(valB.r * 255.0f) & 0xF;
    uint16_t valB_g = static_cast<uint16_t>(valB.g * 255.0f) & 0xF;
    uint16_t valB_b = static_cast<uint16_t>(valB.b * 255.0f) & 0xF;

    size_t indexR = (valA_r * L + valB_r) * 4;
    size_t indexG = (valA_g * L + valB_g) * 4;
    size_t indexB = (valA_b * L + valB_b) * 4;

    std::vector<int16_t> lutValR(lut.begin() + indexR, lut.begin() + indexR + 4);
    std::vector<int16_t> lutValG(lut.begin() + indexG, lut.begin() + indexG + 4);
    std::vector<int16_t> lutValB(lut.begin() + indexB, lut.begin() + indexB + 4);

    std::vector<int16_t> ret;
    ret.insert(ret.end(), lutValR.begin(), lutValR.end());
    ret.insert(ret.end(), lutValG.begin(), lutValG.end());
    ret.insert(ret.end(), lutValB.begin(), lutValB.end());

    return ret;
}

void TEXLoader::rotateBack(std::vector<int16_t>& res) const
{
    for(size_t q = 0; q < 3; ++q)
    {
        int16_t tmp = res[0 + q * 4];
        res[0 + q * 4] = res[1 + q * 4];
        res[1 + q * 4] = res[3 + q * 4];
        res[3 + q * 4] = res[2 + q * 4];
        res[2 + q * 4] = tmp;
    }
}

void TEXLoader::doLSB(size_t x, size_t y, const Ogre::Image& src, const LUTs& luts, std::vector<Ogre::ColourValue>& res) const
{
    Ogre::ColourValue rot_0 = getPixel(x, y, src);

    Ogre::ColourValue rot_0_h = getPixel(x + 1, y, src);
    Ogre::ColourValue rot_0_d = getPixel(x + 1, y - 1, src);

    Ogre::ColourValue rot_90_h = getPixel(x, y - 1, src);
    Ogre::ColourValue rot_90_d = getPixel(x - 1, y - 1, src);

    Ogre::ColourValue rot_180_h = getPixel(x - 1, y, src);
    Ogre::ColourValue rot_180_d = getPixel(x - 1, y + 1, src);

    Ogre::ColourValue rot_270_h = getPixel(x, y + 1, src);
    Ogre::ColourValue rot_270_d = getPixel(x + 1, y + 1, src);


    std::vector<int16_t> vals_0_h = getLUTValsLSB(luts.LSB_HD_H, rot_0, rot_0_h);
    std::vector<int16_t> vals_0_d = getLUTValsLSB(luts.LSB_HD_D, rot_0, rot_0_d);

    std::vector<int16_t> vals_90_h = getLUTValsLSB(luts.LSB_HD_H, rot_0, rot_90_h);
    std::vector<int16_t> vals_90_d = getLUTValsLSB(luts.LSB_HD_D, rot_0, rot_90_d);

    std::vector<int16_t> vals_180_h = getLUTValsLSB(luts.LSB_HD_H, rot_0, rot_180_h);
    std::vector<int16_t> vals_180_d = getLUTValsLSB(luts.LSB_HD_D, rot_0, rot_180_d);

    std::vector<int16_t> vals_270_h = getLUTValsLSB(luts.LSB_HD_H, rot_0, rot_270_h);
    std::vector<int16_t> vals_270_d = getLUTValsLSB(luts.LSB_HD_D, rot_0, rot_270_d);


    std::vector<int16_t> vals_0(vals_0_h);
    std::transform(vals_0.begin(), vals_0.end(), vals_0_d.begin(), vals_0.begin(), std::plus<int16_t>());

    std::vector<int16_t> vals_90(vals_90_h);
    std::transform(vals_90.begin(), vals_90.end(), vals_90_d.begin(), vals_90.begin(), std::plus<int16_t>());
    rotateBack(vals_90);

    std::vector<int16_t> vals_180(vals_180_h);
    std::transform(vals_180.begin(), vals_180.end(), vals_180_d.begin(), vals_180.begin(), std::plus<int16_t>());
    rotateBack(vals_180);
    rotateBack(vals_180);

    std::vector<int16_t> vals_270(vals_270_h);
    std::transform(vals_270.begin(), vals_270.end(), vals_270_d.begin(), vals_270.begin(), std::plus<int16_t>());
    rotateBack(vals_270);
    rotateBack(vals_270);
    rotateBack(vals_270);

    std::transform(vals_0.begin(), vals_0.end(), vals_90.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_180.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_270.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_0.begin(), [](int16_t val){return val / 2;});

    for(size_t q = 0; q < 4; ++q)
    {
        res[q].r = vals_0[q] / 255.0f;
        res[q].g = vals_0[q + 4] / 255.0f;
        res[q].b = vals_0[q + 8] / 255.0f;
    }

}

std::vector<int16_t> TEXLoader::getLUTValsMSB(const std::vector<int8_t>& lut, const Ogre::ColourValue& valA, const Ogre::ColourValue& valB, const Ogre::ColourValue& valC) const
{
    uint16_t valA_r = static_cast<uint16_t>(valA.r * 255.0f) >> 4;
    uint16_t valA_g = static_cast<uint16_t>(valA.g * 255.0f) >> 4;
    uint16_t valA_b = static_cast<uint16_t>(valA.b * 255.0f) >> 4;

    uint16_t valB_r = static_cast<uint16_t>(valB.r * 255.0f) >> 4;
    uint16_t valB_g = static_cast<uint16_t>(valB.g * 255.0f) >> 4;
    uint16_t valB_b = static_cast<uint16_t>(valB.b * 255.0f) >> 4;

    uint16_t valC_r = static_cast<uint16_t>(valC.r * 255.0f) >> 4;
    uint16_t valC_g = static_cast<uint16_t>(valC.g * 255.0f) >> 4;
    uint16_t valC_b = static_cast<uint16_t>(valC.b * 255.0f) >> 4;

    size_t indexR = (valA_r * L * L + valB_r * L + valC_r) * 4;
    size_t indexG = (valA_g * L * L + valB_g * L + valC_g) * 4;
    size_t indexB = (valA_b * L * L + valB_b * L + valC_b) * 4;

    std::vector<int16_t> lutValR(lut.begin() + indexR, lut.begin() + indexR + 4);
    std::vector<int16_t> lutValG(lut.begin() + indexG, lut.begin() + indexG + 4);
    std::vector<int16_t> lutValB(lut.begin() + indexB, lut.begin() + indexB + 4);

    std::vector<int16_t> ret;
    ret.insert(ret.end(), lutValR.begin(), lutValR.end());
    ret.insert(ret.end(), lutValG.begin(), lutValG.end());
    ret.insert(ret.end(), lutValB.begin(), lutValB.end());

    return ret;
}

void TEXLoader::doMSB(size_t x, size_t y, const Ogre::Image& src, const LUTs& luts, std::vector<Ogre::ColourValue>& res) const
{
    Ogre::ColourValue rot_0 = getPixel(x, y, src);

    Ogre::ColourValue rot_0_h = getPixel(x + 1, y, src);
    Ogre::ColourValue rot_0_h2 = getPixel(x + 2, y, src);
    Ogre::ColourValue rot_0_d = getPixel(x + 1, y - 1, src);
    Ogre::ColourValue rot_0_d2 = getPixel(x + 2, y - 2, src);
    Ogre::ColourValue rot_0_b = getPixel(x + 1, y - 2, src);
    Ogre::ColourValue rot_0_b2 = getPixel(x + 2, y - 1, src);

    Ogre::ColourValue rot_90_h = getPixel(x, y - 1, src);
    Ogre::ColourValue rot_90_h2 = getPixel(x, y - 2, src);
    Ogre::ColourValue rot_90_d = getPixel(x - 1, y - 1, src);
    Ogre::ColourValue rot_90_d2 = getPixel(x - 2, y - 2, src);
    Ogre::ColourValue rot_90_b = getPixel(x - 2, y - 1, src);
    Ogre::ColourValue rot_90_b2 = getPixel(x - 1, y - 2, src);

    Ogre::ColourValue rot_180_h = getPixel(x - 1, y, src);
    Ogre::ColourValue rot_180_h2 = getPixel(x - 2, y, src);
    Ogre::ColourValue rot_180_d = getPixel(x - 1, y + 1, src);
    Ogre::ColourValue rot_180_d2 = getPixel(x - 2, y + 2, src);
    Ogre::ColourValue rot_180_b = getPixel(x - 1, y + 2, src);
    Ogre::ColourValue rot_180_b2 = getPixel(x - 2, y + 1, src);

    Ogre::ColourValue rot_270_h = getPixel(x, y + 1, src);
    Ogre::ColourValue rot_270_h2 = getPixel(x, y + 2, src);
    Ogre::ColourValue rot_270_d = getPixel(x + 1, y + 1, src);
    Ogre::ColourValue rot_270_d2 = getPixel(x + 2, y + 2, src);
    Ogre::ColourValue rot_270_b = getPixel(x + 1, y + 2, src);
    Ogre::ColourValue rot_270_b2 = getPixel(x + 2, y + 1, src);


    std::vector<int16_t> vals_0_h = getLUTValsMSB(luts.MSB_HDB_H, rot_0, rot_0_h, rot_0_h2);
    std::vector<int16_t> vals_0_d = getLUTValsMSB(luts.MSB_HDB_D, rot_0, rot_0_d, rot_0_d2);
    std::vector<int16_t> vals_0_b = getLUTValsMSB(luts.MSB_HDB_B, rot_0, rot_0_b, rot_0_b2);

    std::vector<int16_t> vals_90_h = getLUTValsMSB(luts.MSB_HDB_H, rot_0, rot_90_h, rot_90_h2);
    std::vector<int16_t> vals_90_d = getLUTValsMSB(luts.MSB_HDB_D, rot_0, rot_90_d, rot_90_d2);
    std::vector<int16_t> vals_90_b = getLUTValsMSB(luts.MSB_HDB_B, rot_0, rot_90_b, rot_90_b2);

    std::vector<int16_t> vals_180_h = getLUTValsMSB(luts.MSB_HDB_H, rot_0, rot_180_h, rot_180_h2);
    std::vector<int16_t> vals_180_d = getLUTValsMSB(luts.MSB_HDB_D, rot_0, rot_180_d, rot_180_d2);
    std::vector<int16_t> vals_180_b = getLUTValsMSB(luts.MSB_HDB_B, rot_0, rot_180_b, rot_180_b2);

    std::vector<int16_t> vals_270_h = getLUTValsMSB(luts.MSB_HDB_H, rot_0, rot_270_h, rot_270_h2);
    std::vector<int16_t> vals_270_d = getLUTValsMSB(luts.MSB_HDB_D, rot_0, rot_270_d, rot_270_d2);
    std::vector<int16_t> vals_270_b = getLUTValsMSB(luts.MSB_HDB_B, rot_0, rot_270_b, rot_270_b2);


    std::vector<int16_t> vals_0(vals_0_h);
    std::transform(vals_0.begin(), vals_0.end(), vals_0_d.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_0_b.begin(), vals_0.begin(), std::plus<int16_t>());

    std::vector<int16_t> vals_90(vals_90_h);
    std::transform(vals_90.begin(), vals_90.end(), vals_90_d.begin(), vals_90.begin(), std::plus<int16_t>());
    std::transform(vals_90.begin(), vals_90.end(), vals_90_b.begin(), vals_90.begin(), std::plus<int16_t>());
    rotateBack(vals_90);

    std::vector<int16_t> vals_180(vals_180_h);
    std::transform(vals_180.begin(), vals_180.end(), vals_180_d.begin(), vals_180.begin(), std::plus<int16_t>());
    std::transform(vals_180.begin(), vals_180.end(), vals_180_b.begin(), vals_180.begin(), std::plus<int16_t>());
    rotateBack(vals_180);
    rotateBack(vals_180);

    std::vector<int16_t> vals_270(vals_270_h);
    std::transform(vals_270.begin(), vals_270.end(), vals_270_d.begin(), vals_270.begin(), std::plus<int16_t>());
    std::transform(vals_270.begin(), vals_270.end(), vals_270_b.begin(), vals_270.begin(), std::plus<int16_t>());
    rotateBack(vals_270);
    rotateBack(vals_270);
    rotateBack(vals_270);

    std::transform(vals_0.begin(), vals_0.end(), vals_90.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_180.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_270.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_0.begin(), [](int16_t val){return val / 3; });

    for(size_t q = 0; q < 4; ++q)
    {
        res[q].r = vals_0[q] / 255.0f;
        res[q].g = vals_0[q + 4] / 255.0f;
        res[q].b = vals_0[q + 8] / 255.0f;
    }
}

void TEXLoader::doLUTUpscale(Ogre::Image& img, const LUTs& luts) const
{
    size_t width = img.getWidth();
    size_t height = img.getHeight();

    size_t scaleFactor = 2;

    size_t dest_height = height * scaleFactor;
    size_t dest_width = width * scaleFactor;

    size_t bufSize = Ogre::PixelUtil::getMemorySize(dest_width, dest_height, 1, img.getFormat());
    Ogre::uchar* pixelData = OGRE_ALLOC_T(Ogre::uchar, bufSize, Ogre::MEMCATEGORY_GENERAL);

    Ogre::Image img2;
    img2.loadDynamicImage(pixelData, dest_width, dest_height, 1, img.getFormat(), true);

    for(size_t y = 0; y < height; ++y)
    {
        for(size_t x = 0; x < width; ++x)
        {
            Ogre::ColourValue orig = img.getColourAt(x, y, 0);

            std::vector<Ogre::ColourValue> resLSB(4, Ogre::ColourValue::ZERO);
            std::vector<Ogre::ColourValue> resMSB(4, Ogre::ColourValue::ZERO);
            doLSB(x, y, img, luts, resLSB);
            doMSB(x, y, img, luts, resMSB);

            std::vector<Ogre::ColourValue> pixel(4, orig);
            for(size_t q = 0; q < 4; ++q)
            {
                pixel[q] += resLSB[q];
                pixel[q] += resMSB[q];
                pixel[q].saturate();
            }

            img2.setColourAt(pixel[0], x * 2, y * 2, 0);
            img2.setColourAt(pixel[1], x * 2 + 1, y * 2, 0);
            img2.setColourAt(pixel[2], x * 2, y * 2 + 1, 0);
            img2.setColourAt(pixel[3], x * 2 + 1, y * 2 + 1, 0);
        }
    }

    std::swap(img, img2);
}

Ogre::TexturePtr TEXLoader::load(const Ogre::DataStreamPtr& fileToLoad, const std::string& texturename, const LUTs& luts, const Ogre::String& group, Ogre::Real gamma, bool doUpscale) const
{
    Ogre::TexturePtr res;

    typedef unsigned char BYTE;
    typedef unsigned short WORD;

    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        header head;
        fileToLoad->read(&head, sizeof(header));

#if !defined(__ANDROID__)
        Ogre::PixelFormat targetFormat = head.depth == 16 ? Ogre::PF_R5G6B5 : Ogre::PF_BYTE_BGRA;
#else
        Ogre::PixelFormat targetFormat = head.depth == 16 ? Ogre::PF_B5G6R5 : Ogre::PF_BYTE_BGRA;
#endif

        size_t bufSize = Ogre::PixelUtil::getMemorySize (head.width, head.height, 1, targetFormat);

        // Since Ogre 1.6 Shoggoth, the OGRE_ALLOC_T memory macro must be used:
        Ogre::uchar* pixelData = OGRE_ALLOC_T (Ogre::uchar, bufSize, Ogre::MEMCATEGORY_GENERAL);
        fileToLoad->read(pixelData, bufSize);

        Ogre::Image img;
        img.loadDynamicImage (pixelData, head.width, head.height, 1, targetFormat, true); 

        if (gamma != 1.0f)
        {
#if 0
            if (img.getBPP() == 16)
            {
                size_t bufSize2 = Ogre::PixelUtil::getMemorySize(head.width, head.height, 1, Ogre::PF_BYTE_BGRA);
                Ogre::uchar* pixelData2 = OGRE_ALLOC_T(Ogre::uchar, bufSize2, Ogre::MEMCATEGORY_GENERAL);
                Ogre::PixelBox pbConverted(head.width, head.height, 1, Ogre::PF_BYTE_BGRA, pixelData2);

                Ogre::PixelBox pb(head.width, head.height, 1, targetFormat, img.getData());

                Ogre::PixelUtil::bulkPixelConversion(pb, pbConverted);
                Ogre::Image img2;
                img2.loadDynamicImage(pixelData2, head.width, head.height, 1, Ogre::PF_BYTE_BGRA, true);
                std::swap(img,img2);
            }
            img.applyGamma(img.getData(), gamma, img.getSize(), img.getBPP());
#else
            const Ogre::Real exp = 1.0f / gamma;
            Ogre::Real gammaBuf[256];
            for (size_t q = 0; q < 256; ++q)
            {
                gammaBuf[q] = Ogre::Math::Pow(q / 255.0f, exp);
            }
            for (size_t x = 0; x < head.width; ++x)
            {
                for (size_t y = 0; y < head.height; ++y)
                {
                    Ogre::ColourValue color = img.getColourAt(x, y, 0);
                    color.r = gammaBuf[static_cast<size_t>(color.r * 255.0f)];
                    color.g = gammaBuf[static_cast<size_t>(color.g * 255.0f)];
                    color.b = gammaBuf[static_cast<size_t>(color.b * 255.0f)];
                    img.setColourAt(color, x, y, 0);
                }
            }
#endif
        }

        //if(doUpscale) doBicubicUpscale(img);
        if(doUpscale) doLUTUpscale(img, luts);

        res = Ogre::TextureManager::getSingleton().loadImage(texturename, group, img, Ogre::TEX_TYPE_2D);

        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TEXLoader::load]: [" + texturename + "]");
    }

    return res;
}

Ogre::TexturePtr TEXLoader::load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const LUTs& luts, const Ogre::String& group, Ogre::Real gamma, bool doUpscale) const
{
    Ogre::TexturePtr res;
    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        res = load(fileToLoad, texturename, luts, group, gamma, doUpscale);
        fileToLoad->close();
    }
    return res;
}

Ogre::TexturePtr TEXLoader::loadChroma( const PFLoader& pfLoader, 
                                            const std::string& subfolder, const std::string& filename, 
                                            const std::string& texturename, const Ogre::String& group,
                                            const Ogre::ColourValue& keyCol,
                                            float threshold,
                                            bool resize,
                                            size_t newSize) const
{
    Ogre::TexturePtr res;

    typedef unsigned char BYTE;
    typedef unsigned short WORD;

    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        header head;
        fileToLoad->read(&head, sizeof(header));

        size_t sizeOfBuffer = head.width * head.height;

        Ogre::PixelFormat targetFormat = head.depth == 16 ? Ogre::PF_R5G6B5 : Ogre::PF_BYTE_BGRA;

        res = Ogre::TextureManager::getSingleton().createManual(
            texturename,
            group,
            Ogre::TEX_TYPE_2D,
            head.width, head.height,
            Ogre::MIP_UNLIMITED,
            targetFormat,
            Ogre::TU_DEFAULT);

        Ogre::HardwarePixelBufferSharedPtr pixelBuffer = res->getBuffer();
        pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
        const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
        BYTE* pDest = static_cast<BYTE*>(pixelBox.data);

        size_t sizeofformat = Ogre::PixelUtil::getNumElemBytes(targetFormat);

        fileToLoad->read(pDest, sizeofformat * sizeOfBuffer);

        Ogre::ulong pxDataIndex = 0, pxDataIndexStep = Ogre::PixelUtil::getNumElemBytes (targetFormat); 

        for(Ogre::uint y = 0; y < head.height; ++y) 
        {
            for(Ogre::uint x = 0; x < head.width; ++x) 
            {
                Ogre::ColourValue pixCol;
                Ogre::PixelUtil::unpackColour (&pixCol, targetFormat, static_cast<void*> (pDest + pxDataIndex) ); 
                Ogre::ColourValue diffCol = pixCol - keyCol; 
                if(pixCol.a == 1.0f)
                    pixCol.a = ( (fabs(diffCol.r)<threshold) && (fabs(diffCol.g)<threshold) && (fabs(diffCol.b)<threshold) ) ? 0.0f : 1.0f; 
                Ogre::PixelUtil::packColour (pixCol, targetFormat, static_cast<void*> (pDest + pxDataIndex) ); 
                pxDataIndex += pxDataIndexStep; 
            }
        }
        pixelBuffer->unlock();

        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TEXLoader::loadChroma]: [" + texturename + "]");
    }
    else {assert(false && "No texture file");}

    return res;
}
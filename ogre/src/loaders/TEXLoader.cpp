
#include "TEXLoader.h"

#include "../tools/OgreTools.h"
#include "../tools/Conversions.h"

#include <algorithm>
#include <array>

#if defined(__ANDROID__)
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

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

TEXLoader::Pixel TEXLoader::getPixel(int x, int y, const uint8_t* src, size_t stride) const
{
    return std::make_tuple(src[y * stride * 3 + x * 3 + 0], src[y * stride * 3 + x * 3 + 1], src[y * stride * 3 + x * 3 + 2]);
}

TEXLoader::Indexes TEXLoader::getLUTValsLSB(const Pixel& valA, const Pixel& valB) const
{
    uint16_t valA_r = static_cast<uint16_t>(std::get<0>(valA)) & 0xF;
    uint16_t valA_g = static_cast<uint16_t>(std::get<1>(valA)) & 0xF;
    uint16_t valA_b = static_cast<uint16_t>(std::get<2>(valA)) & 0xF;

    uint16_t valB_r = static_cast<uint16_t>(std::get<0>(valB)) & 0xF;
    uint16_t valB_g = static_cast<uint16_t>(std::get<1>(valB)) & 0xF;
    uint16_t valB_b = static_cast<uint16_t>(std::get<2>(valB)) & 0xF;

    size_t indexR = (valA_r * L + valB_r) * 4;
    size_t indexG = (valA_g * L + valB_g) * 4;
    size_t indexB = (valA_b * L + valB_b) * 4;

    return std::make_tuple(indexR, indexG, indexB);
}

void TEXLoader::rotateBack(std::array<int16_t, 12> & res) const
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

void TEXLoader::doLSB(size_t x, size_t y, const uint8_t* src, size_t stride, const LUTs& luts, PixelSigned* res) const
{
    Pixel rot_0 = getPixel(x, y, src, stride);

    Pixel rot_0_h = getPixel(x + 1, y, src, stride);
    Pixel rot_0_d = getPixel(x + 1, y - 1, src, stride);

    Pixel rot_90_h = getPixel(x, y - 1, src, stride);
    Pixel rot_90_d = getPixel(x - 1, y - 1, src, stride);

    Pixel rot_180_h = getPixel(x - 1, y, src, stride);
    Pixel rot_180_d = getPixel(x - 1, y + 1, src, stride);

    Pixel rot_270_h = getPixel(x, y + 1, src, stride);
    Pixel rot_270_d = getPixel(x + 1, y + 1, src, stride);


    Indexes vals_0_h = getLUTValsLSB(rot_0, rot_0_h);
    Indexes vals_0_d = getLUTValsLSB(rot_0, rot_0_d);

    Indexes vals_90_h = getLUTValsLSB(rot_0, rot_90_h);
    Indexes vals_90_d = getLUTValsLSB(rot_0, rot_90_d);

    Indexes vals_180_h = getLUTValsLSB(rot_0, rot_180_h);
    Indexes vals_180_d = getLUTValsLSB(rot_0, rot_180_d);

    Indexes vals_270_h = getLUTValsLSB(rot_0, rot_270_h);
    Indexes vals_270_d = getLUTValsLSB(rot_0, rot_270_d);


    std::array<int16_t, 12> vals_0 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::transform(luts.LSB_HD_H.begin() + std::get<0>(vals_0_h), luts.LSB_HD_H.begin() + std::get<0>(vals_0_h) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
    std::transform(luts.LSB_HD_H.begin() + std::get<1>(vals_0_h), luts.LSB_HD_H.begin() + std::get<1>(vals_0_h) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
    std::transform(luts.LSB_HD_H.begin() + std::get<2>(vals_0_h), luts.LSB_HD_H.begin() + std::get<2>(vals_0_h) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<0>(vals_0_d), luts.LSB_HD_D.begin() + std::get<0>(vals_0_d) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<1>(vals_0_d), luts.LSB_HD_D.begin() + std::get<1>(vals_0_d) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<2>(vals_0_d), luts.LSB_HD_D.begin() + std::get<2>(vals_0_d) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());

    std::array<int16_t, 12> vals_90 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::transform(luts.LSB_HD_H.begin() + std::get<0>(vals_90_h), luts.LSB_HD_H.begin() + std::get<0>(vals_90_h) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
    std::transform(luts.LSB_HD_H.begin() + std::get<1>(vals_90_h), luts.LSB_HD_H.begin() + std::get<1>(vals_90_h) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
    std::transform(luts.LSB_HD_H.begin() + std::get<2>(vals_90_h), luts.LSB_HD_H.begin() + std::get<2>(vals_90_h) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<0>(vals_90_d), luts.LSB_HD_D.begin() + std::get<0>(vals_90_d) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<1>(vals_90_d), luts.LSB_HD_D.begin() + std::get<1>(vals_90_d) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<2>(vals_90_d), luts.LSB_HD_D.begin() + std::get<2>(vals_90_d) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
    rotateBack(vals_90);

    std::array<int16_t, 12> vals_180 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::transform(luts.LSB_HD_H.begin() + std::get<0>(vals_180_h), luts.LSB_HD_H.begin() + std::get<0>(vals_180_h) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
    std::transform(luts.LSB_HD_H.begin() + std::get<1>(vals_180_h), luts.LSB_HD_H.begin() + std::get<1>(vals_180_h) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
    std::transform(luts.LSB_HD_H.begin() + std::get<2>(vals_180_h), luts.LSB_HD_H.begin() + std::get<2>(vals_180_h) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<0>(vals_180_d), luts.LSB_HD_D.begin() + std::get<0>(vals_180_d) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<1>(vals_180_d), luts.LSB_HD_D.begin() + std::get<1>(vals_180_d) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<2>(vals_180_d), luts.LSB_HD_D.begin() + std::get<2>(vals_180_d) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
    rotateBack(vals_180);
    rotateBack(vals_180);

    std::array<int16_t, 12> vals_270 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::transform(luts.LSB_HD_H.begin() + std::get<0>(vals_270_h), luts.LSB_HD_H.begin() + std::get<0>(vals_270_h) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
    std::transform(luts.LSB_HD_H.begin() + std::get<1>(vals_270_h), luts.LSB_HD_H.begin() + std::get<1>(vals_270_h) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
    std::transform(luts.LSB_HD_H.begin() + std::get<2>(vals_270_h), luts.LSB_HD_H.begin() + std::get<2>(vals_270_h) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<0>(vals_270_d), luts.LSB_HD_D.begin() + std::get<0>(vals_270_d) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<1>(vals_270_d), luts.LSB_HD_D.begin() + std::get<1>(vals_270_d) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
    std::transform(luts.LSB_HD_D.begin() + std::get<2>(vals_270_d), luts.LSB_HD_D.begin() + std::get<2>(vals_270_d) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
    rotateBack(vals_270);
    rotateBack(vals_270);
    rotateBack(vals_270);

    std::transform(vals_0.begin(), vals_0.end(), vals_90.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_180.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_270.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_0.begin(), [](int16_t val){return val / 2;});

    for(size_t q = 0; q < 4; ++q)
    {
        res[q] = std::make_tuple(static_cast<int8_t>(vals_0[q]), static_cast<int8_t>(vals_0[q + 4]), static_cast<int8_t>(vals_0[q + 8]));
    }
}

TEXLoader::Indexes TEXLoader::getLUTValsMSB(const Pixel& valA, const Pixel& valB, const Pixel& valC) const
{
    uint16_t valA_r = static_cast<uint16_t>(std::get<0>(valA)) >> 4;
    uint16_t valA_g = static_cast<uint16_t>(std::get<1>(valA)) >> 4;
    uint16_t valA_b = static_cast<uint16_t>(std::get<2>(valA)) >> 4;

    uint16_t valB_r = static_cast<uint16_t>(std::get<0>(valB)) >> 4;
    uint16_t valB_g = static_cast<uint16_t>(std::get<1>(valB)) >> 4;
    uint16_t valB_b = static_cast<uint16_t>(std::get<2>(valB)) >> 4;

    uint16_t valC_r = static_cast<uint16_t>(std::get<0>(valC)) >> 4;
    uint16_t valC_g = static_cast<uint16_t>(std::get<1>(valC)) >> 4;
    uint16_t valC_b = static_cast<uint16_t>(std::get<2>(valC)) >> 4;

    size_t indexR = (valA_r * L * L + valB_r * L + valC_r) * 4;
    size_t indexG = (valA_g * L * L + valB_g * L + valC_g) * 4;
    size_t indexB = (valA_b * L * L + valB_b * L + valC_b) * 4;

    return std::make_tuple(indexR, indexG, indexB);
}

void TEXLoader::doMSB(size_t x, size_t y, const uint8_t* src, size_t stride, const LUTs& luts, PixelSigned* res) const
{
    Pixel rot_0 = getPixel(x, y, src, stride);

    Pixel rot_0_h = getPixel(x + 1, y, src, stride);
    Pixel rot_0_h2 = getPixel(x + 2, y, src, stride);
    Pixel rot_0_d = getPixel(x + 1, y - 1, src, stride);
    Pixel rot_0_d2 = getPixel(x + 2, y - 2, src, stride);
    Pixel rot_0_b = getPixel(x + 1, y - 2, src, stride);
    Pixel rot_0_b2 = getPixel(x + 2, y - 1, src, stride);

    Pixel rot_90_h = getPixel(x, y - 1, src, stride);
    Pixel rot_90_h2 = getPixel(x, y - 2, src, stride);
    Pixel rot_90_d = getPixel(x - 1, y - 1, src, stride);
    Pixel rot_90_d2 = getPixel(x - 2, y - 2, src, stride);
    Pixel rot_90_b = getPixel(x - 2, y - 1, src, stride);
    Pixel rot_90_b2 = getPixel(x - 1, y - 2, src, stride);

    Pixel rot_180_h = getPixel(x - 1, y, src, stride);
    Pixel rot_180_h2 = getPixel(x - 2, y, src, stride);
    Pixel rot_180_d = getPixel(x - 1, y + 1, src, stride);
    Pixel rot_180_d2 = getPixel(x - 2, y + 2, src, stride);
    Pixel rot_180_b = getPixel(x - 1, y + 2, src, stride);
    Pixel rot_180_b2 = getPixel(x - 2, y + 1, src, stride);

    Pixel rot_270_h = getPixel(x, y + 1, src, stride);
    Pixel rot_270_h2 = getPixel(x, y + 2, src, stride);
    Pixel rot_270_d = getPixel(x + 1, y + 1, src, stride);
    Pixel rot_270_d2 = getPixel(x + 2, y + 2, src, stride);
    Pixel rot_270_b = getPixel(x + 1, y + 2, src, stride);
    Pixel rot_270_b2 = getPixel(x + 2, y + 1, src, stride);


    Indexes vals_0_h = getLUTValsMSB(rot_0, rot_0_h, rot_0_h2);
    Indexes vals_0_d = getLUTValsMSB(rot_0, rot_0_d, rot_0_d2);
    Indexes vals_0_b = getLUTValsMSB(rot_0, rot_0_b, rot_0_b2);

    Indexes vals_90_h = getLUTValsMSB(rot_0, rot_90_h, rot_90_h2);
    Indexes vals_90_d = getLUTValsMSB(rot_0, rot_90_d, rot_90_d2);
    Indexes vals_90_b = getLUTValsMSB(rot_0, rot_90_b, rot_90_b2);

    Indexes vals_180_h = getLUTValsMSB(rot_0, rot_180_h, rot_180_h2);
    Indexes vals_180_d = getLUTValsMSB(rot_0, rot_180_d, rot_180_d2);
    Indexes vals_180_b = getLUTValsMSB(rot_0, rot_180_b, rot_180_b2);

    Indexes vals_270_h = getLUTValsMSB(rot_0, rot_270_h, rot_270_h2);
    Indexes vals_270_d = getLUTValsMSB(rot_0, rot_270_d, rot_270_d2);
    Indexes vals_270_b = getLUTValsMSB(rot_0, rot_270_b, rot_270_b2);


    std::array<int16_t, 12> vals_0 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::transform(luts.MSB_HDB_H.begin() + std::get<0>(vals_0_h), luts.MSB_HDB_H.begin() + std::get<0>(vals_0_h) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_H.begin() + std::get<1>(vals_0_h), luts.MSB_HDB_H.begin() + std::get<1>(vals_0_h) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_H.begin() + std::get<2>(vals_0_h), luts.MSB_HDB_H.begin() + std::get<2>(vals_0_h) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<0>(vals_0_d), luts.MSB_HDB_D.begin() + std::get<0>(vals_0_d) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<1>(vals_0_d), luts.MSB_HDB_D.begin() + std::get<1>(vals_0_d) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<2>(vals_0_d), luts.MSB_HDB_D.begin() + std::get<2>(vals_0_d) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<0>(vals_0_b), luts.MSB_HDB_B.begin() + std::get<0>(vals_0_b) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<1>(vals_0_b), luts.MSB_HDB_B.begin() + std::get<1>(vals_0_b) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<2>(vals_0_b), luts.MSB_HDB_B.begin() + std::get<2>(vals_0_b) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());

    std::array<int16_t, 12> vals_90 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::transform(luts.MSB_HDB_H.begin() + std::get<0>(vals_90_h), luts.MSB_HDB_H.begin() + std::get<0>(vals_90_h) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_H.begin() + std::get<1>(vals_90_h), luts.MSB_HDB_H.begin() + std::get<1>(vals_90_h) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_H.begin() + std::get<2>(vals_90_h), luts.MSB_HDB_H.begin() + std::get<2>(vals_90_h) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<0>(vals_90_d), luts.MSB_HDB_D.begin() + std::get<0>(vals_90_d) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<1>(vals_90_d), luts.MSB_HDB_D.begin() + std::get<1>(vals_90_d) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<2>(vals_90_d), luts.MSB_HDB_D.begin() + std::get<2>(vals_90_d) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<0>(vals_90_b), luts.MSB_HDB_B.begin() + std::get<0>(vals_90_b) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<1>(vals_90_b), luts.MSB_HDB_B.begin() + std::get<1>(vals_90_b) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<2>(vals_90_b), luts.MSB_HDB_B.begin() + std::get<2>(vals_90_b) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
    rotateBack(vals_90);

    std::array<int16_t, 12> vals_180 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::transform(luts.MSB_HDB_H.begin() + std::get<0>(vals_180_h), luts.MSB_HDB_H.begin() + std::get<0>(vals_180_h) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_H.begin() + std::get<1>(vals_180_h), luts.MSB_HDB_H.begin() + std::get<1>(vals_180_h) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_H.begin() + std::get<2>(vals_180_h), luts.MSB_HDB_H.begin() + std::get<2>(vals_180_h) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<0>(vals_180_d), luts.MSB_HDB_D.begin() + std::get<0>(vals_180_d) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<1>(vals_180_d), luts.MSB_HDB_D.begin() + std::get<1>(vals_180_d) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<2>(vals_180_d), luts.MSB_HDB_D.begin() + std::get<2>(vals_180_d) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<0>(vals_180_b), luts.MSB_HDB_B.begin() + std::get<0>(vals_180_b) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<1>(vals_180_b), luts.MSB_HDB_B.begin() + std::get<1>(vals_180_b) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<2>(vals_180_b), luts.MSB_HDB_B.begin() + std::get<2>(vals_180_b) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
    rotateBack(vals_180);
    rotateBack(vals_180);

    std::array<int16_t, 12> vals_270 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::transform(luts.MSB_HDB_H.begin() + std::get<0>(vals_270_h), luts.MSB_HDB_H.begin() + std::get<0>(vals_270_h) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_H.begin() + std::get<1>(vals_270_h), luts.MSB_HDB_H.begin() + std::get<1>(vals_270_h) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_H.begin() + std::get<2>(vals_270_h), luts.MSB_HDB_H.begin() + std::get<2>(vals_270_h) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<0>(vals_270_d), luts.MSB_HDB_D.begin() + std::get<0>(vals_270_d) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<1>(vals_270_d), luts.MSB_HDB_D.begin() + std::get<1>(vals_270_d) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_D.begin() + std::get<2>(vals_270_d), luts.MSB_HDB_D.begin() + std::get<2>(vals_270_d) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<0>(vals_270_b), luts.MSB_HDB_B.begin() + std::get<0>(vals_270_b) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<1>(vals_270_b), luts.MSB_HDB_B.begin() + std::get<1>(vals_270_b) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
    std::transform(luts.MSB_HDB_B.begin() + std::get<2>(vals_270_b), luts.MSB_HDB_B.begin() + std::get<2>(vals_270_b) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
    rotateBack(vals_270);
    rotateBack(vals_270);
    rotateBack(vals_270);

    std::transform(vals_0.begin(), vals_0.end(), vals_90.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_180.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_270.begin(), vals_0.begin(), std::plus<int16_t>());
    std::transform(vals_0.begin(), vals_0.end(), vals_0.begin(), [](int16_t val){return val / 3; });

    for(size_t q = 0; q < 4; ++q)
    {
        res[q] = std::make_tuple(static_cast<int8_t>(vals_0[q]), static_cast<int8_t>(vals_0[q + 4]), static_cast<int8_t>(vals_0[q + 8]));
    }
}

void TEXLoader::doLSBMSB(size_t x, size_t y, const uint8_t* src, size_t stride, const LUTs& luts, PixelSigned* resLSB, PixelSigned* resMSB) const
{
    Pixel rot_0 = getPixel(x, y, src, stride);

    Pixel rot_0_h = getPixel(x + 1, y, src, stride);
    Pixel rot_0_d = getPixel(x + 1, y - 1, src, stride);

    Pixel rot_90_h = getPixel(x, y - 1, src, stride);
    Pixel rot_90_d = getPixel(x - 1, y - 1, src, stride);

    Pixel rot_180_h = getPixel(x - 1, y, src, stride);
    Pixel rot_180_d = getPixel(x - 1, y + 1, src, stride);

    Pixel rot_270_h = getPixel(x, y + 1, src, stride);
    Pixel rot_270_d = getPixel(x + 1, y + 1, src, stride);

    //LSB
    {

        Indexes vals_0_h = getLUTValsLSB(rot_0, rot_0_h);
        Indexes vals_0_d = getLUTValsLSB(rot_0, rot_0_d);

        Indexes vals_90_h = getLUTValsLSB(rot_0, rot_90_h);
        Indexes vals_90_d = getLUTValsLSB(rot_0, rot_90_d);

        Indexes vals_180_h = getLUTValsLSB(rot_0, rot_180_h);
        Indexes vals_180_d = getLUTValsLSB(rot_0, rot_180_d);

        Indexes vals_270_h = getLUTValsLSB(rot_0, rot_270_h);
        Indexes vals_270_d = getLUTValsLSB(rot_0, rot_270_d);


        std::array<int16_t, 12> vals_0 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::transform(luts.LSB_HD_H.begin() + std::get<0>(vals_0_h), luts.LSB_HD_H.begin() + std::get<0>(vals_0_h) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
        std::transform(luts.LSB_HD_H.begin() + std::get<1>(vals_0_h), luts.LSB_HD_H.begin() + std::get<1>(vals_0_h) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
        std::transform(luts.LSB_HD_H.begin() + std::get<2>(vals_0_h), luts.LSB_HD_H.begin() + std::get<2>(vals_0_h) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<0>(vals_0_d), luts.LSB_HD_D.begin() + std::get<0>(vals_0_d) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<1>(vals_0_d), luts.LSB_HD_D.begin() + std::get<1>(vals_0_d) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<2>(vals_0_d), luts.LSB_HD_D.begin() + std::get<2>(vals_0_d) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());

        std::array<int16_t, 12> vals_90 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::transform(luts.LSB_HD_H.begin() + std::get<0>(vals_90_h), luts.LSB_HD_H.begin() + std::get<0>(vals_90_h) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
        std::transform(luts.LSB_HD_H.begin() + std::get<1>(vals_90_h), luts.LSB_HD_H.begin() + std::get<1>(vals_90_h) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
        std::transform(luts.LSB_HD_H.begin() + std::get<2>(vals_90_h), luts.LSB_HD_H.begin() + std::get<2>(vals_90_h) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<0>(vals_90_d), luts.LSB_HD_D.begin() + std::get<0>(vals_90_d) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<1>(vals_90_d), luts.LSB_HD_D.begin() + std::get<1>(vals_90_d) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<2>(vals_90_d), luts.LSB_HD_D.begin() + std::get<2>(vals_90_d) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
        rotateBack(vals_90);

        std::array<int16_t, 12> vals_180 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::transform(luts.LSB_HD_H.begin() + std::get<0>(vals_180_h), luts.LSB_HD_H.begin() + std::get<0>(vals_180_h) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
        std::transform(luts.LSB_HD_H.begin() + std::get<1>(vals_180_h), luts.LSB_HD_H.begin() + std::get<1>(vals_180_h) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
        std::transform(luts.LSB_HD_H.begin() + std::get<2>(vals_180_h), luts.LSB_HD_H.begin() + std::get<2>(vals_180_h) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<0>(vals_180_d), luts.LSB_HD_D.begin() + std::get<0>(vals_180_d) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<1>(vals_180_d), luts.LSB_HD_D.begin() + std::get<1>(vals_180_d) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<2>(vals_180_d), luts.LSB_HD_D.begin() + std::get<2>(vals_180_d) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
        rotateBack(vals_180);
        rotateBack(vals_180);

        std::array<int16_t, 12> vals_270 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::transform(luts.LSB_HD_H.begin() + std::get<0>(vals_270_h), luts.LSB_HD_H.begin() + std::get<0>(vals_270_h) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
        std::transform(luts.LSB_HD_H.begin() + std::get<1>(vals_270_h), luts.LSB_HD_H.begin() + std::get<1>(vals_270_h) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
        std::transform(luts.LSB_HD_H.begin() + std::get<2>(vals_270_h), luts.LSB_HD_H.begin() + std::get<2>(vals_270_h) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<0>(vals_270_d), luts.LSB_HD_D.begin() + std::get<0>(vals_270_d) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<1>(vals_270_d), luts.LSB_HD_D.begin() + std::get<1>(vals_270_d) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
        std::transform(luts.LSB_HD_D.begin() + std::get<2>(vals_270_d), luts.LSB_HD_D.begin() + std::get<2>(vals_270_d) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
        rotateBack(vals_270);
        rotateBack(vals_270);
        rotateBack(vals_270);

        std::transform(vals_0.begin(), vals_0.end(), vals_90.begin(), vals_0.begin(), std::plus<int16_t>());
        std::transform(vals_0.begin(), vals_0.end(), vals_180.begin(), vals_0.begin(), std::plus<int16_t>());
        std::transform(vals_0.begin(), vals_0.end(), vals_270.begin(), vals_0.begin(), std::plus<int16_t>());
        std::transform(vals_0.begin(), vals_0.end(), vals_0.begin(), [](int16_t val){return val / 2; });

        for(size_t q = 0; q < 4; ++q)
        {
            resLSB[q] = std::make_tuple(static_cast<int16_t>(vals_0[q]), static_cast<int16_t>(vals_0[q + 4]), static_cast<int16_t>(vals_0[q + 8]));
        }
    }

    //MSB
    {
        Pixel rot_0_h2 = getPixel(x + 2, y, src, stride);
        Pixel rot_0_d2 = getPixel(x + 2, y - 2, src, stride);
        Pixel rot_0_b = getPixel(x + 1, y - 2, src, stride);
        Pixel rot_0_b2 = getPixel(x + 2, y - 1, src, stride);

        Pixel rot_90_h2 = getPixel(x, y - 2, src, stride);
        Pixel rot_90_d2 = getPixel(x - 2, y - 2, src, stride);
        Pixel rot_90_b = getPixel(x - 2, y - 1, src, stride);
        Pixel rot_90_b2 = getPixel(x - 1, y - 2, src, stride);

        Pixel rot_180_h2 = getPixel(x - 2, y, src, stride);
        Pixel rot_180_d2 = getPixel(x - 2, y + 2, src, stride);
        Pixel rot_180_b = getPixel(x - 1, y + 2, src, stride);
        Pixel rot_180_b2 = getPixel(x - 2, y + 1, src, stride);

        Pixel rot_270_h2 = getPixel(x, y + 2, src, stride);
        Pixel rot_270_d2 = getPixel(x + 2, y + 2, src, stride);
        Pixel rot_270_b = getPixel(x + 1, y + 2, src, stride);
        Pixel rot_270_b2 = getPixel(x + 2, y + 1, src, stride);

        Indexes vals_0_h = getLUTValsMSB(rot_0, rot_0_h, rot_0_h2);
        Indexes vals_0_d = getLUTValsMSB(rot_0, rot_0_d, rot_0_d2);
        Indexes vals_0_b = getLUTValsMSB(rot_0, rot_0_b, rot_0_b2);

        Indexes vals_90_h = getLUTValsMSB(rot_0, rot_90_h, rot_90_h2);
        Indexes vals_90_d = getLUTValsMSB(rot_0, rot_90_d, rot_90_d2);
        Indexes vals_90_b = getLUTValsMSB(rot_0, rot_90_b, rot_90_b2);

        Indexes vals_180_h = getLUTValsMSB(rot_0, rot_180_h, rot_180_h2);
        Indexes vals_180_d = getLUTValsMSB(rot_0, rot_180_d, rot_180_d2);
        Indexes vals_180_b = getLUTValsMSB(rot_0, rot_180_b, rot_180_b2);

        Indexes vals_270_h = getLUTValsMSB(rot_0, rot_270_h, rot_270_h2);
        Indexes vals_270_d = getLUTValsMSB(rot_0, rot_270_d, rot_270_d2);
        Indexes vals_270_b = getLUTValsMSB(rot_0, rot_270_b, rot_270_b2);


        std::array<int16_t, 12> vals_0 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::transform(luts.MSB_HDB_H.begin() + std::get<0>(vals_0_h), luts.MSB_HDB_H.begin() + std::get<0>(vals_0_h) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_H.begin() + std::get<1>(vals_0_h), luts.MSB_HDB_H.begin() + std::get<1>(vals_0_h) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_H.begin() + std::get<2>(vals_0_h), luts.MSB_HDB_H.begin() + std::get<2>(vals_0_h) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<0>(vals_0_d), luts.MSB_HDB_D.begin() + std::get<0>(vals_0_d) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<1>(vals_0_d), luts.MSB_HDB_D.begin() + std::get<1>(vals_0_d) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<2>(vals_0_d), luts.MSB_HDB_D.begin() + std::get<2>(vals_0_d) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<0>(vals_0_b), luts.MSB_HDB_B.begin() + std::get<0>(vals_0_b) + 4, vals_0.begin() + 0, vals_0.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<1>(vals_0_b), luts.MSB_HDB_B.begin() + std::get<1>(vals_0_b) + 4, vals_0.begin() + 4, vals_0.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<2>(vals_0_b), luts.MSB_HDB_B.begin() + std::get<2>(vals_0_b) + 4, vals_0.begin() + 8, vals_0.begin() + 8, std::plus<int16_t>());

        std::array<int16_t, 12> vals_90 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::transform(luts.MSB_HDB_H.begin() + std::get<0>(vals_90_h), luts.MSB_HDB_H.begin() + std::get<0>(vals_90_h) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_H.begin() + std::get<1>(vals_90_h), luts.MSB_HDB_H.begin() + std::get<1>(vals_90_h) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_H.begin() + std::get<2>(vals_90_h), luts.MSB_HDB_H.begin() + std::get<2>(vals_90_h) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<0>(vals_90_d), luts.MSB_HDB_D.begin() + std::get<0>(vals_90_d) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<1>(vals_90_d), luts.MSB_HDB_D.begin() + std::get<1>(vals_90_d) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<2>(vals_90_d), luts.MSB_HDB_D.begin() + std::get<2>(vals_90_d) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<0>(vals_90_b), luts.MSB_HDB_B.begin() + std::get<0>(vals_90_b) + 4, vals_90.begin() + 0, vals_90.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<1>(vals_90_b), luts.MSB_HDB_B.begin() + std::get<1>(vals_90_b) + 4, vals_90.begin() + 4, vals_90.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<2>(vals_90_b), luts.MSB_HDB_B.begin() + std::get<2>(vals_90_b) + 4, vals_90.begin() + 8, vals_90.begin() + 8, std::plus<int16_t>());
        rotateBack(vals_90);

        std::array<int16_t, 12> vals_180 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::transform(luts.MSB_HDB_H.begin() + std::get<0>(vals_180_h), luts.MSB_HDB_H.begin() + std::get<0>(vals_180_h) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_H.begin() + std::get<1>(vals_180_h), luts.MSB_HDB_H.begin() + std::get<1>(vals_180_h) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_H.begin() + std::get<2>(vals_180_h), luts.MSB_HDB_H.begin() + std::get<2>(vals_180_h) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<0>(vals_180_d), luts.MSB_HDB_D.begin() + std::get<0>(vals_180_d) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<1>(vals_180_d), luts.MSB_HDB_D.begin() + std::get<1>(vals_180_d) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<2>(vals_180_d), luts.MSB_HDB_D.begin() + std::get<2>(vals_180_d) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<0>(vals_180_b), luts.MSB_HDB_B.begin() + std::get<0>(vals_180_b) + 4, vals_180.begin() + 0, vals_180.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<1>(vals_180_b), luts.MSB_HDB_B.begin() + std::get<1>(vals_180_b) + 4, vals_180.begin() + 4, vals_180.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<2>(vals_180_b), luts.MSB_HDB_B.begin() + std::get<2>(vals_180_b) + 4, vals_180.begin() + 8, vals_180.begin() + 8, std::plus<int16_t>());
        rotateBack(vals_180);
        rotateBack(vals_180);

        std::array<int16_t, 12> vals_270 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::transform(luts.MSB_HDB_H.begin() + std::get<0>(vals_270_h), luts.MSB_HDB_H.begin() + std::get<0>(vals_270_h) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_H.begin() + std::get<1>(vals_270_h), luts.MSB_HDB_H.begin() + std::get<1>(vals_270_h) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_H.begin() + std::get<2>(vals_270_h), luts.MSB_HDB_H.begin() + std::get<2>(vals_270_h) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<0>(vals_270_d), luts.MSB_HDB_D.begin() + std::get<0>(vals_270_d) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<1>(vals_270_d), luts.MSB_HDB_D.begin() + std::get<1>(vals_270_d) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_D.begin() + std::get<2>(vals_270_d), luts.MSB_HDB_D.begin() + std::get<2>(vals_270_d) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<0>(vals_270_b), luts.MSB_HDB_B.begin() + std::get<0>(vals_270_b) + 4, vals_270.begin() + 0, vals_270.begin() + 0, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<1>(vals_270_b), luts.MSB_HDB_B.begin() + std::get<1>(vals_270_b) + 4, vals_270.begin() + 4, vals_270.begin() + 4, std::plus<int16_t>());
        std::transform(luts.MSB_HDB_B.begin() + std::get<2>(vals_270_b), luts.MSB_HDB_B.begin() + std::get<2>(vals_270_b) + 4, vals_270.begin() + 8, vals_270.begin() + 8, std::plus<int16_t>());
        rotateBack(vals_270);
        rotateBack(vals_270);
        rotateBack(vals_270);

        std::transform(vals_0.begin(), vals_0.end(), vals_90.begin(), vals_0.begin(), std::plus<int16_t>());
        std::transform(vals_0.begin(), vals_0.end(), vals_180.begin(), vals_0.begin(), std::plus<int16_t>());
        std::transform(vals_0.begin(), vals_0.end(), vals_270.begin(), vals_0.begin(), std::plus<int16_t>());
        std::transform(vals_0.begin(), vals_0.end(), vals_0.begin(), [](int16_t val){return val / 3; });

        for(size_t q = 0; q < 4; ++q)
        {
            resMSB[q] = std::make_tuple(static_cast<int16_t>(vals_0[q]), static_cast<int16_t>(vals_0[q + 4]), static_cast<int16_t>(vals_0[q + 8]));
        }
    }
}

std::vector<uint8_t> TEXLoader::toRGB(Ogre::Image& img) const
{
    size_t width = img.getWidth();
    size_t height = img.getHeight();

    std::vector<uint8_t> ret(width * height * 3);

    const uint16_t* data = reinterpret_cast<const uint16_t*>(img.getData());

    for(size_t y = 0; y < height; ++y)
    {
        for(size_t x = 0; x < width; ++x)
        {
            uint16_t val = data[y * width + x + 0];
            uint8_t r = (val & 0xF800) >> 8;
            uint8_t g = (val & 0x7E0) >> 3;
            uint8_t b = (val & 0x1F) << 3;

            ret[y * width * 3 + x * 3 + 0] = r;
            ret[y * width * 3 + x * 3 + 1] = g;
            ret[y * width * 3 + x * 3 + 2] = b;
        }
    }

    return ret;
}

std::vector<uint8_t> TEXLoader::AddPadding(const uint8_t * inBuf, size_t width, size_t height, size_t top, size_t bottom, size_t left, size_t right) const
{

    size_t sum_hor_pad = left + right;
    size_t sum_vert_pad = top + bottom;

    size_t stride_width = (width + sum_hor_pad) * 3;

    std::vector<uint8_t> srcPadded(stride_width * (height + sum_vert_pad));

    //main, left, right
    for(size_t q = 0; q < height; ++q){

        //main
        std::memcpy(srcPadded.data() + (q + top) * stride_width + left * 3,
            inBuf + q * width * 3, width * 3);

        //left
        std::memset(srcPadded.data() + (q + top) * stride_width,
            inBuf[q * width * 3], left * 3);

        //right
        std::memset(srcPadded.data() + (q + top) * stride_width + left * 3 + width * 3,
            inBuf[q * width * 3 + (width - 1) * 3], right * 3);
    }

    //top
    for(size_t q = 0; q < top; ++q) {
        std::memcpy(srcPadded.data() + q * stride_width,
            srcPadded.data() + top * stride_width,
            stride_width);
    }

    //bottom
    for(size_t q = top + height; q < height + sum_vert_pad; ++q) {
        std::memcpy(srcPadded.data() + q * stride_width,
            srcPadded.data() + (top + height - 1) * stride_width,
            stride_width);
    }

    return srcPadded;
}

void TEXLoader::doLUTUpscale(Ogre::Image& img, const LUTs& luts, bool convertoRGB, bool swapRGB) const
{
    size_t width = img.getWidth();
    size_t height = img.getHeight();

    size_t scaleFactor = 2;

    size_t dest_height = height * scaleFactor;
    size_t dest_width = width * scaleFactor;

    size_t bufSize = Ogre::PixelUtil::getMemorySize(dest_width, dest_height, 1, Ogre::PF_R8G8B8);
    Ogre::uchar* pixelData = OGRE_ALLOC_T(Ogre::uchar, bufSize, Ogre::MEMCATEGORY_GENERAL);

    Ogre::Image img2;
    img2.loadDynamicImage(pixelData, dest_width, dest_height, 1, Ogre::PF_R8G8B8, true);

    std::vector<uint8_t> rgbImgPadded;
    if(convertoRGB) rgbImgPadded = AddPadding(toRGB(img).data(), width, height, 2, 2, 2, 2);
    else rgbImgPadded = AddPadding(img.getData(), width, height, 2, 2, 2, 2);
    size_t paddedWidth = width + 4;

    size_t indexRGBSelection_r = 0, indexRGBSelection_g = 1, indexRGBSelection_b = 2;
    if(swapRGB) std::swap(indexRGBSelection_r, indexRGBSelection_b);

#if defined(__ANDROID__)
    size_t threadsAmount = 4;
#else
    size_t threadsAmount = 8;
#endif

#define LUT_THREADS

    Ogre::uchar * dataRes = img2.getData();
    {
#if defined(LUT_THREADS)
        thread_pool tPool(threadsAmount);

        auto upscaleLambda = [&](size_t y_start, size_t y_end){

            for(size_t y = y_start; y < y_end; ++y)
#else
            for(size_t y = 0; y < height; ++y)
#endif
            {
                for(size_t x = 0; x < width; ++x)
                {
                    Pixel orig = getPixel(x, y, rgbImgPadded.data() + paddedWidth * 3 * 2 + 2 * 3, paddedWidth);
            
                    PixelSigned resLSB[4];
                    PixelSigned resMSB[4];
                    //doLSB(x, y, rgbImgPadded.data() + paddedWidth * 3 * 2 + 2 * 3, paddedWidth, luts, &resLSB[0]);
                    //doMSB(x, y, rgbImgPadded.data() + paddedWidth * 3 * 2 + 2 * 3, paddedWidth, luts, &resMSB[0]);
                    doLSBMSB(x, y, rgbImgPadded.data() + paddedWidth * 3 * 2 + 2 * 3, paddedWidth, luts, &resLSB[0], &resMSB[0]);

                    int16_t vals[4][3];
                    for(size_t q = 0; q < 4; ++q)
                    {
                        vals[q][0] = clamp(std::get<0>(orig) + std::get<0>(resLSB[q]) + std::get<0>(resMSB[q]), 0, 255);
                        vals[q][1] = clamp(std::get<1>(orig) + std::get<1>(resLSB[q]) + std::get<1>(resMSB[q]), 0, 255);
                        vals[q][2] = clamp(std::get<2>(orig) + std::get<2>(resLSB[q]) + std::get<2>(resMSB[q]), 0, 255);
                    }

                    dataRes[y * 2 * dest_width * 3 + x * 2 * 3 + 0] = static_cast<Ogre::uchar>(vals[0][indexRGBSelection_r]);
                    dataRes[y * 2 * dest_width * 3 + x * 2 * 3 + 1] = static_cast<Ogre::uchar>(vals[0][indexRGBSelection_g]);
                    dataRes[y * 2 * dest_width * 3 + x * 2 * 3 + 2] = static_cast<Ogre::uchar>(vals[0][indexRGBSelection_b]);
                    dataRes[y * 2 * dest_width * 3 + (x * 2 + 1) * 3 + 0] = static_cast<Ogre::uchar>(vals[1][indexRGBSelection_r]);
                    dataRes[y * 2 * dest_width * 3 + (x * 2 + 1) * 3 + 1] = static_cast<Ogre::uchar>(vals[1][indexRGBSelection_g]);
                    dataRes[y * 2 * dest_width * 3 + (x * 2 + 1) * 3 + 2] = static_cast<Ogre::uchar>(vals[1][indexRGBSelection_b]);
                    dataRes[(y * 2 + 1) * dest_width * 3 + x * 2 * 3 + 0] = static_cast<Ogre::uchar>(vals[2][indexRGBSelection_r]);
                    dataRes[(y * 2 + 1) * dest_width * 3 + x * 2 * 3 + 1] = static_cast<Ogre::uchar>(vals[2][indexRGBSelection_g]);
                    dataRes[(y * 2 + 1) * dest_width * 3 + x * 2 * 3 + 2] = static_cast<Ogre::uchar>(vals[2][indexRGBSelection_b]);
                    dataRes[(y * 2 + 1) * dest_width * 3 + (x * 2 + 1) * 3 + 0] = static_cast<Ogre::uchar>(vals[3][indexRGBSelection_r]);
                    dataRes[(y * 2 + 1) * dest_width * 3 + (x * 2 + 1) * 3 + 1] = static_cast<Ogre::uchar>(vals[3][indexRGBSelection_g]);
                    dataRes[(y * 2 + 1) * dest_width * 3 + (x * 2 + 1) * 3 + 2] = static_cast<Ogre::uchar>(vals[3][indexRGBSelection_b]);
                }
            }
#if defined(LUT_THREADS)
        };

        size_t batch = height / threadsAmount;

        for(size_t q = 0; q < threadsAmount; ++q)
        {
            size_t start = q * batch;
            size_t end = (q + 1) * batch;
            tPool.enqueue(upscaleLambda, start, end);
        }
#endif
    }

    std::swap(img, img2);
}

Ogre::TexturePtr TEXLoader::load(const Ogre::DataStreamPtr& fileToLoad, const std::string& texturename, const LUTs& lutsX2, const LUTs& lutsX4, const Ogre::String& group, Ogre::Real gamma, bool doUpscale) const
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
        if(doUpscale)
        {
            std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();

            doLUTUpscale(img, lutsX2, true, true);
            //doLUTUpscale(img, lutsX4, false, false);

            long long timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeStart).count();
            float ms = static_cast<float>(timeTaken) / 1000.0f;
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TEXLoader::load]: [" + texturename + "] upscale time " + Conversions::DMToString(ms) + "ms");

#if defined(__ANDROID__)
            LOGI("TEXLoader[load]: %s upscale time %.2fms", texturename.c_str(), ms);
#endif

            //img.save("1/x2/" + texturename + ".jpg");
        }

        res = Ogre::TextureManager::getSingleton().loadImage(texturename, group, img, Ogre::TEX_TYPE_2D);

        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "[TEXLoader::load]: [" + texturename + "]");
    }

    return res;
}

Ogre::TexturePtr TEXLoader::load(const PFLoader& pfLoader, const std::string& subfolder, const std::string& filename, const std::string& texturename, const LUTs& lutsX2, const LUTs& lutsX4, const Ogre::String& group, Ogre::Real gamma, bool doUpscale) const
{
    Ogre::TexturePtr res;
    Ogre::DataStreamPtr fileToLoad = pfLoader.getFile(subfolder, filename);
    if(fileToLoad.get() && fileToLoad->isReadable())
    {
        res = load(fileToLoad, texturename, lutsX2, lutsX4, group, gamma, doUpscale);
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
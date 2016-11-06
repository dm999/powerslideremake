#include <gtest/gtest.h>

#if !defined(__ANDROID__)
#if 0

#include <boost/thread/detail/config.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

int test_value;

void simple_thread()
{
    test_value = 999;
}


void interruption_point_thread(boost::mutex* m,bool* failed)
{
    boost::mutex::scoped_lock lk(*m);
    boost::this_thread::interruption_point();
    *failed=true;
}

namespace UT
{
    TEST(Boost, do_test_creation)
    {
        test_value = 0;
        boost::thread thrd(&simple_thread);
        thrd.join();
        EXPECT_EQ(test_value, 999);
    }

    TEST(Boost, do_test_thread_interrupts_at_interruption_point)
    {
        boost::mutex m;
        bool failed=false;
        boost::mutex::scoped_lock lk(m);
        boost::thread thrd(boost::bind(&interruption_point_thread,&m,&failed));
        thrd.interrupt();
        lk.unlock();
        thrd.join();
        EXPECT_EQ(failed, false);

        failed=false;
        boost::thread thrd2(boost::bind(&interruption_point_thread,&m,&failed));
        thrd2.join();
        EXPECT_EQ(failed, true);
    }
}
#endif
#endif
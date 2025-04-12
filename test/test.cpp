
#include "starray.h"
#include "dynarray.h"

#include "gtest/gtest.h"


namespace clb 
{
    /************************************************************************/
    /* starray tests                                                        */
    /************************************************************************/

    namespace starray_test
    {

        static constexpr clb::uint32 StArrayCount = 128;

        TEST(starray, empty)
        {
            starray<uint32, StArrayCount> c;
            EXPECT_EQ(c.getSize(), 0);
            EXPECT_EQ(c.getCapacity(), StArrayCount);
            EXPECT_TRUE(c.isEmpty());
            EXPECT_FALSE(c.isValidIndex(0));
        }

        TEST(starray, resize)
        {
            static constexpr uint32 ResizeCount = StArrayCount>>1;
            static_assert(ResizeCount <= StArrayCount);

            starray<uint32, StArrayCount> c;
            c.resize(ResizeCount);
            EXPECT_EQ(c.getSize(), ResizeCount);
            EXPECT_EQ(c.getCapacity(), StArrayCount);
            ASSERT_LE(c.getSize(), c.getCapacity());
            ASSERT_FALSE(c.isEmpty());
            for (uint32 i = 0; i < c.getSize(); ++i)
            {
                ASSERT_TRUE(c.isValidIndex(i));
                *(c.getData() + i) = i;
                EXPECT_EQ(c[i], i);
            }
            c.clear();
            EXPECT_EQ(c.getSize(), 0);
            EXPECT_TRUE(c.isEmpty());
        }

        TEST(starray, push_back_pop)
        {
            starray<uint32, StArrayCount> c;
            ASSERT_TRUE(c.isEmpty());
            for (uint32 i = 0; i < StArrayCount; ++i)
            {
                c.pushBack(i);
                ASSERT_TRUE(c.isValidIndex(i));
                EXPECT_EQ(*(c.getData() + i), i);
                EXPECT_EQ(c[i], i);
                EXPECT_EQ(c.getSize(), i+1);
            }

            uint32 j = StArrayCount-1;
            while (!c.isEmpty())
            {
                EXPECT_EQ(c.getSize(), j+1);
                EXPECT_EQ(c[j], j);
                EXPECT_EQ(*(c.getData()+j), j);
                EXPECT_EQ(c.getBack(), j);
                c.popBack();
                --j;
            }
            EXPECT_TRUE(c.isEmpty());
        }
    }

    /************************************************************************/
    /* dynarray tests                                                       */
    /************************************************************************/

    namespace dynarray_test
    {
        static constexpr uint32 ReserveCount = 1024;

        TEST(dynarray, empty)
        {
            dynarray<uint32> c;
            EXPECT_TRUE(c.isEmpty());
            EXPECT_EQ(c.getSize(), 0);
            EXPECT_EQ(c.getCapacity(), 0);
        }

        TEST(dynarray, reserve)
        {
            dynarray<uint32> c;
            ASSERT_TRUE(c.isEmpty());

            // increase memory with reserve
            c.reserve(ReserveCount);
            ASSERT_TRUE(c.isEmpty());
            EXPECT_EQ(c.getCapacity(), ReserveCount);
            for (uint32 i = 0; i < c.getCapacity(); ++i)
            {
                c.pushBack(i);
                EXPECT_EQ(*(c.getData() + i), i);
                EXPECT_EQ(c.getSize(), i+1);
            }

            // decrease memory with reserve
            uint32 newCapacity = c.getCapacity() >> 1;
            c.reserve(newCapacity);
            EXPECT_EQ(c.getSize(), c.getCapacity());

            c.clear();
            EXPECT_TRUE(c.isEmpty());
            EXPECT_EQ(c.getCapacity(), newCapacity);
            c.shrink();
            EXPECT_EQ(c.getCapacity(), 0);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
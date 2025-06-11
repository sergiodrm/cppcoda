
#include "starray.h"
#include "dynarray.h"
#include "codastring.h"
#include "hashtable.h"

#include "gtest/gtest.h"


namespace coda
{
	namespace starray_test
	{
		/************************************************************************/
		/* starray tests                                                        */
		/************************************************************************/

		static constexpr coda::uint32 StArrayCount = 128;

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
			static constexpr uint32 ResizeCount = StArrayCount >> 1;
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
				EXPECT_EQ(c.getSize(), i + 1);
			}

			uint32 j = StArrayCount - 1;
			while (!c.isEmpty())
			{
				EXPECT_EQ(c.getSize(), j + 1);
				EXPECT_EQ(c[j], j);
				EXPECT_EQ(*(c.getData() + j), j);
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
		static constexpr uint32 ReserveCount = 8;

		static uint32 allocCounter = 0;
		static uint32 reallocCounter = 0;
		static uint32 releaseCounter = 0;

		void cleanStats()
		{
			allocCounter = 0;
			reallocCounter = 0;
			releaseCounter = 0;
		}

		class test_allocator
		{
		public:
			static void* allocate(size_t size)
			{
				++allocCounter;
				return baseallocator::allocate(size);
			}

			static void* reallocate(void* p, size_t size)
			{
				++reallocCounter;
				return baseallocator::reallocate(p, size);
			}

			static void release(void* p)
			{
				++releaseCounter;
				baseallocator::release(p);
			}
		};

		TEST(dynarray, empty)
		{

			dynarray<uint32> c;
			EXPECT_TRUE(c.isEmpty());
			EXPECT_EQ(c.getSize(), 0);
			EXPECT_EQ(c.getCapacity(), 0);
		}

		TEST(dynarray, reserve)
		{
			cleanStats();
			dynarray<uint32, test_allocator> c;
			ASSERT_TRUE(c.isEmpty());

			// increase memory with reserve
			c.reserve(ReserveCount);
			ASSERT_TRUE(c.isEmpty());
			EXPECT_EQ(allocCounter, 1);
			EXPECT_EQ(releaseCounter, 0);
			EXPECT_EQ(reallocCounter, 0);
			EXPECT_EQ(c.getCapacity(), ReserveCount);
			for (uint32 i = 0; i < c.getCapacity(); ++i)
			{
				c.pushBack(i);
				EXPECT_EQ(*(c.getData() + i), i);
				EXPECT_EQ(c.getSize(), i + 1);
			}
			EXPECT_EQ(allocCounter, 1);
			EXPECT_EQ(releaseCounter, 0);
			EXPECT_EQ(reallocCounter, 0);

			// decrease memory with reserve
			uint32 newCapacity = c.getCapacity() >> 1;
			c.reserve(newCapacity);
			EXPECT_EQ(c.getSize(), c.getCapacity());
			EXPECT_EQ(allocCounter, 1);
			EXPECT_EQ(reallocCounter, 1);
			EXPECT_EQ(releaseCounter, 0);

			c.clear();
			EXPECT_TRUE(c.isEmpty());
			EXPECT_EQ(c.getCapacity(), newCapacity);
			c.shrink();
			EXPECT_EQ(releaseCounter, 1);
			EXPECT_EQ(c.getCapacity(), 0);
		}

		/************************************************************************/
		/* String tests                                                         */
		/************************************************************************/

		TEST(string, constructor) {
			coda::string str;
			EXPECT_EQ(str.c_str(), nullptr);
			EXPECT_EQ(str.getLength(), 0);
		}

		TEST(string, copyConstructor) {
			coda::string str1("Hello");
			coda::string str2(str1);
			EXPECT_STREQ(str2.c_str(), "Hello");
			EXPECT_EQ(str2.getLength(), 5);
		}

		TEST(string, moveConstructor) {
			coda::string str1("Hello");
			coda::string str2(std::move(str1));
			EXPECT_STREQ(str2.c_str(), "Hello");
			EXPECT_EQ(str2.getLength(), 5);
			EXPECT_EQ(str1.c_str(), nullptr);
		}

		TEST(string, assignmentOperator) {
			coda::string str1("Hello");
			coda::string str2;
			str2 = str1;
			EXPECT_STREQ(str2.c_str(), "Hello");
			EXPECT_EQ(str2.getLength(), 5);
		}

		TEST(string, moveAssignmentOperator) {
			coda::string str1("Hello");
			coda::string str2;
			str2 = std::move(str1);
			EXPECT_STREQ(str2.c_str(), "Hello");
			EXPECT_EQ(str2.getLength(), 5);
			EXPECT_EQ(str1.c_str(), nullptr);
		}

		TEST(string, setString) {
			coda::string str;
			str.set("Hello");
			EXPECT_STREQ(str.c_str(), "Hello");
			EXPECT_EQ(str.getLength(), 5);
		}

		TEST(string, clear) {
			coda::string str("Hello");
			const void* data = str.c_str();
			size_t s = str.getCapacity();
			str.clear();
			EXPECT_EQ(str.c_str(), data);
			EXPECT_EQ(str.getLength(), 0);
			EXPECT_EQ(str.getCapacity(), s);
			str.clear(true);
			EXPECT_EQ(str.c_str(), nullptr);
			EXPECT_EQ(str.getLength(), 0);
			EXPECT_EQ(str.getCapacity(), 0);
		}

		/************************************************************************/
		/* Hash function test                                                   */
		/************************************************************************/

		TEST(hashtable, construct)
		{
			coda::hashtable<uint64, float> h;
			EXPECT_FALSE(h.contains(3));
			h.createItem(3, 5.f);
			EXPECT_TRUE(h.contains(3));
			coda::hashtableitemid id = h.findId(3);
			float* value = h.findItem(3);
			EXPECT_TRUE(value == h.getById(id) && *value == 5.f);
		}

		TEST(hashtable, fill)
		{
			coda::hashtable<uint32, float> h;

			for (uint32 i = 0; i < h.getSize(); ++i)
			{
				float r = 0.2f * sinf(i) * (rand() % h.getSize());
				float* p = h.createItem(i, r);
				coda::hashtableitemid id = h.findId(i);
				EXPECT_TRUE(id.id != coda::hashtable_invalidId);
				EXPECT_TRUE(h.getById(id) == p);
				EXPECT_TRUE(h.findItem(i) == p);
			}
		}
	}
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
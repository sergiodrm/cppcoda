#pragma once

#include "common.h"
#include "allocator.h"
#include <xhash>

namespace coda
{
    template <typename T>
    uint64 hash_function(const T& k)
    {
        return std::hash<T>()(k);
    }

    enum { hashtable_invalidId = 0x7fffffff };
    union hashtableitemid
    {
        struct
        {
            uint32 bucketId;
            uint32 itemId;
        };
        uint64 id;
    };

    template <typename U, typename T, typename size_type>
    struct hashtablebucket
    {
        U* keys = nullptr;
        T* items = nullptr;
        size_type size = 0;
        size_type count = 0;
        size_type next = TypeLimit<size_type>::max();
        uint64 usedFlags = 0;
    };

    template <typename KeyType, typename ItemType, typename AllocatorType = coda::baseallocator, typename size_type = uint32>
    class hashtable
    {
        static constexpr size_type invalidIndex = TypeLimit<size_type>::max();
    public:
        typedef hashtablebucket<KeyType, ItemType, size_type> buckettype;

        hashtable(size_type _size = 1024);
        ~hashtable();

        ItemType* createItem(const KeyType& key, const ItemType& item);
        hashtableitemid findId(const KeyType& key) const;
        ItemType* getById(hashtableitemid id) const;
        ItemType* findItem(const KeyType& key) const;
        bool contains(const KeyType& key) const;
        void destroyItem(const KeyType& key);

        float getLoadFactor() const;

        size_type getSize() const { return size; }
        size_type getCount() const { return count; }

    private:

        size_type getIndex(const KeyType& key) const;
        void linkItem(size_type index);


    private:
        buckettype* buckets;
        size_type count;
        size_type size;
        size_type first;
    };

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline hashtable<KeyType, ItemType, AllocatorType, size_type>::hashtable(size_type _size)
        : buckets(nullptr), count(0), size(_size), first(invalidIndex)
    {
        // todo: check is valid size

        buckets = (buckettype*)AllocatorType::allocate(size * sizeof(buckettype));
        for (size_type i = 0; i < size; ++i)
            new (&buckets[i]) buckettype();
    }

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline hashtable<KeyType, ItemType, AllocatorType, size_type>::~hashtable()
    {
        size_type it = first;
        while (it != invalidIndex)
        {
            for (size_type i = 0; i < buckets[it].count; ++i)
            {
                buckets[it].items[i].~ItemType();
                buckets[it].keys[i].~KeyType();
            }
            AllocatorType::release(buckets[it].items);
            AllocatorType::release(buckets[it].keys);
            it = buckets[it].next;
        }
        AllocatorType::release(buckets);
    }

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline ItemType* hashtable<KeyType, ItemType, AllocatorType, size_type>::createItem(const KeyType& key, const ItemType& item)
    {
        coda_assert(count < size);
        size_type i = getIndex(key);
        ItemType* ret = nullptr;
        buckettype& bucket = buckets[i];
        // no collision case
        if (!bucket.items)
        {
            bucket.size = 3;
            bucket.count = 1;
            bucket.items = (ItemType*)AllocatorType::allocate(sizeof(ItemType) * bucket.size);
            bucket.keys = (KeyType*)AllocatorType::allocate(sizeof(KeyType) * bucket.size);
            bucket.usedFlags = 0x1;
            ret = bucket.items;
            new (ret) ItemType(item);
            new (bucket.keys) KeyType(key);
            linkItem(i);
        }
        else
        {
            size_type freeSlot = invalidIndex;
            // collision case
            if (bucket.count == bucket.size)
			{
                // search for free slot
                for (size_type i = 0; i < bucket.count; ++i)
                {
                    if (!(bucket.usedFlags & (1i64 << i)))
                    {
                        freeSlot = i;
                        break;
                    }
                }

                if (freeSlot == invalidIndex)
                {
                    // reallocate
                    ++bucket.size;
                    coda_assert(bucket.size <= sizeof(bucket.usedFlags) * 8);
                    ItemType* items = (ItemType*)AllocatorType::reallocate(bucket.items, sizeof(ItemType) * bucket.size);
                    coda_assert(items);
                    bucket.items = items;

                    KeyType* keys = (KeyType*)AllocatorType::reallocate(bucket.keys, sizeof(KeyType) * bucket.size);
                    coda_assert(keys);
                    bucket.keys = keys;

                    freeSlot = bucket.count;
                    ++bucket.count;
                }
            }
            else
            {
                freeSlot = bucket.count++;
            }
            coda_assert(freeSlot != invalidIndex);
            coda_assert(freeSlot < bucket.size);
            ret = &bucket.items[freeSlot];
            new (ret) ItemType(item);
            new (&bucket.keys[freeSlot]) KeyType(key);
            bucket.usedFlags |= (1i64 << (freeSlot));
        }
        coda_assert(ret);
        ++count;
        return ret;
    }

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline hashtableitemid hashtable<KeyType, ItemType, AllocatorType, size_type>::findId(const KeyType& key) const
    {
        hashtableitemid id = {hashtable_invalidId};
        size_type index = getIndex(key);
        if (buckets[index].items)
        {
            id.bucketId = index;
            if (buckets[index].count > 1)
            {
                for (size_type i = 0; i < buckets[index].count; ++i)
                {
                    if (key == buckets[index].keys[i])
                    {
                        id.itemId = i;
                        coda_assert(buckets[index].usedFlags & (1i64 << i));
                        return id;
                    }
                }
            }
            else
            {
                if (key == buckets[index].keys[0])
                {
                    id.itemId = 0;
                    coda_assert(buckets[index].usedFlags & 1i64);
                    return id;
                }
            }
        }
        return {hashtable_invalidId};
    }

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline ItemType* hashtable<KeyType, ItemType, AllocatorType, size_type>::getById(hashtableitemid id) const
    {
        if (id.id != hashtable_invalidId && id.bucketId < size)
        {
            const buckettype& bucket = buckets[id.bucketId];
            if (id.itemId < bucket.count && bucket.usedFlags & (1i64 <<id.itemId))
            {
                return &bucket.items[id.itemId];
            }
        }
        return nullptr;
    }

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline ItemType* hashtable<KeyType, ItemType, AllocatorType, size_type>::findItem(const KeyType& key) const
    {
        hashtableitemid id = findId(key);
        return getById(id);
    }

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline bool hashtable<KeyType, ItemType, AllocatorType, size_type>::contains(const KeyType& key) const
    {
        hashtableitemid id = findId(key);
        return id.id != hashtable_invalidId;
    }

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline void hashtable<KeyType, ItemType, AllocatorType, size_type>::destroyItem(const KeyType& key)
    {
        hashtableitemid id = findId(key);
        if (id.id != hashtable_invalidId)
        {
            buckettype& bucket = buckets[id.bucketId];
            coda_assert(bucket.size > id.itemId);
            coda_assert(bucket.usedFlags & (1i64 << id.itemId));
            ItemType* item = &bucket.items[id.itemId];
            item->~ItemType();
            bucket.usedFlags &= ~(1i64 << id.itemId);
            coda_assert(count);
            --count;
        }
    }

	template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
	inline float hashtable<KeyType, ItemType, AllocatorType, size_type>::getLoadFactor() const
	{
        return count / size;
	}

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline size_type hashtable<KeyType, ItemType, AllocatorType, size_type>::getIndex(const KeyType& key) const
    {
		uint64 h = hash_function(key);
		return safe_cast<size_type>(h % size);
    }

    template<typename KeyType, typename ItemType, typename AllocatorType, typename size_type>
    inline void hashtable<KeyType, ItemType, AllocatorType, size_type>::linkItem(size_type index)
    {
        coda_assert(index < size && buckets[index].next == invalidIndex);
        if (first != invalidIndex)
        {
            size_type it = first;
            while (buckets[it].next != invalidIndex && index > buckets[it].next)
                it = buckets[it].next;
            coda_assert(it < size);
            buckets[index].next = buckets[it].next;
            buckets[it].next = index;
        }
        else
            first = index;
    }

}
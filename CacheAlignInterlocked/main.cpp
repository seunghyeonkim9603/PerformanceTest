#include <Windows.h>
#include <process.h>
#include <iostream>
#include <unordered_map>

#define PROFILE

#include "Profiler.h"
#include "ProfilerManager.h"

#define CACHE_ALIGN __declspec(align(64))

#define NUM_THREAD (4) //Set num cores
#define NUM_TEST (10000)
#define NUM_INTERLOCKED_PER_TEST (1000)

#define PROFILE_FILE_NAME (L"Interlocked_Align_Test.txt")

#define ALIGNED_TAG (L"Cache_Aligned")
#define UNALIGNED_TAG (L"Cache_Unaligned")

struct AlignedData
{
    CACHE_ALIGN INT16 Num1;
    CACHE_ALIGN INT16 Num2;
    CACHE_ALIGN INT16 Num3;
    CACHE_ALIGN INT16 Num4;
};

struct UnalignedData
{
    INT16 Num1;
    INT16 Num2;
    INT16 Num3;
    INT16 Num4;
};

const WCHAR* gProfileTag;

unsigned int __stdcall InterlockedAlignTest(void* param);

int main()
{
    AlignedData     aligned;
    UnalignedData   unaligned;

    HANDLE hThreads[NUM_THREAD];

    gProfileTag = ALIGNED_TAG;
    {
        hThreads[0] = (HANDLE)_beginthreadex(nullptr, 0, InterlockedAlignTest, &aligned.Num1, 0, nullptr);
        hThreads[1] = (HANDLE)_beginthreadex(nullptr, 0, InterlockedAlignTest, &aligned.Num2, 0, nullptr);
        hThreads[2] = (HANDLE)_beginthreadex(nullptr, 0, InterlockedAlignTest, &aligned.Num3, 0, nullptr);
        hThreads[3] = (HANDLE)_beginthreadex(nullptr, 0, InterlockedAlignTest, &aligned.Num4, 0, nullptr);

        WaitForMultipleObjects(NUM_THREAD, hThreads, true, INFINITE);

        for (int i = 0; i < NUM_THREAD; ++i)
        {
            CloseHandle(hThreads[i]);
        }
    }

    gProfileTag = UNALIGNED_TAG;
    {
        hThreads[0] = (HANDLE)_beginthreadex(nullptr, 0, InterlockedAlignTest, &unaligned.Num1, 0, nullptr);
        hThreads[1] = (HANDLE)_beginthreadex(nullptr, 0, InterlockedAlignTest, &unaligned.Num2, 0, nullptr);
        hThreads[2] = (HANDLE)_beginthreadex(nullptr, 0, InterlockedAlignTest, &unaligned.Num3, 0, nullptr);
        hThreads[3] = (HANDLE)_beginthreadex(nullptr, 0, InterlockedAlignTest, &unaligned.Num4, 0, nullptr);

        WaitForMultipleObjects(NUM_THREAD, hThreads, true, INFINITE);

        for (int i = 0; i < NUM_THREAD; ++i)
        {
            CloseHandle(hThreads[i]);
        }
    }

    PROFILES_PRINT(PROFILE_FILE_NAME);

    return 0;
}

unsigned int __stdcall InterlockedAlignTest(void* param)
{
    INT16* num = (INT16*)param;

    for (int i = 0; i < NUM_TEST; ++i)
    {
        PROFILE_BEGIN(gProfileTag);
        {
            for (int j = 0; j < NUM_INTERLOCKED_PER_TEST; ++j)
            {
                InterlockedIncrement16(num);
            }
        }
        PROFILE_END(gProfileTag);
    }
    return 0;
}
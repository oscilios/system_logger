#include "system_logger/StringRingBuffer.h"
#include <thread>
#include <iostream>

using namespace system_logger;

int main()
{
    using StringBuffer = system_logger::threadsafe::StringRingBuffer<char, 32, 1024>;
    StringBuffer buffer;
    int threadCount = 11;
    std::vector<std::string> outputs(threadCount);
    std::vector<std::string> inputs({
        "AaA_AaA_AaA_AaA_AaA_AaA_AaA_AaA_AaA_AaA_AaA_AaA_AaA_AaA",
        "BbB_BbB_BbB_BbB_BbB_BbB_BbB_BbB_BbB_BbB_BbB_BbB_BbB_BbB",
        "CcC_CcC_CcC_CcC_CcC_CcC_CcC_CcC_CcC_CcC_CcC_CcC_CcC_CcC",
        "DdD_DdD_DdD_DdD_DdD_DdD_DdD_DdD_DdD_DdD_DdD_DdD_DdD_DdD",
        "EeE_EeE_EeE_EeE_EeE_EeE_EeE_EeE_EeE_EeE_EeE_EeE_EeE_EeE",
        "FfF_FfF_FfF_FfF_FfF_FfF_FfF_FfF_FfF_FfF_FfF_FfF_FfF_FfF",
        "GgG_GgG_GgG_GgG_GgG_GgG_GgG_GgG_GgG_GgG_GgG_GgG_GgG_GgG",
        "HhH_HhH_HhH_HhH_HhH_HhH_HhH_HhH_HhH_HhH_HhH_HhH_HhH_HhH",
        "IiI_IiI_IiI_IiI_IiI_IiI_IiI_IiI_IiI_IiI_IiI_IiI_IiI_IiI",
        "JjJ_JjJ_JjJ_JjJ_JjJ_JjJ_JjJ_JjJ_JjJ_JjJ_JjJ_JjJ_JjJ_JjJ",
        "KkK_KkK_KkK_KkK_KkK_KkK_KkK_KkK_KkK_KkK_KkK_KkK_KkK_KkK",
    });
    for (int i = 0; i < 100; i++)
    {
        std::vector<std::thread> threads;
        for (int t = 0; t < threadCount; t++)
        {
            threads.emplace_back([&buffer, &inputs, t]() { buffer.write(inputs[t]); });
            threads.emplace_back([&buffer, &outputs, t]() { buffer.read(outputs[t]); });
        }
        for (auto& t : threads)
        {
            t.join();
        }
        for (auto& output : outputs)
        {
            if (!output.empty() && std::find(outputs.begin(), outputs.end(), output) == outputs.end())
            {
                std::cout << "FAILED: output " << output << " not found." << std::endl;
                return -1;
            }
        }
    }
    std::cout << "PASSED" << std::endl;
    return 0;
}

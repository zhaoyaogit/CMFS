#include "plugin/CMarketDataManager.h"
#include <regex>
#include <fstream>

CMarketDataManager::CMarketDataManager()
{
    auto load_cnt = LoadInstrumentFromCSV();
    SPDLOG_INFO("==========  load {} instruments from file", load_cnt);
}

CMarketDataManager::~CMarketDataManager()
{
    
}



size_t CMarketDataManager::LoadInstrumentFromCSV(const std::string& filePath)
{
    size_t instr_count = 0;


    std::ifstream ifs(filePath);
    if (!ifs)
    {
        SPDLOG_ERROR("Failed to find csv file in path: {}", filePath);
        return 0;
    }

    std::string instrumentID;
    char MarketType;
    std::string expireDate;
    std::string exchangeID;

    while (ifs >> instrumentID >> MarketType >> expireDate >> exchangeID)
    {
        SPDLOG_INFO("load instrument: id {}, marketType {}, expireDate {}, exchangeID {}", 
            instrumentID, MarketType, expireDate, exchangeID);
            
        m_MarketDataMap.try_emplace(instrumentID, instrumentID, exchangeID);
        instr_count++;
    }

    return instr_count;
}

void CMarketDataManager::GetAllValidMarketData(std::set<CThostFtdcDepthMarketDataField, MarketDataCmp> &dataSet) 
{
    for (auto& data: m_MarketDataMap)
    {
        dataSet.insert(data.second.data);
    }
}

int CMarketDataManager::SubscribeByInstrumentID(int16_t index, const std::string &instrumentID, std::set<CThostFtdcDepthMarketDataField, MarketDataCmp> &dataSet)
{
    ///
    auto iter = m_MarketDataMap.find(instrumentID);
    if (iter == m_MarketDataMap.end())
    {
        SPDLOG_ERROR("Failed to find such instrument: {}", instrumentID);
        return -1;
    }

    dataSet.insert(iter->second.data);
    iter->second.subscribers.set(index);

    return 1;
}

int CMarketDataManager::SubscribeByRule(
    int16_t index, const std::string &rule, std::set<CThostFtdcDepthMarketDataField, MarketDataCmp> &dataSet)
{
    int cnt = 0;
    for (auto &marketData : m_MarketDataMap)
    {
        std::regex e(rule);
        std::smatch sm;
        auto if_fit = std::regex_search(marketData.first, sm, e);
        if (!if_fit)
            continue;
        cnt ++;
        dataSet.insert(marketData.second.data);
        marketData.second.subscribers.set(index);
    }
    return cnt;
}
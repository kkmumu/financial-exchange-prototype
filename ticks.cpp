#include "ticks.h"

bool TickSizeRule::FromJson(const nlohmann::json &tick_json)
{
    ticks.reserve(tick_json.size());
    
    for (const auto &input_tick : tick_json.items())
    {
        Tick tick;
        for (const auto &item : input_tick.value().items())
        {
            if (item.key() == "from_price")
            {
                tick.from_price = Price4(item.value().get<std::string>());
            }
            if (item.key() == "to_price")
            {
                tick.to_price = Price4(item.value().get<std::string>());
            }
            if (item.key() == "tick_size")
            {
                tick.tick_size = item.value().get<t_tick>();
            }
        }
        ticks.push_back(tick);
    }

    std::sort(ticks.begin(), ticks.end(), [](const Tick &lhs, const Tick &rhs){return lhs.from_price < rhs.from_price;});

    for (std::size_t i = 0; i < ticks.size() - 1; ++i)
    {
        if (ticks[i].from_price >= ticks[i].to_price)
        {
            return false;
        }
        if (ticks[i].to_price != ticks[i + 1].from_price)
        {
            return false;
        }
    }

    return ticks.empty() ? false : true;
}

#include <iostream>
#include <array>

#include "Data.h"
#include <unordered_map>
#include <unordered_set>
#include "BS_thread_pool.hpp"

using namespace std;

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
    }
};

vector<int> knapsack(int W, int n, const std::vector<int> &weights, unordered_map<int, int> &values)
{
    vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

    for (int i = 1; i <= n; i++)
    {
        for (int j = 0; j <= W; j++)
        {
            if (weights[i - 1] <= j)
            {
                dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - weights[i - 1]] + values[i - 1]);
            }
            else
            {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }

    vector<int> selected_items;
    int w = W;
    for (int i = n; i > 0 && w > 0; i--)
    {
        if (dp[i][w] != dp[i - 1][w])
        {
            selected_items.push_back(i - 1);
            w -= weights[i - 1];
        }
    }

    return selected_items;
}

unordered_map<int, int> get_video_reqs_on_connected_endpoints(Data& data, int cache_id)
{
    const auto& endpoints = data.cache_id_to_endpoints[cache_id];

    unordered_map<int, int> video_id_to_nr_reqs;

    for (const auto& endpoint : endpoints)
    {
        const auto& reqs = data.endpoint_to_req[endpoint.id];

        for (const auto& [video_id, nr_req] : reqs)
        {
            auto it = video_id_to_nr_reqs.find(video_id);
            if (it == video_id_to_nr_reqs.end())
                video_id_to_nr_reqs[video_id] = nr_req;
            else
                it->second += nr_req;
        }
    }
    return video_id_to_nr_reqs;
}

vector<int> solve_for_cache(Data& data, int cache_id, std::unordered_map<pair<int, int>, bool, pair_hash> &is_vid_req_satisfied_on_end)
{
    auto video_id_to_nr_reqs = get_video_reqs_on_connected_endpoints(data, cache_id);

    auto videos_per_cache = knapsack(data.cache_size, video_id_to_nr_reqs.size(), data.video_sizes, video_id_to_nr_reqs);

    for (const auto video : videos_per_cache)
    {
        const auto connected_endpoints = data.cache_id_to_endpoints[cache_id];

    }

    return videos_per_cache;
}

vector<pair<int, vector<int>>> solve(Data &data)
{
    BS::thread_pool pool(9);
    std::vector<std::future<std::vector<int>>> results;

    std::unordered_map<pair<int, int>, bool, pair_hash> is_vid_req_satisfied_on_end;
    for (const auto& [cache_id, endpoints] : data.cache_id_to_endpoints)
    {
        results.push_back(pool.submit_task([&]() { return solve_for_cache(data, cache_id, is_vid_req_satisfied_on_end); }));
    }

    std::vector<pair<int, std::vector<int>>> final_results;

    int index = 0;
    for (const auto& [cache_id, _] : data.cache_id_to_endpoints)
    {
        auto result_per_cache = results[index].get();
        if (!result_per_cache.empty())
        {
            final_results.emplace_back(cache_id, result_per_cache);
        }
        ++index;
    }

    return final_results;
}

int main()
{
    const string in_prefix = "../../input_files/";
    const string out_prefix = "../../output_files/sol1/";
    const array<string, 4> input_files = { "me_at_the_zoo.in", "trending_today.in", "kittens.in.txt", "videos_worth_spreading.in"};

    for (const auto& input_file : input_files)
    {
        cout << "Now working on " << input_file;
        Data data(in_prefix + input_file);
        cout << ". Input processed.\n";

        auto final_results = solve(data);

        const string out_file = out_prefix + input_file.substr(0, (input_file.find('.'))) + ".out";
        data.write_to_file(out_file, final_results);
    }
    return 0;
}

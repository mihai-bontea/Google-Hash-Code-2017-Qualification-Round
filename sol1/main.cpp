#include <iostream>
#include <array>

#include "Data.h"
#include <unordered_map>
#include "BS_thread_pool.hpp"

using namespace std;

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

vector<int> solve_for_cache(Data& data, int cache_id)
{
    auto video_id_to_nr_reqs = get_video_reqs_on_connected_endpoints(data, cache_id);

    auto videos_per_cache = knapsack(data.cache_size, video_id_to_nr_reqs.size(), data.video_sizes, video_id_to_nr_reqs);

//        cout << "Placed " << videos_per_cache.size() << " videos on cache " << cache_id << endl;

    return videos_per_cache;
}

void solve(Data &data)
{
    BS::thread_pool pool(9);
    std::vector<std::future<std::vector<int>>> results;

    for (const auto& [cache_id, endpoints] : data.cache_id_to_endpoints)
    {
//        results.push_back(pool.submit_task(solve_for_cache, data, cache_id));
        results.push_back(pool.submit_task([&]() { return solve_for_cache(data, cache_id); }));
    }

    // can be parallelized for caches with no common endpoints

    // Sort caches based on reach and latency? (maybe) se poate face si per sub-graf
    // for cache_id in caches:
        // W = data.cache_size
        // n = requests for videos on associated endpoints(combined, unique)
        // weights = video sizes
        // values ~ value(video_index) = sum of req for connected endpoints
        // would be good to update values in case of 2 or more caches connected to the same endpoints
        // (to not store the same videos multiple times for no reason)
        // probably something like: std::map< <endpoint, video>, bool> is_vid_provided_for_endpoint;
        // auto stored_videos = knapsack(W, n, weights, values);

}

int main()
{
    const string in_prefix = "../../input_files/";
    const string out_prefix = "../../output_files/sol1/";
    const array<string, 4> input_files = { "trending_today.in", "kittens.in.txt", "videos_worth_spreading.in", "me_at_the_zoo.in" };

    for (const auto& input_file : input_files)
    {
        cout << "Now working on " << input_file;
        Data data(in_prefix + input_file);
        cout << ". Input processed.\n";

        solve(data);
    }

    return 0;
}

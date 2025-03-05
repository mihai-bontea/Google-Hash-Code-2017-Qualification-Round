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

vector<vector<int>> split_caches_into_connected_components(Data& data)
{
    unordered_set<int> visited_caches;
    vector<vector<int>> result;
    for (const auto& [cache_id, endpoints] : data.cache_id_to_endpoints)
    {
        auto it = visited_caches.find(cache_id);
        if (it == visited_caches.end())
        {
            vector<int> connected_group;
            for (const auto& endpoint : endpoints)
            {
                for (const auto& [cache_id_2, _] : endpoint.cache_to_latency)
                {
                    auto it_2 = visited_caches.find(cache_id_2);
                    if (it_2 == visited_caches.end())
                    {
                        connected_group.push_back(cache_id_2);
                        visited_caches.insert(cache_id_2);
                    }
                }
            }
            result.push_back(connected_group);
        }
    }
    return result;
}

vector<int> solve_for_cache(Data& data, int cache_id, std::unordered_map<pair<int, int>, bool, pair_hash> &is_vid_req_satisfied_on_end)
{
    auto video_id_to_nr_reqs = get_video_reqs_on_connected_endpoints(data, cache_id);

    auto videos_per_cache = knapsack(data.cache_size, video_id_to_nr_reqs.size(), data.video_sizes, video_id_to_nr_reqs);

//        cout << "Placed " << videos_per_cache.size() << " videos on cache " << cache_id << endl;

    for (const auto video : videos_per_cache)
    {
        const auto connected_endpoints = data.cache_id_to_endpoints[cache_id];

    }

    return videos_per_cache;
}

void solve(Data &data)
{
    BS::thread_pool pool(9);
    std::vector<std::future<std::vector<int>>> results;

    std::unordered_map<pair<int, int>, bool, pair_hash> is_vid_req_satisfied_on_end;
    for (const auto& [cache_id, endpoints] : data.cache_id_to_endpoints)
    {
//        results.push_back(pool.submit_task(solve_for_cache, data, cache_id));
        results.push_back(pool.submit_task([&]() { return solve_for_cache(data, cache_id, is_vid_req_satisfied_on_end); }));
    }

    // can be parallelized for caches with no common endpoints

    // Sort caches based on reach and latency? (maybe) se poate face si per sub-graf
    // for cache_id in caches:
        // W = data.cache_size
        // n = requests for videos on associated endpoints(combined, unique)
        // weights = video sizes
        // values ~ value(video_index) = sum of req for connected endpointsd
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

//        solve(data);
        auto connected_comp = split_caches_into_connected_components(data);
        cout << "We got " << connected_comp.size() << " connected components." << endl;
    }

    return 0;
}

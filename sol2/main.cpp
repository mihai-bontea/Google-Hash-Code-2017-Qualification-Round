#include <array>
#include <iostream>
#include <unordered_map>

#include "Data.h"

using namespace std;

vector<int> knapsack(int W, int n, const vector<int> &weights, unordered_map<int, int> &values)
{
    vector<vector<unsigned long long>> dp(n + 1, vector<unsigned long long>(W + 1, 0));

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

unordered_map<int, int> get_video_values_on_connected_endpoints(
        Data& data, int cache_id,
        vector<unordered_map<int, int>>& endpoint_to_satisfied_videos)
{
    unordered_map<int, int> video_id_to_value;

    for (const auto& endpoint_id : data.cache_id_to_endpoint_ids[cache_id])
    {
        const auto& reqs = data.endpoints[endpoint_id].vid_to_nr_req;

        for (const auto& [video_id, nr_req] : reqs)
        {
            const int datac_latency = data.endpoints[endpoint_id].datacenter_latency;
            const int new_latency = data.endpoints[endpoint_id].cache_to_latency[cache_id];

            int value = nr_req *  (datac_latency - new_latency);

            // If video on this endpoint is already satisfied by another cache, update its value
            // based on the differences between cache latencies
            auto sat_video_it = endpoint_to_satisfied_videos[endpoint_id].find(video_id);
            if (sat_video_it != endpoint_to_satisfied_videos[endpoint_id].end())
            {
                const int old_latency = sat_video_it->second;

                if (new_latency < old_latency)
                    value -= nr_req * (datac_latency - old_latency);
                else
                    value = 0;
            }

            auto it = video_id_to_value.find(video_id);
            if (it == video_id_to_value.end())
                video_id_to_value[video_id] = value;
            else
                it->second += value;
        }
    }
    return video_id_to_value;
}

vector<pair<int, vector<int>>> solve(Data &data)
{
    // {endpoint_id : map{video : latency}}
    vector<unordered_map<int, int>> endpoint_to_satisfied_videos(data.nr_endpoints);

    vector<pair<int, vector<int>>> final_results;

    for (int cache_id = 0; cache_id < data.nr_caches; ++cache_id)
    {
        auto video_id_to_nr_reqs = get_video_values_on_connected_endpoints(data, cache_id, endpoint_to_satisfied_videos);
        auto videos_per_cache = knapsack(data.cache_size, (int)video_id_to_nr_reqs.size(), data.video_sizes, video_id_to_nr_reqs);

        final_results.emplace_back(cache_id, videos_per_cache);

        // Updating the best latency for each video on each endpoint
        for (const auto& endpoint_id : data.cache_id_to_endpoint_ids[cache_id])
        {
            const int new_latency = data.endpoints[endpoint_id].cache_to_latency[cache_id];
            for (const auto& video_id : videos_per_cache)
            {
                // There is a req for video_id on endpoint
                if (data.endpoints[endpoint_id].vid_to_nr_req.count(video_id))
                {
                    auto sat_video_it = endpoint_to_satisfied_videos[endpoint_id].find(video_id);
                    // If video not satisfied yet, or satisfied, but with higher latency, update
                    if (sat_video_it == endpoint_to_satisfied_videos[endpoint_id].end() || sat_video_it->second > new_latency)
                    {
                        endpoint_to_satisfied_videos[endpoint_id][video_id] = new_latency;
                    }
                }
            }
        }
    }
    return final_results;
}

int main()
{
    const string in_prefix = "../../input_files/";
    const string out_prefix = "../../output_files/sol2/";
    const array<string, 4> input_files = { "me_at_the_zoo.in", "trending_today.in", "videos_worth_spreading.in", "kittens.in.txt"};

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
#include <array>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "Data.h"

using namespace std;

vector<int> knapsack(int W, int n, const vector<int> &weights, unordered_map<int, int> &values)
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

unordered_map<int, int> get_video_reqs_on_connected_endpoints(
        Data& data, int cache_id,
        const unordered_map<int, unordered_set<int>>& endpoint_to_satisfied_videos)
{
    const auto& endpoints = data.cache_id_to_endpoints[cache_id];

    unordered_map<int, int> video_id_to_nr_reqs;

    for (const auto& endpoint : endpoints)
    {
        const auto& reqs = data.endpoint_to_req[endpoint.id];

        auto endpoint_it = endpoint_to_satisfied_videos.find(endpoint.id);
        for (const auto& [video_id, nr_req] : reqs)
        {
            int readjusted_nr_req = nr_req;
            if (endpoint_it != endpoint_to_satisfied_videos.end())
            {
                auto video_it = endpoint_it->second.find(video_id);
                if (video_it != endpoint_it->second.end())
                    readjusted_nr_req /= 10;
            }

            auto it = video_id_to_nr_reqs.find(video_id);
            if (it == video_id_to_nr_reqs.end())
                video_id_to_nr_reqs[video_id] = readjusted_nr_req;
            else
                it->second += readjusted_nr_req;
        }
    }
    return video_id_to_nr_reqs;
}

vector<pair<int, vector<int>>> solve(Data &data)
{
    unordered_map<int, unordered_set<int>> endpoint_to_satisfied_videos;
    vector<pair<int, vector<int>>> final_results;

    for (const auto& [cache_id, endpoints] : data.cache_id_to_endpoints)
    {
        auto video_id_to_nr_reqs = get_video_reqs_on_connected_endpoints(data, cache_id, endpoint_to_satisfied_videos);
        auto videos_per_cache = knapsack(data.cache_size, (int)video_id_to_nr_reqs.size(), data.video_sizes, video_id_to_nr_reqs);

        final_results.emplace_back(cache_id, videos_per_cache);

        for (const auto& endpoint : endpoints)
        {
            unordered_set<int> satisfied_videos;
            satisfied_videos.insert(videos_per_cache.begin(), videos_per_cache.end());
            endpoint_to_satisfied_videos[endpoint.id] = move(satisfied_videos);
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
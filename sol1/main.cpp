#include <iostream>
#include <array>

#include "Data.h"

using namespace std;

vector<int> knapsack(int W, int n, std::vector<int> &weights, std::vector<int> &values)
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

void solve(Data &data)
{
    // Sort caches based on reach and latency
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
    const array<string, 5> input_files = { "me_at_the_zoo.in", "kittens.in.txt", "me_at_the_zoo.in", "trending_today.in", "videos_worth_spreading.in" };

    for (const auto& input_file : input_files)
    {
        cout << "Now working on " << input_file;
        Data data(in_prefix + input_file);
        cout << ". Input processed.\n";

        int max_size = 0, min_size = 99999999;
        for (int i = 0; i < data.nr_videos; ++i) {
            if (data.video_sizes[i] > max_size)
                max_size = data.video_sizes[i];

            if (data.video_sizes[i] < min_size)
                min_size = data.video_sizes[i];
        }

        cout << "Cache size: " << data.cache_size << " max video size: " << max_size << " min size: " << min_size << endl;
    }

    return 0;
}

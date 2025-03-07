#pragma once
#include <iostream>
#include <memory>
#include <fstream>
#include <map>
#include <unordered_map>
#include <vector>
#include "Parser.h"

class Endpoint
{
public:
    int id, datacenter_latency;
    std::map<int, int> cache_to_latency;
    std::unordered_map<int, int> vid_to_nr_req;

    Endpoint(int id, int data_lat, std::map<int, int>& cache_lat): id{id}, datacenter_latency{data_lat}, cache_to_latency{cache_lat}
    {
    }
};

class Data
{
public:
    int nr_videos, nr_endpoints, nr_requests, nr_caches, cache_size;
    std::vector<int> video_sizes;
    std::vector<Endpoint> endpoints;
    std::vector<std::vector<int>> cache_id_to_endpoint_ids;

    Data(const std::string& filename)
    {
        std::ifstream fin(filename);
//        InParser fin(filename.c_str());

        fin >> nr_videos >> nr_endpoints >> nr_requests >> nr_caches >> cache_size;

        int video_size;
        for (size_t index = 0; index < nr_videos; ++index)
        {
            fin >> video_size;
            video_sizes.push_back(video_size);
        }

        // Process endpoints
        for (size_t index = 0; index < nr_endpoints; ++index)
        {
            int datacenter_latency, nr_connected_caches;
            fin >> datacenter_latency >> nr_connected_caches;

            int cache_index, latency;
            std::map<int, int> cache_to_latency;
            while (nr_connected_caches--)
            {
                fin >> cache_index >> latency;
                cache_to_latency[cache_index] = latency;
            }
            Endpoint endpoint = Endpoint((int)index, datacenter_latency, cache_to_latency);

            endpoints.push_back(endpoint);

            // Associating cache id to connected endpoint ids
            cache_id_to_endpoint_ids.resize(nr_caches);
            for (auto [cache_id, _] : cache_to_latency)
            {
                cache_id_to_endpoint_ids[cache_id].push_back(endpoint.id);
            }
        }

        // Process requests
        for (size_t index = 0; index < nr_requests; ++index)
        {
            int video_index, endpoint_index, nr_req;
            fin >> video_index >> endpoint_index >> nr_req;

            endpoints[endpoint_index].vid_to_nr_req[video_index] = nr_req;
        }
    }

    void write_to_file(const std::string& filename, const std::vector<std::pair<int, std::vector<int>>>& final_results)
    {
        std::ofstream fout(filename);

        fout << final_results.size() << '\n';

        for (const auto& [cache_id, videos] : final_results)
        {
            fout << cache_id << " ";
            for (const auto& video : videos)
            {
                fout << video << " ";
            }
            fout << '\n';
        }
    }
};
#pragma once
#include <iostream>
#include <memory>
#include <fstream>
#include <map>
#include <vector>
#include "Parser.h"

class Endpoint
{
public:
    int datacenter_latency;
    std::map<int, int> cache_to_latency;

    Endpoint(int data_lat, std::map<int, int>& cache_lat): datacenter_latency{data_lat}, cache_to_latency{cache_lat}
    {
    }
};

class Request
{
public:
    int video_index, endpoint_index, nr_requests;

    Request(int video_index, int endpoint_index, int nr_requests):
        video_index(video_index),
        endpoint_index(endpoint_index),
        nr_requests(nr_requests)
    {
    }
};

class Data
{
public:
    int nr_videos, nr_endpoints, nr_requests, nr_caches;
    std::unique_ptr<int[]> video_sizes;
    std::vector<Endpoint> endpoints;
    std::vector<Request> requests;

    Data(const std::string& filename)
    {
//        std::ifstream fin(filename);
        InParser fin(filename.c_str());

        fin >> nr_videos >> nr_endpoints >> nr_requests >> nr_caches;
        video_sizes = std::make_unique<int[]>(nr_videos);

        for (size_t index = 0; index < nr_videos; ++index)
            fin >> video_sizes[index];

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
            endpoints.emplace_back(datacenter_latency, cache_to_latency);
        }

        // Process requests
        for (size_t index = 0; index < nr_requests; ++index)
        {
            int video_index, endpoint_index, nr_req;
            fin >> video_index >> endpoint_index >> nr_req;

            requests.emplace_back(video_index, endpoint_index, nr_req);
        }
    }
};
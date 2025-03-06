in_prefix = "input_files/";
out_prefix = "output_files/sol1/";
input_files = ["me_at_the_zoo.in", "trending_today.in", "kittens.in.txt", "videos_worth_spreading.in"]
total_score = 0

def vids_fit_on_caches(cache_size, vids_per_cache, video_sizes):
    return all(sum(video_sizes[video_id] for video_id in vids_per_cache[cache_id]) <= cache_size 
               for cache_id in vids_per_cache)


for filename in input_files:
    full_path_in = in_prefix + filename
    full_path_out = out_prefix + filename.split(".", 1)[0] + ".out"
    
    try:
        with open(full_path_out, "r") as out_file:
            lines = out_file.readlines()
            
            vids_per_cache_solution = {int(l.split()[0]): list(map(int, l.split()[1:])) for l in lines[1:]}

            with open(full_path_in, "r") as in_file:
                nr_videos, nr_endpoints, nr_requests, nr_caches, cache_size = list(map(int, (in_file.readline()).split()))

                video_sizes = list(map(int, (in_file.readline()).split()))

                endpoints = []
                for i in range(nr_endpoints):
                    datacenter_latency, nr_connected_caches = list(map(int, (in_file.readline()).split()))
                    cache_to_latency = {}

                    cache_to_latency = {cache_index: latency for cache_index, latency in (map(int, in_file.readline().split()) for _ in range(nr_connected_caches))}
                
                    endpoints.append((datacenter_latency, cache_to_latency))
                
                requests = [(vid_idx, end_idx, nr_req) for vid_idx, end_idx, nr_req in (map(int, in_file.readline().split()) for _ in range(nr_requests))]
                
                if not vids_fit_on_caches(cache_size, vids_per_cache_solution, video_sizes):
                    print(f"Invalid output for file '{full_path_out}', videos don't fit on cache!")
                    continue



                    





    except FileNotFoundError:
        print(f"The file '{full_path_out}' does not exist => 0 points.")


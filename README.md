# Google Hash Code 2017 Qualification Round (Video Streaming)

![Image](https://github.com/user-attachments/assets/6a9e4a85-c608-4b6e-a6c7-cf3f10a965ee)

>Have you ever wondered what happens behind the scenes when you watch a YouTube video? As more and more people watch online videos (and as the size of these videos increases), it is critical that video-serving infrastructure is optimized to handle requests reliably and quickly.
>This typically involves putting in place cache servers, which store copies of popular videos. When a user request for a particular video arrives, it can be handled by a cache server close to the user, rather than by a remote data center thousands of kilometers away.
>But how should you decide which videos to put in which cache servers?

>**Given a description of cache servers, network endpoints and videos, along with predicted requests for individual videos, decide which videos to put in which cache server in order to minimize the average waiting time for all requests.**

## Solution 1

The task of choosing how to place videos on the caches (with limited size) such that the average waiting time is minimal immediately hints towards the **0/1 Knapsack Problem**. For a specific cache, each video has a certain **value**(sum of number of requests on the connected endpoints), and a specific **weight**(video size). A quick and 'dirty' solution is to apply the knapsack algorithm for each cache, without minding repetitions. This is also easy to parallelize with a thread pool, greatly increasing processing speed.

| File Name                        | Score   |
|----------------------------------|--------:|
| me_at_the_zoo.out               | 387043  |
| trending_today.out              | 25805   |
| kittens.out                     | 72104   |
| videos_worth_spreading.out      | 494816  |
| **Final**                       | 979768  |


## Solution 2

Unsurprisingly, solution 1 doesn't score that well: **an endpoint can be serviced by multiple caches**(In fact this is the norm, a quick connected components check shows only 1 to 16 independent components on all datasets). This means that if an endpoint is serviced by 2 caches C<sub>i</sub> and C<sub>j</sub>, and a video is already placed on C<sub>i</sub>, whether it is worth to also place it on C<sub>j</sub> depends on the difference between their latencies to that specific endpoint.

Hence, we need to keep track of which videos are already provided to each endpoint, and with what latency. This, combined with the fact that the knapsack algorithm often "changes its mind"(gives up on a previously selected object for a better one), means that we can no longer parallelize it like in solution 1.

Now, from the point of view of the knapsack algorithm, a video's value is no longer just the sum of its requests on the connected endpoints. Whether it is already provided by another cache is also taken into account. The difficulty now lies in **picking a good formula** for determining a video's value.

The formula chosen is:

value = nr_req * (L<sub>D</sub> - L<sub>cache</sub>), then

value = max(0, value - nr_req * (L<sub>D</sub> - L<sub>old</sub>)) (if video is already provided by another cache)

| File Name                        | Score   |
|----------------------------------|--------:|
| me_at_the_zoo.out               | 405733  |
| trending_today.out              | 499999  |
| kittens.out                     | 959981  |
| videos_worth_spreading.out      | 433928  |
| **Final**                       | 2299641 |

This solution is in the 10th percentile of all submissions.
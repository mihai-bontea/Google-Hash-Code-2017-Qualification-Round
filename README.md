# Google Hash Code 2017 Qualification Round (Video Streaming)

![Image](https://github.com/user-attachments/assets/6a9e4a85-c608-4b6e-a6c7-cf3f10a965ee)

## Solution 1

The task of choosing how to place videos on the caches (with limited size) such that the average waiting time is minimal immediately hints towards the **0/1 Knapsack Problem**. For a specific cache, each video has a certain **value**(sum of number of requests on the connected endpoints), and a specific **weight**(video size). A quick and 'dirty' solution is to apply the knapsack algorithm for each cache, without minding repetitions. This is also easy to parallelize with a thread pool, greatly increasing processing speed.

| File Name                        | Views   |
|----------------------------------|--------:|
| me_at_the_zoo.out               | 387043  |
| trending_today.out              | 25805   |
| kittens.out                     | 72104   |
| videos_worth_spreading.out      | 494816  |
| final score                     | 979768  |


## Solution 2


| File Name                        | Views   |
|----------------------------------|--------:|
| me_at_the_zoo.out               | 405733  |
| trending_today.out              | 499999  |
| kittens.out                     | 959981  |
| videos_worth_spreading.out      | 433928  |
| final score                     | 2299641 |



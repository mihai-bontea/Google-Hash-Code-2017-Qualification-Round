#include <iostream>
#include <array>

#include "Data.h"

using namespace std;

int main()
{
    const string in_prefix = "../../input_files/";
    const string out_prefix = "../../output_files/sol1/";
    const array<string, 4> input_files = { "kittens.in.txt", "me_at_the_zoo.in", "trending_today.in", "videos_worth_spreading.in" };

    for (const auto& input_file : input_files)
    {
        cout << "Now working on " << input_file;
        Data data(in_prefix + input_file);
        cout << ". Input processed.\n";

        cout << "We got " <<data.nr_requests << endl;
    }

    return 0;
}

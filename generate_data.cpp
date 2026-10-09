#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    const size_t N = 1'000'000;
    const string dir = (argc > 1) ? argv[1] : "data";
    fs::create_directories(dir);

    mt19937_64 rng(random_device{}());
    uniform_real_distribution<double> dist(-100000.0, 100000.0);

    for (int d = 1; d <= 10; ++d) {
        vector<double> values(N);
        if (d == 1) {
            for (size_t i = 0; i < N; ++i) values[i] = static_cast<double>(i);
        } else if (d == 2) {
            for (size_t i = 0; i < N; ++i) values[i] = static_cast<double>(N - i);
        } else {
            for (double &x : values) x = dist(rng);
        }

        const string filename = dir + "/day" + to_string(d) + ".txt";
        ofstream out(filename);
        if (!out) {
            cerr << "Khong the tao file: " << filename << '\n';
            return 1;
        }
        out << setprecision(10);
        for (double x : values) out << x << '\n';
        cout << "Da tao " << filename << " (" << N << " so)\n";
    }
    cout << "Hoan tat tao 10 day du lieu.\n";
    return 0;
}

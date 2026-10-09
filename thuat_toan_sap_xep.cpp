
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>

using namespace std;
using Clock = chrono::steady_clock;
using Values = std::vector<double>;

void QuickSort(Values &a, long long left, long long right) {
    while (left < right) {
        long long i = left, j = right;
        const double pivot = a[left + (right - left) / 2];
        while (i <= j) {
            while (a[i] < pivot) ++i;
            while (a[j] > pivot) --j;
            if (i <= j) { swap(a[i], a[j]); ++i; --j; }
        }
        // Recurse on the smaller partition; iterate over the larger one to limit stack depth.
        if (j - left < right - i) {
            if (left < j) QuickSort(a, left, j);
            left = i;
        } else {
            if (i < right) QuickSort(a, i, right);
            right = j;
        }
    }
}

void Heapify(Values &a, size_t n, size_t i) {
    while (true) {
        size_t largest = i;
        size_t l = 2 * i + 1, r = 2 * i + 2;
        if (l < n && a[l] > a[largest]) largest = l;
        if (r < n && a[r] > a[largest]) largest = r;
        if (largest == i) return;
        swap(a[i], a[largest]);
        i = largest;
    }
}

void HeapSort(Values &a) {
    const size_t n = a.size();
    for (size_t i = n / 2; i > 0; --i) Heapify(a, n, i - 1);
    for (size_t end = n; end > 1; --end) {
        swap(a[0], a[end - 1]);
        Heapify(a, end - 1, 0);
    }
}

void Merge(Values &a, Values &temp, size_t left, size_t mid, size_t right) {
    size_t i = left, j = mid, k = left;
    while (i < mid && j < right) temp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i < mid) temp[k++] = a[i++];
    while (j < right) temp[k++] = a[j++];
    for (size_t p = left; p < right; ++p) a[p] = temp[p];
}

void MergeSortImpl(Values &a, Values &temp, size_t left, size_t right) {
    if (right - left <= 1) return;
    size_t mid = left + (right - left) / 2;
    MergeSortImpl(a, temp, left, mid);
    MergeSortImpl(a, temp, mid, right);
    Merge(a, temp, left, mid, right);
}

void MergeSort(Values &a) {
    Values temp(a.size());
    MergeSortImpl(a, temp, 0, a.size());
}

bool IsSorted(const Values &a) {
    return is_sorted(a.begin(), a.end());
}

Values ReadData(const string &filename) {
    ifstream in(filename);
    if (!in) throw runtime_error("Khong mo duoc file: " + filename);
    Values a;
    a.reserve(1'000'000);
    double x;
    while (in >> x) a.push_back(x);
    if (a.empty()) throw runtime_error("File rong hoac khong doc duoc du lieu: " + filename);
    return a;
}

template <class SortFunction>
double Measure(const Values &original, SortFunction sortFunction) {
    Values a = original; // Sao chep nam ngoai vung do thoi gian.
    const auto start = Clock::now();
    sortFunction(a);
    const auto stop = Clock::now();
    if (!IsSorted(a)) throw runtime_error("Loi: mang sau khi sap xep chua tang dan.");
    return chrono::duration<double, milli>(stop - start).count();
}

int main(int argc, char *argv[]) {
    const string dir = (argc > 1) ? argv[1] : "data";
    const string csvName = (argc > 2) ? argv[2] : "results.csv";
    ofstream csv(csvName);
    if (!csv) { cerr << "Khong tao duoc file " << csvName << '\n'; return 1; }
    csv << "Dataset,N,QuickSort_ms,HeapSort_ms,MergeSort_ms,std_sort_ms\n";
    cout << fixed << setprecision(2);

    double totals[4] = {0, 0, 0, 0};
    try {
        for (int d = 1; d <= 10; ++d) {
            const string filename = dir + "/day" + to_string(d) + ".txt";
            Values original = ReadData(filename);
            double q = Measure(original, [](Values &a) { QuickSort(a, 0, static_cast<long long>(a.size()) - 1); });
            double h = Measure(original, [](Values &a) { HeapSort(a); });
            double m = Measure(original, [](Values &a) { MergeSort(a); });
            double s = Measure(original, [](Values &a) { sort(a.begin(), a.end()); });
            double times[] = {q, h, m, s};
            for (int i = 0; i < 4; ++i) totals[i] += times[i];
            csv << fixed << setprecision(2) << d << ',' << original.size() << ',' << q << ',' << h << ',' << m << ',' << s << '\n';
            cout << "Day " << d << " (" << original.size() << " so): QuickSort=" << q
                 << " ms, HeapSort=" << h << " ms, MergeSort=" << m << " ms, std::sort=" << s << " ms\n";
        }
        csv << fixed << setprecision(2) << "Average,1000000," << totals[0] / 10 << ',' << totals[1] / 10
            << ',' << totals[2] / 10 << ',' << totals[3] / 10 << '\n';
        cout << "\nTrung binh 10 bo du lieu: QuickSort=" << totals[0] / 10
             << " ms, HeapSort=" << totals[1] / 10 << " ms, MergeSort=" << totals[2] / 10
             << " ms, std::sort=" << totals[3] / 10 << " ms\n"
             << "Da luu ket qua vao " << csvName << '\n';
    } catch (const exception &e) {
        cerr << "Loi: " << e.what() << '\n';
        return 1;
    }
    return 0;
}

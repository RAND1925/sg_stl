
#include <iostream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <numeric>
int main() {
    int N;
    std::cin >> N;
    std::vector<int> x_range;
    auto start = std::chrono::system_clock::now();
    x_range.resize(N);
    std::iota(x_range.begin(), x_range.end(), 1);
    auto end = std::chrono::system_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "nanosec" << std::endl;
	
	std::vector<int> x_range1;
	start = std::chrono::system_clock::now();
	x_range1.resize(N);
	std::iota(x_range.begin(), x_range.end(), 0);
	end = std::chrono::system_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "nanosec" << std::endl;
	
	
	std::vector<int> x_range2;
    start = std::chrono::system_clock::now();
    x_range2.resize(N);
    for (int i = 0; i < N; ++i) {
      x_range2[i] = i + 1;
    }
    end = std::chrono::system_clock::now();
  	std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "nanosec" << std::endl;
	
	
	std::vector<int> x_range3;
    start = std::chrono::system_clock::now();
    x_range3.reserve(N);
    for (int i = 0; i < N; ++i) {
      x_range3.push_back(i);
    }
	end = std::chrono::system_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "nanosec" << std::endl;
	
}
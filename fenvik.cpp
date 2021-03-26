#include<iostream>
#include<vector>
#include<algorithm>

template<typename T>
class Fenvik {
private:
	std::vector<long long> fenvik;
public:
	Fenvik() = default;
	~Fenvik() = default;
	Fenvik(const Fenvik& other) = delete;
	Fenvik(Fenvik&& other) = delete;
	Fenvik& operator=(const Fenvik& other) = delete;
	Fenvik& operator=(Fenvik&& other) = delete;

	long long f(long long x) { return (x & x + 1); }
	long long g(long long x) { return (x | x + 1); }

	void Build(std::vector<long long>& zero) {
		fenvik = zero;
	}
	void Update(long long x, long long value) {
		for (int i = x; i < fenvik.size(); i = g(i)) {
			fenvik[i] += value;	
		}
	}

	long long GetSum(long long x) {
		long long ans = 0;
		for (int i = x; i >= 0; i = f(i) - 1) {
			ans += fenvik[i];
		}
		return ans;
	}

	long long SumInArea(long long x1, long long x2) {
		return GetSum(x2) - GetSum(x1-1);
	}

};

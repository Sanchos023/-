#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <clocale>
#include <algorithm>
#include <omp.h>

class MatrixMultiplier {
private:
    int n;
    std::vector<std::vector<long long>> A;
    std::vector<std::vector<long long>> B;
    std::vector<std::vector<long long>> C;

public:
    MatrixMultiplier(int size) : n(size) {
        A.resize(n, std::vector<long long>(n));
        B.resize(n, std::vector<long long>(n));
        C.resize(n, std::vector<long long>(n, 0));
    }

    bool readMatrixFromFile(const std::string& filename, std::vector<std::vector<long long>>& matrix) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return false;
        }

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (!(file >> matrix[i][j])) {
                    std::cerr << "Error: Not enough data in file " << filename << std::endl;
                    return false;
                }
            }
        }

        file.close();
        return true;
    }

    bool loadMatrices(const std::string& fileA, const std::string& fileB) {
        std::cout << "Loading matrices from files...\n";

        if (!readMatrixFromFile(fileA, A)) return false;
        if (!readMatrixFromFile(fileB, B)) return false;

        std::cout << "Matrices loaded successfully\n";
        return true;
    }

    void resetResult() {
        for (int i = 0; i < n; ++i) {
            std::fill(C[i].begin(), C[i].end(), 0);
        }
    }

    void multiplySequential() {
        resetResult();

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                long long sum = 0;
                for (int k = 0; k < n; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
    }

    void multiplyParallel(int numThreads) {
        resetResult();

        omp_set_num_threads(numThreads);

#pragma omp parallel for schedule(static)
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                long long sum = 0;
                for (int k = 0; k < n; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
    }

    double runExperiment(int numThreads, bool printMatrix = false) {
        auto start = std::chrono::high_resolution_clock::now();

        if (numThreads == 1) {
            multiplySequential();
        }
        else {
            multiplyParallel(numThreads);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        double time_sec = duration.count() / 1000000.0;
        long long operations = 2LL * n * n * n;
        double mops = operations / (time_sec * 1e6);

        std::cout << std::fixed << std::setprecision(6);
        std::cout << "Threads: " << std::setw(2) << numThreads
            << " | Time: " << std::setw(10) << time_sec << " sec"
            << " | Operations: " << operations
            << " | Performance: " << mops << " MOPS\n";

        if (printMatrix) {
            printMatrixPart(C, "Result Matrix C");
        }

        return time_sec;
    }

    void printMatrixPart(const std::vector<std::vector<long long>>& matrix, const std::string& name) const {
        std::cout << "\n" << name << ":\n";
        for (int i = 0; i < std::min(5, n); ++i) {
            for (int j = 0; j < std::min(5, n); ++j) {
                std::cout << std::setw(10) << matrix[i][j] << " ";
            }
            if (n > 5) std::cout << "...";
            std::cout << "\n";
        }
        if (n > 5) std::cout << "...\n";
    }

    void printInputMatrices() const {
        printMatrixPart(A, "Matrix A");
        printMatrixPart(B, "Matrix B");
    }

    void writeMatrixToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open output file " << filename << std::endl;
            return;
        }

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                file << std::setw(12) << C[i][j] << " ";
            }
            file << "\n";
        }

        file.close();
        std::cout << "Result saved to file: " << filename << std::endl;
    }

    const std::vector<std::vector<long long>>& getMatrixA() const { return A; }
    const std::vector<std::vector<long long>>& getMatrixB() const { return B; }
    const std::vector<std::vector<long long>>& getResult() const { return C; }
    int getSize() const { return n; }
};

class Verifier {
public:
    static bool verify(const std::vector<std::vector<long long>>& A,
        const std::vector<std::vector<long long>>& B,
        const std::vector<std::vector<long long>>& C_result,
        int n) {
        std::cout << "\n=== VERIFICATION ===\n";

        std::vector<std::vector<long long>> C_verify(n, std::vector<long long>(n, 0));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                long long sum = 0;
                for (int k = 0; k < n; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                C_verify[i][j] = sum;
            }
        }

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (C_result[i][j] != C_verify[i][j]) {
                    std::cout << "Verification failed at [" << i << "][" << j << "]\n";
                    std::cout << "Expected: " << C_verify[i][j]
                        << ", got: " << C_result[i][j] << "\n";
                    return false;
                }
            }
        }

        std::cout << "Verification passed\n";
        return true;
    }
};

std::vector<int> getThreadCounts() {
    int maxThreads = omp_get_max_threads();
    std::vector<int> threadCounts = { 1, 2, 4, 8, 16 };

    std::vector<int> filtered;
    for (int t : threadCounts) {
        if (t <= maxThreads) {
            filtered.push_back(t);
        }
    }

    if (filtered.empty()) {
        filtered.push_back(1);
    }

    if (filtered.back() != maxThreads) {
        filtered.push_back(maxThreads);
    }

    std::sort(filtered.begin(), filtered.end());
    filtered.erase(std::unique(filtered.begin(), filtered.end()), filtered.end());

    return filtered;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "C");

    try {
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " <size> <file_A> <file_B> [file_C]\n";
            std::cerr << "Example: " << argv[0] << " 400 matA.txt matB.txt result.txt\n";
            return 1;
        }

        int n = std::stoi(argv[1]);
        std::string fileA = argv[2];
        std::string fileB = argv[3];
        std::string fileC = (argc >= 5) ? argv[4] : "result_matrix.txt";

        std::cout << "\n=== MATRIX MULTIPLICATION WITH OpenMP ===\n";
        std::cout << "Size: " << n << " x " << n << "\n";
        std::cout << "File A: " << fileA << "\n";
        std::cout << "File B: " << fileB << "\n";
        std::cout << "Output: " << fileC << "\n";
        std::cout << "Available processors (OpenMP): " << omp_get_num_procs() << "\n";
        std::cout << "Maximum threads: " << omp_get_max_threads() << "\n\n";

        MatrixMultiplier multiplier(n);

        if (!multiplier.loadMatrices(fileA, fileB)) {
            return 1;
        }

        if (n <= 10) {
            multiplier.printInputMatrices();
        }

        std::vector<int> threadCounts = getThreadCounts();

        std::cout << "\n=== EXPERIMENTS ===\n";
        double baselineTime = 0.0;

        for (size_t i = 0; i < threadCounts.size(); ++i) {
            int threads = threadCounts[i];
            double currentTime = multiplier.runExperiment(threads, false);

            if (threads == 1) {
                baselineTime = currentTime;
            }
            else if (baselineTime > 0.0) {
                double speedup = baselineTime / currentTime;
                double efficiency = speedup / threads * 100.0;

                std::cout << "           Speedup: " << speedup
                    << " | Efficiency: " << efficiency << "%\n";
            }
        }

        std::cout << "\n=== FINAL RUN FOR SAVING RESULT ===\n";
        int bestThreads = threadCounts.back();
        multiplier.runExperiment(bestThreads, n <= 10);

        multiplier.writeMatrixToFile(fileC);

        Verifier::verify(
            multiplier.getMatrixA(),
            multiplier.getMatrixB(),
            multiplier.getResult(),
            multiplier.getSize()
        );
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
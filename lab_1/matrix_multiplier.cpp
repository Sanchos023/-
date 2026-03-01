#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <clocale>

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

    void writeMatrixToFile(const std::string& filename) {
        std::ofstream file(filename);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                file << std::setw(12) << C[i][j] << " ";
            }
            file << "\n";
        }
        file.close();
        std::cout << "Result saved to file: " << filename << std::endl;
    }

    void multiply() {
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

    void printMatrix(const std::vector<std::vector<long long>>& matrix, const std::string& name) {
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

    bool loadMatrices(const std::string& fileA, const std::string& fileB) {
        std::cout << "Loading matrices from files...\n";

        bool loadedA = readMatrixFromFile(fileA, A);
        if (!loadedA) return false;

        bool loadedB = readMatrixFromFile(fileB, B);
        if (!loadedB) return false;

        std::cout << "Matrices loaded successfully\n";
        return true;
    }

    void run(const std::string& fileC) {
        std::cout << "Matrix size: " << n << " x " << n << "\n";
        printMatrix(A, "Matrix A");
        printMatrix(B, "Matrix B");

        auto start = std::chrono::high_resolution_clock::now();

        multiply();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        double time_sec = duration.count() / 1000000.0;
        long long operations = 2LL * n * n * n;

        std::cout << "\n=== RESULTS ===\n";
        std::cout << "Execution time: " << std::fixed << std::setprecision(6) << time_sec << " seconds\n";
        std::cout << "Operations: " << operations << "\n";

        printMatrix(C, "Result Matrix C");
        writeMatrixToFile(fileC);
    }

    const std::vector<std::vector<long long>>& getMatrixA() const { return A; }
    const std::vector<std::vector<long long>>& getMatrixB() const { return B; }
    const std::vector<std::vector<long long>>& getResult() const { return C; }
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

        bool match = true;

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (C_result[i][j] != C_verify[i][j]) {
                    match = false;
                    break;
                }
            }
        }

        if (match) {
            std::cout << " Verification passed\n";
            return true;
        }
        else {
            std::cout << " Verification failed\n";
            return false;
        }
    }
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "C");

    try {
        std::string fileA, fileB, fileC;
        int n;

        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " <size> <file_A> <file_B> [file_C]\n";
            std::cerr << "Example: " << argv[0] << " 4 matA.txt matB.txt result.txt\n";
            return 1;
        }

        n = std::stoi(argv[1]);
        fileA = argv[2];
        fileB = argv[3];
        fileC = (argc >= 5) ? argv[4] : "result_matrix.txt";

        std::cout << "\n=== MATRIX MULTIPLICATION ===\n";
        std::cout << "Size: " << n << "x" << n << "\n";
        std::cout << "File A: " << fileA << "\n";
        std::cout << "File B: " << fileB << "\n";
        std::cout << "Output: " << fileC << "\n\n";

        MatrixMultiplier multiplier(n);

        if (!multiplier.loadMatrices(fileA, fileB)) {
            return 1;
        }

        multiplier.run(fileC);
        Verifier::verify(multiplier.getMatrixA(), multiplier.getMatrixB(), multiplier.getResult(), n);

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
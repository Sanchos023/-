#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;

void readMatrix(const string& filename, vector<long long>& matrix, int n) {
    ifstream file(filename);
    for (int i = 0; i < n * n; ++i)
        file >> matrix[i];
}

void writeMatrix(const string& filename, const vector<long long>& matrix, int n) {
    ofstream file(filename);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            file << setw(12) << matrix[i * n + j] << " ";
        file << "\n";
    }
}

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 4) {
        if (rank == 0)
            cout << "Usage: mpirun -np <proc> ./matrix_mpi <n> <A.txt> <B.txt>\n";
        MPI_Finalize();
        return 1;
    }

    int n = stoi(argv[1]);
    string fileA = argv[2];
    string fileB = argv[3];

    vector<long long> A;
    vector<long long> B(n * n);
    vector<long long> C;

    int rows_per_proc = n / size;
    int remainder = n % size;

    int local_rows = rows_per_proc + (rank < remainder ? 1 : 0);

    vector<long long> local_A(local_rows * n);
    vector<long long> local_C(local_rows * n, 0);

    if (rank == 0) {
        A.resize(n * n);
        C.resize(n * n);
        readMatrix(fileA, A, n);
        readMatrix(fileB, B, n);
    }

    MPI_Bcast(B.data(), n * n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    vector<int> sendcounts(size), displs(size);
    if (rank == 0) {
        int offset = 0;
        for (int i = 0; i < size; ++i) {
            int rows = rows_per_proc + (i < remainder ? 1 : 0);
            sendcounts[i] = rows * n;
            displs[i] = offset;
            offset += rows * n;
        }
    }

    MPI_Scatterv(A.data(), sendcounts.data(), displs.data(),
        MPI_LONG_LONG,
        local_A.data(), local_rows * n, MPI_LONG_LONG,
        0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    for (int i = 0; i < local_rows; ++i) {
        for (int j = 0; j < n; ++j) {
            long long sum = 0;
            for (int k = 0; k < n; ++k)
                sum += local_A[i * n + k] * B[k * n + j];
            local_C[i * n + j] = sum;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    double local_time = end - start;
    double max_time;

    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Gatherv(local_C.data(), local_rows * n, MPI_LONG_LONG,
        C.data(), sendcounts.data(), displs.data(),
        MPI_LONG_LONG,
        0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "\nMatrix size: " << n << "x" << n << endl;
        cout << "Processes: " << size << endl;
        cout << "Execution time: " << fixed << setprecision(6)
            << max_time << " seconds\n";
        cout << "Operations: " << 2LL * n * n * n << endl;
    }

    MPI_Finalize();
    return 0;
}
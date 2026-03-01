import numpy as np
import sys

def verify_matrices(n, fileA, fileB, fileC):
    try:
        # Загрузка матриц
        A = np.loadtxt(fileA).reshape(n, n)
        B = np.loadtxt(fileB).reshape(n, n)
        C_cpp = np.loadtxt(fileC).reshape(n, n)
        
        # Умножение с помощью numpy
        C_numpy = np.dot(A, B)
        
        # Сравнение результатов
        if np.allclose(C_cpp, C_numpy, rtol=1e-10, atol=1e-10):
            print("✓ Верификация с помощью NumPy успешна: результаты совпадают")
            return True
        else:
            diff = np.abs(C_cpp - C_numpy)
            max_diff = np.max(diff)
            mean_diff = np.mean(diff)
            print(f" Ошибка верификации с помощью NumPy")
            print(f"  Максимальное расхождение: {max_diff:.2e}")
            print(f"  Среднее расхождение: {mean_diff:.2e}")
            
            # Вывод статистики по расхождениям
            print(f"  Количество значительных расхождений (>1e-6): {np.sum(diff > 1e-6)}")
            return False
            
    except Exception as e:
        print(f"Ошибка при верификации: {e}")
        return False

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Использование: result_verification.py <размер> <файл_A> <файл_B> <файл_C>")
        sys.exit(1)
    
    n = int(sys.argv[1])
    fileA = sys.argv[2]
    fileB = sys.argv[3]
    fileC = sys.argv[4] if len(sys.argv) > 4 else "result_matrix.txt"
    
    verify_matrices(n, fileA, fileB, fileC)
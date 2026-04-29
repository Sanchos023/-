import numpy as np
import sys

def generate_matrices(size):
    """
    Генерирует две квадратные матрицы размера size x size
    и сохраняет их в файлы matA.txt и matB.txt
    """
    np.random.seed(42) 
    A = np.random.randint(0, 20, (size, size))
    B = np.random.randint(0, 20, (size, size))
    
    np.savetxt('matA.txt', A, fmt='%d', delimiter=' ')
    np.savetxt('matB.txt', B, fmt='%d', delimiter=' ')
    
    print(f"Сгенерированы матрицы {size}x{size}")
    print("Файлы: matA.txt, matB.txt")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Использование: python generate.py <размер>")
        print("Пример: python generate.py 200")
        sys.exit(1)
    
    try:
        size = int(sys.argv[1])
        if size <= 0:
            print("Ошибка: размер должен быть положительным числом")
            sys.exit(1)
        
        generate_matrices(size)
        
    except ValueError:
        print("Ошибка: размер должен быть целым числом")
        sys.exit(1)
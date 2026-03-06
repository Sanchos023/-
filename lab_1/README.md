ОТЧЕТ



== MATRIX MULTIPLICATION ===

Size: 3x3
File A: matA.txt
File B: matB.txt
Output: result.txt

Loading matrices from files...
Matrices loaded successfully
Matrix size: 3 x 3

Matrix A:
         1          3          4
         4          5          6
         7          8          9

Matrix B:
         9          8          7
         6          5          4
         3          2          1

=== RESULTS ===
Execution time: 0.000000 seconds
Operations: 54

Result Matrix C:
        39         31         23
        84         69         54
       138        114         90
Result saved to file: result.txt



Matrix size: 4 x 4

Matrix A:
         1          3          4          3
         4          5          6          4
         7          8          9          4
         4          5          6          3

Matrix B:
         9          8          7          4
         6          5          4          5
         3          2          1          3
         1          2          3          4

=== RESULTS ===
Execution time: 0.000000 seconds
Operations: 128

Result Matrix C:
        42         37         32         43
        88         77         66         75
       142        122        102        111
        87         75         63         71
Result saved to file: result.txt


Matrix size: 5 x 5

Matrix A:
         1          3          4          6          7
         4          5          6          4          6
         7          8          9          5          7
         5          3          7          3          7
         3          6          7          8          5

Matrix B:
         9          8          7          4          5
         6          5          4          6          8
         3          2          1          4         74
         9          1          6          3          6
        10         11         23         53         52

=== RESULTS ===
Execution time: 0.000000 seconds
Operations: 250

Result Matrix C:
       163        114        220        427        725
       180        139        216        400        840
       253        196        281        498       1159
       181        149        233        446        949
       206        131        215        365        889

=== VERIFICATION ===
 Verification passed

 Верификация на python:

 Верификация с помощью NumPy успешна: результаты совпадают
/*
DONE: добавить функцию проверки для набора граничных значений border
TODO: сжать main2 и main3 в одну функцию - всё через массив
TODO: разделить функцию percolate на 2 с выбором направления
TODO: избавиться от обычных матриц и сделать все walled
TODO: добавить функцию вывода поля как картинку (0 - black, 1 - white, 2 - red)
TODO: попробовать переписать поле как класс с методами популяции, проверки проводимости и переписи значений
TODO: заменить все массивы на линеаризованные
*/


#include <iostream>
#include <random>
#include <thread>


//#define field 100
//#define field_w field+2


class Field {
public:
    int field_size;
    int field_size_w = field_size_w + 2;

    double border;

    double* field_lin;
    double* field_lin_tmp;

    Field(int field_size_in) {
        field_size = field_size_in;

        create_arrays();
    }

    void create_arrays() {
        double* field_lin = new double[field_size_w * field_size_w];
        double* field_lin_tmp = new double[field_size_w * field_size_w];
    }


    void populate_zeros() {
        for (int i = 0; i < field_size_w * field_size_w; i++) {
            field_lin[i] = 0;
        }
    }


    void populate_random(int border) {

        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_real_distribution<double> distr(0, 1);

        for (int i = 1; i < field_size_w-1; i++) {
            for (int j = 1; j < field_size_w-1; j++) {
                if (distr(eng) < border) { 
                    field_lin[i + j * field_size_w] = 1; 
                    continue;
                }
                field_lin[i + j * field_size_w] = 0; 
            }
        }
    }


    void copy_to_tmp() {
        for (int i = 0; i < field_size_w; i++) {
            for (int j = 0; j < field_size_w; j++) {
                field_lin_tmp[i + j * field_size_w] = field_lin[i + j * field_size_w];
            }
        }
    }
    
    void infect_wall(int dim) {
        if (dim == 0) {
            for (int i = 0; i < field_size_w; i++) {
                if (field_lin_tmp[1 + i * field_size_w] == 1) {
                    field_lin_tmp[1 + i * field_size_w] = 2;
                }
            }
            return;
        }

        for (int i = 0; i < field_size_w; i++) {
            if (field_lin_tmp[i + 1 * field_size_w] == 1) {
                field_lin_tmp[i + 1 * field_size_w] = 2;
            }
        }
    }


    bool evolve(int dim) {
        bool run_evolution = true;
        bool cycle_infected;

        while (run_evolution) {
            cycle_infected = false;

            for (int i = 1; i < field_size_w - 1; i++) {
                for (int j = 1; j < field_size_w - 1; j++) {
                    if (field_lin_tmp[i+j*field_size_w] == 2) {
                        if (field_lin_tmp[(i+1) + j * field_size_w] == 1) {
                            cycle_infected = true;
                            field_lin_tmp[(i + 1) + j * field_size_w] = 2;
                        }
                        if (field_lin_tmp[(i - 1) + j * field_size_w] == 1) {
                            cycle_infected = true;
                            field_lin_tmp[(i - 1) + j * field_size_w] = 2;
                        }
                        if (field_lin_tmp[i + (j + 1) * field_size_w] == 1) {
                            cycle_infected = true;
                            field_lin_tmp[i + (j + 1) * field_size_w] = 2;
                        }
                        if (field_lin_tmp[i + (j - 1) * field_size_w] == 1) {
                            cycle_infected = true;
                            field_lin_tmp[i + (j - 1) * field_size_w] = 2;
                        }
                    }
                }
            }

            //print_matrix(matrix, matrix_size_w);
            //std::cout << "\n\n";

            if (!cycle_infected) {
                return false;
            }

            if (dim == 0) {
                for (int i = 0; i < field_size_w; i++) {
                    if (field_lin_tmp[field_size_w - 2 + i * field_size_w] == 2) {
                        run_evolution = false;
                    }
                }
            }
            else {
                for (int i = 0; i < field_size_w; i++) {
                    if (field_lin_tmp[i+(field_size_w - 2)* field_size_w] == 2) {
                        run_evolution = false;
                    }
                }
            }

        }
        return true;
    }


    bool percolate_once(double border_in) {
        border = border_in;

        create_arrays();
        populate_zeros();
        populate_random(border);

        copy_to_tmp();
        infect_wall(0);
        if (evolve(0)) {
            return true;
        }

        copy_to_tmp();
        infect_wall(1);
        if (evolve(1)) {
            return true;
        }
        return false;
    }
};


// Функция популяции матрицы нулями
void populate_zeros(double** matrix, int matrix_size) {
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            matrix[i][j] = 0;
        }
    }
}


// Функция популяции матрицы случайными 1 и 0 на основе border
void matrix_rand_populate(double** matrix, int matrix_size, double border) {

    // Создание объекта случайных величин
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> distr(0, 1);

    populate_zeros(matrix, matrix_size);

    // Популяция массива случайными 1 и 0 на основе сл. числа и границы border
    for (int i = 1; i < matrix_size-1; i++) {
        for (int j = 1; j < matrix_size-1; j++) {
            if (distr(eng) < border) {
                matrix[i][j] = 1;
                continue;
            }
            matrix[i][j] = 0;
        }
    }
}

// Функция вывода матрицы в консоль
void print_matrix(double** matrix, int matrix_size) {
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            std::cout << matrix[i][j] << ' ';
        }
        std::cout << '\n';
    }
}


// Функция копирования матрицы в центр walled матрицы
void copy_matrix(double** matrix1, double** matrix2, int matrix_size) {
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            matrix2[i][j] = matrix1[i][j];
        }
    }
}


// Функция заполнения проволящих клеток с одной стороны зараженными
void infect_wall(double** matrix, int matrix_size_walled, int dim) {
    if (dim == 0) {
        for (int i = 0; i < matrix_size_walled; i++) {
            if (matrix[1][i] == 1) {
                matrix[1][i] = 2;
            }
        }
        return;
    }

    for (int i = 0; i < matrix_size_walled; i++) {
        if (matrix[i][1] == 1) {
            matrix[i][1] = 2;
        }
    }
}


// Функция постепенного заражения всего поля и проверка, дошла ли она до другой стороны
bool evolve(double** matrix, int matrix_size_w, int dim) {
    bool run_evolution = true;
    bool cycle_infected;

    while (run_evolution) {
        cycle_infected = false;

        for (int i = 1; i < matrix_size_w-1; i++) {
            for (int j = 1; j < matrix_size_w-1; j++) {
                if (matrix[i][j] == 2) {
                    if (matrix[i + 1][j] == 1) {
                        cycle_infected = true;
                        matrix[i + 1][j] = 2;
                    }
                    if (matrix[i - 1][j] == 1) {
                        cycle_infected = true;
                        matrix[i - 1][j] = 2;
                    }
                    if (matrix[i][j + 1] == 1) {
                        cycle_infected = true;
                        matrix[i][j + 1] = 2;
                    }
                    if (matrix[i][j - 1] == 1) {
                        cycle_infected = true;
                        matrix[i][j - 1] = 2;
                    }
                }
            }
        }

        //print_matrix(matrix, matrix_size_w);
        //std::cout << "\n\n";

        if (!cycle_infected) {
            return false;
        }

        if (dim == 0) {
            for (int i = 0; i < matrix_size_w; i++) {
                if (matrix[matrix_size_w - 2][i] == 2) {
                    return true;
                }
            }
        }
        else {
            for (int i = 0; i < matrix_size_w; i++) {
                if (matrix[i][matrix_size_w - 2] == 2) {
                    return true;
                }
            }
        }
        
    }
}


// Функция проверка, является ли поле проводящим
bool percolate(double** matrix, int matrix_size_w, int dim) {

    double** matrix_tmp = new double* [matrix_size_w];
    for (int i = 0; i < matrix_size_w; i++) {
        matrix_tmp[i] = new double[matrix_size_w];
    }

    populate_zeros(matrix_tmp, matrix_size_w);
    copy_matrix(matrix, matrix_tmp, matrix_size_w);

    bool result;
    if (dim == 0) {
        infect_wall(matrix_tmp, matrix_size_w, 0);
        result = evolve(matrix_tmp, matrix_size_w, 0);
    }
    else {
        infect_wall(matrix_tmp, matrix_size_w, 1);
        result = evolve(matrix_tmp, matrix_size_w, 1);
    }

    for (int i = 0; i < matrix_size_w; i++) { delete[] matrix_tmp[i]; }
    delete[] matrix_tmp;
    return result;

}


// Функция создания и проверки одного поля
int main1() {
    int field_size = 5;
    int field_size_w = field_size+2;
    double border = 0.5;

    // Поле перколяции (2d матрицы)
    double** field_perc = new double* [field_size_w];
    for (int i = 0; i < field_size_w; i++) {
        field_perc[i] = new double[field_size_w];
    }

    // Популяция случайными 1 или 0
    matrix_rand_populate(field_perc, field_size_w, border);
    print_matrix(field_perc, field_size_w);

    bool conductive = false;
    if (percolate(field_perc, field_size_w, 0)) {
        conductive = true;
    }
    else if (percolate(field_perc, field_size_w, 1)) {
        conductive = true;
    }

    for (int i = 0; i < field_size_w; i++) { delete[] field_perc[i]; }
    delete[] field_perc;
    return conductive;
}


int main2() {
    int field_size = 100;
    int samples_per_border = 200;
    double num_borders = 100;

    double borders_start = 0.55;
    double borders_end = 0.64;
    double d_border = (borders_end - borders_start) / num_borders;

    double** field_perc = new double* [field_size];
    for (int i = 0; i < field_size; i++) { 
        field_perc[i] = new double[field_size]; 
    }

    double cur_b;
    int cur_successes;
    double freq;
    for (int i = 0; i < num_borders; i++) {
        cur_b = borders_start + d_border * i;

        cur_successes = 0;
        for (int n = 0; n < samples_per_border; n++) {
            matrix_rand_populate(field_perc, field_size, cur_b);
            if (percolate(field_perc, field_size, 0)) {
                cur_successes++;
            }
            else if (percolate(field_perc, field_size, 1)) {
                cur_successes++;
            }
        }
        freq = cur_successes / double(samples_per_border);

        std::cout << cur_b << ' ' << freq << '\n';
    }

    for (int i = 0; i < field_size; i++) { delete[] field_perc[i]; }
    delete[] field_perc;
    return 0;
}


int main() {
    
    // Однократный тест перколяции
    std::cout << main1();

    // Многократный тест по нескольким border
    //main2();

    // Многократный тест по нескольким border параллельно 
    //main3();

    //Field fieldObj(10);

    //fieldObj.field_size = 10;
    //fieldObj.create_arrays();
    //std::cout << fieldObj.percolate_once(0.5);

    

    return 1;
}
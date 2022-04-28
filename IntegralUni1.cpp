#include <iostream>
#include <thread>
#include <chrono>



double my_simple_function(double x) {
    return x*x;
}


// Рассчёт интеграла методом прямоугольников
double definite_integral(double func(double), double p_1, double p_2, int num_col) {

    // Проверка, не перепутаны ли пределы интегрирования

    if (p_1 > p_2){
        double tmp_val;
        tmp_val = p_1;
        p_1 = p_2;
        p_2 = tmp_val;
    }

    // Разбиваем интеграл на num_col прямоугольников, суммимуем их площади

    double d_l = (p_2 - p_1)/num_col;
    double sum = 0, x = p_1;
    while (x < p_2) {
        sum += func(x);
        x += d_l;
    }
    sum *= d_l;

    return sum;
}


// Рассчёт интеграла методом прямоугольников с выбранной точностью
double auto_integral(double func(double), double p_1, double p_2, double prec) {
    double delta_integral = 1e10;
    int cur_num_col = 10;
    double cur_integral_value = 0;
    double past_integral_value = delta_integral;

    // Считать интеграл методом прямоугольников, увеличивая количество прямоугольников, 
    // пока не достигнем нужной точности

    while (delta_integral > prec) {
        cur_integral_value = definite_integral(func, p_1, p_2, cur_num_col);
        delta_integral = abs(cur_integral_value - past_integral_value);
        past_integral_value = cur_integral_value;

        cur_num_col *= 2;
    }
    cur_num_col /= 2;

    return cur_integral_value;
}


// Рассчёт интеграла методом прямоугольников с выбранной точностью для распалаллеливания
double auto_integral_for_parallel(double func(double), double p_1, double p_2, double prec, double* result_ptr, int* cols_ptr) {

    // Единственное отличие от auto_integral в том, что тут значение потом сохраняется по указателю

    double delta_integral = 1e10;
    int cur_num_col = 10;
    double cur_integral_value = 0;
    double past_integral_value = delta_integral;

    
    while (delta_integral > prec) {
        double d_l = (p_2 - p_1) / cur_num_col;
        cur_integral_value = definite_integral(func, p_1, p_2, cur_num_col);
        delta_integral = abs(cur_integral_value - past_integral_value);
        past_integral_value = cur_integral_value;

        cur_num_col *= 2;
    }
    cur_num_col /= 2;

    *result_ptr = cur_integral_value;
    *cols_ptr = cur_num_col;
    return 0;
}


// Паралелльный рассчёт интеграла методом прямоугольников с выбранной точностью
double auto_integral_parallel(double func(double), double p_1, double p_2, double prec, int num_parts) {

    // Проверка, не перепутаны ли пределы интегрирования

    if (p_1 > p_2) {
        double tmp_val;
        tmp_val = p_1;
        p_1 = p_2;
        p_2 = tmp_val;
    }

    // Создание потоков, массива с результатами, рассчёт пределов интеграрования для каждого интеграла

    std::thread* threads_array = new std::thread[num_parts];
    double* results = new double[num_parts];
    int* cols = new int[num_parts];

    double delta_length = (p_2 - p_1) / double(num_parts);
    double x = p_1;

    // Запуск потоков

    for (int i = 0; i < num_parts; i++) {
        threads_array[i] = std::thread(auto_integral_for_parallel, func, x+i*delta_length, x + (i+1) * delta_length, prec, &results[i], &cols[i]);
    }

    // Ожидание завершения потоков
    
    for (int i = 0; i < num_parts; i++) {
        threads_array[i].join();
    }

    // Суммирование результатов

    double result_val = 0;
    for (int i = 0; i < num_parts; i++) {
        result_val += results[i];
        //std::cout << cols[i] << '\n';
    }


    delete[] threads_array;
    delete[] results;
    delete[] cols;

    return result_val;
}


int main()
{   
    // Задание пределов интегрирования
    double P_1 = 0;
    double P_2 = 600;
    double PREC = 10;
    int NUM_THREADS = 2;

    
    //double result_p = auto_integral_parallel(my_simple_function, P_1, P_2, PREC, NUM_THREADS);
    //double result_n = auto_integral(my_simple_function, P_1, P_2, PREC);

    std::cout << "From: " << P_1 << "\n";
    std::cout << "To: " << P_2 << "\n";
    std::cout << "Precision: " << PREC << "\n";
    std::cout << "Number of threads: " << NUM_THREADS << "\n\n";



    std::cout << "Results:" << "\n";
    
    // Результат параллельного рассчёта интеграла
    double result_p;

    auto start1 = std::chrono::high_resolution_clock::now();
    result_p = auto_integral_parallel(my_simple_function, P_1, P_2, PREC, NUM_THREADS);
    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration1 = end1 - start1;

    std::cout << "Parallel" << " " << result_p << "\n";
    std::cout << "Eval:" << " " << duration1.count() << "s" << "\n\n";
    

    // Результат нормального рассчёта интеграла
    double result_n;

    auto start = std::chrono::high_resolution_clock::now();
    result_n = auto_integral(my_simple_function, P_1, P_2, PREC);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;

    std::cout << "Normal" << " " << result_n << "\n";
    std::cout << "Eval:" << " " << duration.count() << "s" << "\n\n";
}

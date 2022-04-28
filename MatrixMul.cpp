#include <iostream>
#include <random>
#include <thread>
#include <chrono>


void set_values_matrix(double** matrix, int matrix_size, bool zeros=false) {
	int MIN = 1;
	int MAX = 10;

	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<double> distr(MIN, MAX);

	if (zeros == true) {
		for (int i = 0; i < matrix_size; i++) {
			for (int j = 0; j < matrix_size; j++) {
				matrix[i][j] = 0;
			}
		}
		return;
	}

	for (int i = 0; i < matrix_size; i++) {
		for (int j = 0; j < matrix_size; j++) {
			matrix[i][j] = distr(eng);
		}
	}
}


void print_matrix(double** matrix, int matrix_size) {
	for (int i = 0; i < matrix_size; i++) {
		for (int j = 0; j < matrix_size; j++) {
			std::cout << matrix[i][j] << ' ';
		}
		std::cout << '\n';
	}
	std::cout << "\n\n";
}


void mul_matrix_normal(double** matrix1, double** matrix2, double** result_matrix, int matrix_size) {
	for (int i = 0; i < matrix_size; i++) {
		for (int j = 0; j < matrix_size; j++) {
			for (int k = 0; k < matrix_size; k++) {
				result_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
}


void mul_matrix_part(double** matrix1, double** matrix2, double** result_matrix, int i_start, int i_end, int matrix_size) {
	for (int i = i_start; i < i_end; i++) {
		for (int j = 0; j < matrix_size; j++) {
			for (int k = 0; k < matrix_size; k++) {
				result_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
}


void mul_matrix_parallel(double** matrix1, double** matrix2, double** result_matrix, int matrix_size, int num_threads) {

	std::thread* threads_array = new std::thread[num_threads];

	int delta_i = matrix_size / num_threads;
	//std::cout << delta_i << "\n\n";

	int i_last_start = delta_i * (num_threads - 1);
	int i_last_end = matrix_size;

	for (int n = 0; n < num_threads; n++) {
		if (n == num_threads - 1) {
			threads_array[n] = std::thread(mul_matrix_part, matrix1, matrix2, result_matrix, i_last_start, i_last_end, matrix_size);
			continue;
		}
		threads_array[n] = std::thread(mul_matrix_part, matrix1, matrix2, result_matrix, delta_i * n, delta_i * (n + 1), matrix_size);
	}

	for (int n = 0; n < num_threads; n++) { threads_array[n].join(); }

	delete[] threads_array;
}


bool check_equal(double** matrix1, double** matrix2, int matrix_size) {
	for (int i = 0; i < matrix_size; i++) {
		for (int j = 0; j < matrix_size; j++) {
			if (matrix1[i][j] != matrix2[i][j]) {
				return false;
			}
		}
	}
	return true;
}


void transpose(double** matrix, double** res_matrix, int matrix_size) {
	for (int i = 0; i < matrix_size; i++) {
		for (int j = 0; j < matrix_size; j++) {
			res_matrix[i][j] = matrix[j][i];
		}
	}
}


void linearize(double** matrix, double* linear_matrix, int matrix_size) {
	for (int i = 0; i < matrix_size; i++) {
		for (int j = 0; j < matrix_size; j++) {
			linear_matrix[i + j * matrix_size] = matrix[i][j];
		}
	}
}


void mul_matrix_normal_with_linearization(double** matrix1, double** matrix2, double** result_matrix, int matrix_size) {

	double** matrix2_t = new double* [matrix_size];
	for (int i = 0; i < matrix_size; i++) {
		matrix2_t[i] = new double[matrix_size];
	}

	transpose(matrix2, matrix2_t, matrix_size);

	double* matrix1_lin = new double[matrix_size * matrix_size];
	double* matrix2_lin = new double[matrix_size * matrix_size];

	linearize(matrix1, matrix1_lin, matrix_size);
	linearize(matrix2, matrix2_lin, matrix_size);

	for (int i = 0; i < matrix_size; i++) {
		for (int j = 0; j < matrix_size; j++) {
			for (int k = 0; k < matrix_size; k++) {
				result_matrix[i][j] += matrix1_lin[i + k * matrix_size] * matrix2_lin[k + j * matrix_size];
			}
		}
	}

	delete[] matrix2_t;
	delete[] matrix1_lin;
	delete[] matrix2_lin;
}


void mul_matrix_parallel_with_linearization_part(double* matrix1_lin, double* matrix2_lin, double** result_matrix, int i_start, int i_end, int matrix_size) {
	for (int i = i_start; i < i_end; i++) {
		for (int j = 0; j < matrix_size; j++) {
			for (int k = 0; k < matrix_size; k++) {
				result_matrix[i][j] += matrix1_lin[i + k * matrix_size] * matrix2_lin[k + j * matrix_size];
			}
		}
	}
}


void mul_matrix_parallel_with_linearization(double** matrix1, double** matrix2, double** result_matrix, int matrix_size, int num_threads) {
	
	std::thread* threads_array = new std::thread[num_threads];
	
	double** matrix2_t = new double* [matrix_size];
	for (int i = 0; i < matrix_size; i++) {
		matrix2_t[i] = new double[matrix_size];
	}

	transpose(matrix2, matrix2_t, matrix_size);

	double* matrix1_lin = new double[matrix_size * matrix_size];
	double* matrix2_lin = new double[matrix_size * matrix_size];

	linearize(matrix1, matrix1_lin, matrix_size);
	linearize(matrix2, matrix2_lin, matrix_size);

	int delta_i = matrix_size / num_threads;
	int i_last_start = delta_i * (num_threads - 1);
	int i_last_end = matrix_size;

	for (int n = 0; n < num_threads; n++) {
		if (n == num_threads - 1) {
			threads_array[n] = std::thread(mul_matrix_parallel_with_linearization_part, matrix1_lin, matrix2_lin, result_matrix, i_last_start, i_last_end, matrix_size);
			continue;
		}
		threads_array[n] = std::thread(mul_matrix_parallel_with_linearization_part, matrix1_lin, matrix2_lin, result_matrix, delta_i * n, delta_i * (n + 1), matrix_size);
	}

	for (int n = 0; n < num_threads; n++) { threads_array[n].join(); }

	delete[] threads_array;
	delete[] matrix2_t;
	delete[] matrix1_lin;
	delete[] matrix2_lin;
}


int main(){
	int matrix_size = 50;
	int num_parallel_threads = 2;
	bool output_info = true;
	bool output_init = false;
	bool output_results = false;
	bool output_check = true;
	bool output_time = true;

	if (matrix_size > 50 && (output_init || output_results)) {
		std::cout << "!Initial nor result matrices will not be shown!\n\n";
		output_init = false;
		output_results = false;
	}

	// Выделение памяти под матрицы

	double** matrix1 = new double* [matrix_size];
	double** matrix2 = new double* [matrix_size];
	double** result_matrix_normal = new double* [matrix_size];
	double** result_matrix_parallel = new double* [matrix_size];
	double** result_matrix_lin = new double* [matrix_size];
	double** result_matrix_parallel_lin = new double* [matrix_size];

	for (int i = 0; i < matrix_size; i++) {
		matrix1[i] = new double[matrix_size];
		matrix2[i] = new double[matrix_size];
		result_matrix_normal[i] = new double[matrix_size];
		result_matrix_parallel[i] = new double[matrix_size];
		result_matrix_lin[i] = new double[matrix_size];
		result_matrix_parallel_lin[i] = new double[matrix_size];
	}


	// Задание случайных значений матриц
	set_values_matrix(matrix1, matrix_size);
	set_values_matrix(matrix2, matrix_size);
	set_values_matrix(result_matrix_normal, matrix_size, true);
	set_values_matrix(result_matrix_parallel, matrix_size, true);
	set_values_matrix(result_matrix_lin, matrix_size, true);
	set_values_matrix(result_matrix_parallel_lin, matrix_size, true);


	// Вывод общей информации
	if (output_info) {
		std::cout << "Matrix size: " << matrix_size << "\n";
		std::cout << "Threads: " << num_parallel_threads << "\n";
		std::cout << "\n\n";
	}

	// Вывод начальных матриц
	if (output_init) {
		print_matrix(matrix1, matrix_size);
		print_matrix(matrix2, matrix_size);
	}


	// Умножением матриц, замер времени
	
	auto start1 = std::chrono::high_resolution_clock::now();
	mul_matrix_normal(matrix1, matrix2, result_matrix_normal, matrix_size);
	auto end1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration1 = end1 - start1;

	auto start2 = std::chrono::high_resolution_clock::now();
	mul_matrix_parallel(matrix1, matrix2, result_matrix_parallel, matrix_size, num_parallel_threads);
	auto end2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration2 = end2 - start2;

	auto start3 = std::chrono::high_resolution_clock::now();
	mul_matrix_normal_with_linearization(matrix1, matrix2, result_matrix_lin, matrix_size);
	auto end3 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration3 = end3 - start3;

	auto start4 = std::chrono::high_resolution_clock::now();
	mul_matrix_parallel_with_linearization(matrix1, matrix2, result_matrix_parallel_lin, matrix_size, num_parallel_threads);
	auto end4 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration4 = end4 - start4;


	// Вывод результатов
	if (output_results) {
		std::cout << "RESULT\n\n";
		print_matrix(result_matrix_normal, matrix_size);
		print_matrix(result_matrix_parallel, matrix_size);
		print_matrix(result_matrix_lin, matrix_size);
		print_matrix(result_matrix_parallel_lin, matrix_size);
		std::cout << "\n\n";
	}

	// Вывод проверки на равенство
	if (output_check) {
		std::cout << "Parallel correct: " << check_equal(result_matrix_normal, result_matrix_parallel, matrix_size) << "\n";
		std::cout << "Linear correct: " << check_equal(result_matrix_normal, result_matrix_lin, matrix_size) << "\n";
		std::cout << "Linear parallel correct: " << check_equal(result_matrix_normal, result_matrix_parallel_lin, matrix_size) << "\n";
		std::cout << "\n\n";
	}

	// Вывод времени
	if (output_time) {
		std::cout << "Normal time: " << duration1.count() << "\n";
		std::cout << "Parallel time: " << duration2.count() << "\n";
		std::cout << "Linear time: " << duration3.count() << "\n";
		std::cout << "Linear parallel time: " << duration4.count() << "\n";
		std::cout << "\n\n";
	}

	//std::cout << "Hello World!\n";

	for (int i = 0; i < matrix_size; i++) {
		delete[] matrix1[i];
		delete[] matrix2[i];
		delete[] result_matrix_normal[i];
		delete[] result_matrix_parallel[i];
		delete[] result_matrix_lin[i];
		delete[] result_matrix_parallel_lin[i];
	}
}

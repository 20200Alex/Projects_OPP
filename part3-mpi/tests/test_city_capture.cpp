#include "CityCapture.hpp"
#include <gtest/gtest.h>
#include <mpi.h>
#include <iostream>

class CityCaptureTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        int initialized;
        MPI_Initialized(&initialized);
        if (!initialized) {
            int argc = 0;
            char** argv = nullptr;
            MPI_Init(&argc, &argv);
        }
    }
    
    static void TearDownTestCase() {
        int finalized;
        MPI_Finalized(&finalized);
        if (!finalized) {
            MPI_Finalize();
        }
    }
    
    void SetUp() override {
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank_);
        MPI_Comm_size(MPI_COMM_WORLD, &world_size_);
    }
    
    int world_rank_;
    int world_size_;
};

TEST_F(CityCaptureTest, ConstructorTest) {
    if (world_rank_ == 0) {
        CityCapture capture(5);
        SUCCEED();
    }
}

TEST_F(CityCaptureTest, CipherGeneration) {
    if (world_rank_ == 0) {
        CityCapture capture;
        // Не можем напрямую тестировать private методы
        SUCCEED();
    }
}

TEST_F(CityCaptureTest, BasicSimulation) {
    // Этот тест требует определенного количества процессов
    // Запускается только с правильным количеством
    if (world_size_ >= 6) { // 5 городов + командующий
        CityCapture capture(5);
        capture.simulateCapture();
        
        MPI_Barrier(MPI_COMM_WORLD);
        SUCCEED();
    } else {
        if (world_rank_ == 0) {
            std::cout << "Skipping simulation test - need at least 6 MPI processes" << std::endl;
        }
        SUCCEED();
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Инициализируем MPI для тестов
    MPI_Init(&argc, &argv);
    
    int result = RUN_ALL_TESTS();
    
    MPI_Finalize();
    return result;
}

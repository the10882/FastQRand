#include <iostream>
#include <fstream>
#include <array>
#include <random>
#include <thread>
#include <vector>

const char BASES[4] = {'G', 'T', 'A', 'C'};
const int SEQ_LENGTH = 102;
const int DEPTH = 140000000;
const std::string FLENAME = "random.fastq";
const int BUFFER_SIZE = 10000;
const int THREADS = 30;

int nBuffers = DEPTH/BUFFER_SIZE;
int buffersPerChunk = nBuffers / THREADS;

/*
┌─────────────────────────────────────────────────────────────┐
│                         main()                              │
│  - Инициализация основного файла                            │
│  - Создание и запуск потоков                                │
│  - Ожидание завершения (join)                               │
│  - Объединение чанков                                       │
└─────────────────────────────────────────────────────────────┘
                            │
              ┌─────────────┴─────────────┐
              │                           │
              ▼                           ▼
    ┌─────────────────┐         ┌─────────────────┐
    │  WriteChunks(0) │         │  WriteChunks(1) │
    │  Thread 0       │         │  Thread 1       │
    │                 │         │                 │
    │  - Локальный    │         │  - Локальный    │
    │    буфер        │         │    буфер        │
    │  - Генератор    │         │  - Генератор    │
    │  - Запись в     │         │  - Запись в     │
    │    chunk_0      │         │    chunk_1      │
    └─────────────────┘         └─────────────────┘
              │                           │
              └─────────────┬─────────────┘
                            │
                            ▼
                  ┌─────────────────┐
                  │   joinChunks()  │
                  │                 │
                  │  Объединение    │
                  │  chunk_0 +      │
                  │  chunk_1 →      │
                  │  random.fastq   │
                  └─────────────────┘
*/

//генерирование рандомный последоватнльностей
void generateSeq(char seq[SEQ_LENGTH]){
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> baseDist(0, 3);
    for (int i = 0; i<SEQ_LENGTH; i++){
        int BaseIndex = baseDist(gen);
        seq[i] = BASES[BaseIndex];
    }
    seq[SEQ_LENGTH-1] = '\0';
}

//генерирование рандомных оценок качества
void generateScore(char score[SEQ_LENGTH]){
    for (int i = 0; i<SEQ_LENGTH; i++){
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> scoreDist(59, 125); //генерация оценок иллюмина случайно в подходящем диапазоне
        int Random_score = scoreDist(gen);
        char Random_Char_Score = Random_score;
        score[i] = Random_Char_Score;
    }    
    score[SEQ_LENGTH-1] = '\0';
}

//запись массива буфферизации в файл
void writeBuffer2File(char buffer[BUFFER_SIZE][2][SEQ_LENGTH], int BufferN, std::string flename){
    std::ofstream fle(flename, std::ios::app);
    for(int read=0; read<BUFFER_SIZE; read++){
        fle << "@seq" << flename[flename.size()-1]<< BufferN << "B->" << read << std::endl;
        fle << buffer[read][0] << std::endl;
        fle << '+' << std::endl;
        fle << buffer[read][1] << std::endl;
    }
}

//создание массива-буффера, содержащего сиквенсы и оценки качества.
void fillBuffer(char buffer[BUFFER_SIZE][2][SEQ_LENGTH]) {
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        generateSeq(buffer[i][0]);
        generateScore(buffer[i][1]);
    }
    
}

//соединение результатов выполнения разных потоков
void joinChunks() {
    std::ofstream mainFle(FLENAME);
    for (int j = 0; j < THREADS; j++)
    {
        char chunk_idChar = '0'+j;
        std::string chunkname = FLENAME + chunk_idChar;
        std::ifstream chunkFle(chunkname);
        mainFle << chunkFle.rdbuf();
        chunkFle.close();
    }
    mainFle.close();
}

//запись результатов каждого потока в файл с индексом номера потока, наприемр rand.fastq0
void WriteChunks(int chunk_id) {
    thread_local char buffer[BUFFER_SIZE][2][SEQ_LENGTH];
    char chunk_idChar = '0'+chunk_id;
    std::string chunkname = FLENAME + chunk_idChar;
    for (int i=0; i<buffersPerChunk; i++) {
        fillBuffer(buffer);
        writeBuffer2File(buffer, i, chunkname);
    }
}

int main() {
    std::vector<std::thread> threads; 
    srand(time(0));
    for (int i = 0; i < THREADS; i++)
    {
        
        threads.emplace_back(WriteChunks, i);
    }
    for (auto& t : threads) {
        t.join();
    }
    joinChunks();
    return 0;
}


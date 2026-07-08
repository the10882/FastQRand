#include <iostream>
#include <fstream>
#include <array>
#include <random>

const char BASES[4] = {'G', 'T', 'A', 'C'};
const int SEQ_LENGTH = 102;
const int DEPTH = 137000000;
const std::string FLENAME = "random.fastq";
const int BUFFER_SIZE = 1000000;


char buffer[BUFFER_SIZE][2][SEQ_LENGTH]; //вынес из стека, чтобы не валился по seg fault

void generateSeq(char seq[SEQ_LENGTH]){
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> baseDist(0, 3);
    for (int i = 0; i<SEQ_LENGTH; i++){
    int BaseIndex = baseDist(gen);
    seq[i] = BASES[BaseIndex];
    }
    seq[SEQ_LENGTH-1] = '\0';
}

void generateScore(char score[SEQ_LENGTH]){
    for (int i = 0; i<SEQ_LENGTH; i++){
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> scoreDist(59, 125);
    int Random_score = scoreDist(gen);
    char Random_Char_Score = Random_score;
    score[i] = Random_Char_Score;
    }    
    score[SEQ_LENGTH-1] = '\0';
}

void printSeq(char seq[SEQ_LENGTH]){
    for (int i = 0; i<SEQ_LENGTH; i++){
        std::cout << seq[i];
    }
    std::cout << std::endl;
}

void writeBuffer2File(char buffer[BUFFER_SIZE][2][SEQ_LENGTH], int BufferN){
    std::ofstream fle(FLENAME, std::ios::app);
    for(int read=0; read<BUFFER_SIZE; read++){
        fle << "@seq" << BufferN << "B->" << read << std::endl;
        fle << buffer[read][0] << std::endl;
        fle << '+' << std::endl;
        fle << buffer[read][1] << std::endl;
    }
}

void fillBuffer(char buffer[BUFFER_SIZE][2][SEQ_LENGTH]){
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        generateSeq(buffer[i][0]);
        generateScore(buffer[i][1]);
    }
    
}


int main(){
    srand(time(0));
    int nBuffers = DEPTH/BUFFER_SIZE;
    for (int i=0; i<nBuffers; i++){
    fillBuffer(buffer);
    writeBuffer2File(buffer, i);
    }
    return 0;
}
#include <iostream>
#include <bitset>
#include <array>
#include <queue>
#include <cmath>
// addresses are 32 bits long
// each block contains 4 words
// each word is made of 4 bytes
// change these
constexpr int WORDS_IN_BLOCK = 4;
constexpr int BYTES_IN_WORD =  4;
constexpr int ADDRESS_LENGTH = 32; // in binary
constexpr int BYTE_OFFSET_BITS = log2(BYTES_IN_WORD);
constexpr int WORD_OFFSET_BITS = log2(WORDS_IN_BLOCK);
constexpr int TAG_SIZE_BITS = ADDRESS_LENGTH - BYTE_OFFSET_BITS - WORD_OFFSET_BITS;
constexpr int TOTAL_OFFSET = WORD_OFFSET_BITS + BYTE_OFFSET_BITS;
typedef std::array<std::bitset<BYTES_IN_WORD*8>,WORDS_IN_BLOCK> data_block;
// dont change these
// fully associative
template <size_t R,size_t N>
std::bitset<R> extractBits(std::bitset<N> const& source,int start, int end ){
    std::bitset<R> result;
    for(int i=start;i<=end;i++){
        result[i-start] = source[i];
    }
    return result;
}

template <size_t MAIN_MEMORY_SIZE_IN_BITS>
class MM{
    private:
        std::array<data_block,MAIN_MEMORY_SIZE_IN_BITS/WORDS_IN_BLOCK> data; 
    public:
        void getBlock(int i){
            
        }

};

template <size_t TOTAL_CACHE_LINES>
class Cache{
    private:
        typedef std::array<std::bitset<BYTES_IN_WORD*8>,WORDS_IN_BLOCK> data_block;
        std::array<data_block,TOTAL_CACHE_LINES> data;
        std::array<std::bitset<TAG_SIZE_BITS>,TOTAL_CACHE_LINES> tags;
        std::bitset<TOTAL_CACHE_LINES> valid;
        std::queue<std::bitset<TAG_SIZE_BITS>> fifoQueue;
        int checkValueInCache(std::bitset<ADDRESS_LENGTH> address ){
            for (int i=0;i<TOTAL_CACHE_LINES;i++){
                if( extractBits<28,32>(address,TOTAL_OFFSET - 1 ,ADDRESS_LENGTH-TOTAL_OFFSET - 1) == tags[i]){
                    std::cout<<"The value is in the cache"<<'\n'; 
                    return i;
                }
            }
            std::cout<<"the value is not in the cache"<<'\n';
            return false;
        } 
        void printCacheLine(int i){
            // valid // tag // data blocks
            std::cout<<"|" <<valid[i]<< " | " << tags[i] << " | ";
            for (int j=0;j <= WORDS_IN_BLOCK;j++){
                std::cout << data[i][j] << " | ";
            } 
            std::cout<<"\n";
        }
    public:
       // reset function sets everything to 0; 
       void reset(){
            for (auto& dblocks : data){
                for(auto& word : dblocks){
                    word.reset();
                }
            }
            for(auto& address: tags){
                address.reset();
            }
            valid.reset();
            while(!fifoQueue.empty()){
                fifoQueue.pop();
            }
       }
       void printCache(){
            for(size_t i=0;i<TOTAL_CACHE_LINES;i++){
                printCacheLine(i);
            }
       }
       // write function
       void write(std::bitset<BYTES_IN_WORD*8> new_data, std::bitset<ADDRESS_LENGTH> address ){
                  
       }
};


int main(){
    Cache<2> c1;
    c1.reset();
    c1.printCache();
}

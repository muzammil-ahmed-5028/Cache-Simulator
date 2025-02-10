#include <iostream>
#include <bitset>
#include <array>
#include <queue>
#include <cmath>
#include <algorithm>
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

// type definitions and enums
typedef std::bitset<BYTES_IN_WORD*8> data32_t;
typedef std::array<data32_t,WORDS_IN_BLOCK> data_block_t;
typedef std::bitset<ADDRESS_LENGTH> address_t;
typedef std::bitset<TAG_SIZE_BITS> tag_t;

enum Write_Policy {
    WRITE_THROUGH,
    WRITE_BACK
};
enum Allocation_Policy{
    WRITE_ALLOCATE,
    NO_ALLOCATE
};
// dont change these
// fully associative
// method to extract bits from a bitset
template <size_t R,size_t N>
std::bitset<R> extractBits(std::bitset<N> const& source,int start, int end ){
    std::bitset<R> result;
    for(int i=start;i<=end;i++){
        result[i-start] = source[i];
    }
    return result;
}
// helper function for getting the tag from a address
tag_t getTag(address_t address){
    return extractBits<TAG_SIZE_BITS,ADDRESS_LENGTH>(address,TOTAL_OFFSET,ADDRESS_LENGTH-1);
}
// helper function for getting the word select bits from a address
std::bitset<WORD_OFFSET_BITS> getWordSelect(address_t address){
    return extractBits<WORD_OFFSET_BITS,ADDRESS_LENGTH>(address,WORD_OFFSET_BITS + BYTE_OFFSET_BITS -1,BYTE_OFFSET_BITS )
}

template <size_t MAIN_MEMORY_SIZE>
class MM{
    private:
        std::array<data_block,MAIN_MEMORY_SIZE/WORDS_IN_BLOCK> data; 
    public:
        data_block_t readBlock(tag_t readTag){
            return data[readTag.to_ulong()];
        }
        void writeBlock(data_block_t writeData, tag_t writeTag ){
            data[writeTag.to_ulong()] = writeData;
        }
        void reset(){
            for(auto& dBlock:data){
                for (auto& word:dBlock){
                    word.reset();
                }
            }
        }
};

template <size_t TOTAL_CACHE_LINES>
class LRU{
    private:
        std::array<tag_t,TOTAL_CACHE_LINES> lru;
        constexpr int lastElementIndex = TOTAL_CACHE_LINES -1;
        int lru_counter=0;
    public:
        bool add(tag_t in_tag, tag_t& evictedTag){    
            if(lru_counter != lastElementindex){
                lru[lru_counter] = in_tag;
                lru_counter++;
                return 0;
            }else{
                evictedTag = lru[lastElementIndex];
                for(int i=lastElementIndex ; i > 0; i--){
                    std::swap(lru[i],lru[i-1]);
                }
                lru[0] = in_tag;
                return 1;
            }
        }
        void rearrange(tag_t in_tag){
            auto pIn_Tag = std::find(std::start(lru),std::end(lru),in_tag);
            for(auto it = pIn_Tag; it != std::start(lru); it--){
                std::swap(*it, *(it-1));
            }
        }

};


template <size_t TOTAL_CACHE_LINES>
class Cache{
    private:
        std::array<data_block_t,TOTAL_CACHE_LINES> data;
        std::array<tag_t,TOTAL_CACHE_LINES> tags;
        std::bitset<TOTAL_CACHE_LINES> valid;
        std::bitset<TOTAL_CACHE_LINES> dirty;
        std::queue<tag_t> fifoQueue;
        char cache_pointer = 0;
        LRU<TOTAL_CACHE_LINES> lruTracker;
    public:
        
        void printCacheLine(int i){
            // valid // tag // data blocks
            std::cout << "|" << valid[i] << " | " << tags[i] << " | ";
            for (int j = 0;j <= WORDS_IN_BLOCK ; j++ ){
                std::cout << data[i][j] << " | ";
            } 
            std::cout<<"\n";
        }

        void printCache(){
            for(size_t i=0;i<TOTAL_CACHE_LINES;i++){
                printCacheLine(i);
            }
        }

        bool readCache(address_t addr, data32_t& returnData){
            tag_t addressTag = extractBits<TAG_SIZE_BITS,ADDRESS_LENGTH>(addr,TOTAL_OFFSET - 1 ,ADDRESS_LENGTH-TOTAL_OFFSET - 1);
            std::bitset<log2(WORDS_IN_BLOCK)> wordSelect = extractBits<log2(WORDS_IN_BLOCK),ADDRESS_LENGTH>(addr,TOTAL_OFFSET - 2 ,TOTAL_OFFSET - 1);
            for (int i=0;i<TOTAL_CACHE_LINES;i++){
                if( addressTag == tags[i]){
                    std::cout<<"The value is in the cache"<<'\n'; 
                    returnData = data[i][wordSelect.to_ulong()];
                    lruTracker.rearrange(addressTag);
                    return i;
                }
            }
            std::cout<<"the value is not in the cache"<<'\n';
            return -1;
        }

        bool checkValueInCache(tag_t addressTag){
            for (int i=0;i<TOTAL_CACHE_LINES;i++){
                if( addressTag == tags[i]){
                    std::cout<<"The value is in the cache"<<'\n'; 
                    return i;
                }
            }
            std::cout<<"the value is not in the cache"<<'\n';
            return -1;
        }
       // reset function sets everything to 0; 
       void reset(){
            for (auto& dblocks : data){
                for(auto& word : dblocks){
                    word.reset();
                }
            }
            for(auto& tag: tags){
                tag.reset();
            }
            valid.reset();
            while(!fifoQueue.empty()){
                fifoQueue.pop();
            }
       }

       void writeValue(address_t address, data32_t inputData,t){
        /*
         new input data address, input data;
         add the tag to top of lru
         if lru full?
            pop from end of lru for replacemnt tag
            place the thing in replacement tag in corresponding cache line 
         
         */
        tag_t newDataTag = getTag(address);
        tag_t evictedTag;
        // check if there is an evicted tag
        if(lruTracker.add(newDataTag,evictedTag)){
            int i = checkValueInCache(evictedTag);
            if(dirty[i]){
                
            }
        }
        lruTracker.
       }
       
};

template <size_t TOTAL_CACHE_LINES, size_t MAIN_MEMORY_SIZE>
class MemorySystem{
    private:
        Cache<TOTAL_CACHE_LINES> cache;
        MM<MAIN_MEMORY_SIZE> mainMemory;
    public:
        void reset(){
            cache.reset();
            mainMemory.reset();
        }
        data32_t read(address addr){
            data32_t readData;
            if(cache.readCache(addr,readData) > -1){
                return readData;
            }
            // get data from main memory and write to cache
            else{
                tag_t blockaddress = getTag(addr);
                data_block_t block = mainMemory.readBlock(blockaddress);
                cache.writeValue(addr,block);
                return block[getWordSelect(addr).to_ulong];
            }
        }
};


int main(){
    Cache<32> c1;
    c1.reset();
    c1.printCache();
}

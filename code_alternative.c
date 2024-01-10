#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#define SEQ_OG_SIZE 1280
#define block_size 64
#define resolution 16
static int min_limit = 0;
static int max_limit = 65000;
static int threshold = 0;

// static int resolution = 16; // resolution: number of bits to repreent each sample
// static int block_size = 64; // block_size: number of samples in a block

int *pre_processor(int *block, int len, int *seq_delayed, int *delta)
{
    int *seq = block;
    for (int i = 0; i < len; i++)
    {
        seq_delayed[i] = (i > 0) ? seq[i - 1] : threshold;
    }
    int d[block_size] = {0}, t[block_size] = {0};
    for (int i = 0; i < len; i++)
    {
        d[i] = seq[i] - seq_delayed[i];
    }

    int ymax = max_limit;
    int ymin = min_limit;

    for (int i = 0; i < len; i++)
    {
        int a = ymax - seq_delayed[i];
        int b = seq_delayed[i] - ymin;
        int T = (a < b) ? a : b;
        t[i] = T;

        if (0 <= d[i] && d[i] <= T)
        {
            delta[i] = 2 * d[i];
        }
        else if (-T <= d[i] && d[i] < 0)
        {
            delta[i] = 2 * abs(d[i]) - 1;
        }
        else
        {
            delta[i] = T + abs(d[i]);
        }
    }

    printf("pre processed: ");
    for (int i = 0; i < len; i++)
    {
        printf("%d ", delta[i]);
    }
    printf("\n");

    return delta;
}
int *rand_seq_og(int min, int max, int range, int *seq_og) // generating 16 sameple sequence
{
    int i;
    srand(time(NULL));

    for (i = 0; i < range; i++)
    {
        seq_og[i] = rand() % 11 + 9820;
    }

    return seq_og;
}

void breaker(int *sequence, int range, int blocks, int block[blocks][block_size])
{
    int N = block_size; // Length of sublist
    int sequence_length = range;
    int num_sublists = sequence_length / N; // Number of sublists
    int remain_samples = sequence_length % N;
    int array[num_sublists][N];
    if (remain_samples == 0)
    {
        for (int sub_list = 0, i = 0; sub_list < num_sublists; sub_list++, i += N)
        {
            for (int j = 0; j < N; j++)
            {
                block[sub_list][j] = sequence[i + j];
            }
        }
    }
    else if (remain_samples > 0)
    {
        int i = 0, sub_list = 0;
        for (; sub_list < num_sublists; sub_list++, i += N)
        {
            for (int j = 0; j < N; j++)
            {
                block[sub_list][j] = sequence[i + j];
            }
        }
        for (int k = 0; k < N; k++)
        {
            block[sub_list][k] = '\0';
        }
        int rem = 0;
        for (; rem < remain_samples; rem++)
        {
            block[sub_list][rem] = sequence[i + rem];
        }
    }
}
void executor(int *seq_og, int seq_og_size, int blocks, int blocked_sequence[blocks][block_size], int *delta)
{
    // int *encoded_seq = NULL;
    int zero_block_seq[seq_og_size], encoded_seq[seq_og_size];
    // int zero_block_seq[seq_og_size];
    int zero_block_seq_len = 0;
    int N = block_size;
    breaker(seq_og, seq_og_size / sizeof(seq_og[0]), blocks, blocked_sequence);
    int seq_delayed[block_size];
    int sub_block = 0;
    int remain_samples = (seq_og_size / sizeof(seq_og[0])) % N;
    int zero_block_seq_index = 0, encoded_seq_index = 0;
    int zero_block_count = 0, nonzero_block_count = 0;
    for (; sub_block < blocks; sub_block++)
    {
        int zero_flag = 0, non_zero_flag = 0;
        pre_processor(blocked_sequence[sub_block], block_size, seq_delayed, delta);
        for (int i = 0; i < block_size; i++)
        {
            if (delta[i] != 0)
            {
                non_zero_flag += 1;
                break;
            }
        }
        if (non_zero_flag)
        {
            int j = 0;
            for (; j < block_size; j++)
            {
                encoded_seq[encoded_seq_index] = delta[j];
                encoded_seq_index += 1;
            }
            nonzero_block_count++;
        }
        else
        {
            int j = 0;
            for (; j < block_size; j++)
            {
                zero_block_seq[zero_block_seq_index] = delta[j];
                zero_block_seq_index += 1;
            }
            zero_block_count++;
        }
    }
    printf("\nnonzero_block_count=%d zero_block_count=%d\n", nonzero_block_count, zero_block_count);
}

int main()
{
    int seq_og[SEQ_OG_SIZE] = {0};
    int delta[block_size] = {0};
    int blocks = SEQ_OG_SIZE / block_size;
    int remain_samples = SEQ_OG_SIZE % block_size;
    if (remain_samples > 0)
    {
        blocks += 1;
    }
    int blocked_sequence[blocks][block_size];
    int sizeof_seq_og = sizeof(seq_og);
    rand_seq_og(9820, 9830, SEQ_OG_SIZE, seq_og);

    for (int i = 0; i < blocks; i++)
    {
        for (int j = 0; j < block_size; j++)
        {
            blocked_sequence[i][j] = 0;
        }
    }
    executor(seq_og, sizeof_seq_og, blocks, blocked_sequence, delta);

    return 0;
}
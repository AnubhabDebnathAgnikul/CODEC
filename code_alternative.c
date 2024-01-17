#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#define SEQ_OG_SIZE 1280
#define block_size 64
#define resolution 16
static int min_limit = 0;
static int max_limit = 65000;
static int threshold = 0;

char zero_block_id[] = "00000";
char second_extension_id[] = "00001";
char FS_block_id[] = "0001";
char k1_id[] = "0010";
char k2_id[] = "0011";
char k3_id[] = "0100";
char k4_id[] = "0101";
char k5_id[] = "0110";
char k6_id[] = "0111";
char k7_id[] = "1000";
char k8_id[] = "1001";
char k9_id[] = "1010";
char k10_id[] = "1011";
char k11_id[] = "1100";
char k12_id[] = "1101";
char k13_id[] = "1110";
char no_copression_id[] = "1111";

char id_lut[17][50] = {0};
char k_id[13][10];
// static int resolution = 16; // resolution: number of bits to repreent each sample
// static int block_size = 64; // block_size: number of samples in a block
char *
zero_block(int *sequence, int length, char *result, char *encoded);
void breaker(int *sequence, int range, int blocks, int block[blocks][block_size]);
int *pre_processor(int *block, int len, int *seq_delayed, int *delta);
int *rand_seq_og(int min, int max, int range, int *seq_og);
void FS_element(int zeros, char *fs_element);
int *code_select(int *block, char *encoded_block, int index);
char *split_sample(int *block, int k, int size, char *encoded, int *index);
char *second_extension(int *sequence);
void executor(int *seq_og, int seq_og_size, int blocks, int blocked_sequence[blocks][block_size], int *delta);
int *FS_block(int *block, char *encoded_block, int *index);
char *binary_result(int value, char *binary_p);
char *zfill(int number, int zero_resolution, char *binary_p);
int binary_decimal(char *msb, int len);
char *no_compression(int *block);
char *minimum(char *a, char *b);
int *dec(char *seq, char *e);

void entropy_decoder(char *seq, int size);
void entropy_decoder(char *seq, int size)
{
    strcat(id_lut[0], second_extension_id);
    strcat(id_lut[1], FS_block_id);
    strcat(id_lut[2], k1_id);
    strcat(id_lut[3], k2_id);
    strcat(id_lut[4], k3_id);
    strcat(id_lut[5], k4_id);
    strcat(id_lut[6], k5_id);
    strcat(id_lut[7], k6_id);
    strcat(id_lut[8], k7_id);
    strcat(id_lut[9], k8_id);
    strcat(id_lut[10], k9_id);
    strcat(id_lut[11], k10_id);
    strcat(id_lut[12], k11_id);
    strcat(id_lut[13], k12_id);
    strcat(id_lut[14], k13_id);
    strcat(id_lut[15], no_copression_id);

    strcat(k_id[0], k1_id);
    strcat(k_id[1], k2_id);
    strcat(k_id[2], k3_id);
    strcat(k_id[3], k4_id);
    strcat(k_id[4], k5_id);
    strcat(k_id[5], k6_id);
    strcat(k_id[6], k7_id);
    strcat(k_id[7], k8_id);
    strcat(k_id[8], k9_id);
    strcat(k_id[9], k10_id);
    strcat(k_id[10], k11_id);
    strcat(k_id[11], k12_id);
    strcat(k_id[12], k13_id);
    int current_index = 0;
    char block_id[10];
    while (current_index < size)
    {
        if (strncmp(seq + current_index, id_lut[0], 4) == 0 ||
            strncmp(seq + current_index, id_lut[1], 4) == 0 ||
            strncmp(seq + current_index, id_lut[2], 4) == 0 ||
            strncmp(seq + current_index, id_lut[3], 4) == 0 ||
            strncmp(seq + current_index, id_lut[4], 4) == 0 ||
            strncmp(seq + current_index, id_lut[5], 4) == 0 ||
            strncmp(seq + current_index, id_lut[6], 4) == 0 ||
            strncmp(seq + current_index, id_lut[7], 4) == 0 ||
            strncmp(seq + current_index, id_lut[8], 4) == 0 ||
            strncmp(seq + current_index, id_lut[9], 4) == 0 ||
            strncmp(seq + current_index, id_lut[10], 4) == 0 ||
            strncmp(seq + current_index, id_lut[11], 4) == 0 ||
            strncmp(seq + current_index, id_lut[12], 4) == 0 ||
            strncmp(seq + current_index, id_lut[13], 4) == 0 ||
            strncmp(seq + current_index, id_lut[14], 4) == 0 ||
            strncmp(seq + current_index, id_lut[15], 4) == 0)
        {
            strncpy(block_id, seq + current_index, 4);
            block_id[4] = '\0';
            char first_sample[resolution + 1];
            strncpy(first_sample, seq + current_index + 4, resolution);
            first_sample[resolution] = '\0';
            printf("first sample %d\n", strtol(first_sample, NULL, 2));
            if (strcmp(block_id, FS_block_id) == 0)
            {
                printf("FS block\n");
                current_index = current_index + FS_block_decoder(seq + current_index + 4 + resolution, first_sample);
                printf("current index %d\n", current_index);
            }
            else if (strcmp(block_id, no_copression_id) == 0)
            {
                printf("no compression\n");
                current_index = current_index + no_compression_decoder(seq + current_index + 4, first_sample);
                printf("current index %d\n", current_index);
            }
            else
            {
                printf("split sample\n");
                int i = -1, n = 0;
                for (; n < 13; n++)
                {
                    if (strcmp(block_id, k_id[i]) == 0)
                    {
                        i = n;
                        break;
                    }
                }
                if (i > -1)
                    current_index = current_index + split_sample_decoder(seq + current_index + 4 + resolution, i + 1, first_sample);
                printf("current index %d\n", current_index);
            }
        }
        else if (strncmp(seq + current_index, id_lut[0], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[1], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[2], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[3], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[4], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[5], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[6], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[7], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[8], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[9], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[10], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[11], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[12], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[13], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[14], 5) == 0 ||
                 strncmp(seq + current_index, id_lut[15], 5) == 0)
        {
            strncpy(block_id, seq + current_index, 5);
            block_id[5] = '\0';
            char first_sample[resolution + 1];
            strncpy(first_sample, seq + current_index + 5, resolution);
            first_sample[resolution] = '\0';
            printf("first sample %d\n", strtol(first_sample, NULL, 2));
            // continue with the rest of the code...
            if (strcmp(block_id, second_extension_id) == 0)
            {
                current_index = current_index + 5 + resolution + second_extension_decoder(seq + current_index + 5 + resolution, first_sample);
                printf("current index {current_index}");
            }
            else if (strcmp(block_id, zero_block_id) == 0)
            {
                current_index = current_index + zero_block_decoder(seq + current_index + 5);
                printf("current index {current_index}");
            }
        }
        else
            break;
    }
}
int *dec(char *seq, char *e)
{
    int *blocked_seq;
    int *dec_seq;
    int N = resolution;
    int seq_len = strlen(seq);
    int blocked_seq_len = seq_len / N;
    blocked_seq = (int *)malloc(blocked_seq_len * sizeof(int));
    dec_seq = (int *)malloc((blocked_seq_len + 1) * sizeof(int));

    for (int i = 0; i < blocked_seq_len; i++)
    {
        char block[N + 1];
        strncpy(block, seq + i * N, N);
        block[N] = '\0';
        blocked_seq[i] = strtol(block, NULL, 2);
        dec_seq[i + 1] = blocked_seq[i];
    }

    dec_seq[0] = seq_len;
    return dec_seq;
}
char *minimum(char *a, char *b)
{
    char *smallest;
    if (strlen(a) <= strlen(b))
    {
        smallest = a;
    }
    else
    {
        smallest = b;
    }
    return smallest;
}
char *no_compression(int *block)
{
    char *option_id = malloc(sizeof(char) * 6);
    char **n_block = malloc(sizeof(char *) * resolution);
    for (int i = 0; i < resolution; i++)
    {
        n_block[i] = malloc(sizeof(char) * (resolution + 1));
    }

    for (int i = 0; i < resolution; i++)
    {
        itoa(block[i], n_block[i], 2);
        while (strlen(n_block[i]) < resolution)
        {
            memmove(n_block[i] + 1, n_block[i], strlen(n_block[i]) + 1);
            n_block[i][0] = '0';
        }
    }

    if (resolution == 1 || resolution == 2)
    {
        strcpy(option_id, "1");
    }
    else if (resolution == 3 || resolution == 4)
    {
        strcpy(option_id, "11");
    }
    else if (resolution > 4 && resolution <= 8)
    {
        strcpy(option_id, "111");
    }
    else if (resolution > 8 && resolution <= 16)
    {
        strcpy(option_id, "1111");
    }
    else if (resolution > 16 && resolution <= 32)
    {
        strcpy(option_id, "11111");
    }

    char *n_block_str = malloc(sizeof(char) * (resolution * (resolution + 1) + 6));
    strcpy(n_block_str, option_id);
    for (int i = 0; i < resolution; i++)
    {
        strcat(n_block_str, n_block[i]);
    }

    for (int i = 0; i < resolution; i++)
    {
        free(n_block[i]);
    }
    free(n_block);
    free(option_id);

    return n_block_str;
}

int binary_decimal(char *msb, int len)
{
    int dec = 0, bit_dec = 0, i = 0, exponent, base = 2;
    len = len - 1;
    for (; len >= 0; len--, i++)
    {
        int result = 1;
        exponent = len;
        if (msb[len] == '1')
        {
            while (exponent != 0)
            {
                result *= base;
                --exponent;
            }
            bit_dec = 1 * result;
            dec = dec + bit_dec;
        }
        else
        {
            dec = dec + 0;
        }
    }
    return dec;
}
char *zfill(int number, int zero_resolution, char *binary_p)
{
    int bits = sizeof(number) * 8;
    char binary_arr[bits + 1];
    binary_result(number, binary_arr);

    int actual_resolution = strlen(binary_arr);

    if (actual_resolution < zero_resolution)
    {
        int n = zero_resolution - actual_resolution;
        char temp_zero[n + 1];
        memset(temp_zero, '0', n);
        temp_zero[n] = '\0';
        strcat(temp_zero, binary_arr);
        memcpy(binary_p, temp_zero, strlen(temp_zero));
        return binary_p;
    }
    else
    {
        memcpy(binary_p, binary_arr, strlen(binary_arr));
        return binary_p;
    }
}

char *binary_result(int value, char *binary_p)
{
    int bits = sizeof(value) * 8;
    char rev_binary[bits + 1];
    int index = 0, i = 0, temp_value = value, bit = 0;

    if (value == 0)
    {
        binary_p[0] = '0';
        binary_p[1] = '\0';
        return binary_p;
    }
    while (temp_value)
    {
        int bit = (temp_value % 2);
        rev_binary[index] = bit + '0';
        temp_value /= 2;
        index++;
    }

    for (index -= 1; index >= 0; index--, i++)
    {
        binary_p[i] = rev_binary[index];
    }

    binary_p[i] = '\0';

    return binary_p;
}
// char *binary_result(int value, char *binary_p)
// {
//     int index = 0;
//     int bits = sizeof(value) * 8;
//     int bit = 0;
//     for (int i = bits - 1; i >= 0; i--)
//     {
//         bit = (value >> i) & (0x01);
//         binary_p[index] = bit + '0';
//         index++;
//     }
//     return binary_p;
// }
int *FS_block(int *block, char *encoded_block, int *index)
{
    char binary_arr[64];
    char *binary_p = &binary_arr;
    if ((resolution == 3) || (resolution == 4))
    {
        strcat(encoded_block[*index], "01");
    }
    else if ((resolution > 4) && (resolution <= 8))
    {
        strcat(encoded_block[*index], "001");
    }
    else if ((resolution > 8) && (resolution <= 16))
    {
        strcat(encoded_block[*index], "0001");
    }
    else if ((resolution > 16) && (resolution <= 32))
    {
        strcat(encoded_block[*index], "00001");
    }
    zfill(block[0], 16, binary_p);
    strcat(encoded_block, binary_p);
    for (int i = 1; i < 64; i++)
    {
        zfill(block[i], block[i] + 1, binary_p);
        strcat(encoded_block, binary_p);
    }
}
char *second_extension(int *sequence)
{
    int *encoded = malloc(sizeof(int));
    int option_id = 0;
    sequence = realloc(sequence, (sizeof(sequence) + sizeof(int)));
    sequence[sizeof(sequence) - 1] = sequence[sizeof(sequence) - 2];
    printf("len seq %d\n", sizeof(sequence) / sizeof(int));
    int **paired_list = malloc((sizeof(sequence) / sizeof(int) - 2) * sizeof(int *));
    for (int i = 1; i < sizeof(sequence) / sizeof(int) - 1; i += 2)
    {
        paired_list[i / 2] = malloc(2 * sizeof(int));
        paired_list[i / 2][0] = sequence[i];
        paired_list[i / 2][1] = sequence[i + 1];
    }

    int gamma = 0;
    int *gamma_list = malloc((sizeof(sequence) / sizeof(int) - 2) * sizeof(int));
    char fs_element[64];
    for (int i = 0; i < sizeof(sequence) / sizeof(int) - 2; i++)
    {
        int *pair = paired_list[i];
        gamma = 0.5 * (pair[0] + pair[1]) * (pair[0] + pair[1] + 1) + pair[1];
        if (gamma + 1 <= 44)
        {
            encoded = realloc(encoded, (sizeof(encoded) + sizeof(int)));
            FS_element(gamma + 1, fs_element);
            int data = binary_decimal(fs_element, strlen(fs_element));
            encoded[sizeof(encoded) / sizeof(int) - 1] = data;
        }
        else
        {
            printf("not executing second extension\n");
            return 0;
        }
        gamma_list[i] = gamma;
    }

    if (resolution == 1 || resolution == 2)
    {
        option_id = 1;
    }
    else if (resolution == 3 || resolution == 4)
    {
        option_id = 1;
    }
    else if (resolution > 4 && resolution <= 8)
    {
        option_id = 1;
    }
    else if (resolution > 8 && resolution <= 16)
    {
        option_id = 1;
    }
    else if (resolution > 16 && resolution <= 32)
    {
        option_id = 1;
    }

    encoded = realloc(encoded, (sizeof(encoded) + sizeof(int)));
    encoded[sizeof(encoded) / sizeof(int) - 1] = sequence[0];
    encoded = realloc(encoded, (sizeof(encoded) + sizeof(int)));
    encoded[sizeof(encoded) / sizeof(int) - 1] = option_id;

    char *encoded_str = malloc(sizeof(char));
    for (int i = 0; i < sizeof(encoded) / sizeof(int); i++)
    {
        char *binary = malloc(17 * sizeof(char));
        itoa(encoded[i], binary, 2);
        encoded_str = realloc(encoded_str, (sizeof(encoded_str) + 17 * sizeof(char)));
        strcat(encoded_str, binary);
    }

    printf("gamma list en: ");
    for (int i = 0; i < sizeof(gamma_list) / sizeof(int); i++)
    {
        printf("%d ", gamma_list[i]);
    }
    printf("\n");

    return encoded_str;
}

char *split_sample(int *block, int k, int size, char *encoded, int *index)
{
    char option_id[64] = {0}, block_array[64] = {0};
    char msb[64], lsb[16];
    char binary[64];
    int n = resolution;
    char msb_fs[64];
    for (int sample = 1; sample < size; sample++)
    {
        int msb_dec = 0;
        zfill(block[sample], n, binary);
        memcpy(lsb, (void *)&binary[n - k], n - (n - k));
        lsb[n - (n - k) + 1] = '\0';
        memcpy(msb, (void *)&binary[0], (n - k));
        msb[(n - k) + 1] = '\0';
        msb_dec = binary_decimal(msb, strlen(msb));
        FS_element(msb_dec + 1, msb_fs);
        strcat(msb, msb_fs);
    }
    if ((resolution == 3) || (resolution == 4))
    {
        zfill(k + 1, 2, option_id);
    }
    else if ((resolution > 4) && (resolution <= 8))
    {
        zfill(k + 1, 3, option_id);
    }
    else if ((resolution > 8) && (resolution <= 16))
    {
        zfill(k + 1, 4, option_id);
    }
    else if ((resolution > 16) && (resolution <= 32))
    {
        zfill(k + 1, 5, option_id);
    }
    strcat(encoded, option_id);
    memset(block_array, '\0', sizeof(block_array));
    zfill(block[0], 16, block_array);
    strcat(encoded, block_array);
    strcat(encoded, msb);
    strcat(encoded, lsb);

    return encoded;
}

int *code_select(int *block, char *encoded_block, int index)
{
    int *fs_encoded = FS_block(block, encoded_block, &index);

    int *smallest = split_sample(block, 1, block_size, encoded_block, &index);
    for (int i = 1; i < resolution / 2; i++)
    {
        int *temp = split_sample(block, i, block_size);
        if (sizeof(temp) <= sizeof(smallest))
        {
            smallest = temp;
        }
    }
    int *split_sample_encoded = smallest;
    int *second_extension_encoded = second_extension(block);
    int *no_compression_encoded = no_compression(block);

    if (second_extension_encoded == 0)
    {
        encoded_block = minimum(
            minimum(fs_encoded, split_sample_encoded), no_compression_encoded);
    }
    else
    {
        encoded_block = minimum(minimum(fs_encoded, split_sample_encoded), minimum(
                                                                               second_extension_encoded, no_compression_encoded));
    }
    return encoded_block;
}

void FS_element(int zeros, char *fs_element)
{
    int i = 0;
    for (; i < zeros; i++)
    {
        if (i == (zeros - 1))
            fs_element[i] = '1';
        else
            fs_element[i] = '0';
    }
    fs_element[i] = '\0';
}

char *zero_block(int *sequence, int length, char *result, char *encoded)
{
    int zeros = 0;
    int ROS = 4;
    char optiion_id_arr[64];
    char *option_id = &optiion_id_arr;
    int blocks = length / block_size;
    int reminder_samples = length % block_size;
    if (reminder_samples > 0)
    {
        blocks += 1;
    }
    int block_list[blocks][block_size];
    breaker(sequence, length, blocks, block_list);
    int encoded_length = 0;

    for (int i = 0; i < blocks; i++)
    {
        int *block;
        block = block_list[i];
        printf("block_list[i]\n");
        int non_zero_block = 0;
        for (int k = 0; k < block_size; k++)
        {
            printf("%d,", block[k]);
            if (0 != block[i])
            {
                non_zero_block++;
                break;
            }
        }

        if (!non_zero_block)
        {
            zeros += 1;
            printf("%d,", zeros);
        }
        else
        {
            printf("\nnon zero block count=%d\n", non_zero_block);
            if (0 != zeros)
            {
                if (zeros <= ROS)
                {
                    FS_element(zeros, encoded);
                }
                else
                {
                    FS_element(zeros + 1, encoded);
                }
            }
            zeros = 0;
            int index = strlen(encoded);
            code_select(block, encoded, index);
        }
    }

    if (zeros != 0)
    {
        if (zeros <= ROS)
        {
            FS_element(zeros, encoded);
        }
        else
        {
            FS_element(zeros + 1, encoded);
        }
    }

    if (resolution == 1 || resolution == 2)
    {
        option_id = strtol("000", NULL, 2);
    }
    else if (resolution == 3 || resolution == 4)
    {
        option_id = strtol("0000", NULL, 2);
    }
    else if (resolution > 4 && resolution <= 8)
    {
        option_id = strtol("00000", NULL, 2);
    }
    else if (resolution > 8 && resolution <= 16)
    {
        option_id = strtol("000000", NULL, 2);
    }
    else if (resolution > 16 && resolution <= 32)
    {
        option_id = strtol("0000000", NULL, 2);
    }

    strcat(result, option_id);
    strcat(result, encoded);
    int j = strlen(result);
    result[j + 1] = '\0';

    return result;
}

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
    int i = 0;
    srand(time(NULL));

    for (; i < range - 640; i++)
    {
        seq_og[i] = rand() % 11 + 9820;
    }
    for (; i < range - 64; i++)
    {
        seq_og[i] = 0;
    }
    for (; i < range; i++)
    {
        seq_og[i] = rand() % 11 + 9820;
    }

    return seq_og;
}

void breaker(int *sequence, int range, int blocks, int block[blocks][block_size])
{
    int N = block_size; // Length of sublist
    int sequence_length = range;
    int num_of_blocks = sequence_length / N; // Number of sublists
    int remain_samples = sequence_length % N;
    if (remain_samples == 0)
    {
        for (int sub_block = 0, i = 0; sub_block < num_of_blocks; sub_block++, i += N)
        {
            for (int j = 0; j < N; j++)
            {
                block[sub_block][j] = sequence[i + j];
            }
        }
    }
    else if (remain_samples > 0)
    {
        int i = 0, sub_block = 0;
        for (; sub_block < num_of_blocks; sub_block++, i += N)
        {
            for (int j = 0; j < N; j++)
            {
                block[sub_block][j] = sequence[i + j];
            }
        }
        for (int k = 0; k < N; k++)
        {
            block[sub_block][k] = '\0';
        }
        int rem = 0;
        for (; rem < remain_samples; rem++)
        {
            block[sub_block][rem] = sequence[i + rem];
        }
    }
}
void executor(int *seq_og, int seq_og_size, int blocks, int blocked_sequence[blocks][block_size], int *delta)
{
    // int *encoded_seq = NULL;
    int zero_block_seq[seq_og_size], encoded_seq[seq_og_size];
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
            if (zero_block_count != 0)
            {
                printf("zero block count block called\n");
                char encoded[4096];
                char result[4096];
                zero_block(zero_block_seq, zero_block_seq_index, &result[0], &encoded[0]);
                strcat(encoded_seq, result);
                zero_block_count = 0;
                zero_block_seq_index = 0;
            }
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
            printf("\nzero block count=%d,index=%d\n", zero_block_count, zero_block_seq_index);
        }
    }
    if (zero_block_count != 0)
    {
        char encoded[4096];
        char result[4096];
        zero_block(zero_block_seq, zero_block_seq_index, &result[0], &encoded[0]);
        strcat(encoded_seq, result);
        zero_block_count = 0;
        zero_block_seq_index = 0;
    }
    char e[4096];
    dec(encoded_seq, e);
    float c_ratio = len(seq_og) / len(e);
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

    entropy_decoder(seq_og, SEQ_OG_SIZE);
    return 0;
}
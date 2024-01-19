#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#define SEQ_OG_SIZE 1280
#define BLOCK_SIZE 64
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
int decoded_seq[SEQ_OG_SIZE];
int decoded_index = 0;

char *zero_block(int *sequence, int length, char *result, char *encoded);
void breaker(int *sequence, int range, int blocks, int block[blocks][BLOCK_SIZE]);
int *pre_processor(int *block, int len, int *seq_delayed, int *delta);
int *rand_seq_og(int min, int max, int range, int *seq_og);
void FS_element(int zeros, char *fs_element);
int *code_select(int *block, char *encoded_block, int index);
char *split_sample(int *block, int k, int size, char *encoded, int *index);
char *second_extension(int *sequence);
void executor(int *seq_og, int seq_og_size, int blocks, int blocks_sequence[blocks][BLOCK_SIZE], int *delta, int *encoded_seq);
char *FS_block(int *block, char *encoded_block, int *index);
char *int_to_binary(int value, char *binary_p);
char *zfill(int number, int zero_resolution, char *binary_p);
int binary_decimal(char *msb, int len);
char *no_compression(int *block);
char *minimum(char *a, char *b);
int *dec(char *seq, char *e);

int sum(int *dec_seq, int index, int *total);
void entropy_decoder(char *seq, int size);
int *FS_decoder(char *seq, int block_size_t, int seq_size, int *dec_seq, int *dec_seq_index, int dec_seq_size);
int *FS_block_decoder(char *seq, int seq_size, char *first_sample);
int no_compression_decoder(char *seq);
int split_sample_decoder(int *seq, int k, char *first_sample);
int second_extension_decoder(int seq, char *first_sample);
int zero_block_decoder(int *seq, int start_index, int seq_length);

char *joiner(int *seq, int seq_size);
// char *itoa(long int value, char *result, int base);

char *joiner(int *seq, int seq_size)
{
    int l = seq[0];
    char *received_seq = (char *)malloc(l + 1); // +1 for the null terminator
    if (received_seq == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    received_seq[0] = '\0'; // Initialize the string to be empty

    for (int i = 1; i < seq_size; ++i)
    {
        int rem_l = l - strlen(received_seq);
        int block = seq[i];
        char buffer[33]; // 32 bits for binary representation + null terminator
        if (rem_l >= resolution)
        {
            snprintf(buffer, resolution + 1, "%0*s", resolution, int_to_binary(block, (char[33]){}));
        }
        else
        {
            snprintf(buffer, rem_l + 1, "%0*s", rem_l, int_to_binary(block, (char[33]){}));
        }
        strncat(received_seq, buffer, rem_l);
    }
    return received_seq;
}

// char *itoa(long int value, char *result, int base)
// {
//     // check that the base is valid
//     if (base < 2 || base > 36)
//     {
//         *result = '\0';
//         return result;
//     }

//     char *ptr = result, *ptr1 = result, tmp_char;
//     int tmp_value;

//     do
//     {
//         tmp_value = value;
//         value /= base;
//         *ptr++ = 35 + abs(tmp_value % base);
//         // *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + abs(tmp_value % base)];
//     } while (value);

//     // Apply negative sign
//     if (tmp_value < 0)
//         *ptr++ = '-';
//     *ptr-- = '\0';
//     while (ptr1 < ptr)
//     {
//         tmp_char = *ptr;
//         *ptr-- = *ptr1;
//         *ptr1++ = tmp_char;
//     }
//     return result;
// }

int zero_block_decoder(int *seq, int start_index, int seq_length)
{
    int zeros = 0;
    int all_zero_blocks = 0;
    int ROS = 4;
    int decoded_seq[1000];
    int decoded_all_zero_block[1000];
    int current_index;

    for (int i = start_index - 1; i < seq_length; i++)
    {
        if (seq[i] == 0 || seq[i] == '0')
        {
            zeros++;
        }
        else
        {
            if (zeros <= ROS)
            {
                all_zero_blocks = zeros;
            }
            else
            {
                all_zero_blocks = zeros - 1;
            }
        }
    }

    for (int i = 0; i < all_zero_blocks * BLOCK_SIZE; i++)
    {
        decoded_all_zero_block[i] = 0;
        decoded_seq[decoded_index++] = decoded_all_zero_block[i];
    }

    current_index = zeros + 1;

    return current_index + 5;
}

int second_extension_decoder(int seq, char *first_sample)
{
    printf("seq %d\n", seq);
    int index = 0;
    int dec_seq[4096] = {0};
    int *gamma_list = FS_decoder(seq, BLOCK_SIZE / 2 + 1, SEQ_OG_SIZE, dec_seq, &index, 4096);
    int delta[100];
    int ms = 0;
    int beta = 0;
    for (int i = 0, j = 0; i < sizeof(gamma_list) / sizeof(gamma_list[0]); i++)
    {
        if (gamma_list[i] == 0)
        {
            beta = 0;
            ms = 0;
        }
        else if (gamma_list[i] == 1 || gamma_list[i] <= 2)
        {
            beta = 1;
            ms = 1;
        }
        else if (gamma_list[i] >= 3 && gamma_list[i] <= 5)
        {
            beta = 2;
            ms = 3;
        }
        else if (gamma_list[i] >= 6 && gamma_list[i] <= 9)
        {
            beta = 3;
            ms = 6;
        }
        else if (gamma_list[i] >= 10 && gamma_list[i] <= 14)
        {
            beta = 4;
            ms = 10;
        }
        else if (gamma_list[i] >= 15 && gamma_list[i] <= 20)
        {
            beta = 5;
            ms = 15;
        }
        else if (gamma_list[i] >= 21 && gamma_list[i] <= 27)
        {
            beta = 6;
            ms = 21;
        }
        else if (gamma_list[i] >= 28 && gamma_list[i] <= 35)
        {
            beta = 7;
            ms = 28;
        }
        else if (gamma_list[i] >= 36 && gamma_list[i] <= 44)
        {
            beta = 8;
            ms = 36;
        }
        int del_even = gamma_list[i] - ms;
        int del_odd = beta - del_even;
        delta[j] = del_odd;
        delta[j + 1] = del_even;
        j += 2;
    }
    printf("gamma list de: %ls\n", gamma_list);
    int decimal_value = strtol(first_sample, NULL, 2);
    printf("delta %ls\n", delta);
    decoded_seq[decoded_index++] = decimal_value;
    for (int y = 0; y < sizeof(gamma_list) / sizeof(gamma_list[0]); y++)
    {
        decoded_seq[decoded_index++] = delta[y];
    }
    int total = 0;
    int current_index = sizeof(gamma_list) / sizeof(gamma_list[0]) + sum(gamma_list, index, &total);
    return (current_index + 5 + resolution);
}

int split_sample_decoder(int *seq, int k, char *first_sample)
{
    int index = 0;
    int dec_seq[4096] = {0};
    int *msb_dec = FS_decoder(seq, BLOCK_SIZE, SEQ_OG_SIZE, dec_seq, &index, 4096);
    int msb_bin[BLOCK_SIZE][resolution];

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        char binary[resolution - k + 1];
        int_to_binary(msb_dec[i], binary);
        int len = strlen(binary);
        for (int j = 0; j < resolution - k - len; j++)
        {

            msb_bin[i][j] = '0';
        }
        for (int j = 0; j < len; j++)
        {
            msb_bin[i][resolution - k - len + j] = binary[j];
        }
        msb_bin[i][resolution - k] = '\0';
    }
    int total = 0;
    int current_index = BLOCK_SIZE + sum(msb_dec, index, &total);
    int block_remaining_size = k * BLOCK_SIZE;
    int *block_remaining = malloc(block_remaining_size * sizeof(int));
    memcpy(block_remaining, seq + current_index, block_remaining_size * sizeof(int));
    int lsb_bin[BLOCK_SIZE][k];
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < k; j++)
        {
            lsb_bin[i][j] = block_remaining[i * k + j];
        }
    }
    printf("msb ");
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        printf("%ls ", msb_bin[i]);
    }
    printf("& lsb ");
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < k; j++)
        {
            printf("%d ", lsb_bin[i][j]);
        }
    }
    printf("\n");
    char decoded_bin[BLOCK_SIZE][resolution];
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        strcpy(decoded_bin[i], msb_bin[i]);

        for (int j = 0; j < k; j++)
        {
            char binary[2];
            int_to_binary(lsb_bin[i][j], binary);
            strcat(decoded_bin[i], binary);
        }
    }
    int decoded_dec[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        decoded_dec[i] = strtol(decoded_bin[i], NULL, 2);
    }
    decoded_seq[index++] = strtol(first_sample, NULL, 2);
    for (int y = 0; y < BLOCK_SIZE; y++)
    {
        decoded_seq[index++] = decoded_dec[y];
    }
    current_index += k * (BLOCK_SIZE - 1);
    return current_index + resolution + 4;
}
int no_compression_decoder(char *seq)
{
    int i, j;
    int *sampled_dec;
    int result;

    int num_samples = BLOCK_SIZE * resolution;
    int num_blocks = num_samples / resolution;

    sampled_dec = (int *)malloc(num_blocks * sizeof(int));

    for (i = 0; i < num_blocks; i++)
    {
        char *sample = (char *)malloc((resolution + 1) * sizeof(char));
        for (j = 0; j < resolution; j++)
        {
            sample[j] = seq[i * resolution + j];
        }
        sample[resolution] = '\0';
        sampled_dec[i] = strtol(sample, NULL, 2);
        free(sample);
    }
    for (i = 0; i < num_blocks; i++)
    {
        decoded_seq[decoded_index++] = sampled_dec[i];
    }

    result = num_samples + 4;

    free(sampled_dec);

    return result;
}
int sum(int *dec_seq, int index, int *total)
{
    for (int i = 0; i < index; i++)
    {
        *total += dec_seq[i];
    }
    return *total;
}
int *FS_block_decoder(char *seq, int seq_size, char *first_sample)
{
    int dec_seq[4096] = {0};
    int temp_dec_seq;
    printf("fs ");
    for (int i = 0; i < seq_size; i++)
    {
        printf("%d ", seq[i]);
    }
    printf("\n");
    char binary_arr[64];
    char *binary_p = &binary_arr;
    int index = 0;
    FS_decoder(seq, BLOCK_SIZE, seq_size, dec_seq, &index, 4096);
    for (int i = 0; dec_seq != EOF || dec_seq != NULL; i++)
    {
        zfill(dec_seq[i], resolution, binary_p);
    }
    int total = 0;
    int current_index = index + sum(dec_seq, index, &total);
    int dec = binary_decimal(first_sample, strlen(first_sample));
    int temp = 0;
    for (int i = 0, j = 0; i < current_index + 1; i++)
    {
        if (i == 0)
        {
            decoded_seq[decoded_index++] = dec;
        }
        else
        {
            decoded_seq[decoded_index++] = dec_seq[i - 1];
        }
    }
    return (current_index + resolution + 4);
}
int *FS_decoder(char *seq, int block_size_t, int seq_size, int *dec_seq, int *dec_seq_index, int dec_seq_size)
{
    int zeros = 0;
    int ones = 0;
    int i = 0;
    for (; i < dec_seq_size; i++)
    {
        if (seq[i] == 0 || seq[i] == '0')
        {
            zeros += 1;
        }
        else
        {
            ones += 1;
            dec_seq[(*dec_seq_index)++] = zeros;
            zeros = 0;
            if (ones == block_size_t - 1)
            {
                return dec_seq;
            }
        }
    }
    return dec_seq;
}
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
            printf("first sample %ld\n", strtol(first_sample, NULL, 2));
            if (strcmp(block_id, FS_block_id) == 0)
            {
                printf("FS block\n");
                current_index = current_index + FS_block_decoder(seq + current_index + 4 + resolution, SEQ_OG_SIZE, first_sample);
                printf("current index %d\n", current_index);
            }
            else if (strcmp(block_id, no_copression_id) == 0)
            {
                printf("no compression\n");
                current_index = current_index + no_compression_decoder(seq + current_index + 4);
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
            printf("first sample %ld\n", strtol(first_sample, NULL, 2));
            // continue with the rest of the code...
            if (strcmp(block_id, second_extension_id) == 0)
            {
                current_index = current_index + 5 + resolution + second_extension_decoder(seq + current_index + 5 + resolution, first_sample);
                printf("current index {current_index}");
            }
            else if (strcmp(block_id, zero_block_id) == 0)
            {
                int len = strlen(seq);
                int current_index_5 = strlen(seq + current_index + 5);
                current_index = current_index + zero_block_decoder(seq + current_index + 5, current_index_5, len);
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
        int_to_binary(block[i], n_block[i]);
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
    int_to_binary(number, binary_arr);

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

char *int_to_binary(int value, char *binary_p)
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

char *FS_block(int *block, char *encoded_block, int *index)
{
    char binary_arr[64];
    char *binary_p = &binary_arr;
    if ((resolution == 3) || (resolution == 4))
    {
        strcat(encoded_block, "01");
    }
    else if ((resolution > 4) && (resolution <= 8))
    {
        strcat(encoded_block, "001");
    }
    else if ((resolution > 8) && (resolution <= 16))
    {
        strcat(encoded_block, "0001");
    }
    else if ((resolution > 16) && (resolution <= 32))
    {
        strcat(encoded_block, "00001");
    }
    zfill(block[0], 16, binary_p);
    strcat(encoded_block, binary_p);
    for (int i = 1; i < 64; i++)
    {
        zfill(1, block[i] + 1, binary_p);
        strcat(encoded_block, binary_p);
    }
}
char *second_extension(int *sequence)
{
    int *encoded = malloc(sizeof(int));
    int option_id = 0;
    sequence = realloc(sequence, (sizeof(sequence) + sizeof(int)));
    sequence[sizeof(sequence) - 1] = sequence[sizeof(sequence) - 2];
    printf("len seq %ld\n", sizeof(sequence) / sizeof(int));
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
        int_to_binary(encoded[i], binary);
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
    char *fs_encoded = FS_block(block, encoded_block, &index);

    char *smallest = split_sample(block, 1, BLOCK_SIZE, encoded_block, &index);
    for (int i = 1; i < resolution / 2; i++)
    {
        char *temp = split_sample(block, i, BLOCK_SIZE, smallest, &index);
        if (strlen(temp) <= strlen(smallest))
        {
            smallest = temp;
        }
    }
    char *split_sample_encoded = smallest;
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
    if (0 == zeros)
    {
        fs_element[i] = '1';
        fs_element[i += 1] = '\0';
        return;
    }
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
    char optiion_id_arr[100];
    char *option_id = &optiion_id_arr;
    int blocks = length / BLOCK_SIZE;
    int reminder_samples = length % BLOCK_SIZE;
    if (reminder_samples > 0)
    {
        blocks += 1;
    }
    int block_list[blocks][BLOCK_SIZE];
    breaker(sequence, length, blocks, block_list);
    int encoded_length = 0;

    for (int i = 0; i < blocks; i++)
    {
        int *block;
        block = block_list[i];
        printf("block_list[i]\n");
        int non_zero_block = 0;
        for (int k = 0; k < BLOCK_SIZE; k++)
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
        strcat(option_id, "000");
    }
    else if (resolution == 3 || resolution == 4)
    {
        strcat(option_id, "0000");
    }
    else if (resolution > 4 && resolution <= 8)
    {
        strcat(option_id, "00000");
    }
    else if (resolution > 8 && resolution <= 16)
    {
        strcat(option_id, "000000");
    }
    else if (resolution > 16 && resolution <= 32)
    {
        strcat(option_id, "0000000");
    }
    int len = strlen(option_id);
    option_id[len + 1] = '\0';
    strcat(result, option_id);
    strcat(result, encoded);
    int j = strlen(result);
    result[j + 1] = '\0';

    return result;
}

int *pre_processor(int *block, int element_size, int *seq_delayed, int *delta)
{
    int *seq = block;
    for (int i = 0; i < element_size; i++)
    {
        seq_delayed[i] = (i > 0) ? seq[i - 1] : threshold;
    }
    int diff[BLOCK_SIZE] = {0}, t[BLOCK_SIZE] = {0};
    for (int i = 0; i < element_size; i++)
    {
        diff[i] = seq[i] - seq_delayed[i];
    }

    int ymax = max_limit;
    int ymin = min_limit;

    for (int i = 0; i < element_size; i++)
    {
        int a = ymax - seq_delayed[i];
        int b = seq_delayed[i] - ymin;
        int T = (a < b) ? a : b;
        t[i] = T;

        if (0 <= diff[i] && diff[i] <= T)
        {
            delta[i] = 2 * diff[i];
        }
        else if (-T <= diff[i] && diff[i] < 0)
        {
            delta[i] = 2 * abs(diff[i]) - 1;
        }
        else
        {
            delta[i] = T + abs(diff[i]);
        }
    }

    printf("pre processed: ");
    for (int i = 0; i < element_size; i++)
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

void breaker(int *sequence, int range, int blocks, int block[blocks][BLOCK_SIZE])
{
    int N = BLOCK_SIZE; // Length of sublist
    int sequence_length = range;
    int num_of_blocks = sequence_length / N; // Number of sublists
    int remain_samples = sequence_length % N;
    int sub_block = 0, i = 0;

    for (; sub_block < num_of_blocks; sub_block++, i += N)
    {
        for (int j = 0; j < N; j++)
        {
            block[sub_block][j] = sequence[i + j];
        }
    }

    if (remain_samples > 0)
    {
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

void executor(int *seq_og, int seq_og_size, int blocks, int blocks_sequence[blocks][BLOCK_SIZE], int *pre_processed_block, int *e)
{
    int zero_block_seq[seq_og_size], encoded_seq[seq_og_size];
    int zero_block_seq_len = 0;
    int N = BLOCK_SIZE;
    breaker(seq_og, seq_og_size / sizeof(seq_og[0]), blocks, blocks_sequence);
    int seq_delayed[BLOCK_SIZE];
    int sub_block = 0;
    int remain_samples = (seq_og_size / sizeof(seq_og[0])) % N;
    int zero_block_seq_index = 0, encoded_seq_index = 0;
    int zero_block_count = 0, nonzero_block_count = 0;
    for (; sub_block < blocks; sub_block++)
    {
        int zero_flag = 0, non_zero_flag = 0;
        pre_processor(blocks_sequence[sub_block], BLOCK_SIZE, seq_delayed, pre_processed_block);
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            if (pre_processed_block[i] != 0)
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
            for (; j < BLOCK_SIZE; j++)
            {
                encoded_seq[encoded_seq_index] = pre_processed_block[j];
                encoded_seq_index += 1;
            }
            nonzero_block_count++;
        }
        else
        {
            int j = 0;
            for (; j < BLOCK_SIZE; j++)
            {
                zero_block_seq[zero_block_seq_index] = pre_processed_block[j];
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
    dec(encoded_seq, e);
    int len_seq = sizeof(seq_og) / sizeof(seq_og[0]);
    int len_e = sizeof(e) / sizeof(e[0]);
    float c_ratio = len_seq / len_e;
    printf("\nnonzero_block_count=%d zero_block_count=%d\n", nonzero_block_count, zero_block_count);
}

int main()
{
    int seq_og[SEQ_OG_SIZE] = {0};
    int delta[BLOCK_SIZE] = {0};
    int e[SEQ_OG_SIZE] = {0};
    int blocks = SEQ_OG_SIZE / BLOCK_SIZE;
    int remain_samples = SEQ_OG_SIZE % BLOCK_SIZE;
    if (remain_samples > 0)
    {
        blocks += 1;
    }
    int blocks_sequence[blocks][BLOCK_SIZE];
    int sizeof_seq_og = sizeof(seq_og);
    rand_seq_og(9820, 9830, SEQ_OG_SIZE, seq_og);

    for (int i = 0; i < blocks; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            blocks_sequence[i][j] = 0;
        }
    }
    executor(seq_og, sizeof_seq_og, blocks, blocks_sequence, delta, e);

    char *s = joiner(e, SEQ_OG_SIZE);
    int s_len = strlen(s);
    entropy_decoder(s, SEQ_OG_SIZE);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#define SEQ_OG_SIZE 64
#define BLOCK_SIZE 64
#define resolution 16
static int min_limit = 0;
static int max_limit = 65000;
static int threshold = 0;

char id_lut[17][50] = {0};
char k_id[13][10];
int decoded_seq[SEQ_OG_SIZE];
static int decoded_index = 0;

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

char *zero_block(int *sequence, int length, char *result, char *encoded);
void breaker(int *sequence, int range, int blocks, int block[blocks][BLOCK_SIZE]);
int *pre_processor(int *block, int len, int *seq_delayed, int *delta);
int *rand_seq_og(int min, int max, int range, int *seq_og);
void FS_element(int zeros, char *fs_element);
char *code_select(int *block, char *encoded_block, int index, int block_size);
char *split_sample(int *block, int k, int size, char *encoded, int index);
char *second_extension(int *sequence, int block_size, char *second_extension_encoded);
void executor(int *seq_og, int seq_og_size, int blocks, int blocks_sequence[blocks][BLOCK_SIZE], int *delta, int *encoded_seq);
char *FS_block(int *block, char *fs_encoded, int index);
char *int_to_binary(int value, char *binary_p);
char *zfill(int number, int zero_resolution, char *binary_p);
int char_binary_to_decimal(char *msb, int len);
char *no_compression(int *block, char *no_compression);
char *minimum(char *a, char *b);
int *dec(char *seq, int *e);

int sum(int *dec_seq, int index, int total);
void entropy_decoder(char *seq, int size);
int *FS_decoder(char *seq, int block_size_t, int seq_size, int *dec_seq, int dec_seq_index, int dec_seq_size);
int FS_block_decoder(char *seq, int seq_size, char *first_sample);
int no_compression_decoder(char *seq);
int split_sample_decoder(char *seq, int k, char *first_sample);
int second_extension_decoder(char *seq, char *first_sample);
int zero_block_decoder(char *seq, int start_index, int seq_length);

char *joiner(int *seq, int seq_size, char *s);
// char *itoa(long int value, char *result, int base);
int *post_processor(int *decoded_seq);

char *joiner(int *seq, int seq_size, char *s)
{
    int l = seq[0];
    printf("\n%d\n", l);
    char received_seq[l];
    memset(received_seq, '\0', sizeof(received_seq));
    printf("\n before for loop\n");
    for (int i = 1; i < seq_size; ++i)
    {
        int rem_l = l - strlen(received_seq);
        int block = seq[i];
        char buffer[33] = {'\0'};
        if (rem_l >= resolution)
        {
            zfill(block, resolution, buffer);
        }
        else
        {
            zfill(block, rem_l, buffer);
        }
        strncat(received_seq, buffer, rem_l);
    }
    strcat(s, received_seq);
    return s;
}

int zero_block_decoder(char *seq, int start_index, int seq_length)
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
        // printf("\nzero block decoder called decoded_index=%d\n", decoded_index);
    }

    current_index = zeros + 1;

    return current_index + 5;
}

int second_extension_decoder(char *seq, char *first_sample)
{
    // printf("seq %d\n", seq);
    int index = 0;
    int dec_seq[4096] = {0};
    int *gamma_list = FS_decoder(seq, BLOCK_SIZE / 2 + 1, SEQ_OG_SIZE, dec_seq, index, 4096);
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
    // printf("gamma list de: %ls\n", gamma_list);
    int decimal_value = strtol(first_sample, NULL, 2);
    // printf("delta %ls\n", delta);
    decoded_seq[decoded_index++] = decimal_value;
    printf("\nsecond extention decoder called decoded_index=%d\n", decoded_index);
    for (int y = 0; y < sizeof(gamma_list) / sizeof(gamma_list[0]); y++)
    {
        decoded_seq[decoded_index++] = delta[y];
        printf("\nsecond extention decoder called decoded_index=%d\n", decoded_index);
    }
    int total = 0;
    int current_index = sizeof(gamma_list) / sizeof(gamma_list[0]) + sum(gamma_list, index, total);
    return (current_index + 5 + resolution);
}

int split_sample_decoder(char *seq, int k, char *first_sample)
{
    // printf("\n in split sample decoder\n");
    int index = 0;
    int dec_seq[4096] = {0};
    int *msb_dec = FS_decoder(seq, BLOCK_SIZE, SEQ_OG_SIZE, dec_seq, index, 4096);

    char msb_bin[BLOCK_SIZE][resolution];
    memset(msb_bin, '\0', sizeof(msb_bin));

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        char binary[resolution - k + 1];
        zfill(msb_dec[i], resolution - k, msb_bin[i]);
    }
    int total = 0;
    int current_index = BLOCK_SIZE + sum(msb_dec, index, total);

    int N = k * BLOCK_SIZE * sizeof(char);
    char block_remaining[N];
    memcpy(block_remaining, seq + current_index, N);

    char lsb_bin[BLOCK_SIZE][k + 1];
    memset(lsb_bin, '\0', sizeof(lsb_bin));

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        for (int j = 0; j < k; j++)
        {
            lsb_bin[i][j] = block_remaining[i * k + j];
        }
    }

    char decoded_bin[BLOCK_SIZE][resolution + 1];
    memset(decoded_bin, '\0', sizeof(decoded_bin));

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        strncat(decoded_bin[i], msb_bin[i], strlen(msb_bin[i]));
        strncat(decoded_bin[i], lsb_bin[i], strlen(lsb_bin[i]));
    }
    int decoded_dec[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        decoded_dec[i] = strtol(decoded_bin[i], NULL, 2);
        printf("%d,", decoded_dec[i]);
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
    int result;

    int num_samples = BLOCK_SIZE * resolution;
    int num_blocks = num_samples / resolution;
    int sampled_dec[num_blocks];

    for (i = 0; i < num_blocks; i++)
    {
        char sample[resolution + 1];
        for (j = 0; j < resolution; j++)
        {
            sample[j] = seq[i * resolution + j];
        }
        sample[resolution] = '\0';
        sampled_dec[i] = strtol(sample, NULL, 2);
    }
    for (i = 0; i < num_blocks; i++)
    {
        decoded_seq[decoded_index++] = sampled_dec[i];
        // printf("\nsplit sample decoder called decoded_index=%d\n", decoded_index);
    }

    result = num_samples + 4;

    return result;
}
int sum(int *dec_seq, int index, int total)
{
    for (int i = 0; dec_seq[i] != NULL; i++)
    {
        total += dec_seq[i];
    }
    return total;
}
int FS_block_decoder(char *seq, int seq_size, char *first_sample)
{
    // printf("\nin fs block decoder\n");
    // printf("\n%s\n", first_sample);
    int dec_seq[4096] = {0};
    int temp_dec_seq;
    int i = 0;
    // printf("\nseq_len=%ld\n", strlen(seq));
    for (; i < strlen(seq); i++)
    {
        printf("%c", seq[i]);
    }
    // printf("\nmsc_dec:");
    char binary_p[64] = {'\0'};
    int index = 0;
    FS_decoder(seq, BLOCK_SIZE, seq_size, dec_seq, index, 4096);
    int size = 0;
    for (int i = 0; dec_seq[i] != NULL; i++)
    {
        printf("%d,", dec_seq[i]);
        zfill(dec_seq[i], resolution, binary_p);
        size++;
    }
    int total = 0;
    int current_index = 0;
    current_index = size + sum(dec_seq, index, total);
    // printf("\nfs block decoder current index=%d\n", current_index);
    int dec = char_binary_to_decimal(first_sample, strlen(first_sample));
    int temp = 0;
    // printf("\ndec=%d\n", dec);
    for (int i = 0, j = 0; i < size + 1; i++)
    {
        if (i == 0)
        {
            decoded_seq[decoded_index++] = dec;
            // printf("\nfs block decoder called decoded_index=%d\n", decoded_index);
        }
        else
        {
            decoded_seq[decoded_index++] = dec_seq[i - 1];
            // printf("\nfs block decoder called decoded_index=%d\n", decoded_index);
        }
    }
    return (current_index + resolution + 4);
}
int *FS_decoder(char *seq, int block_size_t, int seq_size, int *dec_seq, int dec_seq_index, int dec_seq_size)
{
    int zeros = 0;
    int ones = 0;
    int i = 0, j = 0;
    for (; i < dec_seq_size; i++)
    {

        if (seq[i] == '0')
        {
            zeros += 1;
        }
        else
        {
            ones += 1;
            dec_seq[j] = zeros;
            j++;
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
    size = strlen(seq);
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
    int count = 0;
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
            // printf("in 4 bit\n");
            strncpy(block_id, seq + current_index, 4);
            block_id[4] = '\0';
            char first_sample[resolution + 1];
            strncpy(first_sample, seq + current_index + 4, resolution);
            first_sample[resolution] = '\0';
            printf("in 4 bit first sample %ld\n", strtol(first_sample, NULL, 2));
            // printf("\nblockid=%s,fs_block_id=%s\n", block_id, FS_block_id);
            if (strncmp(block_id, FS_block_id, 4) == 0)
            {
                printf("fs blockid\n");
                // printf("\nbefore current index=%d\n", current_index);
                current_index = current_index + FS_block_decoder(seq + current_index + 4 + resolution, SEQ_OG_SIZE, first_sample);
                // printf("\ncurrent index %d\n", current_index);
            }
            else if (strncmp(block_id, no_copression_id, 4) == 0)
            {
                printf("no compression\n");
                current_index = current_index + no_compression_decoder(seq + current_index + 4);
                printf("\ncurrent index %d\n", current_index);
            }
            else
            {
                printf("split sample\n");
                int i = -1, n = 0;
                for (; n < 13; n++)
                {
                    int size = strlen(k_id[n]);
                    if (strncmp(block_id, k_id[n], size) == 0)
                    {
                        i = n;
                        break;
                    }
                }
                if (i > -1)
                    current_index = current_index + split_sample_decoder(seq + current_index + 4 + resolution, i + 1, first_sample);
                // printf("\ncurrent index %d\n", current_index);
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
int *dec(char *seq, int *e)
{
    int N = resolution;
    int seq_len = strlen(seq);
    int blocked_seq_len = seq_len / N;
    int blocked_seq[blocked_seq_len];
    int i = 0;
    for (; i < blocked_seq_len + 1; i++)
    {
        char block[N + 1];
        strncpy(block, seq + i * N, N);
        block[N] = '\0';
        blocked_seq[i] = strtol(block, NULL, 2);
        e[i + 1] = blocked_seq[i];
    }
    e[0] = seq_len;
    return e;
}
char *minimum(char *a, char *b)
{
    if (strlen(a) <= strlen(b))
    {
        printf("\nsmall:%s\n", a);
        return a;
    }
    else
    {
        printf("\nsmall:%s\n", b);
        return b;
    }
}
char *no_compression(int *block, char *no_compression_encoded)
{
    char option_id[100] = {'\0'};

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
    strcat(no_compression_encoded, option_id);
    char binary_p[64];
    for (int i = 0; i < 64; i++)
    {
        zfill(block[i], 16, binary_p);
        strcat(no_compression_encoded, binary_p);
    }
    printf("\nno_compression_encoded:%s\n", no_compression_encoded);
    return no_compression_encoded;
}

int char_binary_to_decimal(char *msb, int len)
{
    // printf("n char to decimal\n");
    // printf("\n%s,%d\n", msb, len);
    char *ptr;
    ptr = msb;
    char str[200] = {'\0'};
    int dec = 0, bit_dec = 0, i = 0, exponent = 0, base = 2;
    // for (int i = 0; i < len; i++)
    // {
    //     printf("ptr[%d]=%c,", i, ptr[i]);
    // }
    // printf("\n%s,%ld\n", ptr, strlen(ptr));
    for (; i < len; i++)
    {
        exponent = len - (i + 1);
        int n = msb[i] - '0';
        int result = 1;
        if (n == 1)
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
        // exponent--;
    }
    // printf("\n dec=%d\n", dec);
    return dec;
}
char *zfill(int number, int zero_resolution, char *binary_p)
{
    int bits = sizeof(number) * 8;
    char binary_arr[bits + 1];
    int_to_binary(number, binary_arr);
    int actual_resolution = strlen(binary_arr);
    char temp_zero[100] = {'\0'};
    if (actual_resolution < zero_resolution)
    {
        int n = zero_resolution - actual_resolution;
        memset(temp_zero, '0', n);
        temp_zero[n] = '\0';
        strcat(temp_zero, binary_arr);
        memcpy(binary_p, temp_zero, strlen(temp_zero));
        return binary_p;
    }
    else
    {
        memcpy(binary_p, binary_arr, strlen(binary_arr));
        // printf("\n%s\n", binary_arr);
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

char *FS_block(int *block, char *fs_encoded, int index)
{
    char binary_p[64] = {'\0'};
    memset(fs_encoded, '\0', sizeof(fs_encoded));
    if ((resolution == 3) || (resolution == 4))
    {
        strcat(fs_encoded, "01");
    }
    else if ((resolution > 4) && (resolution <= 8))
    {
        strcat(fs_encoded, "001");
    }
    else if ((resolution > 8) && (resolution <= 16))
    {
        strcat(fs_encoded, "0001");
    }
    else if ((resolution > 16) && (resolution <= 32))
    {
        strcat(fs_encoded, "00001");
    }
    zfill(block[0], 16, binary_p);
    strcat(fs_encoded, binary_p);
    for (int i = 1; i < 64; i++)
    {
        memset(binary_p, '\0', sizeof(binary_p));
        zfill(1, block[i] + 1, binary_p);
        strcat(fs_encoded, binary_p);
        memset(binary_p, '\0', sizeof(binary_p));
    }
}
char *second_extension(int *sequence, int block_size, char *second_extension_encoded)
{
    char binary_p[64] = {'\0'};
    char option_id[100] = {'\0'};
    sequence[block_size] = sequence[block_size - 1];
    int paired_list[(block_size - 1) / 2][2];
    for (int i = 1; i < block_size; i += 2)
    {
        paired_list[i / 2][0] = sequence[i];
        paired_list[i / 2][1] = sequence[i + 1];
    }

    if (resolution == 1 || resolution == 2)
    {
        zfill(1, 2, option_id);
    }
    else if (resolution == 3 || resolution == 4)
    {
        zfill(1, 3, option_id);
    }
    else if (resolution > 4 && resolution <= 8)
    {
        zfill(1, 4, option_id);
    }
    else if (resolution > 8 && resolution <= 16)
    {
        zfill(1, 5, option_id);
    }
    else if (resolution > 16 && resolution <= 32)
    {
        zfill(1, 6, option_id);
    }
    int n = strlen(option_id);
    memset(second_extension_encoded, '\0', sizeof(second_extension_encoded));
    strcat(second_extension_encoded, option_id);

    zfill(sequence[0], 16, binary_p);
    strcat(second_extension_encoded, binary_p);

    float gamma = 0;
    float gamma_list[(block_size - 1) / 2];
    char fs_element[64] = {'\0'};
    for (int i = 0; i < ((block_size - 1) / 2); i++)
    {
        int *pair = paired_list[i];
        gamma = (0.5 * (pair[0] + pair[1]) * (pair[0] + pair[1] + 1) + pair[1]);
        if (gamma + 1 <= 44)
        {
            FS_element(gamma + 1, fs_element);
            strcat(second_extension_encoded, fs_element);
        }
        else
        {
            memset(second_extension_encoded, '\0', sizeof(second_extension_encoded));
            second_extension_encoded[0] = '\0';
            if (second_extension_encoded == '\0')
            {
                printf("\nmemset cleare\n");
            }
            printf("not executing second extension\n");
            return 0;
        }
        gamma_list[i] = gamma;
    }

    // printf("gamma list en: ");
    // for (int i = 0; i < ((block_size - 1) / 2); i++)
    // {
    //     printf("%f ", gamma_list[i]);
    // }
    // printf("\n");
    // printf("\nsec ext=%s\n", second_extension_encoded);
    return second_extension_encoded;
}

char *split_sample(int *block, int k, int size, char *encoded, int index)
{
    printf("in split sample\n");
    char option_id[64], block_array[64];
    char msb[64], lsb[16];
    char binary[64];
    int n = resolution;
    char msb_fs[64];
    char msb_bunch[4096], lsb_bunch[4096];
    memset(msb_bunch, '\0', sizeof(msb_bunch));
    memset(lsb_bunch, '\0', sizeof(lsb_bunch));
    for (int sample = 1; sample < size; sample++)
    {
        int msb_dec = 0;
        memset(binary, '\0', sizeof(binary));
        zfill(block[sample], n, binary);
        memset(lsb, '\0', sizeof(lsb));
        memset(msb, '\0', sizeof(msb));
        memcpy(lsb, (void *)&binary[n - k], n - (n - k));
        lsb[n - (n - k) + 1] = '\0';
        memcpy(msb, (void *)&binary[0], (n - k));
        msb[(n - k) + 1] = '\0';
        // printf("\nmsb_len=%ld,lsb_len=%ld,msb=%s,lsb=%s\n", strlen(msb), strlen(lsb), msb, lsb);
        msb_dec = char_binary_to_decimal(msb, strlen(msb));
        // printf("\nmsb_dec=%d\n", msb_dec);
        memset(msb_fs, '\0', sizeof(msb_fs));
        FS_element(msb_dec + 1, msb_fs);
        strcat(msb_bunch, msb_fs);
        strcat(lsb_bunch, lsb);
        // printf("\nmsb_bunch=%s,lsb_bunch=%s\n", msb_bunch, lsb_bunch);
    }
    // printf("\nmsb_bunch=%s,lsb_bunch=%s\n", msb_bunch, lsb_bunch);
    memset(option_id, '\0', sizeof(option_id));
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
    memset(encoded, '\0', sizeof(encoded));
    strcat(encoded, option_id);
    memset(block_array, '\0', sizeof(block_array));
    zfill(block[0], 16, block_array);
    strcat(encoded, block_array);
    memset(block_array, '\0', sizeof(block_array));
    strcat(encoded, msb_bunch);
    strcat(encoded, lsb_bunch);
    printf("\nsplit sample:%s\n", encoded);
    printf("\nstr len=%ld\n", strlen(encoded));
    return encoded;
}

char *code_select(int *block, char *encoded_block, int index, int block_size)
{
    memset(encoded_block, '\0', sizeof(encoded_block));
    char smallest[4096] = {'\0'};
    char temp[4096] = {'\0'};
    char fs_encoded[4096] = {'\0'};
    char encoded_smallest[4096] = {'\0'};
    char *smallest_encoded = encoded_smallest;
    FS_block(block, fs_encoded, index);
    // printf("\nfs block:%s\n", fs_encoded);
    split_sample(block, 1, BLOCK_SIZE, smallest, index);
    for (int i = 1; i < resolution / 2; i++)
    {
        split_sample(block, i, BLOCK_SIZE, temp, index);
        if (strlen(temp) <= strlen(smallest))
        {
            memset(smallest, '\0', sizeof(smallest));
            memcpy(smallest, temp, sizeof(temp));
        }
    }
    char *split_sample_encoded = smallest;
    char second_extension_encoded[4096] = {'\0'};
    second_extension(block, block_size, second_extension_encoded);
    char no_compression_encoded[4096] = {'\0'};
    no_compression(block, no_compression_encoded);
    if (second_extension_encoded[0] == '\0')
    {
        printf("\n in if block\n");
        // printf("\nfs_encoded=%s\n", fs_encoded);
        // printf("\split_sample_encoded=%s\n", split_sample_encoded);
        // printf("\no_compression_encoded=%s\n", no_compression_encoded);
        smallest_encoded = minimum(
            minimum(fs_encoded, split_sample_encoded), no_compression_encoded);
        printf("\nsmallest:%s\n", smallest_encoded);
    }
    else
    {
        printf("\nin else block\n");
        smallest_encoded = minimum(minimum(fs_encoded, split_sample_encoded), minimum(second_extension_encoded, no_compression_encoded));
        printf("\nsmallest:%s\n", smallest_encoded);
    }
    strcat(encoded_block, smallest_encoded);
    printf("\nend code_select=%s\n", encoded_block);
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
    char option_id[100];
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
        int block[4096] = {'\0'};
        memcpy(block, block_list[i], sizeof(block_list[i]));
        int non_zero_block = 0;
        for (int k = 0; k < BLOCK_SIZE; k++)
        {
            if (0 != block[i])
            {
                non_zero_block++;
                break;
            }
        }

        if (!non_zero_block)
        {
            zeros += 1;
        }
        else
        {
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
            code_select(block, encoded, index, BLOCK_SIZE);
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
    strcat(result, option_id);
    strcat(result, encoded);
    int j = strlen(result);
    result[j + 1] = '\0';
    // printf("\nresult=%s\n", result);
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

    printf("\npre processed: ");
    for (int i = 0; i < element_size; i++)
    {
        printf("%d ", delta[i]);
    }
    printf("\n");
    printf("pre processor done\n");
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
    // printf("\nblocked sequence\n");
    for (; sub_block < num_of_blocks; sub_block++, i += N)
    {
        for (int j = 0; j < N; j++)
        {
            block[sub_block][j] = sequence[i + j];
            // printf("%d,", block[sub_block][j]);
        }
    }
    // printf("\nafter blocked sequence\n");
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
    int zero_block_seq[seq_og_size];
    char encoded_seq[10000];
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
        memset(pre_processed_block, '\0', sizeof(pre_processed_block));
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
                char encoded[4096] = {'\0'};
                char result[4096] = {'\0'};
                printf("\nzero block count !=0\n");
                zero_block(zero_block_seq, zero_block_seq_index, &result[0], &encoded[0]);
                strcat(encoded_seq, result);
                zero_block_count = 0;
                zero_block_seq_index = 0;
            }
            int j = 0;
            char encoded[4096] = {'\0'};
            printf("in non zero flag before code_select fun\n");
            code_select(pre_processed_block, encoded, encoded_seq_index, BLOCK_SIZE);
            printf("\nafter code select\n");
            strcat(encoded_seq, encoded);

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
            // printf("\nzero block count=%d,index=%d\n", zero_block_count, zero_block_seq_index);
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
    printf("\nencoded_seq before dec call=%s\n", encoded_seq);
    dec(encoded_seq, e);
}
int *post_processor(int *decoded_seq)
{
    int x[BLOCK_SIZE] = {0};
    int xmax = max_limit;
    int xmin = min_limit;
    int delta[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
    }
}
int main()
{
    int seq_og[SEQ_OG_SIZE] = {0};
    int delta[BLOCK_SIZE] = {0};
    int e[4096] = {0};
    int *e_ptr = &e[0];
    int blocks = SEQ_OG_SIZE / BLOCK_SIZE;
    int remain_samples = SEQ_OG_SIZE % BLOCK_SIZE;
    if (remain_samples > 0)
    {
        blocks += 1;
    }
    int blocks_sequence[blocks][BLOCK_SIZE];
    int sizeof_seq_og = sizeof(seq_og);
    // rand_seq_og(9820, 9830, SEQ_OG_SIZE, seq_og);
    printf("before for loop\n");
    for (int i = 0; i < SEQ_OG_SIZE; i++)
    {
        seq_og[i] = 1900 + (i * 10);
        printf("%d,", seq_og[i]);
    }
    // printf("after for loop\n");
    for (int i = 0; i < blocks; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            blocks_sequence[i][j] = 0;
        }
    }
    struct timespec tnsec1, tnsec2;
    long int diff;
    clock_gettime(CLOCK_REALTIME, &tnsec1);
    executor(seq_og, sizeof_seq_og, blocks, blocks_sequence, delta, e_ptr);
    clock_gettime(CLOCK_REALTIME, &tnsec2);
    diff = tnsec2.tv_nsec - tnsec1.tv_nsec;
    printf("\ndiff time in nano sec=%ld\n", diff);
    printf("\n");
    int e_size = 0;
    for (int m = 0; e[m] != NULL; m++)
    {
        printf("%d,", e[m]);
        e_size += 1;
    }
    printf("\n");
    int len_seq = sizeof(seq_og) / sizeof(seq_og[0]);
    int len_e = e_size;
    float c_ratio = (float)len_seq / (float)len_e;
    printf("\nlen_seq=%d,len_e=%d,c_ratio=%f\n", len_seq, len_e, c_ratio);
    char s[4096] = {'\0'};
    joiner(e, SEQ_OG_SIZE, s);
    int s_len = strlen(s);
    printf("\ns_len=%d\n", s_len);
    entropy_decoder(s, SEQ_OG_SIZE);
    printf("\ndecoded_seq=%d\n", decoded_index);
    for (int i = 0; decoded_seq[i] != NULL; i++)
    {
        printf("%d,", decoded_seq[i]);
    }
    int seq_reconstruct[SEQ_OG_SIZE];
    memset(seq_reconstruct, '\0', sizeof(seq_reconstruct));
    for (int i = 0; decoded_seq[i] != NULL; i + BLOCK_SIZE)
    {
        post_processor(&decoded_seq[i]);
    }
    return 0;
}
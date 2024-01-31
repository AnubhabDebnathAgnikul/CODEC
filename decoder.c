#include "compressor_decompressor.h"
extern int final_decoded_seq[SEQ_OG_SIZE];
extern int decoded_seq[SEQ_OG_SIZE];
int final_decoded_index;
int decoded_index;
char id_lut[17][50] = {0};
char k_id[13][10];

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
void entropy_decoder(char *seq, int size)
{
    size = strlen(seq);
    printf("\nsize=%d\n", size);
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
            // printf("in 4 bit first sample %ld\n", strtol(first_sample, NULL, 2));
            // printf("\nblockid=%s,fs_block_id=%s\n", block_id, FS_block_id);
            if (strncmp(block_id, FS_block_id, 4) == 0)
            {
                // printf("fs blockid\n");
                // printf("\nbefore current index=%d\n", current_index);
                current_index = current_index + FS_block_decoder(seq + current_index + 4 + resolution, SEQ_OG_SIZE, first_sample);
                // printf("\ncurrent index %d\n", current_index);
            }
            else if (strncmp(block_id, no_copression_id, 4) == 0)
            {
                // printf("no compression\n");
                current_index = current_index + no_compression_decoder(seq + current_index + 4);
                // printf("\ncurrent index %d\n", current_index);
            }
            else
            {
                // printf("split sample\nblock_id=%s", block_id);
                int i = -1, n = 0, size = 0;
                for (; n < 13; n++)
                {
                    size = strlen(k_id[n]);
                    if (strncmp(block_id, k_id[n], size) == 0)
                    {
                        i = n;
                        // printf("\ni=%d\n", i);
                        break;
                    }
                }
                // printf("\ncur inde=%d\n", current_index);
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
            // printf("first sample %ld\n", strtol(first_sample, NULL, 2));
            // continue with the rest of the code...
            if (strcmp(block_id, second_extension_id) == 0)
            {
                current_index = current_index + 5 + resolution + second_extension_decoder(seq + current_index + 5 + resolution, first_sample);
                // printf("current index {current_index}");
            }
            else if (strcmp(block_id, zero_block_id) == 0)
            {
                int len = strlen(seq);
                int current_index_5 = strlen(seq + current_index + 5);
                current_index = current_index + zero_block_decoder(seq + current_index + 5, current_index_5, len);
                // printf("current index {current_index}");
            }
        }
        else
            break;
    }
}

int FS_block_decoder(char *seq, int seq_size, char *first_sample)
{
    printf("\nin fs block decoder\n");
    // printf("\n%s\n", first_sample);
    int I = strlen(seq);
    int dec_seq[I];
    memset(dec_seq, 0, sizeof(dec_seq));
    int temp_dec_seq;
    int i = 0;
    // printf("\nseq_len=%ld\n", strlen(seq));
    for (; i < strlen(seq); i++)
    {
        printf("%c", seq[i]);
    }
    // printf("\nmsc_dec:");
    char binary_p[64] = {'\0'};
    int fs_index = 0, index = 0;
    fs_index = FS_decoder(seq, BLOCK_SIZE, seq_size, dec_seq, fs_index, I);
    printf("\nfs_index=%d", fs_index);
    int size = 0;
    for (int i = 0; i < (fs_index + 1); i++)
    {
        printf("%d,", dec_seq[i]);
        memset(binary_p, '\0', sizeof(binary_p));
        zfill(dec_seq[i], resolution, binary_p);
        size++;
    }
    int total = 0;
    int current_index = 0;

    current_index = (fs_index + 1) + sum(dec_seq, fs_index, total);
    // printf("\nfs block decoder current index=%d\n", current_index);
    int dec = char_binary_to_decimal(first_sample, strlen(first_sample));
    int temp = 0;
    printf("\ndec=%d\n", dec);
    for (int i = 0, j = 0; i < (fs_index + 1); i++)
    {
        if (i == 0)
        {
            decoded_seq[decoded_index++] = dec;
            printf("\nfs block decoder called decoded_index=%d\n", decoded_index);
        }
        else
        {
            decoded_seq[decoded_index++] = dec_seq[i - 1];
            printf("\nfs block decoder called decoded_index=%d\n", decoded_index);
        }
    }
    return (current_index + resolution + 4);
}
int FS_decoder(char *seq, int block_size_t, int seq_size, int *dec_seq, int dec_seq_index, int dec_seq_size)
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
        else if (seq[i] == '1')
        {
            ones += 1;
            dec_seq[j] = zeros;
            dec_seq_index = j;
            // printf("\ndec_seq[%d]=%d,", j, dec_seq[j]);
            // printf("dec_seq_index=%d\n", dec_seq_index);
            j++;
            zeros = 0;
            if (ones == block_size_t - 1)
            {
                return dec_seq_index;
            }
        }
    }
    return dec_seq_index;
}
int no_compression_decoder(char *seq)
{
    int i, j;
    int result;

    int num_samples = BLOCK_SIZE * resolution;
    int num_blocks = num_samples / resolution;
    int sampled_dec[num_blocks];

    char sample[resolution + 1];
    for (i = 0; i < num_blocks; i++)
    {
        memset(sample, '\0', sizeof(sample));
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
int split_sample_decoder(char *seq, int k, char *first_sample)
{
    // printf("\n in split sample decoder\n");
    // printf("\nk=%d\n", k);
    int fs_index = 0, index = 0;
    int dec_seq[4096] = {0};
    fs_index = FS_decoder(seq, BLOCK_SIZE, SEQ_OG_SIZE, dec_seq, fs_index, 4096);
    int *msb_dec = &dec_seq[0];
    // printf("\nindex=%d\n", fs_index);
    char msb_bin[BLOCK_SIZE][resolution];
    memset(msb_bin, '\0', sizeof(msb_bin));

    for (int i = 0; i < (fs_index + 1); i++)
    {
        // char binary[resolution - k + 1];
        zfill(dec_seq[i], resolution - k, msb_bin[i]);
        // printf("%s,", msb_bin[i]);
    }
    int total = 0;
    int current_index = (fs_index + 1) + sum(msb_dec, fs_index, total);
    // printf("\ncurrent_index=%d\n", current_index);
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
    // printf("\ndecoded_dec:");
    for (int i = 0; i < BLOCK_SIZE - 1; i++)
    {
        decoded_dec[i] = strtol(decoded_bin[i], NULL, 2);
        // printf("%d,", decoded_dec[i]);
    }
    decoded_seq[decoded_index++] = strtol(first_sample, NULL, 2);
    for (int y = 0; y < BLOCK_SIZE - 1; y++)
    {
        decoded_seq[decoded_index++] = decoded_dec[y];
    }
    // printf("\ndecoded_seq:");
    for (int i = 0; i < decoded_index; i++)
    {
        // printf("%d,", decoded_seq[i]);
    }
    current_index += k * (BLOCK_SIZE - 1);
    return current_index + resolution + 4;
}
int second_extension_decoder(char *seq, char *first_sample)
{
    // printf("seq %d\n", seq);
    int index = 0, fs_index = 0;
    int dec_seq[4096] = {0};
    int *gamma_list;
    fs_index = FS_decoder(seq, BLOCK_SIZE / 2 + 1, SEQ_OG_SIZE, dec_seq, fs_index, 4096);
    gamma_list = &dec_seq[0];
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
    printf("\nsecond extention decoder called decoded_index=%d\n", decoded_index);
    for (int y = 0; y < (fs_index + 1); y++)
    {
        decoded_seq[decoded_index++] = delta[y];
        printf("\nsecond extention decoder called decoded_index=%d\n", decoded_index);
    }
    int total = 0;
    int current_index = (fs_index + 1) + sum(gamma_list, fs_index, total);
    return (current_index + 5 + resolution);
}
int zero_block_decoder(char *seq, int start_index, int seq_length)
{
    printf("\nin zero block decoder\n");
    int zeros = 0;
    int all_zero_blocks = 0;
    int ROS = 4;
    int current_index;

    for (int i = start_index - 1; i < seq_length; i++)
    {
        if (seq[i] == '0')
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

    int decoded_all_zero_block[all_zero_blocks];
    for (int i = 0; i < all_zero_blocks * BLOCK_SIZE; i++)
    {
        // decoded_all_zero_block[i] = 0;
        decoded_seq[decoded_index++] = 0;
        // decoded_seq[decoded_index++] = decoded_all_zero_block[i];
        printf("\nzero block decoder called decoded_index=%d\n", decoded_index);
    }

    current_index = zeros + 1;

    return current_index + 5;
}
int *post_processor(int *seq)
{
    // int x[BLOCK_SIZE + 1] = {0};
    int *x = (int *)calloc(BLOCK_SIZE + 1, sizeof(int));
    int xmax = MAX_LIMIT;
    int xmin = MIN_LIMIT;
    int delta[BLOCK_SIZE + 1] = {0};
    x[0] = 0;
    delta[0] = 0;
    int theta = 0, a = 0, b = 0, di = 0, index = 1;
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        // printf("\n=================================");
        index += 1;
        if (i == 0)
        {
            a = x[0] - xmin;
            b = xmax - x[0];
        }
        else
        {
            a = x[i - 1] - xmin;
            b = xmax - x[i - 1];
        }
        theta = (a < b) ? a : b;
        // printf("\na=%d,b=%d,theta=%d,", a, b, theta);
        // printf("\n=================================");
        if (seq[i] <= 2 * theta)
        {
            if (seq[i] % 2 == 0)
            {
                di = seq[i] / 2;
            }
            else
            {
                di = -(seq[i] + 1) / 2;
            }
            delta[i + 1] = di;
        }
        else if (theta == a || theta == b)
        {
            if (theta == a)
            {
                di = seq[i] - theta;
            }
            else if (theta == b)
            {
                di = theta - seq[i];
            }
            delta[i + 1] = di;
        }
        else
        {
            printf("\ntheta or seq[i] not match with any data\n");
        }
        // printf("\n");
        // printf("index=%d\n", index);
        // for (int i = 0; i < index; i++)
        // {
        //     printf("delta=%d,", delta[i]);
        // }
        // printf("\n%%%%%%%%%%\n");
        prop(&delta[0], index, &x[0]);
        // printf("\n");

        // printf("\n%d\n", x[index - 2]);
        // X[final_decoded_index] = x[index - 2];
        final_decoded_seq[final_decoded_index] = x[index - 2];
        // printf("%d,", final_decoded_index);
        if (final_decoded_index < SEQ_OG_SIZE - 1)
        {
            final_decoded_index++;
        }
        // printf("=================================");
    }
    free(x);
    // printf("\nfinalindex=%d\n", final_decoded_index);
    // return X;
}

int *prop(int *seq, int index, int *x)
{
    int delta = 0;
    int *y = (int *)calloc((index + 1), sizeof(int));

    for (int i = 0; i < index; i++)
    {
        delta = seq[i] + y[i];
        y[i + 1] = delta;
    }

    int i = 0, j;
    for (j = 2; j < index + 1; i++, j++)
    {
        x[i] = y[j];
        // printf("x[%d]=%d,", i, x[i]);
    }

    free(y);
    if (index > 1)
    {
        // printf("\n index>1\n");
        return x;
    }
}
int sum(int *dec_seq, int index, int total)
{
    for (int i = 0; i < (index + 1); i++)
    {
        total += dec_seq[i];
    }
    // printf("\nsum=%d\n", total);
    return total;
}
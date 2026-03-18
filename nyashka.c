#include <wchar.h>
#include <math.h>

#define MAX_LEN 200000
#define ALPHABET_SIZE 32

wchar_t alphabet[] = L"абвгдежзийклмнопрстуфхцчшщъыьэюя";

double freq_table[ALPHABET_SIZE] =
{
0.0807,0.0168,0.0400,0.0186,0.0311,0.0842,0.0091,0.0141,
0.0570,0.0108,0.0373,0.0501,0.0308,0.0671,0.1151,0.0255,
0.0435,0.0523,0.0676,0.0310,0.0011,0.0075,0.0019,0.0186,
0.0102,0.0026,0.0002,0.0175,0.0198,0.0066,0.0077,0.0236
};

int letter_index(wchar_t ch)
{
    for(int i = 0; i < ALPHABET_SIZE; i++)
        if(alphabet[i] == ch)
            return i;

    return -1;
}

void normalize(wchar_t *input, wchar_t *output)
{
    int out_pos = 0;
    int start = 0;

    if(input[0] == 0xFEFF) start = 1;

    for(int i = start; i < wcslen(input); i++)
    {
        wchar_t ch = input[i];

        if(ch == L'ё') ch = L'е';

        if(letter_index(ch) >= 0)
        {
            output[out_pos++] = ch;
        }
    }

    output[out_pos] = L'\0';
}

void encode(wchar_t *plain, wchar_t *key, wchar_t *cipher)
{
    int key_len = wcslen(key);

    for(int i = 0; i < wcslen(plain); i++)
    {
        int p = letter_index(plain[i]);
        int k = letter_index(key[i % key_len]);

        int c = (p + k) % ALPHABET_SIZE;

        cipher[i] = alphabet[c];
    }

    cipher[wcslen(plain)] = L'\0';
}


void decode(wchar_t *cipher, wchar_t *key, wchar_t *plain)
{
    int key_len = wcslen(key);

    for(int i = 0; i < wcslen(cipher); i++)
    {
        int c = letter_index(cipher[i]);
        int k = letter_index(key[i % key_len]);

        int p = (c - k + ALPHABET_SIZE) % ALPHABET_SIZE;

        plain[i] = alphabet[p];
    }

    plain[wcslen(cipher)] = L'\0';
}


double coincidence_index(wchar_t *text)
{
    int freq[ALPHABET_SIZE] = {0};
    int N = wcslen(text);

    for(int i = 0; i < N; i++)
    {
        int id = letter_index(text[i]);

        if(id >= 0)
            freq[id]++;
    }

    double sum = 0;

    for(int i = 0; i< ALPHABET_SIZE; i++)
        sum += freq[i] * (freq[i] - 1);

    return sum / (N * (N - 1.0));
}


int detect_key_length(wchar_t *cipher)
{
    double target = 0.055;
    double best_diff = 100;

    int best = 1;

    for(int r = 2; r <= 30; r++)
    {
        double avg = 0;

        for(int j = 0; j<r ; j++)
        {
            wchar_t block[MAX_LEN];
            int pos = 0;

            for(int i = j; i < wcslen(cipher); i+=r)
                block[pos++] = cipher[i];

            block[pos] = L'\0';

            if(pos > 1)
                avg += coincidence_index(block);
        }

        avg /= r;

        double diff = fabs(avg - target);

        if(diff < best_diff)
        {
            best_diff = diff;
            best = r;
        }
    }

    return best;
}


void guess_key_simple(wchar_t *cipher, int key_len, wchar_t *key)
{
    for(int j = 0;j < key_len; j++)
    {
        int freq[ALPHABET_SIZE] = {0};

        for(int i = j; i < wcslen(cipher); i += key_len)
        {
            int id = letter_index(cipher[i]);
            freq[id]++;
        }

        int max_id = 0;

        for(int i = 1;i < ALPHABET_SIZE; i++)
            if(freq[i] > freq[max_id])
                max_id = i;

        int o_index = letter_index(L'о');

        int shift = (max_id - o_index + ALPHABET_SIZE) % ALPHABET_SIZE;

        key[j] = alphabet[shift];
    }

    key[key_len] = L'\0';
}


void guess_key_mg(wchar_t *cipher, int key_len, wchar_t *key)
{
    for(int j = 0; j < key_len; j++)
    {
        int freq[ALPHABET_SIZE] = {0};
        int size = 0;

        for(int i = j; i < wcslen(cipher); i+=key_len)
        {
            int id = letter_index(cipher[i]);
            freq[id]++;
            size++;
        }

        double best_score = -1;
        int best_shift = 0;

        for(int g = 0; g < ALPHABET_SIZE; g++)
        {
            double score = 0;

            for(int c = 0; c < ALPHABET_SIZE; c++)
            {
                int p = (c - g + ALPHABET_SIZE) % ALPHABET_SIZE;

                score += ((double)freq[c]/size) * freq_table[p];
            }

            if(score > best_score)
            {
                best_score = score;
                best_shift = g;
            }
        }

        key[j] = alphabet[best_shift];
    }

    key[key_len] = L'\0';
}

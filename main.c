#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>

#define MAX_LEN 200000

void normalize(wchar_t *input, wchar_t *output);
void encode(wchar_t *plain, wchar_t *key, wchar_t *cipher);
void decode(wchar_t *cipher, wchar_t *key, wchar_t *plain);

double coincidence_index(wchar_t *text);
int detect_key_length(wchar_t *cipher);

void guess_key_simple(wchar_t *cipher, int key_len, wchar_t *key);
void guess_key_mg(wchar_t *cipher, int key_len, wchar_t *key);

int main()
{
    setlocale(LC_ALL,"");

    FILE *f;

    wchar_t raw[MAX_LEN];
    char buffer[MAX_LEN];
    wchar_t plain[MAX_LEN];
    wchar_t cipher[MAX_LEN];
    wchar_t decrypted[MAX_LEN];
    wchar_t key[64];

    f = fopen("text.txt","r");
    fgetws(raw, MAX_LEN, f);
    fclose(f);

    normalize(raw, plain);
    
    wchar_t sample_key[] = L"ждлорпавыфячсми";

    encode(plain, sample_key, cipher);
    //wprintf(L"\nencrypted text:\n%ls\n", cipher);
    
    decode(cipher, sample_key, decrypted);
    //wprintf(L"\ndecrypted text:\n%ls\n", decrypted);

    wprintf(L"ic encrypted: %lf\n", coincidence_index(cipher));
    wprintf(L"ic decrypted: %lf\n", coincidence_index(decrypted));

    f = fopen("11.txt","r");
    fgetws(raw, MAX_LEN, f);
    fclose(f);

    normalize(raw, cipher);

    int key_len = detect_key_length(cipher);

    wprintf(L"estimated key length: %d\n", key_len);

    guess_key_simple(cipher, key_len, key);
    wprintf(L"first key guess: %ls\n", key);

    guess_key_mg(cipher, key_len, key);
    wprintf(L"second key guess: %ls\n", key);

    decode(cipher, key, decrypted);

    wprintf(L"\ndecrypted text:\n%ls\n", decrypted);

    wprintf(L"\nic decrypted: %lf\n", coincidence_index(decrypted));

    return 0;
}

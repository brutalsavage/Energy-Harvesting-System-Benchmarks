/* test case mostly by Andres Molina-Markham amolina AT cs umass edu */

/*
 * Copyright 2010 UMass Amherst. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY UMass Amherst ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



#include <math.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <nvs.h>
#include <ctpl.h>
#include <msp430.h>
#include <ctpl_low_level.h>
/*
 * generate a public key and exponent suitable for RSA encryption like this:
 * % openssl genrsa -out foo.rsa <numbits>
 * % openssl rsa -in foo.rsa -pubout | openssl rsa -pubin -noout -text
 *   <will output n and e>
*/
nvs_data_handle nvsHandle1;
nvs_data_handle nvsHandle4;
#define KEYLEN 4

typedef struct data {
     //set_to_zero(_tmpglobal_ab,2*KEYLEN);
    uint16_t add_mp_elements_epsilon;
    int counteri;
    int counterj;
    int counterk;
    int counteru;
    int counterw;
    int counterf;
    int bit_length_counter;
    int last_non_zero_word;
    int mp_bit_length_counter;
    uint16_t _tmpglobal_q[KEYLEN];
    uint16_t _tmpglobal_x[2*KEYLEN];
    uint16_t _tmpglobal_ybnt[2*KEYLEN];  //set_to_zero(data._tmpglobal_ybnt, wordlength_out);
    uint16_t _tmpglobal_ybit[2*KEYLEN];
    uint16_t ciphertext[KEYLEN];
    uint16_t plaintext[KEYLEN];
    uint16_t _tmpglobal_qitybit[2*KEYLEN];
    uint16_t _tmpglobal_temp[KEYLEN];
    uint16_t n[KEYLEN];
    uint16_t _tmpglobal_ab[2*KEYLEN];
    uint16_t e[2];
    uint16_t subtract_mp_elements_epsilon;
    uint16_t divide_mp_elements_ls[3];
    uint16_t multiply_sp_by_mp_element_uv;
    uint16_t multiply_sp_by_mp_element_carry;
    uint16_t UV[2];
    uint16_t temp1[2];
    uint16_t temp2[2];
    uint16_t* a;
    uint16_t* b;
    uint16_t* p;
    uint16_t* c;
    uint16_t temp_ls[2];
    uint16_t rs[3];
    uint16_t temp;
    uint16_t* pfe_c;
    uint16_t* pfe_a;
    uint16_t* pfe_b;
    uint16_t* mult_a;
    uint16_t* mult_out;
    uint16_t wordlength;
    uint16_t kvalue;
    uint16_t t;
    uint16_t f;
    uint16_t uv;
    uint16_t wordlength_out;
    uint16_t multiply_sp_by_mp_element_a;
    uint16_t wordlength_b;
    uint16_t u;
    uint16_t v;
    uint16_t* set_to_zero;
    uint16_t* compare_mp_elements_a;
    uint16_t* compare_mp_elements_b;
    int totalCounter;
} data_t;

#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(nvsStorage4, ".infoA")
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma location="INFOA"
__no_init
#endif


#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(nvsStorage1)
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent
#endif

uint8_t nvsStorage4[NVS_DATA_STORAGE_SIZE(sizeof(uint64_t))];

uint8_t nvsStorage1[NVS_DATA_STORAGE_SIZE(sizeof(data_t))] = {0};

#pragma PERSISTENT(finish);
int finish = 1;

int check = 1;

volatile int temp;
uint16_t status;
data_t data;
int count;
uint16_t KEYLEN1;


#define ADC_MONITOR_THRESHOLD   2.2
#define ADC_MONITOR_FREQUENCY   1000
#define MCLK_FREQUENCY          1000000
#define SMCLK_FREQUENCY         1000000

extern void initGpio(void);
extern void initClocks(void);
extern void initAdcMonitor(void);
extern int ctpl_magic_number;
extern int whichBox;
void save(void);

/* would be nice if we could use malloc instead of globals here */
/**
 * One word addition with a carry bit
 * c_i = (a_1 + b_i + epsilon_prime) mod 0xFFFF
 */

uint16_t add_word(uint16_t * c_i, uint16_t a_i, uint16_t b_i, uint16_t epsilon_prime) {
    uint16_t epsilon; //The carry bit
    *c_i = (a_i + b_i + epsilon_prime);

    if ((*c_i >= (a_i + epsilon_prime)) && (*c_i >= b_i)) {
        epsilon = 0;
    } else {
        epsilon = 1;
    }
    return epsilon;
}

/**
 * One word subtraction with a borrow bit
 * c_i = (a_1 - b_i - epsilon_prime) mod 0xFFFF
 */
uint8_t subtract_word(uint16_t * c_i, uint16_t a_i, uint16_t b_i, uint8_t epsilon_prime) {
    uint8_t epsilon; //The carry bit
    *c_i = (a_i - b_i - epsilon_prime);
    if (0xFFFF == b_i) {
        epsilon = 1;
    } else {
        if (a_i < b_i) {
            epsilon = 1;
        } else {
            epsilon = 0;
        }
    }

    return epsilon;
}

/**
 * Multiprecision addition c.f. Alg. 2.5
 * Input: a, b \in [0,2^{Wt})
 * Output: (epsilon, c) where c = a + b mod 2^{Wt} and epsilon is the carry bit
 */

uint16_t add_mp_elements(uint16_t * pfe_c, uint16_t * pfe_a, uint16_t * pfe_b, uint8_t wordlength) {
    //uint16_t epsilon; //The carry bit
     // index for loop
    //1.
    //epsilon = add_word(&pfe_c[0], pfe_a[0], pfe_b[0], 0);
    //2.
    data.pfe_c = pfe_c;
    data.pfe_a = pfe_a;
    data.pfe_b = pfe_b;
    data.wordlength = wordlength;
    data.counteri = 1;
    data.add_mp_elements_epsilon = add_word(&data.pfe_c[0], data.pfe_a[0], data.pfe_b[0], 0);
    while(data.counteri<wordlength) {
        data.add_mp_elements_epsilon = add_word(&data.pfe_c[data.counteri], data.pfe_a[data.counteri], data.pfe_b[data.counteri], data.add_mp_elements_epsilon);
        data.counteri = data.counteri +1;
        if (check == 0) {
            save();
        }
    }
    //3.
    return data.add_mp_elements_epsilon;
}

/**
 * Multiprecision subtraction c.f. Alg. 2.6
 * Input: a, b \in [0,2^{Wt})
 * Output: (epsilon, c) where c = a - b mod 2^{Wt} and epsilon is the borrow bit
 */

uint16_t subtract_mp_elements(uint16_t * pfe_c, uint16_t * pfe_a, uint16_t * pfe_b, uint8_t wordlength) {
    //1.
    data.counteri = 1;
    data.pfe_c=pfe_c;
    data.pfe_a=pfe_a;
    data.pfe_b=pfe_b;
    data.wordlength = wordlength;
    data.subtract_mp_elements_epsilon = subtract_word(&data.pfe_c[0], data.pfe_a[0],  data.pfe_b[0], 0);
    while(data.counteri< data.wordlength) {
        data.subtract_mp_elements_epsilon = subtract_word(&data.pfe_c[data.counteri], data.pfe_a[data.counteri],  data.pfe_b[data.counteri], data.subtract_mp_elements_epsilon);
        data.counteri = data.counteri + 1;
       if (check == 0 ) {
            save();
        }
    }
    //3.
    return data.subtract_mp_elements_epsilon;
}

/**
 * Addition in F_p c.f. Alg. 2.7
 * Input: a, b \in [0,p-1)
 * Output: c = a + b mod p
 */

void add_mod_p(uint16_t * c, uint16_t * a, uint16_t * b, uint16_t * p, uint8_t wordlength) { //unused function
    uint8_t epsilon;

    //1.
    epsilon = add_mp_elements(c, a, b, wordlength);
    //2.
    if (1 == epsilon) {
        subtract_mp_elements(c, c, p, wordlength);
    } else {
        if (1 == compare_mp_elements(c, p, wordlength)) {
            subtract_mp_elements(c, c, p, wordlength);
        }
    }
}

/**
 * subtraction in F_p c.f. Alg. 2.8
 * Input: a, b \in [0,p-1)
 * Output: c = a - b mod p
 */
void subtract_mod_p(uint16_t * c, uint16_t * a, uint16_t * b, uint16_t * p, uint8_t wordlength) { //unused function
    uint8_t epsilon; //The carry bit
    //1.
    epsilon = subtract_mp_elements(c, a, b, wordlength);
    if (1 == epsilon) {
        add_mp_elements(c, c, p, wordlength);
    }
}

/**
 * Sets a bn to zero
 */
void set_to_zero(uint16_t * c, uint8_t wordlength) {
    data.counterf = 0;
    data.wordlength = wordlength;
    data.set_to_zero = c;
    while (data.counterf< data.wordlength) {
        data.set_to_zero[data.counterf] = 0x0000;
        data.counterf= data.counterf+1;
        if (check == 0) {
            save();
        }
    }
}

/**
 * Multiply two single words into a double word
 * Input: a,b words
 * Output: uv one 2-word
 */
void multiply_words(uint16_t a, uint16_t b, uint16_t * uv) {
    uint32_t uv_32;
    uint16_t u,v;
    uv_32 = ((uint32_t) a) * ((uint32_t) b);
    u = (uint16_t) (uv_32 >> 16);
    v = (uint16_t) uv_32;
    uv[1] = u;
    uv[0] = v;
}

/**
 * Multiply two single words into a double word
 * Input: a,b words
 * Output: uv one 2-word
 */

void multiply_words_2(uint16_t a, uint16_t b, uint16_t * uv) { //unused function
    uint16_t a0, a1, b0, b1;
    uint16_t t[2];
    uint16_t s[2];
    uint16_t m;
    uint16_t borrow;

    a0 = (a & 0xFF00) >> 8;
    a1 = a & 0x00FF;
    b0 = (b & 0xFF00) >> 8;
    b1 = b & 0x00FF;

    //1.
    m = a1 * b1;
    t[0] = m & 0x00FF;
    borrow = (m & 0xFF00) >> 8;

    //2.
    m = a0 * b1 + borrow;
    t[0] ^= ((m & 0x00FF) << 8);
    t[1] = ((m & 0xFF00) >> 8);

    //3.
    m = a1*b0;
    s[0] = (m & 0x00FF) << 8;
    borrow = (m & 0xFF00) >> 8;

    //4.
    m = a0 * b0 + borrow;
    s[1] = m;


    //5.
    //Add two rows s ,t
    add_mp_elements(uv, s, t, 2);

}

/**
 * Multiprecision Multiplication c.f. Alg. 2.9
 * Input: a, b \in [0,p-1]
 * Output: c = a*b
 */

void multiply_mp_elements(uint16_t * c, uint16_t * a, uint16_t * b, uint8_t KEYLEN1) {
    data.a = a;
    data.b = b;
    data.c = c;
    data.counterw = 0;
    data.counterj = 0;
    data.t = KEYLEN1;
    //1. Set c[i] = 0 for 0 \leq i \leq wordlength-1
    //set_to_zero(c, 2 * wordlength);
    //2.
    set_to_zero(data.UV, 2);
    while(data.counterw  < data.t) {
        data.UV[1] = 0;
       while(data.counterj < data.t) {
            //UV = c[i+j] + a[i]*b[j] + UV[0];
            data.temp2[0] = data.UV[1];
            data.temp2[1] = 0x0000;
            multiply_words(data.a[data.counterw], b[data.counterj], data.UV);
            data.temp1[0] = data.c[data.counterw + data.counterj];
            data.temp1[1] = 0x0000;
            add_mp_elements(data.UV,data.UV,data.temp1,2);
            add_mp_elements(data.UV,data.UV,data.temp2,2);
            data.c[data.counterw + data.counterj] = data.UV[0];
            data.counterj = data.counterj+1;
            if (check == 0) {
               save();
           }

        }
       data.c[data.counterw + data.wordlength] = data.UV[1];
       data.counterw=  data.counterw +1;
       if (check == 0) {
         save();
      }
    }
}

/**
 * Multiprecision Multiplication c.f. Alg. 2.9
 * Input: a, b \in [0,p-1]
 * Output: c = a*b
 */
void multiply_mp_elements2(uint16_t * c, uint16_t * a, uint8_t wordlength_a, uint16_t * b, uint8_t wordlength_b) { //unused functions
    uint16_t UV[2];
    uint16_t temp1[2];
    uint16_t temp2[2];

    int i, j;
    //1. Set c[i] = 0 for 0 \leq i \leq wordlength-1
    set_to_zero(c, wordlength_a + wordlength_b);
    //2.
    set_to_zero(UV, 2);
    for (i = 0; i < wordlength_b; i++) {
        UV[1] = 0;
        for (j = 0; j < wordlength_a; j++) {
            //UV = c[i+j] + a[i]*b[j] + UV[0];
            temp2[0] = UV[1];
            temp2[1] = 0x0000;
            multiply_words(a[i], b[j], UV);
            temp1[0] = c[i + j];
            temp1[1] = 0x0000;
            add_mp_elements(UV, UV, temp1, 2);
            add_mp_elements(UV, UV, temp2, 2);
            c[i + j] = UV[0];
        }
        c[i + wordlength_b] = UV[1];
    }
}



/**
 * Compares two big nums
 * Returns 1 if a >= b 0 otherwise
 */
uint8_t compare_mp_elements(uint16_t * a, uint16_t * b, uint8_t wordlength) {
    data.wordlength = wordlength;
    data.counterj = data.wordlength-1;
    data.compare_mp_elements_a = a;
    data.compare_mp_elements_b = b;
    while (data.counterj > -1) {
        //if (a[i] > b[i]) {
        //    return 1;
        //}
        if (data.compare_mp_elements_a[data.counterj] < data.compare_mp_elements_b[data.counterj]) {
            return 0;
        }
        if (data.compare_mp_elements_a[data.counterj] > data.compare_mp_elements_b[data.counterj]) {
            return 1;
        }
        data.counterj = data.counterj-1;
        if (check == 0) {
            save();
        }
    }
    //The elements are equal
    return 1;
}

/**
 Multiply by a power of b
 * out = a*b^k
 */

void mult_by_power_of_b(uint16_t * out, uint16_t wordlength_out, uint16_t * a,
                        uint16_t wordlength_a, uint16_t k) {
    //initialize out
    set_to_zero(out,wordlength_out);
    data.mult_a = a;
    data.mult_out = out;
    data.counteri  = 0;
    data.wordlength = wordlength_a;
    data.wordlength_out = wordlength_out;
    data.kvalue = k;
    while(data.counteri + data.kvalue < data.wordlength_out) {
        if(data.counteri<data.wordlength) {
            data.mult_out[data.counteri+data.kvalue] = data.mult_a[data.counteri];
        }
        data.counteri =data.counteri+1;
        if (check == 0) {
            save();
        }
     }

}

void mod_pow_of_b(uint16_t * out, uint16_t wordlength_out, uint16_t * a,
                  uint16_t wordlength_a, uint16_t k){ // unused function
    int i =0;

    while(i < wordlength_out) {
        if(i < wordlength_a) {
            out[i] = a[i];
        }
        else {
            out[i] = 0;
        }
        i++;
    }
}

/*
 Divide by a power of b
 */
void div_by_power_of_b(uint16_t * out, uint16_t out_len, uint16_t * a, uint16_t a_len, uint16_t k) { //unused function
    int i;
    //initialize z_div
    set_to_zero(out, out_len);
    if (out_len + 1 > a_len - k) {
        for (i = 0; i < out_len; i++) {
            if(k+i<a_len) {
                out[i] = a[k+i];
            }
        }
    }
}

/**
 * @param c An output BigNum such that c = a * b
 * @param a A 16-bit unsigned integer
 * @param b A BigNum of size wordlength_b in 16-bit words.
 * @param wordlength_b
 */

void multiply_sp_by_mp_element(uint16_t * c, uint16_t a, uint16_t * b,
                               uint16_t wordlength_b) {
    data.u=0;
    data.v=0;
    data.c = c;
    data.multiply_sp_by_mp_element_a = a;
    data.b = b;
    data.wordlength_b = wordlength_b;
    //1. Set c[i] = 0 for 0 \leq i \leq wordlength-1
    set_to_zero(data.c, data.wordlength_b + 1);
    //2. Perform paper and pencil multiplication
    data.multiply_sp_by_mp_element_uv = 0;
    data.multiply_sp_by_mp_element_carry = 0;
    data.counterj = 0;
    data.uv = 0;
    while(data.counterj< data.wordlength_b) {
        data.multiply_sp_by_mp_element_uv = ((uint32_t) data.multiply_sp_by_mp_element_a) * ((uint32_t) data.b[data.counterj]) + ((uint32_t) data.multiply_sp_by_mp_element_carry);
        data.u = (uint16_t) (data.uv >> 16);
        data.v = (uint16_t) data.uv;
        data.c[data.counterj] = data.v;
        data.multiply_sp_by_mp_element_carry = data.u;
        data.counterj = data.counterj+1;
        if (check == 0) {
          save();
      }
    }
    data.c[data.wordlength_b] = data.multiply_sp_by_mp_element_carry;
}

int are_mp_equal(uint16_t * a, uint16_t * b, uint8_t wordlength){ //unused function
    int i =0;
    int answ = 1;

    while((1 == answ) && (i<wordlength)){
        if(a[i] != b[i]) {
            answ = 0;
        }
        i++;
    }
    return answ;
}

void copy_mp(uint16_t * a, uint16_t * b, int wordlength){
    data.a = a;
    data.b = b;
    data.counteri = 0;
    while(data.counteri< wordlength) {
        data.a[data.counteri] = data.b[data.counteri];
        data.counteri = data.counteri+1;
        if(check == 0) {
            save();
        }
    }
}

int ith_bit(uint16_t e, int i){
    uint16_t mask;
    mask = 0x0001 << i;
    mask = e & mask;
    if(0x0000 == mask){
        return 0;
    } else {
        return 1;
    }
}

int bit_length(uint16_t e){
    data.bit_length_counter = 15;
    int found_one = 0;
    while(0 == found_one){
        if(1 == ith_bit(e, data.bit_length_counter)) {
            found_one = 1;
        } else {
            data.bit_length_counter = data.bit_length_counter-1;
        }
        if (check == 0) {
            save();
        }
    }
    return data.bit_length_counter;
}

int mp_bit_length(uint16_t * e, uint16_t wordlength){
    data.wordlength = wordlength;
    data.mp_bit_length_counter = data.wordlength-1;
    //data.e = e;
    data.last_non_zero_word = -1;
    while((data.mp_bit_length_counter>-1)&&(data.last_non_zero_word < 0)){
        if(0!=data.e[data.mp_bit_length_counter]){
            data.last_non_zero_word =data.mp_bit_length_counter;
        }
        data.mp_bit_length_counter = data.mp_bit_length_counter-1;
        if (check == 0) {
            save();
        }
    }
    return (int)(16*data.last_non_zero_word + bit_length(data.e[data.last_non_zero_word]));
}

int mp_ith_bit(uint16_t * e, int i){
    uint16_t word;
    uint16_t word_bit;
    word = (int) i/16;
    word_bit = ith_bit(e[word],i - word *16);
    return word_bit;
}

int mp_non_zero_words(uint16_t * e, uint16_t wordlength){ //unused function
    int i = wordlength - 1;
    int last_non_zero_word = -1;
    while((i>-1)&&(last_non_zero_word < 0)){
        if(0!= e[i]){
            last_non_zero_word = i;
        }
        i--;
    }
    return last_non_zero_word;
}

/**
 * Multiplication in F_p
 * Input: a, b \in [0,p-1)
 * Output: c = a * b mod p
 */
void multiply_mod_p(uint16_t * c, uint16_t * a, uint16_t * b, uint16_t * p,
        uint8_t KEYLEN1){
    data.a = a;
    data.c = c;
    data.b = b;
    data.p = p;
    multiply_mp_elements(data._tmpglobal_ab, data.a, data.b, KEYLEN);
    divide_mp_elements(data._tmpglobal_q, data.c, data._tmpglobal_ab, 2*data.t, data.p, KEYLEN);
}

/*
 Returns floor((x1*(0xFFFF) + x0)/y)
 */

uint16_t divide_2_word_by_1_word(uint16_t x1, uint16_t x0, uint16_t y) {
    uint32_t result;
    if (0 != y) {
        //result = (uint32_t ) ((uint32_t ) x1) * (0x00010000);
        result = (((uint32_t) x1) << 16);
        //result = result | x0;
        result = result + (uint32_t) x0;
        result = (result) / ((uint32_t) y);
        return result;
    } else {
        return 0;
    }
}

/**
 * Multiprecision Division c.f. Alg. 14.20 Handbook of Applied Crypto (Menezes, et.al.)
 * Input: x = (x_n . . . x_1 x_0)_b, y = (y_t . . . y_1 y_0)_b with n >= t > 1, y_t != 0
 * Output: q = (q_{n-t} . . . q_1 q_0)_b, r = (r_t . . . r_1 r_0)_b
 * such that x = qy +r, 0 \leq r < y.
 */

void divide_mp_elements(uint16_t * q, uint16_t * r, uint16_t * x_in, int n,
        uint16_t * y, int t) {
    data.c =r;
    data.f = n;
    data.t= t;
    data.p = y;
    data.counteri = 0;
    data.counterj = 0;
    //For step 3.2
    //uint64_t ls, rs;
    //0) copy x_in to x
    while(data.counteri <data.f) {
        data._tmpglobal_x[data.counteri] = data._tmpglobal_ab[data.counteri];
        data.counteri = data.counteri +1;
        if (check == 0) {
           save();
       }
    }
    //1)
    while (data.counterj <= data.f-data.t) {
        data._tmpglobal_q[data.counterj] = 0x0000;
        data.counterj = data.counterj +1;
        if (check == 0) {
           save();
       }
    }
    //2)
    mult_by_power_of_b(data._tmpglobal_ybnt, data.f, data.p, data.t, data.f - data.t);

    while (1 == compare_mp_elements(data._tmpglobal_x, data._tmpglobal_ybnt, (int) data.f)) {
        data._tmpglobal_q[data.f - t] = data._tmpglobal_q[data.f - data.t] + 1;
        subtract_mp_elements(data._tmpglobal_x, data._tmpglobal_x, data._tmpglobal_ybnt, data.f);
        if (check ==0 ) {
            save();
        }
    }

    //3)
    data.counterk = data.f-1;
    while(data.counterk > data.t - 1) { //<--- check index here
       data.temp = data._tmpglobal_x[data.counterk] -  data.p[data.t - 1];
        //3.1)
        if (0 == data.temp) {
            data._tmpglobal_q[data.counterk - data.t] = 0xFFFF; //<--- check index here
        } else {
            data._tmpglobal_q[data.counterk - data.t] = divide_2_word_by_1_word(data._tmpglobal_x[data.counterk], data._tmpglobal_x[data.counterk - 1], data.p[data.t - 1]);
        }
        //3.2)
        //ls = ((uint64_t) q[i - t])*((((uint64_t) y[t - 1]) << 16) + ((uint64_t) y[t - 2]));
        data.temp_ls[0] = data.p[data.t - 2];
        data.temp_ls[1] = data.p[data.t - 1];
        multiply_sp_by_mp_element(data.divide_mp_elements_ls, data._tmpglobal_q[data.counterk - data.t], data.temp_ls, 2);

        //rs = (((uint64_t) x[i]) << 32) + (((uint64_t) x[i - 1]) << 16) + ((uint64_t) x[i - 2]);
        data.rs[0] = data._tmpglobal_x[data.counterk - 2];
        data.rs[1] = data._tmpglobal_x[data.counterk - 1];
        data.rs[2] = data._tmpglobal_x[data.counterk];

        //if((0xBCD3 != rs[2]) && (0x78FC != rs[1]) && (0x1917 != rs[0])) {

        //while (ls > rs) {
        while (0 == compare_mp_elements(data.rs, data.divide_mp_elements_ls, 3)) {
            data._tmpglobal_q[data.counterk - data.t] =  data._tmpglobal_q[data.counterk - data.t] - 1;
            //ls = ((uint64_t) q[i - t])*((((uint64_t) y[t - 1]) << 16)+ ((uint64_t) y[t - 2]));
            multiply_sp_by_mp_element(data.divide_mp_elements_ls, data._tmpglobal_q[data.counterk - data.t], data.temp_ls, 2);\
            if (check ==0 ) {
                save();
            }
        }

        //3.3)
        mult_by_power_of_b(data._tmpglobal_ybit, data.f, data.p, data.t, data.counterk - data.t);
        multiply_sp_by_mp_element(data._tmpglobal_qitybit, data._tmpglobal_q[data.counterk - data.t], data._tmpglobal_ybit, data.f);
        //3.3 + 3.4) Last part of 3.3 merged with 3.4 to avoid negative comparisons
        if (0 == compare_mp_elements(data._tmpglobal_x, data._tmpglobal_qitybit, data.f)) {
            add_mp_elements(data._tmpglobal_x, data._tmpglobal_x, data._tmpglobal_ybit, data.f);
            subtract_mp_elements(data._tmpglobal_x, data._tmpglobal_x, data._tmpglobal_qitybit, data.f);
            data._tmpglobal_q[data.counterk - data.t] = data._tmpglobal_q[data.counterk - data.t] - 1;
        } else {
            subtract_mp_elements(data._tmpglobal_x, data._tmpglobal_x, data._tmpglobal_qitybit, data.f);
        }
        //4
        data.counteri = 0;
        while(data.counteri < t) {
            data.c[data.counteri] = data._tmpglobal_x[data.counteri];
            data.counteri = data.counteri+1;
            if (check == 0) {
                save();
            }
        }
        data.counterk = data.counterk- 1;
        if (check ==0 ) {
           save();
       }
    }
    //}
}

/**
 * Implementation of the left to right modular exponentiation algorithm
 * as described in the HAC book by Menezes et.al.
 *
 * @param A The result of raising g to the power of e
 * @param g an element of Z*_p
 * @param e a multi-precission exponent
 * @param e_legth the wordlength of the multi-precission exponent
 */

void mod_exp(uint16_t * a, uint16_t *b, uint16_t * e, uint16_t e_length,
        uint16_t * n, uint16_t KEYLEN1) {
   // data.ciphertext= a;
   // data.plaintext = b;
   // data.e = e;
  //  data.n = n;
    data.counteru = mp_bit_length(data.e, 1);
    set_to_zero(data.ciphertext, KEYLEN);
    data.ciphertext[0] = 1;

    while(data.counteru>=0)  {// Note, first decrease, then work
        //2.1 A = A*A mod p

        multiply_mod_p(data._tmpglobal_temp, data.ciphertext, data.ciphertext, data.n, KEYLEN);
        copy_mp(data.ciphertext, data._tmpglobal_temp, KEYLEN);

        //2.2 If e_i = 1 then A = Mont(A,x_hat)
        if (1 == mp_ith_bit(data.e, data.counteru)) {
            multiply_mod_p(data._tmpglobal_temp, data.ciphertext, data.plaintext, data.n, KEYLEN);
            copy_mp(data.ciphertext,data._tmpglobal_temp, KEYLEN);

        }
        //save();
        data.counteru = data.counteru-1;
        if (check == 0) {
           save();
        }


    }
    //3.
    //copy_mp(out,A,12); // no need to copy the result, already stored in A
}

void test_rsa_encrypt(){
    mod_exp(data.ciphertext, data.plaintext, data.e, 1, data.n, KEYLEN1);
}
void gpioPinSetup () {
    P4OUT &=~ BIT3;
    P4SEL0 &=~ (BIT3);
    P4SEL1 &=~ (BIT3);
    P4REN &=~ BIT3;
    P4DIR |= BIT3;

    P2OUT |= BIT5;
    P2SEL0 &=~ (BIT5);
    P2REN |= BIT5;
    P2DIR |= BIT5;
    P2OUT &=~ BIT5;

    P2OUT &=~ BIT6;
    P2SEL0 &=~ (BIT6);
    P2REN |= BIT6;
    P2DIR |= BIT6;
}
//MEMENTOS_MAIN_ATTRIBUTES
int main (void) {
    WDTCTL = WDTPW + WDTHOLD; // stop WDT


    initGpio();
    gpioPinSetup();
    initClocks();
    initAdcMonitor();
    P1DIR |= BIT0;
    P1DIR |= BIT1;
    P1OUT &=~ BIT0;
    P1OUT &=~ BIT1;
/*
    if (finish == 1) {
    TB0CCR0 = 2000; //setting the compared time
    TB0CCTL0 = CM_1 + CCIE; //enable interrupt for comparison
    TB0CTL = TBSSEL_2 + MC__UP  + TBCLR; //setting the time to SMCLK with Time that counts up to ... and divider of 4
    }
    */
    __enable_interrupt();
    //nvsHandle4 = nvs_data_init(nvsStorage4, sizeof(int));
    //status = nvs_data_restore(nvsHandle4, &count);
    nvsHandle1 = nvs_data_init(nvsStorage1, sizeof(data_t));
    status = nvs_data_restore(nvsHandle1, &data,ctpl_magic_number,whichBox);
    ctpl_init();
    //count = 0;
    //status = nvs_data_commit(nvsHandle4, &count);
    /* see above comment about generating these */

    data.totalCounter = 0;
    while(data.totalCounter<50) {
        data.n[0] = 0xab78; data.n[1] = 0xafba; data.n[2] = 0x88e7; data.n[3] = 0x496d;
          data.e[0] = 0x0001; data.e[1] = 0x0001; // e = 65537
          data.plaintext[0] = 0x4d65; // Me
          data.plaintext[1] = 0x6d65; // me
          data.plaintext[2] = 0x6e74; // nt
          data.plaintext[3] = 0x6f73; // os
    test_rsa_encrypt();
    data.totalCounter = data.totalCounter +1;
    }
    //__delay_cycles(300000);
    asm volatile ("NOP");
    if (data._tmpglobal_ybnt[7] == 18797) {
        P1OUT |= BIT0;
        P4OUT |= BIT3;
        P4OUT &=~ BIT3;
        P2OUT |= BIT6;
        P2OUT &=~ BIT6;
        finish = 0;
    }

    while(1) {
       //save();
    }
    return 11;
}
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMERB_ISR(void) {
    //P1OUT ^= BIT0;
    //count = count+1;
    //status = nvs_data_commit(nvsHandle4, &count);
}
void save(void) {
    check = 1;
    P4OUT |= BIT3;
    P4OUT &=~ BIT3;
    status = nvs_data_commit(nvsHandle1, &data,whichBox);
    ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
}

void initClocks(void)
{
    /* Clock System Setup, MCLK = SMCLK = DCO (1MHz), ACLK = VLOCLK */
    CSCTL0_H = CSKEY >> 8;
    CSCTL1 = DCOFSEL_0;
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;
    CSCTL0_H = 0;
}

void initAdcMonitor(void)
{
    /* Initialize timer for ADC trigger. */
              TA0CCR0 = (SMCLK_FREQUENCY/ADC_MONITOR_FREQUENCY);
              TA0CCR1 = TA0CCR0/2;
              TA0CCTL1 = OUTMOD_3;
              TA0CTL = TASSEL__SMCLK | MC__UP;

              /* Configure internal 2.0V reference. */
              while(REFCTL0 & REFGENBUSY);
              REFCTL0 |= REFVSEL_1 | REFON;
              while(!(REFCTL0 & REFGENRDY));
              /*
               * Initialize ADC12_B window comparator using the battery monitor.
               * The monitor will first enable the high side to the monitor voltage plus
               * 0.1v to make sure the voltage is sufficiently above the threshold. When
               * the high side is triggered the interrupt service routine will switch to
               * the low side and wait for the voltage to drop below the threshold. When
               * the voltage drops below the threshold voltage the device will invoke the
               * compute through power loss shutdown function to save the application
               * state and enter complete device shutdown.
               */
              ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
              ADC12CTL1 = ADC12SHS_1 | ADC12SSEL_0 | ADC12CONSEQ_2 | ADC12SHP;
              ADC12CTL3 = ADC12BATMAP;
              ADC12MCTL0 = ADC12INCH_31 | ADC12VRSEL_1 | ADC12WINC;
              ADC12HI = (uint16_t)(4096*((ADC_MONITOR_THRESHOLD+0.1)/2)/(2.0));
              ADC12LO = (uint16_t)(4096*(ADC_MONITOR_THRESHOLD/2)/(2.0));
              ADC12IFGR2 &= ~(ADC12HIIFG | ADC12LOIFG);
              ADC12IER2 = ADC12HIIE;
              ADC12CTL0 |= ADC12ENC;
}
void initGpio(void)
{
    /* Configure GPIO to default state */
    P1OUT  = 0; P1DIR  = 0xFF;
    P2OUT  = 0; P2DIR  = 0xFF;
    P3OUT  = 0; P3DIR  = 0xFF;
    P4OUT  = 0; P4DIR  = 0xFF;
    P5OUT  = 0; P5DIR  = 0xFF;
    P6OUT  = 0; P6DIR  = 0xFF;
    P7OUT  = 0; P7DIR  = 0xFF;
    P8OUT  = 0; P8DIR  = 0xFF;
    PJOUT  = 0; PJDIR  = 0xFFFF;

    /* Disable the GPIO power-on default high-impedance mode. */
    PM5CTL0 &= ~LOCKLPM5;
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12LOIFG)) {
        case ADC12IV_NONE:        break;        // Vector  0: No interrupt
        case ADC12IV_ADC12OVIFG:  break;        // Vector  2: ADC12MEMx Overflow
        case ADC12IV_ADC12TOVIFG: break;        // Vector  4: Conversion time overflow
        case ADC12IV_ADC12HIIFG:                // Vector  6: Window comparator high side
            /* Disable the high side and enable the low side interrupt. */
           // P1OUT |= BIT1;
           // __delay_cycles(300000);
           // P1OUT &=~ BIT1;
            ADC12IER2 &= ~ADC12HIIE;
            ADC12IER2 |= ADC12LOIE;
            ADC12IFGR2 &= ~ADC12LOIFG;
            break;
        case ADC12IV_ADC12LOIFG:                // Vector  8: Window comparator low side
            //P1OUT |= BIT0;
            //P1OUT &=~ BIT0;
            check = 0;
            /* Disable the low side and enable the high side interrupt. */
            ADC12IER2 &= ~ADC12LOIE;
            ADC12IER2 |= ADC12HIIE;
            ADC12IFGR2 &= ~ADC12HIIFG;
            break;
        default: break;
    }
}


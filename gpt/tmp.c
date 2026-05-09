// #include <stdlib.h>
// #include <pthread.h>
// #include <semaphore.h>

// #define N_ 8

// void matmul_forward(float* out,
//                     float* inp, float* weight, float* bias,
//                     int B, int T, int C, int OC) {
//     // most of the running time is spent here and in matmul_backward
//     // OC is short for "output channels"
//     // inp is (B,T,C), weight is (OC, C), bias is (OC)
//     // out will be (B,T,OC)
//     for (int b = 0; b < B; b++) {
//         for (int t = 0; t < T; t++) {
//             float* out_bt = out + b * T * OC + t * OC;
//             float* inp_bt = inp + b * T * C + t * C;
//             for (int o = 0; o < OC; o++) {
//                 float val = (bias != NULL) ? bias[o] : 0.0f;
//                 float* wrow = weight + o*C;
//                 for (int i = 0; i < C; i++) {
//                     val += inp_bt[i] * wrow[i];
//                 }
//                 out_bt[o] = val;
//             }
//         }
//     }
// }

// typedef struct Element {
//     int start_id, end_id;
//     float *out, *inp, *weight, *bias;
//     int C, OC, B, T;
// } Element;

// void *callback(void *arg) {
//     Element *e = (Element *)arg;
//     int s = e->start_id, l = e->end_id;
//     float *out = e->out, *inp = e->inp, *weight = e->weight, *bias = e->bias;
//     int B = e->B, T = e->T, C = e->C, OC = e->OC;
//     for (int i = s; i <= l; i++) {
//         int b = i / T;
//         int t = i % T;
//         float* out_bt = out + b * T * OC + t * OC;
//         float* inp_bt = inp + b * T * C + t * C;
//         for (int o = 0; o < OC; o++) {
//             float val = (bias != NULL) ? bias[o] : 0.0f;
//             float* wrow = weight + o*C;
//             for (int j = 0; j < C; j++) {
//                 val += inp_bt[j] * wrow[j];
//             }
//             out_bt[o] = val;
//         }
//     }
//     return NULL;
// }

// void matmul_forward0(float* out,
//                     float* inp, float* weight, float* bias,
//                     int B, int T, int C, int OC) {
    
//     pthread_t tid[N_];
//     Element e[N_];
//     int total = B * T;
//     int n = total / N_;
//     int mod = total % N_;
//     n = n + (mod != 0);
//     for (int i = 0; i < N_; i++) {
//         e[i].start_id = i * n;
//         e[i].end_id = (i + 1) * n - 1; 
//         e[i].out = out;
//         e[i].inp = inp;
//         e[i].weight = weight;
//         e[i].bias = bias;
//         e[i].C = C;
//         e[i].OC = OC;
//         e[i].B = B;
//         e[i].T = T;
//     }

//     for (int i = 0; i < N_; i++) {
//         pthread_create(&tid[i], NULL, callback, (void *)&e[i]);
//     }
//     for (int i = 0; i < N_; i++) {
//         pthread_join(tid[i], NULL);
//     }
// }
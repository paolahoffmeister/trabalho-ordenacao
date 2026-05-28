#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include <chrono>
#include <string>
#include <iomanip>

using namespace std;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

// ===================== ALGORITMOS =====================

void bubbleSort(vector<int> arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j+1]) swap(arr[j], arr[j+1]);
}

void insertionSort(vector<int> arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i], j = i - 1;
        while (j >= 0 && arr[j] > key) { arr[j+1] = arr[j]; j--; }
        arr[j+1] = key;
    }
}

void selectionSort(vector<int> arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i+1; j < n; j++) if (arr[j] < arr[minIdx]) minIdx = j;
        swap(arr[i], arr[minIdx]);
    }
}

void heapify(vector<int>& arr, int n, int i) {
    int largest = i, l = 2*i+1, r = 2*i+2;
    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;
    if (largest != i) { swap(arr[i], arr[largest]); heapify(arr, n, largest); }
}

void heapSort(vector<int> arr) {
    int n = arr.size();
    for (int i = n/2-1; i >= 0; i--) heapify(arr, n, i);
    for (int i = n-1; i > 0; i--) { swap(arr[0], arr[i]); heapify(arr, i, 0); }
}

void shellSort(vector<int> arr) {
    int n = arr.size();
    for (int gap = n/2; gap > 0; gap /= 2)
        for (int i = gap; i < n; i++) {
            int temp = arr[i], j = i;
            while (j >= gap && arr[j-gap] > temp) { arr[j] = arr[j-gap]; j -= gap; }
            arr[j] = temp;
        }
}

void mergeHelper(vector<int>& arr, int l, int m, int r) {
    vector<int> L(arr.begin()+l, arr.begin()+m+1);
    vector<int> R(arr.begin()+m+1, arr.begin()+r+1);
    int i = 0, j = 0, k = l;
    while (i < (int)L.size() && j < (int)R.size())
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < (int)L.size()) arr[k++] = L[i++];
    while (j < (int)R.size()) arr[k++] = R[j++];
}

void mergeSortHelper(vector<int>& arr, int l, int r) {
    if (l < r) { int m = (l+r)/2; mergeSortHelper(arr, l, m); mergeSortHelper(arr, m+1, r); mergeHelper(arr, l, m, r); }
}

void mergeSort(vector<int> arr) {
    if (arr.size() > 1) mergeSortHelper(arr, 0, arr.size()-1);
}

int partitionRand(vector<int>& arr, int low, int high) {
    uniform_int_distribution<int> dist(low, high);
    int pivotIdx = dist(rng);
    swap(arr[pivotIdx], arr[high]);
    int pivot = arr[high], i = low - 1;
    for (int j = low; j < high; j++) if (arr[j] <= pivot) swap(arr[++i], arr[j]);
    swap(arr[i+1], arr[high]);
    return i+1;
}

void quickSortHelper(vector<int>& arr, int low, int high) {
    while (low < high) {
        int pi = partitionRand(arr, low, high);
        if (pi - low < high - pi) { quickSortHelper(arr, low, pi-1); low = pi+1; }
        else { quickSortHelper(arr, pi+1, high); high = pi-1; }
    }
}

void quickSort(vector<int> arr) {
    if (arr.size() > 1) quickSortHelper(arr, 0, arr.size()-1);
}

// ===================== ARRAYS =====================

vector<int> gerarCrescente(int n) {
    vector<int> arr(n); iota(arr.begin(), arr.end(), 1); return arr;
}
vector<int> gerarDecrescente(int n) {
    vector<int> arr(n); iota(arr.begin(), arr.end(), 1); reverse(arr.begin(), arr.end()); return arr;
}
vector<int> gerarAleatorioSemRep(int n) {
    vector<int> arr(n); iota(arr.begin(), arr.end(), 1); shuffle(arr.begin(), arr.end(), rng); return arr;
}
vector<int> gerarAleatorioComRep(int n) {
    uniform_int_distribution<int> dist(1, n);
    vector<int> arr(n);
    for (int i = 0; i < n; i++) arr[i] = dist(rng);
    return arr;
}

// ===================== ESTATISTICA =====================

int loopsInternos(int n) {
    if (n <= 512)   return 1000;
    if (n <= 4096)  return 100;
    if (n <= 16384) return 10;
    return 1;
}

long long medirTempo(void(*fn)(vector<int>), const vector<int>& original, int loops) {
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < loops; i++) fn(original);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::nanoseconds>(end - start).count() / loops;
}

double mediaFiltrada(vector<long long> tempos) {
    double soma = 0;
    for (int i = 0; i < (int)tempos.size(); i++) soma += tempos[i];
    double media = soma / tempos.size();

    double somaVar = 0;
    for (int i = 0; i < (int)tempos.size(); i++) somaVar += (tempos[i]-media)*(tempos[i]-media);
    double dp = sqrt(somaVar / (tempos.size()-1));

    double lower = media - dp, upper = media + dp;
    vector<long long> filtrados;
    for (int i = 0; i < (int)tempos.size(); i++)
        if (tempos[i] >= lower && tempos[i] <= upper)
            filtrados.push_back(tempos[i]);

    if (filtrados.empty()) return media;
    double soma2 = 0;
    for (int i = 0; i < (int)filtrados.size(); i++) soma2 += filtrados[i];
    return soma2 / filtrados.size();
}

// ===================== MAIN =====================

typedef void(*SortFn)(vector<int>);
typedef vector<int>(*GenFn)(int);

int main() {
    int tamanhos[] = {128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
    int nTamanhos = 10;
    int repeticoes = 10;

    string nomesAlg[] = {"Bubble Sort", "Insertion Sort", "Selection Sort", "Heap Sort", "Shell Sort", "Merge Sort", "Quick Sort"};
    SortFn algs[] = {bubbleSort, insertionSort, selectionSort, heapSort, shellSort, mergeSort, quickSort};
    int nAlgs = 7;

    string nomesCen[] = {"Crescente", "Decrescente", "Aleatorio sem repeticao", "Aleatorio com repeticao"};
    GenFn gens[] = {gerarCrescente, gerarDecrescente, gerarAleatorioSemRep, gerarAleatorioComRep};
    int nCen = 4;

    ofstream md("resultados.md");

    for (int c = 0; c < nCen; c++) {
        cout << "\n=== Cenario: " << nomesCen[c] << " ===" << endl;
        md << "## Cenario: " << nomesCen[c] << "\n\n";
        md << "| n | Bubble Sort | Insertion Sort | Selection Sort | Heap Sort | Shell Sort | Merge Sort | Quick Sort |\n";
        md << "|---|---|---|---|---|---|---|---|\n";

        for (int t = 0; t < nTamanhos; t++) {
            int n = tamanhos[t];
            int loops = loopsInternos(n);
            cout << "  n = " << n << " (loops=" << loops << ")..." << flush;
            md << "| " << n;

            vector<int> original = gens[c](n);

            for (int a = 0; a < nAlgs; a++) {
                vector<long long> tempos;
                for (int r = 0; r < repeticoes; r++)
                    tempos.push_back(medirTempo(algs[a], original, loops));
                double media = mediaFiltrada(tempos);
                md << " | " << (long long)media;
            }

            md << " |\n";
            cout << " OK" << endl;
        }
        md << "\n";
    }

    md.close();
    cout << "\nPronto! Resultados salvos em resultados.md" << endl;
    return 0;
}
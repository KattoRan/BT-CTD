#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define MAXN 10001
#define MAXLINE 10001

typedef struct {
    char word[100];
    int count;
    int lines[MAXLINE];
    int line_count;
} WordEntry;
WordEntry indexTable[MAXN];
int indexSize = 0;

void toLowerCase(char *str) {
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}

char stopWords[MAXN][100];
int stopWordCount = 0;
void readStopw(const char *filename){
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error opening file: %s\n", filename);
        return;
    }
    char word[100];
    while (fscanf(f, "%s", word) != EOF) {
        toLowerCase(word);
        strcpy(stopWords[stopWordCount++], word);
    }
    fclose(f);
}

int isStopWord(const char *word) {
    for (int i = 0; i < stopWordCount; i++) {
        if (strcmp(stopWords[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

void addWord(const char *word, int lineNumber) {
    char lowerWord[100];
    strcpy(lowerWord, word);
    toLowerCase(lowerWord);
    for (int i = 0; i < indexSize; i++) {
        if (strcmp(indexTable[i].word, lowerWord) == 0) {
            indexTable[i].count++;
            int found = 0;
            for (int j = 0; j < indexTable[i].line_count; j++) {
                if (indexTable[i].lines[j] == lineNumber) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                indexTable[i].lines[indexTable[i].line_count++] = lineNumber;
            }
            return;
        }
    }
    strcpy(indexTable[indexSize].word, lowerWord);
    indexTable[indexSize].count = 1;
    indexTable[indexSize].lines[0] = lineNumber;
    indexTable[indexSize].line_count = 1;
    indexSize++;
}

int compareWords(const void *a, const void *b) {
    return strcmp(((WordEntry *)a)->word, ((WordEntry *)b)->word);
}

int main(int argc, char *argv[]){
    readStopw("stopw.txt");
    if (argc < 2) {
        printf("Loi: Thieu ten file dau vao.\n");
        printf("Cach su dung: %s <ten_file_van_ban>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        printf("Khong tim thay file\n");
        return 1;
    }
    FILE *out = fopen("out.txt", "w");
    if (!out) {
        printf("Khong the tao file out.txt\n");
        return 1;
    }
    char ch;
    char word[100];
    int index = 0;
    int lineNumber = 1;
    while ((ch = fgetc(f)) != EOF) {
        if (isalpha(ch)) {
            word[index++] = (char)ch;
        } else {
            if (index > 0) {
                word[index] = '\0';
                index = 0;
                int isProperNoun = 0;
                if (isupper(word[0])) {
                    isProperNoun = 1;
                }
                if (!isStopWord(word)&&!isProperNoun) {
                        addWord(strlwr(word), lineNumber);
                    }
                }
            if (ch == '\n') {
                lineNumber++;
            }
        }
    }
    qsort(indexTable, indexSize, sizeof(WordEntry), compareWords);  
    for (int i = 0; i < indexSize; i++) {
        fprintf(out, "%s: %d, ", indexTable[i].word, indexTable[i].count);
        for (int j = 0; j < indexTable[i].line_count; j++) {
            fprintf(out, "%d", indexTable[i].lines[j]);
            if (j < indexTable[i].line_count - 1) {
                fprintf(out, ", ");
            }
        }
        fprintf(out, "\n");
    }
    fclose(f);
    fclose(out);
    return 0;
}
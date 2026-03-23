#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

#define PID_MAX 4194304

typedef struct Node {
	pid_t val;
	int sons_cnt;
	int sons_cap;
	struct Node **sons;
} Node;

Node *nodes[PID_MAX+1];
bool vis[PID_MAX+1];

void Pstree_Print(pid_t root,int indent){
	for(int i=0;i<indent;i++) printf(" ");
	printf("%d\n",(int)root);

	for(int i=0;i<nodes[(int)root]->sons_cnt;i++){
		Pstree_Print(nodes[(int)root]->sons[i]->val,indent+1);
	}
}

int main(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        assert(argv[i]);
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    assert(!argv[argc]);

	//read /proc dirs
    DIR *dir;
    struct dirent *entry;
    dir=opendir("/proc");
    if(dir==NULL){
        perror("opendir");
		return 1;
    }
	
	//construct tree
	int root_cnt=0;
	while((entry=readdir(dir))!=NULL){
		char *DirName=entry->d_name;
		if(DirName[0]>='0' && DirName[0]<='9'){
			char path[512];
			snprintf(path, sizeof(path), "/proc/%s/status", DirName);
			pid_t son=(pid_t)atoi(DirName);

			if(!vis[(int)son]){
				nodes[(int)son]=(Node *)malloc(sizeof(Node));
				vis[(int)son]=true;
				nodes[(int)son]->val=son;
				nodes[(int)son]->sons_cnt=0;
				nodes[(int)son]->sons_cap=2;
				nodes[(int)son]->sons=(Node **)malloc(2*sizeof(Node *));
			}

			char prefix[100];
			int ppid;
			FILE *fp=fopen(path,"r");
			if(!fp){
				perror("failed to open file");
				return 1;
			}

			char line[256];
			while(fgets(line, sizeof(line), fp)) {
				if (sscanf(line, "PPid:\t%d", &ppid) == 1 || sscanf(line, "PPid: %d", &ppid) == 1) {

					if(!vis[ppid]){
						nodes[ppid]=(Node *)malloc(sizeof(Node));
						vis[ppid]=true;
						nodes[ppid]->val=ppid;
						nodes[ppid]->sons_cnt=0;
						nodes[ppid]->sons_cap=2;
						nodes[ppid]->sons=(Node **)malloc(2*sizeof(Node *));
					}

					Node *parnode=nodes[ppid];
					if(parnode->sons_cnt==parnode->sons_cap){
						parnode->sons_cap*=2;
						parnode->sons=realloc(parnode->sons,parnode->sons_cap*sizeof(Node *));
					}
					parnode->sons[parnode->sons_cnt++]=nodes[(int)son];

				}
			}

			fclose(fp);
		}
	}

	//Print
	Pstree_Print(1,0);

	closedir(dir);
    return 0;
}

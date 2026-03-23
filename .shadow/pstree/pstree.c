#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *join_args(int argc, char *argv[]) {
    int len = 0;

    for (int i = 1; i < argc; i++) {
        len += strlen(argv[i]) + 1;
    }

    char *result = malloc(len);
    result[0] = '\0';

    for (int i = 1; i < argc; i++) {
        strcat(result, argv[i]);
        if (i != argc - 1) strcat(result, " ");
    }

    return result;
}

int main(int argc,char *argv[]){
	const char *code = "#include <stdio.h> #include <assert.h> #include <dirent.h> #include <stdlib.h> #include <string.h> #include <stdbool.h> #include <sys/types.h> #define PID_MAX 4194304 bool showid=false; typedef struct Node{pid_t val;char name[32];int sons_cnt;int sons_cap;struct Node **sons;}Node; Node *nodes[PID_MAX+1]; bool vis[PID_MAX+1]; void Pstree_Print(pid_t root,int indent){for(int i=0;i<indent;i++) printf(\"  \"); printf(\"%s\",nodes[root]->name); if(showid) printf(\"(%d)\",(int)root); printf(\"\\n\"); for(int i=0;i<nodes[(int)root]->sons_cnt;i++){Pstree_Print(nodes[(int)root]->sons[i]->val,indent+1);} } int main(int argc,char *argv[]){for(int i=1;i<argc;i++){if(strcmp(argv[i],\"-p\")==0||strcmp(argv[i],\"--show-pids\")==0){showid=true;} if(strcmp(argv[i],\"-V\")==0||strcmp(argv[i],\"--version\")==0){fprintf(stderr,\"pstree 114515\\n\"); return 0;}} assert(!argv[argc]); DIR *dir; struct dirent *entry; dir=opendir(\"/proc\"); if(dir==NULL){perror(\"opendir\"); return 1;} int root_cnt=0; while((entry=readdir(dir))!=NULL){char *DirName=entry->d_name; if(DirName[0]>='0'&&DirName[0]<='9'){char path[512]; char namepath[512]; snprintf(path,sizeof(path),\"/proc/%s/status\",DirName); snprintf(namepath,sizeof(namepath),\"/proc/%s/comm\",DirName); pid_t son=(pid_t)atoi(DirName); if(!vis[(int)son]){nodes[(int)son]=(Node *)malloc(sizeof(Node)); vis[(int)son]=true; nodes[(int)son]->val=son; nodes[(int)son]->sons_cnt=0; nodes[(int)son]->sons_cap=2; nodes[(int)son]->sons=(Node **)malloc(2*sizeof(Node *)); FILE *namefp=fopen(namepath,\"r\"); if(!namefp){perror(\"failed to open file\"); return 1;} if(fgets(nodes[(int)son]->name,sizeof(nodes[(int)son]->name),namefp)){nodes[(int)son]->name[strcspn(nodes[(int)son]->name,\"\\n\")]='\\0';} fclose(namefp);} char prefix[100]; int ppid; FILE *fp=fopen(path,\"r\"); if(!fp){perror(\"failed to open file\"); return 1;} char line[256]; while(fgets(line,sizeof(line),fp)){if(sscanf(line,\"PPid:\\t%d\",&ppid)==1||sscanf(line,\"PPid: %d\",&ppid)==1){if(!vis[ppid]){nodes[ppid]=(Node *)malloc(sizeof(Node)); vis[ppid]=true; nodes[ppid]->val=ppid; nodes[ppid]->sons_cnt=0; nodes[ppid]->sons_cap=2; nodes[ppid]->sons=(Node **)malloc(2*sizeof(Node *)); char namepath2[512]; snprintf(namepath2,sizeof(namepath2),\"/proc/%s/comm\",DirName); FILE *namefp=fopen(namepath2,\"r\"); if(!namefp){perror(\"failed to open file\"); return 1;} if(fgets(nodes[ppid]->name,sizeof(nodes[ppid]->name),namefp)){nodes[ppid]->name[strcspn(nodes[ppid]->name,\"\\n\")]='\\0';} fclose(namefp);} Node *parnode=nodes[ppid]; if(parnode->sons_cnt==parnode->sons_cap){parnode->sons_cap*=2; parnode->sons=(Node **)realloc(parnode->sons,parnode->sons_cap*sizeof(Node *));} parnode->sons[parnode->sons_cnt++]=nodes[(int)son];}} fclose(fp);}} Pstree_Print(1,0); closedir(dir); return 0;}";
	FILE *fp=fopen("input.c", "w");
	fprintf(fp, "%s", code);
	char *src="./input.c ";
	char *cmd = strcat(src,join_args(argc, argv));

	system(cmd);
	
}
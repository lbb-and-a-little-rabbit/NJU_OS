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
	const char *code =
		"#include <sys/types.h>\n"
		"#include <vector>\n"
		"#include <string>\n"
		"#include <iostream>\n"
		"#include <filesystem>\n"
		"#include <fstream>\n"
		"#include <unordered_map>\n"
		"#include <cstring>\n"
		"\n"
		"using namespace std;\n"
		"\n"
		"bool showid;\n"
		"\n"
		"struct Node{\n"
		"    pid_t id;\n"
		"    string name;\n"
		"    vector<Node *>sons;\n"
		"\n"
		"    Node(pid_t id,string name) : id(id),name(name) {}; \n"
		"};\n"
		"\n"
		"unordered_map<pid_t, Node *> map;\n"
		"\n"
		"void Print(int root,int indent){\n"
		"    for(int i=0;i<indent;i++) cout << \"  \";\n"
		"    cout << map[root]->name;\n"
		"    if(showid) {\n"
		"        cout << '(';\n"
		"        cout << map[root]->id;\n"
		"        cout << ')';\n"
		"    }\n"
		"    cout << '\\n';\n"
		"\n"
		"    for(auto s:map[root]->sons){\n"
		"        Print(s->id, indent+1);\n"
		"    }\n"
		"}\n"
		"\n"
		"int main(int argc,char *argv[]){\n"
		"    for (int i = 1; i < argc; i++) {\n"
		"        if(strcmp(argv[i],\"-p\")==0 || strcmp(argv[i], \"--show-pids\")==0){\n"
		"            showid=true;\n"
		"        }\n"
		"        if(strcmp(argv[i], \"-V\")==0 || strcmp(argv[i],\"--version\")==0){\n"
		"            cerr << \"pstree 114514\\n\";\n"
		"            return 0;\n"
		"        }\n"
		"    }\n"
		"\n"
		"    string srcpath=\"/proc\";\n"
		"    for(const auto& entry : filesystem::directory_iterator(srcpath)){\n"
		"        string dirname=entry.path().filename();\n"
		"        if(dirname[0]>='0' && dirname[0]<='9'){\n"
		"            ifstream namefile(\"/proc/\"+dirname+\"/comm\");\n"
		"            string pidname;\n"
		"            getline(namefile ,pidname);\n"
		"            namefile.close();\n"
		"\n"
		"            ifstream statusfile(\"/proc/\"+dirname+\"/status\");\n"
		"            string line;\n"
		"            pid_t ppid;\n"
		"            while(getline(statusfile,line)){\n"
		"                if(!line.rfind(\"PPid\",0)){\n"
		"                    ppid=(pid_t)stoi(line.substr(5));\n"
		"                    break;\n"
		"                }\n"
		"            }\n"
		"            statusfile.close();\n"
		"\n"
		"            pid_t pid=(pid_t)stoi(dirname);\n"
		"            if(!map.count(pid)){\n"
		"                Node *node=new Node(pid, pidname);\n"
		"                map[pid]=node;\n"
		"            }\n"
		"            if(!map.count(ppid)){\n"
		"                ifstream namefile2(\"/proc/\"+dirname+\"/comm\");\n"
		"                string pidname2;\n"
		"                getline(namefile2 ,pidname2);\n"
		"                namefile2.close();\n"
		"                Node *node=new Node(ppid,pidname2);\n"
		"                map[ppid]=node;\n"
		"            }\n"
		"\n"
		"            map[ppid]->sons.push_back(map[pid]);\n"
		"        }\n"
		"    }\n"
		"    Print(1, 0);\n"
		"    return 0;\n"
		"}\n";
	FILE *fp=fopen("input.cpp", "w");
	fprintf(fp, "%s", code);
	fclose(fp);
	char *args = join_args(argc, argv);

	char *cmd = malloc(strlen("./input ") + strlen(args) + 1);
	sprintf(cmd, "./input %s", args);
	system("g++ input.cpp -o input");
	system(cmd);
	free(cmd);
	free(args);
	return 0;
}
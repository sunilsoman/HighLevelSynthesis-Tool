#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <string>
#include <math.h>
using std::vector;
using std::list;
using std::string;

const int MAX_CHARS_PER_LINE = 10240;
const int MAX_TOKENS_PER_LINE = 1000;
const char* const DELIMITER = " ,\t";

int flag = 0;
char hash[]="#";


//Assignment- 3 change
int opcount = 0;

struct Node
{
    char name[1000];
    float delay;
    int size;
    char dataType[1000];
    char nodeType[1000];
    char color[100];
    int id;
    float dist;
    struct Node *next;
    
};

//Assignment- 3 Change
struct OperationNode
{
    int id;
    char type[1000];
    int latency;
    int alapTime;
    int asapTime;
    float force;
    struct OperationNode *next;
    char input[40][1000];
    char output[1000];
};

vector<OperationNode *> operationGraph;

int CreateStructure(char inputArray[1000][1000], int &k, Node * pointer);
int WriteVerilog(char inputArray[1000][1000], int k, char** argv);
int CreateVerilog(char inputArray[1000][1000], int k, char** argv, Node * pointer);
int GetSizeType(char input[1000], int *size, char datatype[1000]);

//Assignment - 3 Change
int CreateOperationStructure(char inputArray[1000][1000], int &k, OperationNode * operation, Node * pointer);
int AddOperationEdge(OperationNode &src, OperationNode dest);
int CreateOperationGraph(OperationNode * operation, int v0Index, int vnIndex);
void OpPrint();
void InitiateAsapCalculation();
int CalculateAsap(int index, int time, int exe);
int InitiateAlapCalculation(int latency);
int CalculateAlap(int id, int alapTime);
int InitiateFdsCalculation ();
int ChildRecursiveCheck(int id);
int ParentRecursiveCheck(int id);




int main(int argc, char** argv)
{
    if ( argc != 4 ){
        
        cout<<"USAGE: hlsyn <InputFileName> <latency> <OutputFileName> \n";
        return 1;
    }
    else {
        
        
        ifstream fin ( argv[1] );
        
        if ( !fin.is_open() ) {
            cout<<"Could not open file\n";
            return 1;
        }
    }
    
    char inputArray[1000][1000] = {};
    Node * pointer = new Node[100];
    OperationNode * operation = new OperationNode[100];
    
    int k=0;
    
    
    ifstream fin;
    fin.open(argv[1]);
    if (!fin.good())
    {
        cout<<"Input file not found. Please place the input file in the current folder."<<endl;
        return 1;
    }
    
    
    while (!fin.eof())
    {
        
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
        // cout << buf << endl;
        
        
        int n = 0;
        
        
        const char* token[MAX_TOKENS_PER_LINE] = {};
        
        
        token[0] = strtok(buf, " \t");
        if (token[0] != NULL)
        {
            for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
            {
                token[n] = strtok(0, DELIMITER);
                if (!token[n]) break;
            }
        }
        else
            continue;
        
        
        
        for (int i = 0; i<= n; i++)
        {
            if(token[i])
                strcpy (inputArray[k], token[i]);
            else
                strcpy (inputArray[k], hash);
            k++;
        }
    }
    
    
    //for (int m = 0; m<k; m++)
    //    cout << inputArray[m] << endl;
    
    CreateStructure(inputArray, k, pointer);
    
    if (!flag)
    
    {
    CreateOperationStructure(inputArray, k, operation, pointer);
    
    //OpPrint();
    
    if(!flag)
    {
    
    InitiateAsapCalculation();
    
    InitiateAlapCalculation(atoi(argv[2]));
    
    if(!flag)
    {
    OpPrint();
        
    InitiateFdsCalculation();
        
    OpPrint();
        
    CreateVerilog(inputArray,k,argv,pointer);
    }
    }
    }
    
    
}


int WriteVerilog(char outputArray[1000][1000], int k, char** argv)
{
   
    for (int m=0; m<k; m++) {
        if (strcmp (outputArray[m],hash)!=0)
            cout << outputArray[m] <<" ";
        else
            cout<<endl;
    }

    ofstream fout;
    fout.open(argv[3]);
    
    for (int m= 0; m<k; m++)
    {
        if (strcmp (outputArray[m],hash)!=0)
            fout << outputArray[m] <<" ";
        else
            fout<<endl;
    }
    fout.close();
    return 0;
    
}


int CreateVerilog(char inputArray[1000][1000], int k, char** argv, Node * pointer)
{
    
    
    char outputArray[1000][1000] = {};
    int m = 0;
    
    
    strcpy(outputArray[m], "`timescale 1ns / 1ps");
    m++;
    strcpy(outputArray[m], "\n");
    m++;
    strcpy(outputArray[m++], "module HLSM (Clk, Rst, Start, Done,");
    
    
    //filling module line inputs and outputs
    for(int j= 0;j<k;j++)
    {
        if(strcmp(pointer[j].nodeType, "variable")!=0 )
        strcpy(outputArray[m], pointer[j].name);
        
        
        if(strcmp(pointer[j+1].name, hash)==0)
        {
            strcat(outputArray[m], ");");
            m++;
            strcat(outputArray[m], hash);
            m++;
            break;
        }
        else if(strcmp(pointer[j+1].nodeType, "variable")!=0 )
        {
            strcat(outputArray[m], ",");
            m++;
            
        }
    }
    
    strcat(outputArray[m], hash);
    m++;
    
    strcat(outputArray[m], "input Clk, Rst, Start;");
    m++;
    strcat(outputArray[m], hash);
    m++;
    
    strcat(outputArray[m], "output reg Done;");
    m++;
    strcat(outputArray[m], hash);
    m++;
    strcat(outputArray[m], "parameter");
    m++;
    
    
    for (int j=0 ; j<operationGraph[opcount-1]->alapTime+1; j++) {
     
        if(j==0)
        sprintf(outputArray[m++], "S_Wait = %d,", j);
        else if(j==operationGraph[opcount-1]->alapTime)
        sprintf(outputArray[m++], "S_Final = %d;", j);
        else
        sprintf(outputArray[m++], "S%d = %d,", j,j);
    
    }
    
    strcat(outputArray[m], hash);
    m++;
    
    
    int j=0;
    
    
    //Variable initialization
    for (j=0; j<k; j++)
    {
        
        int inputSize=0;
        char type[10];
        
        if(strcmp(inputArray[j], "input") == 0 || strcmp(inputArray[j], "output") == 0 || strcmp(inputArray[j], "variable") == 0)
        {
            while(strcmp(inputArray[j], hash)!=0)
            {
                if(strcmp(inputArray[j], "input") == 0 || strcmp(inputArray[j], "output") == 0 || strcmp(inputArray[j], "variable") == 0)
                {
                    
                    if(strcmp(inputArray[j], "variable") != 0 && strcmp(inputArray[j], "output") != 0)
                        strcpy(outputArray[m], inputArray[j]);
                    else if (strcmp(inputArray[j], "output")== 0)
                    {
                        strcpy(outputArray[m++], "output");
                        strcpy(outputArray[m], "reg");
                    }
                    else
                        strcpy(outputArray[m], "reg");
                    
                    m++;
                    j= j+2;
                    
                    for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                    {
                        if(strcmp(pointer[l].name, inputArray[j]) == 0)
                        {
                            inputSize= pointer[l].size;
                            strcpy(type, pointer[l].dataType);
                            break;
                        }
                    }
                    
                    if(strcmp(type, "Int") == 0)
                    {
                        strcat(outputArray[m], "signed");
                        m++;
                    }
                    
                    
                    char res[10];
                    int zero = 0;
                    
                    if(inputSize-1>0)
                    {
                        sprintf(res, "[%d:%d]", inputSize-1,zero);
                        strcat(outputArray[m], res);
                        m++;
                    }
                    
                }
                
                strcpy(outputArray[m], inputArray[j]);
                
                j++;
                m++;
                
                if (strcmp(inputArray[j], hash)!=0)
                    strcat(outputArray[m-1], ",");
                else
                {
                    strcat(outputArray[m-1], ";");
                    strcpy(outputArray[m], hash);
                    m++;
                    break;
                }
                
            }
            
        }
        else
            break;
    }
    
    
    
    int maxlatency = 0;
    int maxAlap = 0;
    
    for (int s = 1; s<opcount-1; s++) {
        
        if (operationGraph[s]->alapTime>maxAlap) {
            
            maxAlap= operationGraph[s]->alapTime;
            maxlatency= operationGraph[s]->latency;
        }
    }
    
    strcat(outputArray[m], "reg");
    m++;
    
    

    
    int power = ceil(log10(operationGraph[opcount-1]->alapTime+1)/log10(2));
    
    sprintf(outputArray[m], "[%d:%d] State;", power-1,0);
    m++;
    strcat(outputArray[m], hash);
    m++;
    strcat(outputArray[m], hash);
    m++;
    
    
    //High level state machine
    strcat(outputArray[m], "always @(posedge Clk) begin");
    m++;
    strcat(outputArray[m], hash);
    m++;
    
    strcat(outputArray[m], "if (Rst==1) begin");
    m++;

    strcat(outputArray[m], hash);
    m++;
    
    strcat(outputArray[m], "State <= S_Wait;");
    m++;
    
    strcat(outputArray[m], hash);
    m++;
    
    
    for (int p=0; strcmp(pointer[p].name, hash)!=0; p++) {
        
        if (strcmp(pointer[p].nodeType, "input")!=0) {
            
            strcpy(outputArray[m++], pointer[p].name);
            strcpy(outputArray[m++], "<= 0;");
            strcpy(outputArray[m++], hash);
        }
    }

    strcpy(outputArray[m++], "Done <=0;");
    strcat(outputArray[m++], hash);
    strcpy(outputArray[m++], "end");
    strcat(outputArray[m++], hash);
    
    strcpy(outputArray[m++], "else begin");
    strcat(outputArray[m++], hash);
    
    strcpy(outputArray[m++], "case(State)");
    strcat(outputArray[m++], hash);
    
    
    for (int p=0; p<operationGraph[opcount-1]->alapTime+1; p++) {
        
        if (p==0) {
            
            strcpy(outputArray[m++], "S_Wait: begin");
            strcat(outputArray[m++], hash);
            strcpy(outputArray[m++], "Done <=0;");
            strcat(outputArray[m++], hash);
            if(opcount>1)
            {
            strcpy(outputArray[m++], "if (Start==1)");
            strcat(outputArray[m++], hash);
            strcpy(outputArray[m++], "State <= S1;");
            strcat(outputArray[m++], hash);
            strcpy(outputArray[m++], "else");
            strcat(outputArray[m++], hash);
            strcpy(outputArray[m++], "State <= S_Wait;");
            strcat(outputArray[m++], hash);
            strcpy(outputArray[m++], "end");
            strcat(outputArray[m++], hash);
            }
        }
        else if(p!=operationGraph[opcount-1]->alapTime)
        {
         sprintf(outputArray[m++], "S%d: begin", p);
         strcat(outputArray[m++], hash);
            for (int i=1; i<opcount-1; i++) {
                
                if (operationGraph[i]->asapTime==p) {
                    
                    int v =0;
                    for (v=0; strcmp(pointer[v].name, hash)!=0; v++) {
                        
                        if(strcmp(pointer[v].name, operationGraph[i]->output) == 0)
                        {
                            if(strcmp(pointer[v].nodeType,"input")==0)
                            {
                                flag = 1;
                                cout<<"result of an operation cannot be assigned to an input element. Verilog file creation failed"<<endl;
                                return 0;
                            }
                            
                            strcpy(outputArray[m++], operationGraph[i]->output);
                            strcpy(outputArray[m++], "<=");
                            
                            break;
                        }

                    }
                    
                    int operatorIndex = m+1;
                    
                    if (strcmp(pointer[v].name, hash)==0) {
                        
                        flag = 1;
                        cout<<operationGraph[i]->output<<" - is uninitialized. Verilog file creation failed."<<endl;
                        return 0;
                    }
                    
                    int isSigned = 0;
                    for (v=0;strcmp(operationGraph[i]->input[v], hash)!=0; v++) {
                        
                        int h=0;
                        
                        for ( h=0; strcmp(pointer[h].name, hash)!=0; h++) {
                            
                            if(strcmp(pointer[h].name, operationGraph[i]->input[v]) == 0)
                            {
                                if(strcmp(pointer[h].nodeType,"output")==0)
                                {
                                    flag = 1;
                                    cout<<"Output element cannot be used as an input for an operation. Verilog file creation failed"<<endl;
                                    return 0;
                                }
                                
                                if(strcmp(pointer[h].dataType,"Int")!=0 && isSigned==1)
                                {
                                    //isSigned=1;
                                    sprintf(outputArray[m], "$signed(%s)", operationGraph[i]->input[v]);
                                    m=m+2;
                                }
                                else if(strcmp(pointer[h].dataType,"Int")==0 )
                                {
                                    isSigned=1;
                                    strcpy(outputArray[m],operationGraph[i]->input[v]);
                                    m=m+2;
                                }
                                else
                                {
                                    strcpy(outputArray[m],operationGraph[i]->input[v]);
                                    m=m+2;
                                }
                                
                                break;
                            }
                            
                        }
                        
                        if (strcmp(pointer[h].name, hash)==0) {
                            
                            flag = 1;
                            cout<<operationGraph[i]->input[v]<<" - is uninitialized. Verilog file creation failed."<<endl;
                            return 0;
                        }
                        
                        
                    }
                    
                    m=m-2;
                    strcat(outputArray[m++], ";");
                    strcpy(outputArray[m++], hash);
                    strcpy(outputArray[operatorIndex], operationGraph[i]->type);
                    
                    
                    if (strcmp(operationGraph[i]->type,"?")==0) {
                       strcpy(outputArray[operatorIndex+2], ":");
                        
                    }
                    
                    
                }
                
            }
            if(p!=operationGraph[opcount-1]->alapTime-1)
            sprintf(outputArray[m++], "State <= S%d;", p+1);
            else
            strcpy(outputArray[m++], "State <= S_Final;");
            
            strcpy(outputArray[m++], hash);
            strcpy(outputArray[m++], "end");
            strcpy(outputArray[m++], hash);
            
            
        }
        
        else
        {
         strcpy(outputArray[m++], "S_Final: begin");
         strcpy(outputArray[m++], hash);
         strcpy(outputArray[m++], "Done <= 1;");
         strcpy(outputArray[m++], hash);
         strcpy(outputArray[m++], "State <= S_Wait;");
         strcpy(outputArray[m++], hash);
         strcpy(outputArray[m++], "end");
         strcpy(outputArray[m++], hash);
         strcpy(outputArray[m++], "endcase");
         strcpy(outputArray[m++], hash);
         strcpy(outputArray[m++], "end");
         strcpy(outputArray[m++], hash);
         strcpy(outputArray[m++], "end");
         strcpy(outputArray[m++], hash);
         strcpy(outputArray[m++], "endmodule");
         strcpy(outputArray[m++], hash);
            
        }
        
    }
    
    
    WriteVerilog(outputArray, m, argv);
    
    return 0;
}



int CreateStructure(char inputArray[1000][1000], int &k, Node * pointer)
{
    int size = 0;
    char datatype[5];
    
    int index = 0;
    
    int i=0;
    
    for (i=0; strcmp(inputArray[i+1], "=") != 0;i++)
    {
        if(strcmp(inputArray[i], "input") == 0)
        {
            i++;
            
            GetSizeType(inputArray[i], &size, datatype);
            
            if (flag)
                return 0;
            
            i++;
            
            while (strcmp(inputArray[i], hash)!=0)
            {
                
                for (int d=0; d<index; d++) {
                    if(strcmp(inputArray[i],pointer[d].name)==0)
                    {
                        flag=1;
                        cout <<inputArray[i]<<" is initialized more than once. Verilog file creation failed"<<endl;
                        return 0;
                    }
                }
                
                
                strcpy(pointer[index].dataType, datatype);
                strcpy(pointer[index].name, inputArray[i]);
                pointer[index].size = size;
                pointer[index].delay = 0;
                strcpy(pointer[index].nodeType, "input");
                index++;
                i++;
            }
        }
        
        else if(strcmp(inputArray[i], "output") == 0)
        {
            i++;
            
            GetSizeType(inputArray[i], &size, datatype);
            
            if (flag)
                return 0;
            
            i++;
            
            while (strcmp(inputArray[i], hash)!=0)
            {
                for (int d=0; d<index; d++) {
                    if(strcmp(inputArray[i],pointer[d].name)==0)
                    {
                        flag=1;
                        cout <<inputArray[i]<<" - is initialized more than once. Verilog file creation failed"<<endl;
                        return 0;
                    }
                }
                
                
                strcpy(pointer[index].dataType, datatype);
                strcpy(pointer[index].name, inputArray[i]);
                pointer[index].size = size;
                pointer[index].delay = 0;
                strcpy(pointer[index].nodeType, "output");
                index++;
                i++;
            }
            
            
        }
        
        else if(strcmp(inputArray[i], "variable") == 0)
        {
            i++;
            
            GetSizeType(inputArray[i], &size, datatype);
            
            if (flag)
                return 0;
            
            i++;
            
            while (strcmp(inputArray[i], hash)!=0)
            {
                for (int d=0; d<index; d++) {
                    if(strcmp(inputArray[i],pointer[d].name)==0)
                    {
                        flag=1;
                        cout <<inputArray[i]<<" - is initialized more than once. Verilog file creation failed"<<endl;
                        return 0;
                    }
                }
                
                strcpy(pointer[index].dataType, datatype);
                strcpy(pointer[index].name, inputArray[i]);
                pointer[index].size = size;
                pointer[index].delay = 0;
                strcpy(pointer[index].nodeType, "variable");
                index++;
                i++;
            }
        }
        
        else
        {
            flag = 1;
            cout<<inputArray[i]<<" - unknown Data Input type. Verilog File creation failed"<<endl;
            return 0;
        }
    }
    
    
    for (int m = i; m<k; m++) {
        
        if(strcmp(inputArray[m], "input") == 0 || strcmp(inputArray[m], "output") == 0 || strcmp(inputArray[m], "variable") == 0)
        {
            flag =1;
            cout<< "Signal declaration after component initialization is not allowed. Verilog file creation failed."<<endl;
            return 0;
        }
        
        else if(strcmp(inputArray[m+1], "=")!=0)
        {
            flag =1;
            cout<<inputArray[m+1] <<" <- Syntax Error. Verilog file creation failed."<<endl;
            return 0;
        }
        else
        {
            while (strcmp(inputArray[m], hash)!=0) {

                m++;
            }
        }
        
    }
    
    strcpy(pointer[index].name, hash);
    
    
    return 0;
}


//Assignment - 3 change
int CreateOperationStructure(char inputArray[1000][1000], int &k, OperationNode * operation, Node * pointer)
{
    int index = 0;
    int count = 1;
    
    for (int i=0; i<k; i++) {
        
        
        if (strcmp(inputArray[i+1],"=")== 0) {
            
            operation[index].id=count++;
            
            for (int v=0; strcmp(pointer[v].name, hash)!=0; v++) {
                
                if (strcmp(pointer[v].name, inputArray[i])==0) {
                    
                    if(strcmp(pointer[v].nodeType, "input")==0)
                    {
                        cout<<"Result of an operation cannot be assigned to an input element. Verilog file creation failed ";
                        flag=1;
                        return 0;
                    }
                    else
                        break;
                }
            }
            strcpy(operation[index].output,inputArray[i]);
            
            //   if(strcmp(inputArray[i+3],hash)!= 0)
            strcpy(operation[index].type, inputArray[i+3]);
            //   else
            //    strcpy(operation[index].type, inputArray[i+1]);
            
            if(strcmp(inputArray[i+3], "*")==0)
                operation[index].latency = 2;
            else if(strcmp(inputArray[i+3], "/")==0 || strcmp(inputArray[i+3], "%")==0)
                operation[index].latency = 3;
            else if(strcmp(inputArray[i+3], "+")==0 || strcmp(inputArray[i+3], "-")==0 || strcmp(inputArray[i+3], ">")==0 || strcmp(inputArray[i+3], "<")==0||strcmp(inputArray[i+3], "==")==0 || strcmp(inputArray[i+3], "?")==0 || strcmp(inputArray[i+3], ">>")==0 || strcmp(inputArray[i+3], "<<")==0)
                operation[index].latency = 1;
            else
            {
                cout<<" Operation is not supported. Verilog file creation failed.";
                flag=1;
                return 0;
            }
            
            i=i+2;
            int j=0;
            
            
            while (strcmp(inputArray[i], hash)!=0)
            {
                for (int v=0; strcmp(pointer[v].name, hash)!=0; v++) {
                    
                    if (strcmp(pointer[v].name, inputArray[i])==0) {
                        
                        if(strcmp(pointer[v].nodeType, "output")==0)
                        {
                            cout<<"Output element cannot be used as an input for an operation. Verilog file creation failed";
                            flag=1;
                            return 0;
                        }
                        else
                            break;
                    }
                }
                
                strcpy(operation[index].input[j],inputArray[i]);
                if (strcmp(inputArray[i+1],hash)== 0)
                    break;
                else
                {
                    i=i+2;
                    j++;
                }
            }
            
            j++;
            
            strcpy(operation[index].input[j],hash);
            index++;
            
        }
        
    }
    strcpy(operation[index].type, hash);
    index++;
    
    int v0Index = index;
    strcpy(operation[index].type, "V0");
    operation[index].id=0;
    index++;
    
    int vnIndex = index;
    strcpy(operation[index].type, "Vn");
    operation[index].id=count++;
    index++;
    
    opcount = count;
    
    for (int i=0; strcmp(operation[i].type,hash)!= 0; i++) {
        
        cout<<"Id -> "<<operation[i].id<<endl;
        cout<<"Type -> "<<operation[i].type<<endl;
        cout<<"Latency -> "<<operation[i].latency<<endl;
        cout<<"Alaptime -> "<<operation[i].alapTime<<endl;
        cout<<"Asaptime -> "<<operation[i].asapTime<<endl;
        cout<<"Force -> "<<operation[i].force<<endl;
        cout<<"Output -> "<<operation[i].output<<endl;
        cout<<"Inputs"<<endl;
        
        for (int k=0; strcmp(operation[i].input[k],hash)!=0; k++) {
            cout<<operation[i].input[k]<<endl;
        }
        
        cout <<endl;
        
    }
    
    
    for(int i=0;i<opcount;i++)
    {
        operationGraph.push_back(NULL);
        
    }
    
    
    CreateOperationGraph(operation, v0Index, vnIndex);
    
    return 0;
}


//Assignment - 3 change
int CreateOperationGraph(OperationNode * operation, int v0Index, int vnIndex)
{
    int flag = 0;
    int outFlag = 0;
    
    for (int i = 0 ; strcmp(operation[i].type,hash)!= 0; i++) {
        
        flag = 0;
        outFlag = 0;
        
        //TODO - need to check the below if condition
        //  if (strcmp(operation[i].type,"?")!= 0)
        //  {
        // Loop - check whether current node's input is coming from any another node
        for (int j=0; strcmp(operation[i].input[j],hash)!= 0; j++) {
            
            for (int k = 0; strcmp(operation[k].type,hash)!= 0; k++) {
                
                if (strcmp(operation[i].input[j], operation[k].output) == 0) {
                    
                    flag = 1;
                    
                    AddOperationEdge(operation[i], operation[k]);
                    
                    //break;
                    
                }
                
            }
            
            //if (flag) {
            //    break;
            //}
            
        }
        
        if (!flag) {
            AddOperationEdge(operation[i], operation[v0Index]);
        }
        
        
        
        // Loop - check whether current node's output is going as an input to another node
        
        for ( int j = 0; strcmp(operation[j].type,hash)!= 0; j++)
        {
            for (int k = 0; strcmp(operation[j].input[k],hash)!= 0; k++) {
                
                if (strcmp(operation[j].input[k], operation[i].output) == 0) {
                    
                    outFlag = 1;
                    break;
                    
                }
                
            }
            
            if (outFlag) {
                break;
            }
        }
        
        if (!outFlag) {
            
            AddOperationEdge(operation[vnIndex], operation[i]);
        }
    }
    
    // }
    
    for (int i=0; strcmp(operation[i].type,hash)!=0; i++)
    {
        AddOperationEdge(operation[i], operation[i]);
        
    }
    
    AddOperationEdge(operation[v0Index], operation[v0Index]);
    
    AddOperationEdge(operation[vnIndex], operation[vnIndex]);
    
    return 0;
}


int GetSizeType(char input[10], int *size, char datatype[5])
{
    if(strcmp(input, "Int1") == 0)
    {
        *size = 1;
        strcpy(datatype, "Int");
    }
    
    else if(strcmp(input, "Int8") == 0)
    {
        *size = 8;
        strcpy(datatype, "Int");
    }
    else if(strcmp(input, "Int16") == 0)
    {
        *size = 16;
        strcpy(datatype, "Int");
    }
    else if(strcmp(input, "Int32") == 0)
    {
        *size = 32;
        strcpy(datatype, "Int");
    }
    else if(strcmp(input, "Int64") == 0)
    {
        *size = 64;
        strcpy(datatype, "Int");
    }
    else if(strcmp(input, "UInt1") == 0)
    {
        *size = 1;
        strcpy(datatype, "UInt");
    }
    else if(strcmp(input, "UInt8") == 0)
    {
        *size = 8;
        strcpy(datatype, "UInt");
    }
    else if(strcmp(input, "UInt16") == 0)
    {
        *size = 16;
        strcpy(datatype, "UInt");
    }
    else if(strcmp(input, "UInt32") == 0)
    {
        *size = 32;
        strcpy(datatype, "UInt");
    }
    else if(strcmp(input, "UInt64") == 0)
    {
        *size = 64;
        strcpy(datatype, "UInt");
    }
    
    else
    {
        flag = 1;
        cout<<input<<" - datatype is not supported. Verilog file creation failed"<<endl;
        
    }
    
    return 0;
}




// Assignment 3 change
int AddOperationEdge(OperationNode &src, OperationNode dest)
{
    
    struct OperationNode *temp=(struct OperationNode *)malloc(sizeof(struct OperationNode));
    
    
    temp->id=src.id;
    
    temp->latency=src.latency;
    
    strcpy(temp->type, src.type);
    
    if(strcmp(src.type, "Vn")!=0 && strcmp(src.type, "V0")!=0 )
    {
        strcpy(temp->output,src.output);
        
        temp->alapTime = src.alapTime;
        
        temp->asapTime = src.asapTime;
        
        temp->force = src.force;
        
        int i =0;
        for (i = 0; strcmp(src.input[i], hash)!=0; i++) {
            
            strcpy(temp->input[i],src.input[i]);
        }
        
        strcpy(temp->input[i],src.input[i]);
    }
    
    
    
    temp->next=operationGraph[dest.id];
    operationGraph[dest.id]=temp;
    
    
    
    return 0;
}




// Assignement - 3 change
void OpPrint()
{
    struct OperationNode *temp;
    for(int i=0;i<opcount;i++)
    {
        temp=operationGraph[i];
        while(temp!=NULL)
        {
            temp=temp->next;
        }
        
        
    }
    
    for(int i=0;i<opcount;i++)
    {
        temp=operationGraph[i];
        printf("node %d ",i);
        while(temp!=NULL)
        {
            printf(" -> %d %s %d %d %d %s",temp->id,temp->type,temp->latency,temp->asapTime,temp->alapTime,temp->output);
            cout<<endl;
            temp=temp->next;
        }
        printf("\n");
    }
    
    
}


void InitiateAsapCalculation()
{
    struct OperationNode *temp;
    struct OperationNode *temp2;
    
    CalculateAsap(0,1,0);
    
    
    for(int i=0;i<opcount;i++)
    {
        temp=operationGraph[i];
        
        while (temp!=NULL) {
            
            temp2=operationGraph[temp->id];
            if(temp->asapTime<temp2->asapTime)
                temp->asapTime = temp2->asapTime;
            temp=temp->next;
        }
        
        
        
    }
}


// assignement - 3 change
int CalculateAsap(int index, int time, int exe)
{
    struct OperationNode * temp;
    struct OperationNode * temp2;
    
    temp = operationGraph[index];
    
    if(index != 0)
    {
        temp=temp->next;
    }
    
    
    while(temp!=NULL)
    {
        if(temp->asapTime<time + exe)
            temp->asapTime=time + exe;
        
        temp2 = operationGraph[temp->id];
        
        if (temp2->asapTime < temp->asapTime)
            temp2->asapTime = temp->asapTime;
        else
            temp->asapTime = temp2->asapTime;
        
        temp=temp->next;
    }
    
    temp = operationGraph[index];
    temp = temp->next;
    // time++;
    
    while(temp!=NULL)
    {
        CalculateAsap(temp->id, temp->asapTime, temp->latency);
        temp= temp->next;
    }
    
    return 0;
}

// assignement - 3 change
int InitiateAlapCalculation(int latency)
{
    
    struct OperationNode * temp;
    temp=operationGraph[opcount-1];
    temp->alapTime=latency+1;
    
    //Added so as to get the correct alaptime for parent nodes of Vn
    // temp->latency=1;
    
    if(latency+1<temp->asapTime)
    {
        flag=1;
        cout<<"Alap scheduling Failed. Give a Higher latency value";
        return 0;
    }
    CalculateAlap(temp->id, temp->alapTime);
    
    
    return 0;
}

// assignement - 3 change
int CalculateAlap(int id, int alapTime)
{
    struct OperationNode * temp;
    struct OperationNode * temp2;
    int successorflag = 0;
    
    
    for (int i=0; i<opcount-1; i++) {
        temp = operationGraph[i];
        
        
        temp= temp->next;
        while (temp!=NULL) {
            if(temp->id==id)
            {
                temp->alapTime =alapTime;
                break;
            }
            temp=temp->next;
            
        }
    }
    
    for (int i = 0; i<opcount-1; i++) {
        
        temp = operationGraph[i];
        
        if (temp->alapTime==0 && strcmp(temp->type, "V0")!=0) {
            
            
            temp2= temp->next;
            int minValue = 10000;
            successorflag = 0;
            
            while(temp2!=NULL)
            {
                if(temp2->alapTime==0)
                {
                    successorflag = 1;
                    break;
                }
                
                
                
                else if(temp2->alapTime - temp2->latency< minValue)
                {
                    minValue =temp2->alapTime - temp->latency;
                    //if(strcmp(temp2->type, "Vn")==0)
                      //  minValue--;
                }
                
                temp2 = temp2->next;
            }
            
            if(!successorflag)
            {
                temp->alapTime = minValue;
             
                CalculateAlap(temp->id, temp->alapTime);
            }
            
        }
    }
    
    return 0;
}


//Assignement-3 change
int InitiateFdsCalculation()
{
    struct OperationNode *temp;
    struct OperationNode *temp2;
    struct OperationNode *temp3;
    struct OperationNode *source;
    temp=operationGraph[opcount-1];
    int predecessorFlag=0;
   // int successorFlag=0;
    
    double minforce = 100;
    int minRow=0;
    int minColumn=0;
    
    
    double DGMUL[temp->alapTime];
    double DGADD[temp->alapTime];
    double DGDIV[temp->alapTime];
    double DGLOG[temp->alapTime];
    
    double DG[opcount][temp->alapTime];
    double FDG[opcount][temp->alapTime];
    
    for (int i=0; i<opcount-1; i++)
    {
        for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
        {
            FDG[i][j]=0;
        }
    }

    
    for (int k = 1; k<opcount-1; k++) {
    
    
    for (int i=1; i<opcount-1; i++)
    {
        for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
        {
            int start = operationGraph[i]->asapTime-1;
            int stop = operationGraph[i]->alapTime-1;
            
            if (j>=start && j<=stop)
            {
                DG[i][j]=  (double)(1)/(double)(stop-start+1);
                
            }
            else
            {
                DG[i][j]=0.00;
            }
            
        }
    }
    
    for (int i=1; i<opcount-1; i++)
    {
        for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
        {
            cout<<DG[i][j]<<" ";
        }
        cout<<endl;
    }
    
    cout<<endl;
   
    
    for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
    {
        DGADD[j]=0;
        DGMUL[j]=0;
        DGDIV[j]=0;
        DGLOG[j]=0;
    }
    
    for (int i=1; i<opcount-1; i++)
    {
        if (strcmp(operationGraph[i]->type,"*")==0 )
        {
            for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
            {
                DGMUL[j]=DGMUL[j]+DG[i][j];
            }
        }
        else
            if ( (strcmp(operationGraph[i]->type,"+")==0) || (strcmp(operationGraph[i]->type,"-")==0) )
            {
                for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
                {
                    DGADD[j]=DGADD[j]+DG[i][j];
                }
            }
            else
                if ((strcmp(operationGraph[i]->type,"/")==0) || (strcmp(operationGraph[i]->type,"%")==0))
                {
                    for ( int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
                    {
                        DGDIV[j]=DGDIV[j]+DG[i][j];
                    }
                }
                else
                {
                    for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
                    {
                        DGLOG[j]=DGLOG[j]+DG[i][j];
                    }
                    
                }
        
        
    }
        
        cout<<"DGMUL ";
         for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
         {
         cout<<DGMUL[j]<<" ";
         }
         
         cout<<endl;
         
         cout<<"DGADD ";
         for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
         {
         cout<<DGADD[j]<<" ";
         }
         
         cout<<endl;
         
         cout<<"DGDIV ";
         for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
         {
         cout<<DGDIV[j]<<" ";
         }
         
         cout<<endl;
         
         cout<<"DGLOG ";
         for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
         {
         cout<<DGLOG[j]<<" ";
         }
         
         cout<<endl;
        cout<<endl;
        
        //Filling FDG table
        for (int i=1; i<opcount-1; i++) {
        
            
            for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++) {
                
                
                temp=operationGraph[i];
                
                double selfForce = 0;
                double precessorForce=0;
                double successorForce=0;
               // float check =0;
               // float chk1 = 0;
               // float chk2 = 0;
                
                if(j>=temp->asapTime-1 && j<=temp->alapTime-1 && temp->alapTime!=temp->asapTime)
                {
                //Self force Calculation
                if(strcmp(temp->type,"*")==0)
                {
                    
                    for (int l =0; l<operationGraph[opcount-1]->alapTime-1; l++) {
                        
                        if(j==l)
                           {
                            selfForce= selfForce+DGMUL[l]*(1-DG[i][l]);
                            
                           }
                            else
                            {
                               // chk1=DGMUL[l];
                               // chk2=DG[i][l];
                               // check=DGMUL[l]*DG[i][l];
                            selfForce= selfForce-(DGMUL[l]*DG[i][l]);
                            }
                    }
                    
                }
                
                else if(strcmp(temp->type,"+")==0 || strcmp(temp->type,"-")==0)
                {
                    
                    for (int l =0; l<operationGraph[opcount-1]->alapTime-1; l++) {
                        
                        if(j==l)
                            selfForce= selfForce+DGADD[l]*(1-DG[i][l]);
                        else
                            selfForce= selfForce+DGADD[l]*(0-DG[i][l]);
                    }
                    
                }
                
                else if(strcmp(temp->type,"/")==0 || strcmp(temp->type,"%")==0)
                {
                    
                    for (int l =0; l<operationGraph[opcount-1]->alapTime-1; l++) {
                        
                        if(j==l)
                            selfForce= selfForce+DGDIV[l]*(1-DG[i][l]);
                        else
                            selfForce= selfForce+DGDIV[l]*(0-DG[i][l]);
                    }
                    
                }
                
                else
                {
                    
                    for (int l =0; l<operationGraph[opcount-1]->alapTime-1; l++) {
                        
                        if(j==l)
                            selfForce= selfForce+DGLOG[l]*(1-DG[i][l]);
                        else
                            selfForce= selfForce+DGLOG[l]*(0-DG[i][l]);
                    }
                    
                }
                
                    //predecessor force calculation
                    for (int m=1; m<opcount-1; m++) {
                        
                        predecessorFlag=0;
                        
                        source =operationGraph[m];
                        temp2= source->next;
                        
                        
                        while (temp2!=NULL) {
                            if (temp2->id==temp->id) {
                                
                                predecessorFlag=1;
                                break;
                            }
                            temp2=temp2->next;
                        }
                        
                        if(predecessorFlag)
                        {
                            
                            if (source->asapTime!=source->alapTime) {
                                
                                if (j==source->asapTime) {
                                    
                                    if (strcmp(source->type,"*")==0) {
                                        
                                    
                                    for (int n=0; n<operationGraph[opcount-1]->alapTime-1; n++) {
                
                                        if (n==j-1)
                                            precessorForce = precessorForce +DGMUL[n]*(1-DG[source->id][n]);
                                            else
                                            precessorForce = precessorForce +DGMUL[n]*(0-DG[source->id][n]);
                                        
                                   
                                    }
                                    }
                                    
                                    else if (strcmp(source->type,"+")==0 || strcmp(source->type,"-")==0) {
                                        
                                        
                                        for (int n=0; n<operationGraph[opcount-1]->alapTime-1; n++) {
                                            
                                            if (n==j-1)
                                                precessorForce = precessorForce +DGADD[n]*(1-DG[source->id][n]);
                                            else
                                                precessorForce = precessorForce +DGADD[n]*(0-DG[source->id][n]);
                                            
                                            
                                        }
                                    }
                                    
                                    else if (strcmp(source->type,"/")==0 || strcmp(source->type,"%")==0) {
                                        
                                        
                                        for (int n=0; n<operationGraph[opcount-1]->alapTime-1; n++) {
                                            
                                            if (n==j-1)
                                                precessorForce = precessorForce +DGDIV[n]*(1-DG[source->id][n]);
                                            else
                                                precessorForce = precessorForce +DGDIV[n]*(0-DG[source->id][n]);
                                            
                                            
                                        }
                                    }
                                    
                                    else {
                                        
                                        
                                        for (int n=0; n<operationGraph[opcount-1]->alapTime-1; n++) {
                                            
                                            if (n==j-1)
                                                precessorForce = precessorForce +DGLOG[n]*(1-DG[source->id][n]);
                                            else
                                                precessorForce = precessorForce +DGLOG[n]*(0-DG[source->id][n]);
                                            
                                            
                                        }
                                    }
                                    
                                    
                                }
                                
                            }
                            
                        }
                        
                    }
                
                //successor force calculation
                temp3= temp->next;
                
                while (temp3!=NULL && strcmp(temp3->type,"Vn")!=0) {
                    
                     if (operationGraph[temp3->id]->asapTime!=operationGraph[temp3->id]->alapTime) {
                     
                         if (j==operationGraph[temp3->id]->alapTime-2) {
                             
                             if (strcmp(operationGraph[temp3->id]->type,"*")==0) {
                                 
                                 
                                 for (int n=0; n<operationGraph[opcount-1]->alapTime-1; n++) {
                                     
                                     if (j==n-1)
                                         successorForce = successorForce +DGMUL[n]*(1-DG[temp3->id][n]);
                                     else
                                         successorForce = successorForce +DGMUL[n]*(0-DG[temp3->id][n]);
                                     
                                     
                                 }
                             }
                             
                             else if (strcmp(operationGraph[temp3->id]->type,"+")==0||strcmp(operationGraph[temp3->id]->type,"-")==0) {
                                 
                                 for (int n=0; n<operationGraph[opcount-1]->alapTime-1; n++) {
                                     
                                     if (j==n-1)
                                         successorForce = successorForce +DGADD[n]*(1-DG[temp3->id][n]);
                                     else
                                         successorForce = successorForce +DGADD[n]*(0-DG[temp3->id][n]);
                                     
                                 }
                             }
                             
                             else if (strcmp(operationGraph[temp3->id]->type,"/")==0||strcmp(operationGraph[temp3->id]->type,"%")==0) {
                                 
                                 for (int n=0; n<operationGraph[opcount-1]->alapTime-1; n++) {
                                     
                                     if (j==n-1)
                                         successorForce = successorForce +DGDIV[n]*(1-DG[temp3->id][n]);
                                     else
                                         successorForce = successorForce +DGDIV[n]*(0-DG[temp3->id][n]);
                                     
                                 }
                             }
                             
                             else {
                                 
                                 for (int n=0; n<operationGraph[opcount-1]->alapTime-1; n++) {
                                     
                                     if (j==n-1)
                                         successorForce = successorForce +DGLOG[n]*(1-DG[temp3->id][n]);
                                     else
                                         successorForce = successorForce +DGLOG[n]*(0-DG[temp3->id][n]);
                                     
                                     
                                 }
                             }
                             
                             
                         }
                     
                     }
                    temp3=temp3->next;
               
               }
                
            }
                 FDG[i][j]=selfForce+precessorForce+successorForce;
            
        }
        }
        
        
        for (int i=1; i<opcount-1; i++)
        {
            for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
            {
                cout<<FDG[i][j]<<" ";
                    
                
            }
            cout<<endl;
        }
         cout<<endl;
         cout<<endl;
        
    
        // finding smallest value
    
        minforce=100;
        //minRow=1;
        //minColumn=0;
        for (int i=1; i<opcount-1; i++)
        {
            for (int j=0; j<operationGraph[opcount-1]->alapTime-1; j++)
            {
                if (FDG[i][j]<minforce){
                minforce =FDG[i][j];
                minRow= i;
                minColumn=j;
                }
            }
        }
        
        
        //updating graph
        
        temp= operationGraph[minRow];
        temp->alapTime=minColumn+1;
        temp->asapTime=minColumn+1;
        
        
        //Time frame correction of immediate parent nodes in case of an overlap
        struct OperationNode * parent;
        for (int i =1; i<opcount; i++) {
            
            parent = operationGraph[i];
            temp2 =parent->next;
            
            while (temp2!=NULL) {
              
                if(temp2->id==temp->id)
                {
                    break;
                }
                temp2=temp2->next;
            }
            
            if(temp2!=NULL)
            {
                if(parent->alapTime>=temp->asapTime)
                    parent->alapTime=temp->asapTime-parent->latency;
            }
        }
        
        //Time frame correction in case of dependency between the parent nodes - (All ancestor nodes are getting checked here)
        for (int i =1 ; i<opcount; i++) {
            
            parent = operationGraph[i];
            temp2= parent->next;
            
            while (temp2!=NULL) {
                
                if(temp2->id==temp->id)
                {
                    ParentRecursiveCheck(parent->id);
                    break;
                }
                temp2=temp2->next;
            }

            
        }
        
        
        
        
        
        
        //Time frame correction of child nodes in case of an overlap
        struct OperationNode * child;
        
        child=temp->next;
        
        while (child!=NULL) {
            
            if (operationGraph[child->id]->asapTime<=temp->alapTime) {
                
                     operationGraph[child->id]->asapTime=temp->alapTime+temp->latency;
                }
            
            
            child=child->next;
        }
       
        
        //Time frame correction in case of dependency between the child nodes - (All descendent nodes are getting checked here)
        child = temp->next;
        
            while(child!=NULL)
            {
                ChildRecursiveCheck(child->id);
                child=child->next;
            }
        
        
        
    }
    
    
    for (int i = 1 ; i<opcount-1; i++) {
        
        if(operationGraph[i]->asapTime!=operationGraph[i]->alapTime)
        {
            operationGraph[i]->alapTime= operationGraph[i]->asapTime;
            
        }
    }
    
    return 0;
    
}

int ChildRecursiveCheck(int id)
{
    
    struct OperationNode * src;
    struct OperationNode * dst;
    src = operationGraph[id];
    
    if (src->alapTime==src->asapTime) {
        
        dst = src->next;
        while (dst!=NULL && strcmp(dst->type, "Vn")!=0) {
            
            if(dst->asapTime<=src->alapTime)
            {
                operationGraph[dst->id]->asapTime=src->alapTime+src->latency;
                ChildRecursiveCheck(dst->id);
            
            }
            
            dst=dst->next;
        }
    }
    return 0;
}


int ParentRecursiveCheck(int id)
{
    
    struct OperationNode * src;
    struct OperationNode * parent;
    struct OperationNode * temp;
    
    src = operationGraph[id];
    
    if(src->asapTime==src->alapTime)
    {
    for (int i=1; i<opcount; i++) {
        parent = operationGraph[i];
        temp = parent->next;
        
        while (temp!=NULL) {
            
            if (temp->id==src->id) {
                
                if (parent->alapTime>=src->asapTime ) {
                    
                    parent->alapTime = src->asapTime-parent->latency;
                    
                    ParentRecursiveCheck(parent->id);
                    
                    
                }
                
            }
            temp=temp->next;
        
        }
        }
    }
    
    return 0;
}











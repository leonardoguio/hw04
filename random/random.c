// ALEATORIO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INSTRUCTIONS_IN       6 
#define INSTRUCTIONS_OUT      8
#define INSTRUCTIONS_QUANTITY (INSTRUCTIONS_IN + INSTRUCTIONS_OUT)

#define MAX_COST 999999

// INPUT COORDINATES
#define X_MAX  30
#define Y_MAX  10

// INPUT COORDINATES
#define X_IN  0
#define Y_IN  4

// OUTPUT COORDINATES
#define X_OUT 31
#define Y_OUT 4

// CRANE START COORDINATES
#define X_START 15
#define Y_START 4

// INSTRUCTION TYPES
#define INPUT  0
#define OUTPUT 1

int count=0;
int verified_solutions=0;

typedef struct
{
  int type;
  int x;
  int y;
} Instruction;

typedef struct
{
  Instruction ins[INSTRUCTIONS_QUANTITY];
} Sequence;

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
void PrintInstruction(Instruction ins)
{
  printf("[%02d][%s][%02d][%02d]\n", (ins.y*X_MAX)+(ins.x), ins.type%2==0? "INPUT " : "OUTPUT", ins.x, ins.y);
  // if(ins.type == INPUT)
    // printf("Instruction: INPUT  x=%02d y=%02d\n", ins.x, ins.y);
  // else
    // printf("Instruction: OUTPUT x=%02d y=%02d\n", ins.x, ins.y);
}

void PrintOptimal(Instruction array[], int length, int cost)
{
  int i=0;
  for(i=0; i<length; i++)
    printf("%02d,", (array[i].y*X_MAX)+(array[i].x));
  printf(",%d,", cost);
}

void PrintInstructions(Instruction array[], int length)
{
  int i=0;
  for(i=0; i<length; i++)
    PrintInstruction(array[i]);
}

void PrintArray(int array[], int length)
{
  int i=0;
  for(i=0; i<length; i++)
    printf("%02d,", array[i]);
  printf(",");
}

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
int Manhattan(int x_from, int y_from, int x_to, int y_to)
{
  return (abs(x_from-x_to) + abs(y_from-y_to));
}

int Cost(Instruction sequence[])
{
  int cost=0;
  int i=0;
  int i_type = 0;

  int previous_x = X_START;
  int previous_y = Y_START;

  for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
  {
    if(sequence[i].type == INPUT)
    {
      cost += Manhattan(previous_x, previous_y, X_IN, Y_IN);
      // printf("cost=%d\n", cost);
      cost += Manhattan(X_IN, Y_IN, sequence[i].x, sequence[i].y);
      // printf("cost=%d\n", cost);
      previous_x = sequence[i].x;
      previous_y = sequence[i].y;
    }
    else
    {
      cost += Manhattan(previous_x, previous_y, sequence[i].x, sequence[i].y);
      // printf("cost=%d\n", cost);
      cost += Manhattan(sequence[i].x, sequence[i].y, X_OUT, Y_OUT);
      // printf("cost=%d\n", cost);
      previous_x = X_OUT;
      previous_y = Y_OUT;
    }
  }
  cost += Manhattan(previous_x, previous_y, X_START, Y_START);
  return cost;
}

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
void Swap(Instruction* a, Instruction* b)
{
  Instruction temp;
  temp = *a;
  *a = *b;
  *b = temp;
}

void Permutations_recursive(Instruction *array, unsigned int left, unsigned int right, int length, int *lower_cost, Instruction optimalSequence[])
{
  int cost=0;
  if (left == right)
  {
    // printf("%d\n", VerifyInstructions(array));
    if(VerifyInstructions(array))
    {
      cost = Cost(array);
      // PrintInstructions(array, length);
      // printf("count=%d Cost = %d\n", count++, cost);
      if(cost < *lower_cost)
      {
        *lower_cost = cost;
        // printf("sizeof=%d\n", length*sizeof(Instruction));
        memcpy(optimalSequence, array, length*sizeof(Instruction));
        // printf("Lower!\n");
      }
      // printf("\n");
    }
  }
  else
  {
    unsigned int i;
    for (i = left; i <= right; i++)
    {
      Swap(array + left, array + i);
      Permutations_recursive(array, left + 1, right, length, lower_cost, optimalSequence);
      Swap(array + left, array + i);
    }
  }
}

void Permutations(Instruction *array, int length, int *lower_cost, Instruction optimalSequence[])
{
  // Permutations_recursive(array, 0, length - 1, length, lower_cost, optimalSequence);
  int i,j,k;
  int temp;
  int swaps[length];
  
  for(i=0; i<length;i++)
    swaps[i] = i;
  
  srand((time(NULL)+(*lower_cost))*count++);
  for(i=0; i<length*length; i++)
  {
    j=rand()%length;
    k=rand()%length;
    // printf("j=%d, k=%d\n", j, k);
    
    // Swapping process
    temp = swaps[j];
    swaps[j] = swaps[k];
    swaps[k] = temp;
  }
  // PrintArray(swaps, length);
  for(i=0; i<length; i++)
  {
    memcpy(&(optimalSequence[swaps[i]]), &(array[i]), sizeof(Instruction)); 
  }
  *lower_cost = Cost(optimalSequence);
  verified_solutions++;
  //memcpy(optimalSequence, array, length*sizeof(Instruction));
}

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
void CombinationUtil(Instruction array[], Instruction data[], int start, int end, int index, int r, int *lower_cost, Instruction optimalSequence[])
{
  // Current combination is ready to be printed, print it
  if (index == r)
  {
    // printf("Array %d\n", start);
    // for (int j=0; j<r; j++)
    // {
      // printf("Array %d\n", j+1);
      // PrintInstructions(data, r);
      // PrintInstruction(data[j]);
      // printf("%d ", data[j]);
    // }
    Permutations(data, r, lower_cost, optimalSequence);
    return;
  }

  // replace index with all possible elements. The condition
  // "end-i+1 >= r-index" makes sure that including one element
  // at index will make a combination with remaining elements
  // at remaining positions
  for (int i=start; i<=end && end-i+1 >= r-index; i++)
  {
    memcpy(&(data[index]), &(array[i]), sizeof(Instruction));
    // data[index] = array[i];
    CombinationUtil(array, data, i+1, end, index+1, r, lower_cost, optimalSequence);
  }
}

void PrintCombination(Instruction array[], int n, int r, int *lower_cost, Instruction optimalSequence[])
{
  // A temporary array to store all combination one by one
  Instruction data[r];

  // Print all combination using temprary array 'data[]'
  CombinationUtil(array, data, 0, n-1, 0, r, lower_cost, optimalSequence);
}

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
void CreateInstruction(Sequence *seq, int array[])
{
  // int array[] = {0,13,48,34,22,10};
  int i=0;
  
  for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
  {
    seq->ins[i].type = (i < (INSTRUCTIONS_IN)) ? INPUT : OUTPUT;
    seq->ins[i].x = ((array[i]-1)%X_MAX)+1;
    seq->ins[i].y = (int)((array[i]-1)/X_MAX);
    // printf("[%02d][%s][%02d][%02d]\n", array[i], (seq->ins[i].type)%2==0? "INPUT " : "OUTPUT", seq->ins[i].x, seq->ins[i].y);
  }
}

int VerifyInstructions(Sequence *seq)
{
  // int i=0;
  
  // if(seq->ins[0].type == INPUT  && seq->ins[2].type == INPUT  && seq->ins[4].type == INPUT &&
     // seq->ins[1].type == OUTPUT && seq->ins[3].type == OUTPUT && seq->ins[5].type == OUTPUT)
  // {
    // return TRUE;
  // }
  // return FALSE;
  return TRUE;
}

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  int lower_cost;
  Sequence sequence;
  Sequence optimalSequence;
  int array[INSTRUCTIONS_QUANTITY];
  int i=0;
  
  if(argc!=2)
  {
    fprintf(stderr, "usage: hw04 <input file>\n");
    return 0;
  }
  FILE* stream = fopen(argv[1], "r");
  if(stream==NULL)
  {
    fprintf(stderr, "Unable to read file %s\n", argv[1]);
    return 0;
  }

  char line[1024];
  while (fgets(line, 1024, stream))
  {
    verified_solutions=0;
    lower_cost = MAX_COST;
    memset(&sequence, 0x00, sizeof(Sequence));
    memset(&optimalSequence, 0x00, sizeof(Sequence));
    memset(&array, 0x00, INSTRUCTIONS_QUANTITY*sizeof(int));
    // printf("line:%s\n", line);
    // char* tmp = strdup(line);
    // printf("tmp:%s\n", tmp);
    
    for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
    {
      // getfield(tmp, i+1);
      // printf("%s\n", tmp);
      // array[i] = atoi(tmp);
      array[i] = ((line[i*4]-'0')*100+(line[i*4+1]-'0')*10+(line[i*4+2]-'0'));
    }
    // printf("---------------------------------------\n");
    //printf("INPUT:");
    PrintArray(array, INSTRUCTIONS_QUANTITY);
    
    CreateInstruction(&sequence, array);
    // PrintInstructions(sequence.ins, INSTRUCTIONS_QUANTITY);
    Permutations(sequence.ins, INSTRUCTIONS_QUANTITY, &lower_cost, optimalSequence.ins);
    //printf("OUTPUT:");
    PrintOptimal(optimalSequence.ins, INSTRUCTIONS_QUANTITY, lower_cost);
    //PrintInstructions(optimalSequence.ins, INSTRUCTIONS_QUANTITY);
    //printf("Lower Cost: %d\n", lower_cost);
    printf("%d\n", verified_solutions);
    
    //printf("Field 3 would be %s\n", getfield(tmp, 3));
    // NOTE strtok clobbers tmp
    // free(tmp);
  }
  
  // CreateInstruction(&sequence);
  // Permutations(instructions, INSTRUCTIONS_QUANTITY, &lower_cost, optimalSequence);
  // Permutations(sequence.ins, INSTRUCTIONS_QUANTITY, &lower_cost, optimalSequence.ins);
  //PrintCombination(sequence.ins, INSTRUCTIONS_QUANTITY, 6, &lower_cost, optimalSequence.ins);
  // PrintInstructions(sequence.ins, INSTRUCTIONS_QUANTITY);
  // printf("Lower Cost: %d\n", lower_cost);
  return 0;
}

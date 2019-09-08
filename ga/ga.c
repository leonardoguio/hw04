// Genetic Algorithm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//----------------------------------------------------------------------------------------
// Params
//----------------------------------------------------------------------------------------
#define INSTRUCTIONS_IN       6
#define INSTRUCTIONS_OUT      8
#define INSTRUCTIONS_QUANTITY (INSTRUCTIONS_IN + INSTRUCTIONS_OUT)

#define MAX_COST 999999

// Evolutionary Params
#define GENERATION_QTTY      100
#define GENERATION_GROWTH    2
#define PARENTS_QTTY         10
#define POPULATION_SIZE     (PARENTS_QTTY * GENERATION_GROWTH)

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

// BOOLEAN TYPES
#define FALSE  0
#define TRUE   1

// SEARCH TYPE
#define IN  0
#define OUT 1

int count=0;
int verified_solutions=0;

//----------------------------------------------------------------------------------------
// Global Data
//----------------------------------------------------------------------------------------
int population_cost;

void PrintParents();
void PrintPopulation();

//----------------------------------------------------------------------------------------
// Data Structures
//----------------------------------------------------------------------------------------
// Gene
typedef struct
{
  int type;
  int x;
  int y;
} Instruction;

// Individual
typedef struct
{
  Instruction ins[INSTRUCTIONS_QUANTITY];
  int cost;
} Sequence;

Sequence population[POPULATION_SIZE];
Sequence parents[PARENTS_QTTY];

//----------------------------------------------------------------------------------------
// Comparator
//----------------------------------------------------------------------------------------
int Comparator(const void *a, const void *b)
{
  return ((*(Sequence*)a).cost - (*(Sequence*)b).cost);
}

//----------------------------------------------------------------------------------------
// Prints
//----------------------------------------------------------------------------------------
void PrintInstruction(Instruction *ins)
{
  printf("[%02d][%s][%02d][%02d]\n", ((*ins).y*X_MAX)+((*ins).x), (*ins).type%2==0? "INPUT " : "OUTPUT", (*ins).x, (*ins).y);
}

void PrintInstructionOneLine(Instruction *ins)
{
  //printf("[%02d%s:%02d,%02d]", ((*ins).x*7)+((*ins).y-1), (*ins).type%2==0? "I" : "O", (*ins).x, (*ins).y);
  printf("%02d,", ((*ins).y*X_MAX)+((*ins).x));
}

void PrintInstructions(Instruction inss[])
{
  int i=0;
  for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
    PrintInstructionOneLine(&(inss[i]));
  //printf("\n");
}

void PrintSequence(Sequence *seq)
{
    PrintInstructions(seq->ins);
    printf(" Cost:%03d\n", seq->cost);
}

void PrintParents()
{
  int i=0;
  for(i=0; i<PARENTS_QTTY; i++)
  {
    printf("Parent %03d\n", i);
    PrintSequence(&(parents[i]));
  }
}

void PrintPopulation()
{
  int i=0;
  for(i=0; i<POPULATION_SIZE; i++)
  {
    printf("Individual %03d\n", i);
    PrintSequence(&(population[i]));
  }
}

void PrintOptimal(int length)
{
  int i=0, temp_min_cost=MAX_COST, index_min=0;
  
  for(i=0; i<POPULATION_SIZE; i++)
  {
    if(population[i].cost<temp_min_cost)
    {
      temp_min_cost = population[i].cost;
      index_min = i;
    }
  }
  
  for(i=0; i<length; i++)
    printf("%02d,", (population[index_min].ins[i].y*X_MAX)+(population[index_min].ins[i].x));
  printf(",%d,", population[index_min].cost);
}

void PrintIntArray(int array[], int length)
{
  int i=0;
  for(i=0; i<length; i++)
    printf("%03d,", array[i]);
  printf(",");
}

//----------------------------------------------------------------------------------------
// Objetive Function
//----------------------------------------------------------------------------------------
// Auxiliary function
int ManhattanDistance(int x_from, int y_from, int x_to, int y_to)
{
  return (abs(x_from-x_to) + abs(y_from-y_to));
}

// ObjetiveFunction
int ObjetiveFunction(Sequence *s)
{
  int cost=0;
  int i=0;
  int i_type = 0;

  // Crane starting position
  int previous_x = X_START;
  int previous_y = Y_START;

  for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
  {
    if(s->ins[i].type == INPUT)
    {
      cost += ManhattanDistance(previous_x, previous_y, X_IN, Y_IN);
      cost += ManhattanDistance(X_IN, Y_IN, s->ins[i].x, s->ins[i].y);
      previous_x = s->ins[i].x;
      previous_y = s->ins[i].y;
    }
    else
    {
      cost += ManhattanDistance(previous_x, previous_y, s->ins[i].x, s->ins[i].y);
      cost += ManhattanDistance(s->ins[i].x, s->ins[i].y, X_OUT, Y_OUT);
      previous_x = X_OUT;
      previous_y = Y_OUT;
    }
  }
  // Crane ending position (equal starting position)
  cost += ManhattanDistance(previous_x, previous_y, X_START, Y_START);
  s->cost = cost;
  return cost;
}

//----------------------------------------------------------------------------------------
// Population Initialization
//----------------------------------------------------------------------------------------
void InitializePopulation(Instruction inss[])
{
  int i,j;
  int idx_a, idx_b;
  Instruction temp;
  Sequence s;
  s.cost = 0;

  memset(&population, 0x00, POPULATION_SIZE * sizeof(Sequence));

  srand(time(NULL));

  // generate individuals
  for(i=0; i<POPULATION_SIZE; i++)
  {
    // random process to generate individuals based on the random swap of indexes
    for(j=0; j<INSTRUCTIONS_QUANTITY; j++)
    {
      idx_a = rand() % INSTRUCTIONS_QUANTITY;
      idx_b = rand() % INSTRUCTIONS_QUANTITY;
      
      // Swapping process
      memcpy(&temp, &(inss[idx_a]), sizeof(Instruction));
      memcpy(&(inss[idx_a]), &(inss[idx_b]), sizeof(Instruction));
      memcpy(&(inss[idx_b]), &temp, sizeof(Instruction));
    }
    memcpy(s.ins, inss, INSTRUCTIONS_QUANTITY*sizeof(Instruction));
    memcpy(&(population[i]), &s, sizeof(Sequence));
  }
}
//----------------------------------------------------------------------------------------
// Population Evaluation
//----------------------------------------------------------------------------------------
void EvaluatePopulation()
{
  int i;
  
  population_cost = 0;
  for(i=0; i<POPULATION_SIZE; i++)
  {
    population_cost += ObjetiveFunction(&(population[i]));
  }
}

//----------------------------------------------------------------------------------------
// Parents Selection
//----------------------------------------------------------------------------------------
void SelectParents()
{
  int i, j;
  int random_cost;
  int highest_cost, complementar_cost, population_cost_complementar, temp_population_cost, accumulated_cost;
  int (*comp_ptr)(const void*,const void*) = &Comparator;
  
  qsort(population, POPULATION_SIZE, sizeof(Sequence), comp_ptr);
  // printf("After quicksort\n");
  // PrintPopulation();
  
  highest_cost = population[POPULATION_SIZE-1].cost;
  population_cost_complementar = POPULATION_SIZE * (highest_cost+1);
  temp_population_cost = population_cost_complementar - population_cost;
  // printf("highest_cost=%d, population_cost_complementar=%d, temp_population_cost=%d\n", highest_cost, population_cost_complementar, temp_population_cost);
  
  srand(time(NULL));

  // Random selection of individuals. The chance of an individual to be chosen is proportional to its
  // complementary cost. Complementary cost is the highest cost among individuals minus the indivual cost + 1
  // Example: 3 individuals with costs 1, 2, 6.
  // highest_cost = 6
  // population_cost = 9 = 1+2+6
  // population_cost_complementar = 21 = 3*(6+1) = ((6-1)+1)+((6-2)+1)+((6-6)+1) = 6 + 5 + 1
  // temp_population_cost = 12 = 21 - 9 = 6+5+1
  // The individuals with costs 1, 2, 6 have their chances as 6/12, 5/12, 1/12, respectively. It is
  // calculated as the individual complementary cost divided by the sum of the individuals complementary cost,
  // which is 'temp_population_cost'.
  for(i=0; i<PARENTS_QTTY; i++)
  {
    j = 0;
    random_cost = rand() % temp_population_cost;
    accumulated_cost = 0;
    // printf("random_cost=%d temp_population_cost=%d\n", random_cost, temp_population_cost);
    while(accumulated_cost < random_cost)
    {
      complementar_cost = highest_cost - population[j].cost + 1;
      // printf("  complementar_cost=%d\n", complementar_cost);
      accumulated_cost += complementar_cost;
      j++;
    }
    // printf("accumulated_cost=%d j=%d\n", accumulated_cost, j);
    temp_population_cost -= (highest_cost - population[j].cost + 1);
    memcpy(&(parents[i]), &(population[j]), sizeof(Sequence));
    memcpy(&(population[j]), &(population[POPULATION_SIZE-i-1]), sizeof(Sequence));
  }
}

//----------------------------------------------------------------------------------------
// Crossover
//----------------------------------------------------------------------------------------
int ExistsInChild(Instruction *array, Instruction *ins, int idx_x, int idx_y, int type)
{
  int i;
  
  // PrintInstructions(array);
  // PrintInstruction(ins);
  if(type == IN)
  {
    i=idx_x;
    // for(i=idx_x; i<idx_y; i++)
    while(i<idx_y)
    {
      if(array[i].x == (*ins).x && array[i].y == (*ins).y && array[i].type == (*ins).type)
      {
        // printf("Exist\n");
        return 1;
      }
      i++;
    }
  }
  else
  {
    i=0;
    // for(i=0; (i<idx_x || i>=idx_y) && i<INSTRUCTIONS_QUANTITY; i++)
    while(i<idx_x)
    {
      if(array[i].x == (*ins).x && array[i].y == (*ins).y && array[i].type == (*ins).type)
      {
        // printf("Exist\n");
        return 1;
      }
      i++;
    }
    i = idx_y;
    while(i<INSTRUCTIONS_QUANTITY)
    {
      if(array[i].x == (*ins).x && array[i].y == (*ins).y && array[i].type == (*ins).type)
      {
        // printf("Exist\n");
        return 1;
      }
      i++;
    }
  }
  // printf("NON Exist\n");
  return 0;
}

void Crossover()
{
  int i, j, k, l, m;
  int idx_parent_a, idx_parent_b;
  int cross_idx_x, cross_idx_y;
  int temp, found;
  int crossings = (int)(PARENTS_QTTY/2);
  
  memset(&population, 0x00, POPULATION_SIZE * sizeof(Sequence));
  srand(time(NULL));
  
  for(i=0; i<GENERATION_GROWTH; i++)
  {
    for(j=0; j<crossings; j++)
    {
      // Select parents randomly
      idx_parent_a = rand() % PARENTS_QTTY;
      idx_parent_b = rand() % PARENTS_QTTY;
      // idx_parent_a = 2;
      // idx_parent_b = 0;

      if(idx_parent_a == idx_parent_b)
      {
        idx_parent_b++;
        idx_parent_b = idx_parent_b % PARENTS_QTTY;
      }
      // printf("idx_parent_a=%d, idx_parent_b=%d\n", idx_parent_a, idx_parent_b);

      // Select cut indexes
      cross_idx_x = rand() % INSTRUCTIONS_QUANTITY;
      cross_idx_y = rand() % INSTRUCTIONS_QUANTITY;
      // cross_idx_x = 0;
      // cross_idx_y = 4;
      
      if(cross_idx_x > cross_idx_y)
      {
        temp = cross_idx_x;
        cross_idx_x = cross_idx_y;
        cross_idx_y = temp;
      }
      
      if(cross_idx_x == cross_idx_y)
      {
        if(cross_idx_x == 0) cross_idx_y++;
        else cross_idx_x--;
      }
      
      // printf("idx_parent_a=%d, idx_parent_b=%d, cross_idx_x=%d, cross_idx_y=%d\n", idx_parent_a, idx_parent_b, cross_idx_x, cross_idx_y);
      // printf("%d\n",i*PARENTS_QTTY + j*GENERATION_GROWTH);
      
      // Get genes from parent A (from cross_idx_x to cross_idx_y)
      for(k=0; k<INSTRUCTIONS_QUANTITY; k++)
      {
        if(k>=cross_idx_x && k<cross_idx_y)
          // Get genes from parent A to child A
          memcpy(&(population[i*PARENTS_QTTY + j*GENERATION_GROWTH].ins[k]), &(parents[idx_parent_a].ins[k]), sizeof(Instruction));
        else
          // Get genes from parent A to child B
          memcpy(&(population[i*PARENTS_QTTY + j*GENERATION_GROWTH + 1].ins[k]), &(parents[idx_parent_a].ins[k]), sizeof(Instruction));
      }
      
      // Get genes from parent B to child A
      int idx_child = 0;
      for(k=0; k<INSTRUCTIONS_QUANTITY; k++)
      {
        if(idx_child == cross_idx_x)
          idx_child = cross_idx_y;
        if(idx_child >= INSTRUCTIONS_QUANTITY)
          break;
        
        if(!ExistsInChild(population[i*PARENTS_QTTY + j*GENERATION_GROWTH].ins, &(parents[idx_parent_b].ins[k]), cross_idx_x, cross_idx_y, IN))
        {
          memcpy(&(population[i*PARENTS_QTTY + j*GENERATION_GROWTH].ins[idx_child]), &(parents[idx_parent_b].ins[k]), sizeof(Instruction));
          //PrintInstructions(&(population[i*PARENTS_QTTY + j*GENERATION_GROWTH].ins[idx_child]));
          idx_child++;
        }
      }
      // PrintInstructions(population[i*PARENTS_QTTY + j*GENERATION_GROWTH].ins);
      
      // Get genes from parent B to child B
      idx_child = cross_idx_x;
      for(k=0; k<INSTRUCTIONS_QUANTITY; k++)
      {
        if(idx_child >= cross_idx_y)
          break;
        
        if(!ExistsInChild(population[i*PARENTS_QTTY + j*GENERATION_GROWTH + 1].ins, &(parents[idx_parent_b].ins[k]), cross_idx_x, cross_idx_y, OUT))
        {
          memcpy(&(population[i*PARENTS_QTTY + j*GENERATION_GROWTH + 1].ins[idx_child]), &(parents[idx_parent_b].ins[k]), sizeof(Instruction));
          //PrintInstructions(&(population[i*PARENTS_QTTY + j*GENERATION_GROWTH + 1].ins[idx_child]));
          idx_child++;
        }
      }
      // PrintInstructions(population[i*PARENTS_QTTY + j*GENERATION_GROWTH + 1].ins);
    }
  }
  
  // printf("--POPULATION--\n");
  // PrintPopulation();

  for(i=0; i<GENERATION_GROWTH; i++)
  {
    for(j=0; j<crossings; j++)
    {
      Mutation(&(population[i*PARENTS_QTTY + j*GENERATION_GROWTH]));
      Mutation(&(population[i*PARENTS_QTTY + j*GENERATION_GROWTH + 1]));
    }
  }
}

//----------------------------------------------------------------------------------------
// Mutation
//----------------------------------------------------------------------------------------
Mutation(Sequence *s)
{
  int idx_x, idx_y;
  Instruction temp;
  
  // srand(time(NULL));
  
  // Select swap indexes
  idx_x = rand() % INSTRUCTIONS_QUANTITY;
  idx_y = rand() % INSTRUCTIONS_QUANTITY;
  
  // printf("idx_x=%d, idx_y=%d\n", idx_x, idx_y);

  if(idx_x != idx_y)
  {
    // printf("BEFORE:\n   IDX_X: ");
    // PrintInstruction(&(s->ins[idx_x]));
    // printf("   IDX_Y: ");
    // PrintInstruction(&(s->ins[idx_y]));
    memcpy(&temp, &(s->ins[idx_x]), sizeof(Instruction));
    memcpy(&(s->ins[idx_x]), &(s->ins[idx_y]), sizeof(Instruction));
    memcpy(&(s->ins[idx_y]), &temp, sizeof(Instruction));
    // printf("AFTER:\n   TEMP: ");
    // PrintInstruction(&temp);
    // printf("   IDX_X: ");
    // PrintInstruction(&(s->ins[idx_x]));
    // printf("   IDX_Y: ");
    // PrintInstruction(&(s->ins[idx_y]));
  }
}

//----------------------------------------------------------------------------------------
// Instructions Creator
//----------------------------------------------------------------------------------------
void CreateInstructions(Instruction inss[], int array[])
{
  int i=0;
  
  for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
  {
    inss[i].type = (i < (INSTRUCTIONS_IN)) ? INPUT : OUTPUT;
    inss[i].x = ((array[i]-1)%X_MAX)+1;
    inss[i].y = (int)((array[i]-1)/X_MAX);
  }
}

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Instruction instructions[INSTRUCTIONS_QUANTITY];
  int array[INSTRUCTIONS_QUANTITY];
  int i;
  
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
    verified_solutions = 0;
    memset(&array, 0x00, INSTRUCTIONS_QUANTITY*sizeof(int));

    for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
    {
      array[i] = ((line[i*4]-'0')*100+(line[i*4+1]-'0')*10+(line[i*4+2]-'0'));
    }
    PrintIntArray(array, INSTRUCTIONS_QUANTITY);
    // printf("\n");
    
    CreateInstructions(instructions, array);
    // PrintInstructions(instructions);
    // printf("\n");
    
    InitializePopulation(instructions);
    // PrintPopulation();
    // printf("\n");
    
    EvaluatePopulation();
    // PrintPopulation();
    // printf("\n");
    
    // SelectParents();
    
    // //PrintPopulation();
    // printf("---PARENTS----\n");
    // PrintParents();
    
    // Crossover();
    // EvaluatePopulation();
    
    // printf("--POPULATION--\n");
    // PrintPopulation();
    
    // srand(time(NULL));
    // for(i=0;i<10;i++)
      // printf("AAA %d\n", rand() % PARENTS_QTTY);
    
    for(i=0; i<GENERATION_QTTY; i++)
    {
      // printf("%02d GENERATION: Cost:", i);
      SelectParents();
      // PrintParents();
      // printf("\n");
      
      Crossover();
      EvaluatePopulation();
      // printf("%02d\n", population_cost);
      //PrintParents();
      
      // printf("POPULATION\n");
      // PrintPopulation();
      // printf("\n");
    }
    PrintOptimal(INSTRUCTIONS_QUANTITY);
    // printf("%d\n", verified_solutions);
    printf("\n");
  }
  return 0;
}

